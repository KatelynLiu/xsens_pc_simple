#include "xsens.h"

xsens::xsens(const char* path):
	    _task_should_exit(false), _task_running(false)
{
}

xsens::~xsens()
{
    _task_should_exit = true;
    //delete(read_buffer);		//如果是定义的指针的形式就需要这么写
}

//这个函数返回的整型uart其实就是串口的文件句柄
int xsens::open_port(const char* device)
{
    const int uart = open( device , O_RDWR|O_NOCTTY );
    if( uart < 0 )
    {
	perror( "Failed to open port" );
	return -1;
    }
    return uart;
}

/*
 *串口的设置其实就是设置结构体struct termios的各个成员值
 *c_iflag 输入模式标志
 *c_oflag 输出模式标志
 *c_cflag 控制模式标志,校验的设置
 *c_lflag 局部模式控制
 *c_line  线协议
 * c_cc[NCC] 控制角色
 */
int xsens::set_baudrate( const int uart , unsigned baud)
{
    int speed;
    switch(baud){
      case 9600:speed = B9600;break;
      case 19200:speed = B19200;break;
      case 38400:speed = B38400;break;
      case 57600:speed = B57600;break;
      case 115200:speed = B115200;break;
      default:
	perror("Wrong BaudRate!");
	return -1;
    }
    //将结构体放到新的配置中,备份串口配置到uart_config中
    struct termios uart_config;
    int termios_state;
    //以新的配置填充结构体,设置某个选项,那么就使用"|="运算,如果关闭某个选项就使用"&="和"~"运算
    //获取终端参数
    tcgetattr( uart , &uart_config );
    //清除标志位
    uart_config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
				 INLCR | PARMRK | INPCK | ISTRIP | IXON);
    uart_config.c_oflag = 0;
    uart_config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    uart_config.c_cflag &= ~(CSTOPB | PARENB);
    //uart_config.c_oflag &= ~ONLCR;
    //设置波特率,注意输入和输出波特率是分开设置的
    if( (termios_state = cfsetispeed( &uart_config , speed ) ) < 0 )
    {
	//close( uart );
	printf("ERR: %d (cfsetispeed)" , termios_state );
	return -1;
    }
    if( ( termios_state = cfsetospeed( &uart_config , speed ) ) < 0 ) 
    {
	//close( uart );
	printf("ERR: %d ( cfsetospeed)" , termios_state );
	return -1;
    }
    //将uart的状态放入到uart_config中
    if( ( termios_state = tcsetattr( uart , TCSANOW , &uart_config ) ) < 0 )
    {
	//close( uart );
	printf( "ERR: %d (tcsetattr)" , termios_state );
	return -1;
    }
    tcflush( uart , TCIOFLUSH );
    return 0;
}

/**
 * @brief 读取期望消息ID 的消息中的测量数据
 * @param message 输入已经分解为MID/LEN/data的消息
 * @param expectedID 输入期望的消息ID 
 * @return 如果期望消息ID和解析得到的ID一致,则读取消息数据成功,返回0,否则返回-1
 */
int xsens::getMessageData(XbusMessage message, uint16_t* expectedID)
{
    if( message.mid == XMID_MtData2 )
    {
	printf( " MTData2: ");
	uint16_t itemId;
	uint8_t itemSize;
	//这里指定输入的是XDI_Eulerngles的消息类型,如果对的话,raw非空,同时返回消息的类型和长度
	uint8_t const* raw = getPointerToData( expectedID , message.data , message.length , &itemId , &itemSize );
	if( raw )
	{
	    switch( itemId )
	    {
	      case XDI_PacketCounter:
		raw = Xbus_readU16( &counter , raw );
		break;
	      case XDI_Quaternion:
		readFloats( Quaternion , raw , 4 );
		break;
	      case XDI_EulerAngles:
		readFloats( Euler , raw , 3 );
		break;
	      case XDI_DeltaQ:
		readFloats( deltaQ , raw , 4 );
		break;
	      case XDI_DeltaV:
		readFloats( deltaV , raw , 3 );
		break;
	      case XDI_Acceleration:
		readFloats( acc , raw , 3 );
		break;
	      case XDI_RateOfTurn:
		readFloats( gyro , raw , 3 );
		break;
	      case XDI_MagneticField:
		readFloats( mag , raw , 3 );
		break;
	      default:
		printf("No this dataID!");
		break;
	    }
	    return 0;
	}else{
	    return -1;
	}
    }
}

/**
 * @brief 读取期望消息ID 的消息中的测量数据
 * @param message 输入已经分解为MID/LEN/data的消息
 * @param expectedID 输入期望的消息ID 
 * @return 如果期望消息ID和解析得到的ID一致,则读取消息数据成功,返回0,否则返回-1
 */
