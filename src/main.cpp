#include <string.h>

#include "xsens.h"
int main( int argc , char *argv[] )
{
    int uart;
    int nread;
    struct XbusMessage read_message;		//读取到的消息,不能用const来定义,否则就成了只读了
    const char *path = DEFAULT_UART;
    uint16_t dataID = (uint16_t)XDI_Acceleration;
    xsens *xsens_dev = nullptr;
    xsens_dev = new xsens( path );
    xsens_dev -> _task_running = true;
    int read_counter = 0;
    //打开串口
    uart = xsens_dev->open_port( path );
    if( uart< 0 )
    {
	path = "/dev/ttyUSB1";
	xsens_dev = new xsens (path);
	uart = xsens_dev->open_port( path );
	if(uart < 0)
	{
	    path = "/dev/ttyUSB0";
	    xsens_dev = new xsens (path);
	    uart = xsens_dev->open_port( path );
	    if(uart < 0)
	    {
		printf(" Opening uart failed.\n");
		xsens_dev->_task_running = false;
		return -1;
	    }
	}
    }
    //设置波特率
    if( xsens_dev->set_baudrate( uart , 115200 ) < 0 )
    {
	    printf("Set baudrate failed.\n");
	    return -1;
    }
    	printf(" Init uart successfully!\n");
	printf("open return uart is : %d\n" , uart );
	while( true )
	{
	    //读取串口传上来的消息
	  xsens_dev->readMessage( uart , &dataID );
	  /*
	    if( dataID == xsens_dev->readMessage( uart , &dataID ) )
	    {
		xsens_dev->printMessageData( dataID );
	    }    
	    */
	}
	close( uart );
	xsens_dev->_task_should_exit = false;
	return 0;
}
