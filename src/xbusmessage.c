#include "xbusmessage.h"

/**
 * @brief 读取8位数据,每次读取一个字节
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位1个字节的数据
 * @return 指向输入消息的下一个字节
 */
uint8_t const* Xbus_readU8(uint8_t* out, const uint8_t* in)
{
    *out = *in;
    return ++in;
}

/**
 * @brief 读取8位数据,每次读取两个字节,共16位
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位2个字节的数据
 * @return 指向输入消息的下一个字节
 */
uint8_t const* Xbus_readU16(uint16_t* out, const uint8_t* in)
{
    *out = ( in[0] << 8)|in[1];
    return in + sizeof( uint16_t );
}

/**
 * @brief 读取8位数据,每次读取四个字节,共32位
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位4个字节的数据
 * @return 指向输入消息的下一个字节
 */
uint8_t const* Xbus_readU32(uint32_t* out, const uint8_t* in)
{
    *out = ( in[0] << 24 ) | ( in[1] << 16 ) | ( in[2] << 8 ) | in[3];
    return in + sizeof(uint32_t);
}

/**
 * @brief 写入8位数据,每次把in的一个字节写入到out中,共8位
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位1个字节的数据
 * @return 指向输入消息的下一个字节
 */
uint8_t* Xbus_writeU8(uint8_t* out, uint8_t in)
{
    *out++ = in;
    return out;
}

/**
 * @brief 写入8位数据,每次把in的2个字节写入到out中,共16位
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位2个字节的数据
 * @return 指向输入消息的下一个字节
 */
uint8_t* Xbus_writeU16(uint8_t* out, uint16_t in)
{
    *out++ = ( in >> 8 ) & 0xFF;
    *out++ = in & 0xFF;
    return out;
}

/**
 * @brief 写入8位数据,每次把in的4个字节写入到out中,共32位
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位4个字节的数据
 * @return 指向输入消息的下一个字节
 */
uint8_t* Xbus_writeU32(uint8_t* out, uint32_t in)
{
    *out++ = ( in >> 24 ) & 0xFF;
    *out++ = ( in >> 16 ) & 0xFF;
    *out++ = ( in >> 8 ) & 0xFF;
    *out++ = in & 0xFF;
    return out;
}

/**
 * @brief 写入8位数据,每次把in的4个字节写入到out中,共32位
 * @param in 输入,都是8位一个字节的数据,应该是一整条消息
 * @param out 输出,8位4个字节的数据
 * @return 指向输入消息的下一个字节
 */
/*
static uint16_t messageLength( struct XbusMessage const* message )
{
    switch( message->mid )
    {
      case XMID_SetOutputConfig:
	return message->length * 2 * sizeof( uint16_t );
      default:
	return message->length;
    }
}
*/

/**
 * @brief 设置输出数据的输出数据类型和频率,将配置输出的消息转换为串口上能够传输的消息
 * @param message 输入的配置输出格式的消息
 * @param raw 解析好的消息将被转换为串口上传输的消息格式进行传输
 */
/*
static void formatOutputConfig( uint8_t* raw , struct XbusMessage const* message )
{
    struct OutputConfiguration* conf = message->data;
    for( int i = 0 ; i < message->length ; ++i )
    {
	raw = Xbus_writeU16( raw , conf->dtype );
	raw = Xbus_writeU16( raw , conf->freq );
	++conf;
    }
}
*/
/**
 * @brief 将message转换为串口上能够传输的消息
 * @param message 输入的配置输出格式的消息
 * @param raw 解析好的消息将被转换为串口上传输的消息格式进行传输,注意raw中只包含了message中的测量数据部分--data部分
 */
/*
static void formatPayload( uint8_t* raw , struct XbusMessage const* message)
{
    switch( message->mid )
    {
      case XMID_SetOutputConfig:
	formatOutputConfig( raw , message );
	break;
      default:
	for( int i = 0 ; i < message->length ; ++i )
	{
	    *raw++ = ( ( uint8_t*)message->data )[i];
	}
	break;
    }
}
*/

/**
 * @brief 给输入的消息message加上消息帧头,然后将其内容组织成为能够在串口传输的16进制数据包格式
 * @param message 输入的配置输出格式的消息
 * @param raw 解析好的消息将被转换为串口上传输的消息格式进行传输,注意raw中只包含了message中的测量数据部分--data部分
 * @return 返回的是整条消息占的字节数
 */
