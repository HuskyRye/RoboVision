#pragma once

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>     /*Unix标准函数定义*/
#include <sys/types.h>  /**/
#include <sys/stat.h>   /**/
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <sys/ioctl.h>

#define UART_SUCCESS 0
#define UART_ERROR -1

int uart_open(const char* port);
void uart_close(int fd);
int uart_init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
int uart_send(int fd, char *send_buf,int data_len);
void send_pixle(int fd, int pixle);
void send_angle(int fd, float yaw);
void send_angle(int fd, float yaw, float pitch);

union turn
{
	float d;
	unsigned char data[4];
}t;
