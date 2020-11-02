#include "motor.h"
#include <string.h>
#include "delay.h"


#define Lock_RUNTIME 	600
#define MOTOR_RUNTIME 600
#define Disassemble_RUNTIME 600

MotorStatus_t glockSet = motor_stop;//这个值存储指令发送过来的开关,只有确实开盖,或者时间到了才会赋值为空

MotorStatus_t gmotorStatus = motor_stop;	
MotorStatus_t gassembleStatus = motor_stop;

volatile uint32_t beginMotoTime = 0;

void Moto_Init()
{
  GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_11);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

void motor_test()
{	
	switch (gmotorStatus)
  {
  	case motor_stop:
			MOTOR2_HANDLE(motor_stop);
		  gmotorStatus = motor_null;
			MOTOR_PWR(0);
		  //printf("motor2_stop\r\n");
  		break;
  	case motor_close:
			MOTOR_PWR(1);
		  //if(strcmp(CONFIG_Meter.Lid_Type, "2")==0)
			{
			  delay_ms(1000);
		    MOTOR_PWR(0);
			}
			MOTOR2_HANDLE(motor_close);	
			delay_ms(MOTOR_RUNTIME);
			gmotorStatus = motor_stop;
		  //printf("motor2_close\r\n");
  		break;
		case motor_open:
			MOTOR_PWR(1);
		  //if(strcmp(CONFIG_Meter.Lid_Type, "2")==0)
			{
			  delay_ms(1000);
		    MOTOR_PWR(0);
			}
			MOTOR2_HANDLE(motor_open);
			delay_ms(MOTOR_RUNTIME);
			gmotorStatus = motor_stop;		
		  //printf("motor2_open\r\n");
			break;
  	default:
  		break;
  }
}

void assemble_test()
{	
	switch (gassembleStatus)
  {
  	case motor_stop:
			Assemble_HANDLE(motor_stop);
		  gassembleStatus = motor_null;
			MOTOR_PWR(0);
  		break;
  	case motor_close:
			MOTOR_PWR(1);
		  //if(strcmp(CONFIG_Meter.Lid_Type, "2")==0)
			{
			  delay_ms(1000);
		    MOTOR_PWR(0);
			}
			Assemble_HANDLE(motor_close);	
			delay_ms(Disassemble_RUNTIME);
			gassembleStatus = motor_stop;
  		break;
		case motor_open:
			MOTOR_PWR(1);
		  //if(strcmp(CONFIG_Meter.Lid_Type, "2")==0)
			{
			  delay_ms(1000);
		    MOTOR_PWR(0);
			}
			Assemble_HANDLE(motor_open);
			delay_ms(Disassemble_RUNTIME);
			gassembleStatus = motor_stop;		
			break;
  	default:
  		break;
  }
}