int xsens::getMessageData_AccGyroMag(XbusMessage message)
{
    if( message.mid == XMID_MtData2 )
    {
	printf( " MTData2_AccGyroMag:\n ");
	uint16_t itemId;
	uint8_t itemSize;
	uint16_t dataID1 = (uint16_t)XDI_Acceleration;
	uint16_t dataID2 = (uint16_t)XDI_RateOfTurn;
	uint16_t dataID3 = (uint16_t)XDI_MagneticField;
	//这里指定输入的是XDI_Eulerngles的消息类型,如果对的话,raw非空,同时返回消息的类型和长度
	uint8_t const* raw = getPointerToData( &dataID1 , message.data , message.length , &itemId , &itemSize );
	if( raw && itemId == XDI_Acceleration )
	{
	    readFloats( acc , raw , 3 );
	    printf("itemID1 = %x  " , itemId );
	    printMessageData( itemId );
	}else{
	    return -1;
	}
	raw = getPointerToData( &dataID2 , message.data , message.length , &itemId , &itemSize );
	if( raw && itemId == XDI_RateOfTurn )
	{
	    readFloats( gyro , raw , 3 );
	    printf("itemID2 = %x  " , itemId );
	    printMessageData( itemId );
	}else{
	    return -1;
	}
	raw = getPointerToData( &dataID3 , message.data , message.length , &itemId , &itemSize );
	if( raw && itemId == XDI_MagneticField )
	{
	    readFloats( mag , raw , 3 );
	    printf("itemID3 = %x  " , itemId );
	    printMessageData( itemId );
	}else{
	    return -1;
	}
    }
}

/**
 * @brief 打印的消息数据
 * @param itemID 输入的消息ID 
 * @return 如果期望消息ID和解析得到的ID一致,则读取消息数据成功,返回0,否则返回-1
 */
void xsens::printMessageData( uint16_t itemID )
{
	    switch( itemID )
	    {
	      case XDI_PacketCounter:
		printf( "Packet counter : %5d " , counter );
		break;
	      case XDI_Quaternion:
		printf("Quaternion : %.5f , %.5f , %.5f , %.5f" , (double)Quaternion[0] , (double)Quaternion[1] , (double)Quaternion[2] , (double)Quaternion[3] );
		break;
	      case XDI_EulerAngles:
		printf( "EulerAngles : roll = %.5f ,  pitch = %.5f ,  yaw = %.5f" , (double)Euler[0] , (double)Euler[1] , (double)Euler[2] );
		break;
	      case XDI_DeltaQ:
		printf( "DeltaQ :  %.5f , %.5f , %.5f , %.5f" , deltaQ[0] , deltaQ[1] , deltaQ[2] , deltaQ[3] );
		break;
	      case XDI_DeltaV:
		printf("DeltaV : %.5f , %.5f , %.5f" , deltaV[0] , deltaV[1] , deltaV[2] );
		break;
	      case XDI_Acceleration:
		printf("Acceleration : %.5f , %.5f , %.5f" , acc[0] , acc[1] , acc[2] );
		break;
	      case XDI_RateOfTurn:
		printf("Gyro : %.5f , %.5f , %.5f" , gyro[0] , gyro[1] , gyro[2] );
		break;
	      case XDI_MagneticField:
		printf("mag : %.5f , %.5f , %.5f" , mag[0] , mag[1] , mag[2] );
		break;
	      default:
		printf("Not this ID's data!");
	    }
	printf("\n");
}

/**
 * @brief 读取整条消息并进行校验
 * @param uart 读取数据的串口
 * @param expectedID 输入期望的消息ID 
 * @return 如果校验通过,且期望消息ID和解析得到的ID一致,则读取消息数据成功,返回数据的ID,注意并非消息id,否则返回-1
 */
uint16_t xsens::readMessage(int uart , uint16_t* expectedID )
{
    //读取串口传上来的数据
	    memset( read_buffer , 0 , sizeof( read_buffer ) );	//清空读取数据的缓存区
	    read_counter = read( uart , read_buffer , sizeof( read_buffer ) );		//返回read函数读取到的字节数
	    int buffer_length = sizeof( read_buffer );
	    
		//进行数据帧校验,这部分提取出消息,并将数据存起来
		int buffer_i = 0;
		while( buffer_i < read_counter )
		{
		  //检测帧头,如果检测到0xFA和0xFF才继续,否则指针向后移一位继续向后查找
		    if( read_buffer[buffer_i++] == XBUS_PREAMBLE )
		    {
			if(read_buffer[buffer_i++] == XBUS_MASTERDEVICE)
			{
			    uint8_t checksum = 0xFF;
			    checksum = -checksum;
			    read_message.mid = read_buffer[buffer_i++];
			    checksum -= read_message.mid;
			    read_message.length = read_buffer[buffer_i++];
			    checksum -= read_message.length;
			    for( int data_i = 0 ; data_i < (read_message.length) ; data_i++)
			    {
			      checksum -= read_buffer[buffer_i];
				read_message.data[data_i] = read_buffer[buffer_i++];
			    }
			    if( checksum == read_buffer[buffer_i++] )
			    {
			      /*
			      printf("useful read = ");
			      int j = 0;
			      while(j < read_message.length)
				    {
				      printf("  %x  " , read_message.data[j++]);
				    }
				    printf("\n");
				    */
				if( *expectedID == XDI_Acceleration )
				    getMessageData_AccGyroMag( read_message );
				else
				    getMessageData( read_message , expectedID );
				return *expectedID;
			    }else{
				printf("Wrong message!");
				return -1;
			    }
			}
		    }
		}
		return -1;
}




