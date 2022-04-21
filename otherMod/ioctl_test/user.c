#include "../ioctl_htc.h" 
 
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
 
/* Functions for the ioctl calls */ 

pid_t p_work;

void avoid_zombies_handler(int signo)  
{  
    pid_t pid;  
    int exit_status;  
    int saved_errno = errno;  
  
    while ((pid = waitpid(-1, &exit_status, WNOHANG)) > 0) {  
        /* do nothing */  
    }  
  
    errno = saved_errno;  
}  
 
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
    // printf("get_msg message:%s", message); 
 
    return ret; 
} 
 
int ioctl_get_nth_byte(int file_desc) 
{ 
    int i, c; 
 
    // printf("get_nth_byte message:"); 
 
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
    char *msg = "Message passed by ioctl2\n";
    struct sigaction child_act;   
  
    memset(&child_act, 0, sizeof(struct sigaction));  
    child_act.sa_handler = avoid_zombies_handler;  
    child_act.sa_flags = SA_RESTART | SA_NOCLDSTOP;   
    sigemptyset(&child_act.sa_mask);  
    if (sigaction(SIGCHLD, &child_act, NULL) == -1) {  
        perror("sigaction error");  
        _exit(EXIT_FAILURE);  
    }

    file_desc = open(DEVICE_PATH, O_RDWR); 
    if (file_desc < 0) { 
        printf("Can't open device file: %s, error:%d\n", DEVICE_PATH, 
               file_desc); 
        exit(EXIT_FAILURE); 
    } 

    for (; ; ) {
        ret = ioctl_set_msg(file_desc, msg); 
        p_work = fork();
        if (p_work == 0) {
            close(file_desc); 
            int code = system("sudo insmod ../test_module.ko &");
            printf("system end\n");
            exit(0);
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
        }
        sleep(1);
    }

loop_out:
    close(file_desc); 
    
    return 0; 
error: 
    close(file_desc); 
    exit(EXIT_FAILURE); 
}