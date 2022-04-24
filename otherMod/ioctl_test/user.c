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
 
int ioctl_set_msg(int file_desc, union virtio_htc_ioctl_message *message) 
{ 
    int ret; 
 
    ret = ioctl(file_desc, IOCTL_SET_MSG, message->message); 
 
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
 
    return ret; 
}
 
/* Main - Call the ioctl functions */ 
int main(void) 
{ 
    int file_desc, ret, status, len; 

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
            printf("%s\n", message);
            message.command_message.status = 1;
            
            p_work = fork();
            if (p_work == 0) {
                close(file_desc); 
                int code = system(message.command_message.htc_command.command_str);
                exit(0);
            }
            else {
                ret = waitpid(p_work, &status, 0);
            }
            ret = ioctl_set_msg(file_desc, message.message);
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