/*
size_t XbusMessage_format( uint8_t* raw, XbusMessage* message )
{
    uint8_t* dptr = raw;
    //raw的前两帧先写入消息帧头
    *dptr++ = XBUS_PREAMBLE;
    *dptr++ = XBUS_MASTERDEVICE;
    uint8_t checksum = (uint8_t)(-XBUS_MASTERDEVICE);
    *dptr = message->mid;		//传输的第三个字节是消息的id
    checksum -= *dptr++;
    uint16_t length = messageLength( message );
    //处理消息长度
    if( length < XBUS_EXTENDED_LENGTH )
    {
	*dptr = length;
	checksum -= *dptr++;
    }else{
      //这里的意思是消息长度超过了0xFF,所以一个字节放不下,需要两个字节,注意这里先处理高8位,然后才处理低8位
	*dptr = XBUS_EXTENDED_LENGTH;
	checksum -= *dptr++;
	*dptr = length >> 8;
	checksum -= *dptr++;
	*dptr = length & 0xFF;
	checksum -= *dptr++;
    }
    //处理消息的正式内容
    formatPayload( dptr , message );
    for( int i = 0 ; i < length ; ++i )
    {
	checksum -= *dptr++;
    }
    *dptr++ = checksum;
    //返回的是整条消息占的字节数
    return dptr - raw;
}
*/
/**
 * @brief 输入去除了帧头的消息,然后输出一个指向真正数据的指针,即去掉FA/FF/MID/LEN的数据
 * @param id 输入消息的类型
 * @param data 输入的去除了帧头的消息
 * @param datalength 数据的长度
 * @return 返回指向数据真正内容的指针
 */
uint8_t const* getPointerToData( uint16_t* id , uint8_t const* data , uint16_t datalength , uint16_t* itemId , uint8_t* itemSize )
{
    uint8_t const* dptr = data;
    //如果是在一条消息的数据data范围内,则进行下面的循环
    while( dptr < data + datalength )
    {
	dptr = Xbus_readU16( itemId , dptr );
	dptr = Xbus_readU8( itemSize , dptr );
	if( *id == *itemId )
	  return dptr;		//如果输入数据的id就是从数据中中道的则返回,此时返回的时候,指针实际上已经指向的是数据长度之后的真正的数据内容
	dptr += (*itemSize);	//如果输入的数据id和找到的实际上的id不一致,则将指针指向下一个数据包
    }
    return NULL;
}

/**
 * @brief 从输入的raw中读取floats个浮点数的字节,注意每个floats都是一个32位的数据
 * @param out Pointer to where to output data.指向输出数据的指针
 * @param raw Pointer to the start of the raw float data.指向原始数据头的指针
 * @param floats The number of floats to read.要读取的浮点数的数目
 */
//void readFloats(float* out, uint8_t const* raw, uint8_t floats)
void readFloats(float* out, uint8_t const* raw, uint8_t floats)
{
	int i;
	for ( i = 0; i < floats; i++)
	{
		raw = Xbus_readU32((uint32_t*)&out[i], raw);
	}
}

/**
 * @brief Get a data item from an XMID_MtData2 Xbus message.从消息中根据不同的数据id读取数据信息(数据的位数不一样),得到数据包,
 * @param item Pointer to where to store the data.存储数据的数据包的指针
 * @param id The data identifier to get.得到的数据id
 * @param message The message to read the data item from.从这个消息中读取数据包
 * @returns true if the data item is found in the message, else false.消息中找到数据包就返回true
 */
bool XbusMessage_getDataItem(void* item, uint16_t* itemId , uint8_t const* raw )
{
    //uint8_t const* raw = getPointerToData( id , message.data , message.length );
    //只有当上面这个函数判断得出输入的数据id和我们指定的id一致才进行分id讨论
    if( raw )
    {
	switch( *itemId )
	{
	  case XDI_PacketCounter:
	    raw = Xbus_readU16( item , raw );	//数据包计数只有16位
	    break;
	  case XDI_SampleTimeFine:
	  case XDI_StatusWord:
	    raw = Xbus_readU32( item , raw );	//状态和采样时间只有32位
	    break;
	  case XDI_Quaternion:
	  case XDI_DeltaQ:
	    readFloats( item , raw , 4 );	//四元数和旋转增量的数据占了4个浮点数,4*32位,共计16个字节
	    break;
	  case XDI_DeltaV:
	  case XDI_Acceleration:
	  case XDI_RateOfTurn:
	  case XDI_MagneticField:
	    readFloats( item , raw , 3 );	//速度增量/加速度/陀螺仪/磁力计的值占了3个浮点数,3*32位,12个字节
	    break;
	  default:
	    return false;					//非上述所列的消息类型则错误
	}
	return true;
    }else{
	return false;					//输入数据的dataID和指定输入的id不一样直接返回错误
    }
}





