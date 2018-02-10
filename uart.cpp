#include "uart.h"

/*
 * @brief 打开串口
 */
int uart_open(const char* port)
{
    int fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);
    if (UART_ERROR == fd)
    {
        perror("Can't Open Serial Port");
        return UART_ERROR;
    }
    //恢复串口为阻塞状态
    if(fcntl(fd, F_SETFL, 0) < 0)
    {
        printf("fcntl failed!\n");
        return UART_ERROR;
    }
//    //测试是否为终端设备（设置开机启动时需要注释掉这一段）
//    if(0 == isatty(STDIN_FILENO))
//    {
//        printf("standard input is not a terminal device\n");
//        return UART_ERROR;
//    }
    return fd;
}

/*
 * @brief 关闭串口
 */
void uart_close(int fd)
{
    close(fd);
}

/*
 * @brief  设置串口数据位，停止位和效验位
 * @param  fd 类型  int  打开的串口文件句柄*
 * @param  speed 类型  int  波特率
 * @param  databits 类型  int  数据位   取值 为 7 或者8*
 * @param  stopbits 类型  int  停止位   取值为 1 或者2*
 * @param  parity 类型  int  效验类型 取值为N,E,O,S
 */
int uart_init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
    int i;
    int speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};

    struct termios options;

    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1.
    */
    if  (tcgetattr(fd,&options)  !=  0)
    {
        perror("SetupSerial 1");
        return UART_ERROR;
    }

    //设置串口输入波特率和输出波特率
    for ( i=0;  i<sizeof(speed_arr)/sizeof(int);  i++)
    {
        if  (speed == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);
            cfsetospeed(&options, speed_arr[i]);
        }
    }

    //修改控制模式，保证程序不会占用串口
    options.c_cflag |= CLOCAL;
    //修改控制模式，使得能够从串口中读取输入数据
    options.c_cflag |= CREAD;

    //设置数据流控制
    switch(flow_ctrl)
    {
    case 0 : //不使用流控制
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1 : //使用硬件流控制
        options.c_cflag |= CRTSCTS;
        break;
    case 2 : //使用软件流控制
        options.c_cflag |= IXON | IXOFF | IXANY;
        break;
    }
    //设置数据位
    //屏蔽其他标志位
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 5    :
        options.c_cflag |= CS5;
        break;
    case 6    :
        options.c_cflag |= CS6;
        break;
    case 7    :
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return UART_ERROR;
    }
    //设置校验位
    switch (parity)
    {
    case 'n':
    case 'N': //无奇偶校验位。
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O'://设置为奇校验
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E'://设置为偶校验
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 's':
    case 'S': //设置为空格
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported parity\n");
        return UART_ERROR;
    }
    // 设置停止位
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB; break;
    case 2:
        options.c_cflag |= CSTOPB; break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        return UART_ERROR;
    }

    //修改输出模式，原始数据输出
    options.c_oflag &= ~OPOST;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的
    //options.c_lflag &= ~(ISIG | ICANON);

    //设置等待时间和最小接收字符
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */

    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读
    tcflush(fd, TCIFLUSH);

    //激活配置 (将修改后的termios数据设置到串口中）
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        perror("com set error!\n");
        return UART_ERROR;
    }
    return UART_SUCCESS;
}

/*
 * @brief 串口数据发送
 */
int uart_send(int fd, char *send_buf, int data_len)
{
    int len = 0;

    len = write(fd, send_buf, data_len);
    if (len == data_len ) {
        return len;
    } else {
        tcflush(fd, TCOFLUSH);
        return UART_ERROR;
    }
}

void send_pixle(int fd,int pixle)
{
    unsigned char uart_data[5];

    uart_data[0] = 0xEE;
    uart_data[1] = 0;
    if (pixle < 0) {
        pixle = -pixle;
        uart_data[1] = 1;
    }
    uart_data[2] = (pixle>>8)&0xff;
    uart_data[3] = pixle&0xff;
    uart_data[4] = 0xAA;

    uart_send(fd, (char *)uart_data, 5);
}

void send_angle(int fd, float yaw)
{
	unsigned char uart_data[6];
	t.d = yaw;
	uart_data[0] = 0xEE;
	uart_data[1] = t.data[0];
	uart_data[2] = t.data[1];
	uart_data[3] = t.data[2];
	uart_data[4] = t.data[3];
	uart_data[5] = 0xAA;

	uart_send(fd, (char*)uart_data, 6);
}
