#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"
#include "md5.h"

typedef  void (*iapfun)(void);

/* 此处的地址要和userapp中的起始地址对应 */
//#define FLASH_APP1_ADDR		0x08010000
//#define FLASH_APP2_ADDR		0x08027000

#define  FLASH_APP1_ADDR        0x08008000 //APP1起始位置
#define  FLASH_APP2_ADDR        0x08021000 //APP1起始位置
#define  BOOTFLAG_ADDR         1300  //片外FRAM文件起始位置
//#define  BOOTFLAG_ADDR         0x7FFFF0  //升级标志位置

#define	REAL_DATA_PARAM_Address     			400

#define REAL_DATA_PARAM_READ()    				MB85RS16A_READ(REAL_DATA_PARAM_Address,(uint8_t*)(&REAL_DATA_PARAM),sizeof(REAL_DATA_PARAM_t)) 
#define REAL_DATA_PARAM_Write()   				MB85RS16A_WRITE(REAL_DATA_PARAM_Address,(uint8_t*)(&REAL_DATA_PARAM),sizeof(REAL_DATA_PARAM_t))

typedef struct
{
 uint8_t BOOTFLAG; 
 uint32_t FILESIZE;
 char MD5CODE[16]; 
 char URL_ADDR[128]; 	
}updata_t;

//仪表总成的一些信息
typedef struct
{
	float BatVoltage;      //剩余电量
	float Tank_ARP;		     //气量剩余百分比 000-100 
	float TankQuality;     //本次气罐容量 KG ,由软件或者服务器发送本次的实际重量  Tank quality.unit(kg).

	//Electric_Meter_t Electric_Meter; //仪表的机械状态
	
	char  LidSensorStatus[2];      //表盖状态 0:Lid closed, 1: Lid open
	char  LidElectricLock[2];      //表盖锁的状态  0: UnLock, 1: Lock
	char  NEEDLESensorStatus[2];   //针阀的状态	0:needle up(close), 1: needle down(open)
	char	ElectricValveStatus[2];	 //阀门的状态  0:Valve closed, 1: Valve open
	char  TankSensorStatus[2];     //连接传感器的状态 0: Disassemble, 1: Assemble  现在用不着
	char  TankLockStatus[2];			 //连接锁的状态 0: UnLock, 1: Lock 
	
	uint8_t GprsCsq;           //GPRS信号强度
	
	char CUSTOMER_ID[32];
}REAL_DATA_PARAM_t;	

void iap_load_app(u32 appxaddr);
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);

//连接到TCP服务器
int connect_update_server(char* ip, char* port, int timeout);

//打印MD5字符串
void MD5Print(char* md5_str);

//从GSM模块中一段一段的读出数据并且校验
int check_firmware_in_sim900a(const char* md5);

//从GSM模块中一段一段的读出数据并写入到flash中
int write_firmware_from_sim900a(uint32_t dst);

//从指定的url下载固件，校验完成后写入
int update_from_sim900a(uint32_t dst, char* url, const char* md5);

#endif
