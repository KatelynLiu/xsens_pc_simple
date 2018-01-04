/**
 * @file 
 * @copyright Katelyn Liu ( ZJU ) , 2017.12.13
 * 这个文件中是对xsens的消息进行底层的读取和解析的操作,开发接口的时候可能需要
 */
#ifndef __XBUSMESSAGE_H
#define __XBUSMESSAGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

//数据包内容的定义
/*! \brief Xbus message preamble byte.消息的前导字节 */
#define XBUS_PREAMBLE (0xFA)
/*! \brief Xbus message bus ID for master devices.主设备的ID地址*/
#define XBUS_MASTERDEVICE (0xFF)
/*! \brief Xbus length byte for messages without payload.没有有效载荷的消息字节长度,即没有内容的消息 */
#define XBUS_NO_PAYLOAD (0x00)
/*! \brief Xbus length byte for message with an extended payload.有扩展载荷的消息长度 */
#define XBUS_EXTENDED_LENGTH (0xFF)


#ifdef __cplusplus
extern "C"{
#endif

  //每条消息中的第三个数据MID--消息ID,说明消息的作用,8位,1字节
  enum XsMessageId
  {
	  XMID_Wakeup             = 0x3E,
	  XMID_WakeupAck          = 0x3F,
	  XMID_ReqDid             = 0x00,
	  XMID_DeviceId           = 0x01,
	  XMID_GotoConfig         = 0x30,
	  XMID_GotoConfigAck      = 0x31,
	  XMID_GotoMeasurement    = 0x10,
	  XMID_GotoMeasurementAck = 0x11,
	  XMID_MtData2            = 0x36,
	  XMID_ReqOutputConfig    = 0xC0,
	  XMID_SetOutputConfig    = 0xC0,
	  XMID_OutputConfig       = 0xC1,
	  XMID_Reset              = 0x40,
	  XMID_ResetAck           = 0x41,
	  XMID_Error              = 0x42,
  };
  
  //测量数据的消息类型ID,16位,2字节
  enum XsDataIdentifier
  {
	  XDI_PacketCounter  = 0x1020,
	  XDI_SampleTimeFine = 0x1060,
	  XDI_Quaternion     = 0x2010,
	  XDI_EulerAngles = 0x2030,
	  XDI_DeltaV         = 0x4010,
	  XDI_Acceleration   = 0x4020,
	  XDI_RateOfTurn     = 0x8020,
	  XDI_DeltaQ         = 0x8030,
	  XDI_MagneticField  = 0xC020,
	  XDI_StatusWord     = 0xE020,
  };
  
  //消息结构体
  struct XbusMessage
  {
     uint8_t mid;		//消息id
     uint16_t length;				//消息长度
     uint8_t data[255];				//指向一整条消息中的数据的指针
  };
  
  //输出配置的结构体
  struct OutputConfiguration
  {
      enum XsDataIdentifier dtype;	//输出的数据类型
      uint16_t freq;					//输出频率
  };
  
  //读取数据和写入数据的函数
  uint8_t const* Xbus_readU8(uint8_t* out, uint8_t const* in);
  uint8_t const* Xbus_readU16(uint16_t* out, uint8_t const* in);
  uint8_t const* Xbus_readU32(uint32_t* out, uint8_t const* in);

  uint8_t* Xbus_writeU8(uint8_t* out, uint8_t in);
  uint8_t* Xbus_writeU16(uint8_t* out, uint16_t in);
  uint8_t* Xbus_writeU32(uint8_t* out, uint32_t in);
  //将消息转换为能够写入设备的16进制数据包的格式
  //static uint16_t messageLength( struct XbusMessage message );
  uint8_t const* getPointerToData( uint16_t* id , uint8_t const* data , uint16_t datalength , uint16_t* itemId , uint8_t* itemSize );
  void readFloats(float* out, uint8_t const* raw, uint8_t floats);
  bool XbusMessage_getDataItem(void* item, uint16_t* itemId , uint8_t const* raw );
  
  
#ifdef __cplusplus
}
#endif //extern "C"

#endif	//__XBUSMESSAGE_H
