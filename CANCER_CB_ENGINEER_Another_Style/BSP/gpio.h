#ifndef __GPIO_H
#define __GPIO_H

#include "system.h"

/*----------------------------led---------------------------*/
#define LED_BLUE_Pin         			        GPIO_Pin_13
#define LED_ORANGE_Pin       			        GPIO_Pin_14
#define LED_GREEN_Pin        			        GPIO_Pin_10
#define LED_RED_Pin          			        GPIO_Pin_11
#define LED_CAMERA_Pin         		        GPIO_Pin_13
#define LED_ONBOARD_GPIO_Port             GPIOC
#define LED_CAMERA_GPIO_Port              GPIOD
/*----------------------------relay---------------------------*/
/*tube_claw catch & loose*/
#define TUBE_CLAW_CL_GPIO_Port				    GPIOD
#define TUBE_CLAW_CL_Pin			      	    GPIO_Pin_4//PD4:抱杆爪子 
#define TUBE_CLAW_CL_BACKUP_GPIO_Port     GPIOE
#define TUBE_CLAW_CL_BACKUP_Pin           GPIO_Pin_15//PE15:抱杆爪子
/*bomb_claw catch & loose*/
#define BOMB_CLAW_CL_GPIO_Port            GPIOE
#define BOMB_CLAW_CL_Pin                  GPIO_Pin_3//PE3:取弹爪子
#define BOMB_CLAW_CL_BACKUP_GPIO_Port     GPIOA
#define BOMB_CLAW_CL_BACKUP_Pin           GPIO_Pin_2//PA2:取弹爪子
/*bomb_eject push & pull*/
#define BOMB_EJECT_PP_GPIO_Port           GPIOE
#define BOMB_EJECT_PP_Pin                 GPIO_Pin_4//PE4:箱子弹射 
#define BOMB_EJECT_PP_BACKUP_GPIO_Port    GPIOA
#define BOMB_EJECT_PP_BACKUP_Pin          GPIO_Pin_5//PA5:箱子弹射 
/*bomb_claw push & pull*/
#define BOMB_CLAW_PP_GPIO_Port            GPIOD
#define BOMB_CLAW_PP_Pin                  GPIO_Pin_5//PD5:取弹爪子推拉
#define BOMB_CLAW_PP_BACKUP_GPIO_Port     GPIOE
#define BOMB_CLAW_PP_BACKUP_Pin           GPIO_Pin_14//PE14:取弹爪子推拉
/*trailer_hook push & pull*/
#define TRAILER_HOOK_PP_GPIO_Port         GPIOD
#define TRAILER_HOOK_PP_Pin               GPIO_Pin_6//PD6:救援 
#define TRAILER_HOOK_PP_BACKUP_GPIO_Port  GPIOE
#define TRAILER_HOOK_PP_BACKUP_Pin        GPIO_Pin_13//PE13:救援 
/*CCTV_switch */
#define CCTV_SWITCH_GPIO_Port             GPIOE
#define CCTV_SWITCH_Pin                   GPIO_Pin_6//PE6:小电视切换	
#define CCTV_SWITCH_BACKUP_GPIO_Port      GPIOA
#define CCTV_SWITCH_BACKUP_Pin            GPIO_Pin_4//PA4:小电视切换
/*CCTV_switch the third eye*/
#define THE_THIRD_EYE_GPIO_Port           GPIOE
#define THE_THIRD_EYE_Pin                 GPIO_Pin_5//PE5:补弹摄像头	
#define THE_THIRD_EYE_BACKUP_GPIO_Port    GPIOC
#define THE_THIRD_EYE_BACKUP_Pin          GPIO_Pin_0//PC0:补弹摄像头 
/*magazine open & close*/
#define MAGAZINE_OC_GPIO_Port             GPIOD
#define MAGAZINE_OC_Pin                   GPIO_Pin_7//PD7:弹仓开合	
#define MAGAZINE_OC_BACKUP_GPIO_Port      GPIOE
#define MAGAZINE_OC_BACKUP_Pin            GPIO_Pin_12//PE12:弹仓开合
/*----------------------------optic_switch---------------------------*/
/*single on chassis*/
#define OPTIC_SWITCH_S_GPIO_Port          GPIOB
#define OPTIC_SWITCH_S_Pin                GPIO_Pin_1//PB1:底盘上的光电
/*the one on bomb claw*/
#define OPTIC_SWITCH_M_GPIO_Port          GPIOC
#define OPTIC_SWITCH_M_Pin                GPIO_Pin_6//PC6:爪子上的光电
/*----------------------------CAN---------------------------*/
/*----CAN1_TX-----PA12----*/
/*----CAN1_RX-----PA11----*/
#define CAN1_GPIO_Port                    GPIOA
#define CAN1_TX_Pin                       GPIO_Pin_12
#define CAN1_RX_Pin                       GPIO_Pin_11
/*----CAN2_TX-----PB13----*/
/*----CAN2_RX-----PB12----*/
#define CAN2_GPIO_Port                    GPIOB
#define CAN2_TX_Pin                       GPIO_Pin_13
#define CAN2_RX_Pin                       GPIO_Pin_12
/*----------------------------servo---------------------------*/
#define GIM_SERVO_GPIO_Port               GPIOD
#define GIM_SERVO_Pin                     GPIO_Pin_12
/*----------------------------ducted---------------------------*/
#define DUCTED_GPIO_Port                  GPIOB
#define DUCTED_R_Pin                      GPIO_Pin_8
#define DUCTED_L_Pin                      GPIO_Pin_9
/*----------------------------uart---------------------------*/
/*remote_ctrl*/
#define RC_GPIO_Port                      GPIOA
#define RC_Pin                            GPIO_Pin_3
/*vision*/
#define VISION_GPIO_Port                  GPIOA
#define VISION_Pin                        GPIO_Pin_0 | GPIO_Pin_1
/*judgement*/
#define JUDGEMENT_TX_GPIO_Port            GPIOC
#define JUDGEMENT_TX_Pin                  GPIO_Pin_12
#define JUDGEMENT_RX_GPIO_Port            GPIOD
#define JUDGEMENT_RX_Pin                  GPIO_Pin_2


void GPIO_init(void);

#endif
