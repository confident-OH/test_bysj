#include "../ioctl_htc.h" 
 
#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h> 
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <sys/ioctl.h>
 
/* Functions for the ioctl calls */ 

pid_t p_work;
 
int ioctl_set_msg(int file_desc, char *message) 
{ 
    int ret; 
 
    ret = ioctl(file_desc, IOCTL_SET_MSG, message); 
 
    if (ret < 0) { 
        printf("ioctl_set_msg failed:%d\n", ret); 
    } 
 
    return ret; 
} 
 
int ioctl_get_msg(int file_desc) 
{ 
    int ret; 
    char message[100] = { 0 }; 
 
    /* Warning - this is dangerous because we don't tell  
   * the kernel how far it's allowed to write, so it  
   * might overflow the buffer. In a real production  
   * program, we would have used two ioctls - one to tell 
   * the kernel the buffer length and another to give  
   * it the buffer to fill 
   */ 
    ret = ioctl(file_desc, IOCTL_GET_MSG, message); 
 
    if (ret < 0) { 
        printf("ioctl_get_msg failed:%d\n", ret); 
    } 
    printf("get_msg message:%s", message); 
 
    return ret; 
} 
 
int ioctl_get_nth_byte(int file_desc) 
{ 
    int i, c; 
 
    printf("get_nth_byte message:"); 
 
    i = 0; 
    do { 
        c = ioctl(file_desc, IOCTL_GET_NTH_BYTE, i++); 
 
        if (c < 0) { 
            printf("\nioctl_get_nth_byte failed at the %d'th byte:\n", i); 
            return c; 
        } 
 
        putchar(c); 
    } while (c != 0); 
 
    return 0; 
} 
 
/* Main - Call the ioctl functions */ 
int main(void) 
{ 
    int file_desc, ret; 
    char *msg = "Message passed by ioctl\n"; 
 
    file_desc = open(DEVICE_PATH, O_RDWR); 
    if (file_desc < 0) { 
        printf("Can't open device file: %s, error:%d\n", DEVICE_PATH, 
               file_desc); 
        exit(EXIT_FAILURE); 
    } 
 
    ret = ioctl_set_msg(file_desc, msg); 
    p_work = fork();
    if (p_work == 0) {
        int code = system("sudo insmod ../test_module.ko");
    }
    else {
        if (ret) 
            goto error; 
        ret = ioctl_get_nth_byte(file_desc); 
        if (ret) 
            goto error; 
        ret = ioctl_get_msg(file_desc); 
        if (ret) 
            goto error; 
    
        close(file_desc); 
    }
    
    return 0; 
error: 
    close(file_desc); 
    exit(EXIT_FAILURE); 
}