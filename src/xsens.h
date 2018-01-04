#include <stdio.h>	//标准输入输出定义
#include <stdlib.h>	//标准函数库定义
#include <unistd.h>	//unix标准函数定义
#include <sys/types.h>	
#include <sys/stat.h>
#include <fcntl.h>		//文件控制定义
#include <termios.h>	//PPSIX终端控制定义
#include <errno.h>	//错误号定义
#include <stdint.h>	//无符号整型需要的头文件
#include <string.h>

#include "xbusmessage.h"	//处理xsens消息的底层文件

//使用的串口的定义------------考虑改成串口扫描的形式
#define DEFAULT_UART "/dev/ttyUSB2"		//这个需要插上串口后看一下,然后改掉

class xsens
{
public:
    xsens(const char *path );
    virtual ~xsens();
     
    int open_port( const char* device );
    int set_baudrate( const int uart , unsigned baud );
    //int writeData( int uart , uint8_t *buffer , size_t size );
    //void sendMessage( int uart , struct XbusMessage const* m );		//是不是应该换成size_t类型?----------
    //int gotoConfig( int uart );
    int getMessageData( struct XbusMessage message , uint16_t* expectedID );
    int getMessageData_AccGyroMag( struct XbusMessage read_message );
    void printMessageData( uint16_t itemID );
    uint16_t readMessage(int uart, uint16_t* expectedID );
    
    
    uint8_t read_buffer[99];			//读取数据的缓存区
    uint8_t *message_buffer;			//存放单条消息的缓冲区
    bool  _task_should_exit;		//进程结束标志
    bool _task_running;		//程序正在运行
    int read_counter;		//一次read读取到的字节数
    struct XbusMessage read_message;		//读取到的消息
    uint16_t counter;
    float Quaternion[4];
    float Euler[3];
    float deltaQ[4];
    float deltaV[3];
    float acc[3];
    float gyro[3];
    float mag[3];
};