/**
  ******************************************************************************
  // Header:
  // File Name: motor.h
  // Author:     xudong
  // Date:
  ******************************************************************************
**/
  

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H
#define __MOTOR_H


typedef enum 
{
	motor_null =0,
	motor_open =1,
	motor_close =2,
	motor_stop =3
}MotorStatus_t;


#define MOTOR_PWR(X)		X>0? GPIO_SetBits(GPIOC,GPIO_Pin_11)\
														:GPIO_ResetBits(GPIOC,GPIO_Pin_11)

#define ENBOOST_PWR(X)	X>0? GPIO_SetBits(GPIOB,GPIO_Pin_1)\
														:GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define MOTOR2_HANDLE(X)\
{\
	if(X == motor_open)\
	{\
		GPIO_SetBits(GPIOC,GPIO_Pin_0);\
		GPIO_ResetBits(GPIOC,GPIO_Pin_1);\
	}\
	else 	if(X == motor_close)\
	{\
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);\
		GPIO_SetBits(GPIOC,GPIO_Pin_1);\
	}\
	else 	if(X == motor_stop)\
	{\
		GPIO_SetBits(GPIOC,GPIO_Pin_0);\
		GPIO_SetBits(GPIOC,GPIO_Pin_1);\
	}\
}\

#define Assemble_HANDLE(X)\
{\
	if(X == motor_open)\
	{\
		GPIO_SetBits(GPIOC,GPIO_Pin_2);\
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);\
	}\
	else 	if(X == motor_close)\
	{\
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);\
		GPIO_SetBits(GPIOC,GPIO_Pin_3);\
	}\
	else 	if(X == motor_stop)\
	{\
		GPIO_SetBits(GPIOC,GPIO_Pin_2);\
		GPIO_SetBits(GPIOC,GPIO_Pin_3);\
	}\
}\
	
extern MotorStatus_t gmotorStatus;	 
extern MotorStatus_t gassembleStatus;	

void motor_test(void);
void assemble_test(void);	
void Moto_Init(void);

//void motor_test(volatile MotorStatus_t motorStatus,uint16_t runtimeout);
//void motor_control(uint8_t NUM,MotorStatus_t motorStatus);

#endif /*_MOTOR_H */



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



