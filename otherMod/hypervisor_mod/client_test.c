
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/un.h>
#include <net/sock.h>
#include <linux/delay.h>
#include <linux/posix_types.h>
#include <linux/time.h>

#define BUFFER_SIZE 2048

struct socket *sock;
struct sockaddr_un s_addr;
char un_path_test[32] = "/opt/zyq_qmp.socket";
unsigned long time_start, time_end;

struct kvec send_vec, recv_vec;
struct msghdr send_msg, recv_msg;
char qmp_message_init[BUFFER_SIZE] = "{'execute':'qmp_capabilities'}";
char qmp_speed_test[BUFFER_SIZE] = "{'execute': 'htc_zyq', 'arguments': {'id': 1, 'htc_str': '1'}}";
char *send_buf, *recv_buf;

int hyper_send_message(char *hy_send, size_t len)
{
    int ret = 0;
    send_vec.iov_base = hy_send;
    send_vec.iov_len = len;
    // init message
    ret = kernel_sendmsg(sock, &send_msg, &send_vec, 1, len);
    if (ret < 0) {
        printk("client: kernel_sendmsg error!\n");
        return ret;
    }
    else {
        // printk("send: %s len: %d \n", (char *)send_vec.iov_base, len);
    }
    return ret;
}

int hyper_receive_message(void)
{
    int ret = 0;
    memset(recv_buf, 0, BUFFER_SIZE);
    recv_vec.iov_base = recv_buf;
    recv_vec.iov_len = BUFFER_SIZE;
    ret = kernel_recvmsg(sock, &recv_msg, &recv_vec, 1, BUFFER_SIZE, 0);
    return ret;
}

int hyper_connect_init(void)
{
    int ret = 0, i;
    /* kmalloc a send buffer*/
    send_buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (send_buf == NULL) {
        printk("client: send_buf kmalloc error!\n");
        return -1;
    }
    /* kmalloc a receive buffer*/
    recv_buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if(recv_buf == NULL){
        printk("client: recv_buf kmalloc error!\n");
        return -1;
    }
    memset(&s_addr, 0, sizeof(s_addr));
    memset(&send_msg, 0, sizeof(send_msg));
    memset(&send_vec, 0, sizeof(send_vec));
    memset(recv_buf, 0, BUFFER_SIZE);
    memset(send_buf, 0, BUFFER_SIZE);
    memset(&recv_vec, 0, sizeof(recv_vec));
    memset(&recv_msg, 0, sizeof(recv_msg));

    // build a sock
    s_addr.sun_family = AF_UNIX;
    strcpy(s_addr.sun_path, un_path_test);
    ret = sock_create(AF_UNIX, SOCK_STREAM, 0, &sock);
    if (ret < 0) {
        printk("client:socket create error!\n");
        return ret;
    }
    printk("client: socket create ok!\n");
    // connect
    ret = sock->ops->connect(sock, (struct sockaddr *)&s_addr, sizeof(s_addr), 0);
    if (ret != 0) {
        printk("client: connect error!\n");
        return ret;
    }
    printk("client: connect ok!\n");
    msleep(500);
    hyper_receive_message();
    // printk("client: received message:\n %s\n", recv_buf);

    msleep(1000);
    // start qmp command
    hyper_send_message(qmp_message_init, strlen(qmp_message_init) + 1);
    msleep(500);
    hyper_receive_message();
    // printk("client: received message:\n %s\n", recv_buf);
    
    // qmp test
    // 100
    time_start = rdtsc();
    for (i = 0; i<100; i++) {
        hyper_send_message(qmp_speed_test, strlen(qmp_speed_test) + 1);
        hyper_receive_message();
        
    }
    time_end = rdtsc();
    printk("100: %ld(cpu circle)\n", time_end - time_start);
     // 1000
    time_start = rdtsc();
    for (i = 0; i<1000; i++) {
        hyper_send_message(qmp_speed_test, strlen(qmp_speed_test) + 1);
        hyper_receive_message();
        
    }
    time_end = rdtsc();
    printk("1000: %ld(cpu circle)\n", time_end - time_start);
     // 10000
    time_start = rdtsc();
    for (i = 0; i<10000; i++) {
        hyper_send_message(qmp_speed_test, strlen(qmp_speed_test) + 1);
        hyper_receive_message();
        
    }
    time_end = rdtsc();
    printk("10000: %ld(cpu circle)\n", time_end - time_start);
     // 100000
    time_start = rdtsc();
    for (i = 0; i<100000; i++) {
        hyper_send_message(qmp_speed_test, strlen(qmp_speed_test) + 1);
        hyper_receive_message();
        
    }
    time_end = rdtsc();
    printk("100000: %ld(cpu circle)\n", time_end - time_start);

     // 1000000
    time_start = rdtsc();
    for (i = 0; i<1000000; i++) {
        hyper_send_message(qmp_speed_test, strlen(qmp_speed_test) + 1);
        hyper_receive_message();
        
    }
    time_end = rdtsc();
    printk("1000000: %ld(cpu circle)\n", time_end - time_start);

    return 0;
}

static int client_example_init(void){
    printk("client: init\n");
    return hyper_connect_init();
}
static void client_example_exit(void){
    // shutdown connection
    kernel_sock_shutdown(sock, SHUT_RDWR);
    sock_release(sock);
    printk("client: exit!\n");
}
module_init(client_example_init);
module_exit(client_example_exit);
MODULE_LICENSE("GPL");