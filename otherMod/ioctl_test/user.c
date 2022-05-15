#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <fcntl.h> 
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <sys/ioctl.h>
#include "user.h"
 
/* Functions for the ioctl calls */ 

pid_t p_work;

union virtio_htc_ioctl_message message;
char ret_msg_htc[BUF_LEN];
 
int ioctl_set_msg(int file_desc, union virtio_htc_ioctl_message *message) 
{ 
    int ret; 
 
    ret = ioctl(file_desc, IOCTL_SET_MSG, message->message); 
 
    if (ret < 0) { 
        printf("ioctl_set_msg failed:%d\n", ret); 
    } 
 
    return ret; 
}

int ioctl_send_msg(int file_desc, char *message) 
{ 
    int ret; 
 
    ret = ioctl(file_desc, IOCTL_GET_EXE_INFO, message); 
 
    if (ret < 0) { 
        printf("ioctl_set_msg failed:%d\n", ret); 
    } 
 
    return ret; 
}
 
int ioctl_get_msg(int file_desc) 
{ 
    int ret; 
    /* Warning - this is dangerous because we don't tell  
   * the kernel how far it's allowed to write, so it  
   * might overflow the buffer. In a real production  
   * program, we would have used two ioctls - one to tell 
   * the kernel the buffer length and another to give  
   * it the buffer to fill 
   */ 
    ret = ioctl(file_desc, IOCTL_GET_MSG, message.message); 
 
    if (ret < 0) { 
        printf("ioctl_get_msg failed:%d\n", ret); 
    } 
    // printf("test: %s\n", message.command_message.htc_command.command_str);
    return ret; 
}
 
/* Main - Call the ioctl functions */ 
int main(void) 
{ 
    int file_desc, ret, status, len;
    FILE *fp = NULL;

    file_desc = open(DEVICE_PATH, O_RDWR); 
    if (file_desc < 0) { 
        printf("Can't open device file: %s, error:%d\n", DEVICE_PATH, 
               file_desc); 
        exit(EXIT_FAILURE); 
    }

    for (; ; ) {
        ret = ioctl_get_msg(file_desc); 
        if (ret) {
            goto error; 
        }
        if (message.command_message.status == 0) {
            printf("%s\n", message.command_message.command_str);
            message.command_message.status = 1;
            if (!strcmp(message.command_message.command_str, "user_exit")) {
                ret = ioctl_set_msg(file_desc, &message);
                goto loop_out;
            }
            ret = ioctl_set_msg(file_desc, &message);
            int code = system(message.command_message.command_str);
            strcat(message.command_message.command_str, " > user_out.txt");
            code = system(message.command_message.command_str);
            fp = fopen("user_out.txt", "r");
            if (fp != NULL) {
                char s;
                int i = 0;
                do {
                    s = getc(fp);
                    ret_msg_htc[i] = s;
                    i++;
                } while(s);
                ret = ioctl_send_msg(file_desc, ret_msg_htc);
            }
            else {
                ret = ioctl_send_msg(file_desc, "cannnot open file");
            }
        }
        usleep(10000);
    }

loop_out:
    close(file_desc); 
    
    return 0; 
error: 
    close(file_desc); 
    exit(EXIT_FAILURE); 
}