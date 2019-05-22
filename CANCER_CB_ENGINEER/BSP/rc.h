#ifndef __RC_H
#define __RC_H

#include "system.h"

#define  USART2_DMA_RX_BUF_LEN               30u  
#define  RC_FRAME_LENGTH                     18u

#define RC_MIN 	-300
#define RC_MAX 	300
/* ----------------------- RC Channel Definition---------------------------- */
#define RC_CH_VALUE_MIN ((uint16_t)364 )
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)

/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)
#define RC_SW_OFFLINE ((uint16_t)4)//Cancer自己加的

/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)0x01<<0)		//0x01
#define KEY_PRESSED_OFFSET_S ((uint16_t)0x01<<1)		//0x02
#define KEY_PRESSED_OFFSET_A ((uint16_t)0x01<<2)		//0x04
#define KEY_PRESSED_OFFSET_D ((uint16_t)0x01<<3)		//0x08
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)0x01<<4)	//0x10
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)0x01<<5)		//0x20
#define KEY_PRESSED_OFFSET_Q ((uint16_t)0x01<<6)		//0x40
#define KEY_PRESSED_OFFSET_E ((uint16_t)0x01<<7)		//0x80
#define KEY_PRESSED_OFFSET_R ((uint16_t)0x01<<8)		//0x100
#define KEY_PRESSED_OFFSET_F ((uint16_t)0x01<<9)		//0x200
#define KEY_PRESSED_OFFSET_G ((uint16_t)0x01<<10)		//0x400
#define KEY_PRESSED_OFFSET_Z ((uint16_t)0x01<<11)		//0x800
#define KEY_PRESSED_OFFSET_X ((uint16_t)0x01<<12)		//0x1000
#define KEY_PRESSED_OFFSET_C ((uint16_t)0x01<<13)		//0x2000
#define KEY_PRESSED_OFFSET_V ((uint16_t)0x01<<14)		//0x4000
#define KEY_PRESSED_OFFSET_B ((uint16_t)0x01<<15)		//0x8000

/* ----------------------- My Key Confirm-------------------------------- */
#define KEY_W_PRESSED       ((RC_Ctl.key.v & 0x01)    ==  KEY_PRESSED_OFFSET_W) 
#define KEY_S_PRESSED       ((RC_Ctl.key.v & 0x02)    ==  KEY_PRESSED_OFFSET_S)
#define KEY_A_PRESSED       ((RC_Ctl.key.v & 0x04)    ==  KEY_PRESSED_OFFSET_A)
#define KEY_D_PRESSED       ((RC_Ctl.key.v & 0x08)    ==  KEY_PRESSED_OFFSET_D)
#define KEY_SHIFT_PRESSED   ((RC_Ctl.key.v & 0x10)    ==  KEY_PRESSED_OFFSET_SHIFT)
#define KEY_CTRL_PRESSED 	  ((RC_Ctl.key.v & 0x20)    ==  KEY_PRESSED_OFFSET_CTRL)
#define KEY_Q_PRESSED       ((RC_Ctl.key.v & 0x40)    ==  KEY_PRESSED_OFFSET_Q)
#define KEY_E_PRESSED 		  ((RC_Ctl.key.v & 0x80)    ==  KEY_PRESSED_OFFSET_E)
#define KEY_R_PRESSED 		  ((RC_Ctl.key.v & 0x100)   ==  KEY_PRESSED_OFFSET_R)
#define KEY_F_PRESSED 		  ((RC_Ctl.key.v & 0x200)   ==  KEY_PRESSED_OFFSET_F)
#define KEY_G_PRESSED 		  ((RC_Ctl.key.v & 0x400)   ==  KEY_PRESSED_OFFSET_G)
#define KEY_Z_PRESSED 		  ((RC_Ctl.key.v & 0x800)   ==  KEY_PRESSED_OFFSET_Z)
#define KEY_X_PRESSED 		  ((RC_Ctl.key.v & 0x1000)  ==  KEY_PRESSED_OFFSET_X)
#define KEY_C_PRESSED 		  ((RC_Ctl.key.v & 0x2000)  ==  KEY_PRESSED_OFFSET_C)
#define KEY_V_PRESSED 		  ((RC_Ctl.key.v & 0x4000)  ==  KEY_PRESSED_OFFSET_V)
#define KEY_B_PRESSED 		  ((RC_Ctl.key.v & 0x8000)  ==  KEY_PRESSED_OFFSET_B)
#define MOUSE_L_PRESSED     (RC_Ctl.mouse.press_l==1)
#define MOUSE_R_PRESSED     (RC_Ctl.mouse.press_r==1)

#define KEY_W_NOT_PRESSED       ((RC_Ctl.key.v & 0x01)    !=  KEY_PRESSED_OFFSET_W) 
#define KEY_S_NOT_PRESSED       ((RC_Ctl.key.v & 0x02)    !=  KEY_PRESSED_OFFSET_S)
#define KEY_A_NOT_PRESSED       ((RC_Ctl.key.v & 0x04)    !=  KEY_PRESSED_OFFSET_A)
#define KEY_D_NOT_PRESSED       ((RC_Ctl.key.v & 0x08)    !=  KEY_PRESSED_OFFSET_D)
#define KEY_SHIFT_NOT_PRESSED   ((RC_Ctl.key.v & 0x10)    !=  KEY_PRESSED_OFFSET_SHIFT)
#define KEY_CTRL_NOT_PRESSED 	  ((RC_Ctl.key.v & 0x20)    !=  KEY_PRESSED_OFFSET_CTRL)
#define KEY_Q_NOT_PRESSED       ((RC_Ctl.key.v & 0x40)    !=  KEY_PRESSED_OFFSET_Q)
#define KEY_E_NOT_PRESSED 		  ((RC_Ctl.key.v & 0x80)    !=  KEY_PRESSED_OFFSET_E)
#define KEY_R_NOT_PRESSED 		  ((RC_Ctl.key.v & 0x100)   !=  KEY_PRESSED_OFFSET_R)
#define KEY_F_NOT_PRESSED 		  ((RC_Ctl.key.v & 0x200)   !=  KEY_PRESSED_OFFSET_F)
#define KEY_G_NOT_PRESSED 		  ((RC_Ctl.key.v & 0x400)   !=  KEY_PRESSED_OFFSET_G)
#define KEY_Z_NOT_PRESSED 	  	((RC_Ctl.key.v & 0x800)   !=  KEY_PRESSED_OFFSET_Z)
#define KEY_X_NOT_PRESSED 	   	((RC_Ctl.key.v & 0x1000)  !=  KEY_PRESSED_OFFSET_X)
#define KEY_C_NOT_PRESSED 		  ((RC_Ctl.key.v & 0x2000)  !=  KEY_PRESSED_OFFSET_C)
#define KEY_V_NOT_PRESSED 	  	((RC_Ctl.key.v & 0x4000)  !=  KEY_PRESSED_OFFSET_V)
#define KEY_B_NOT_PRESSED 	  	((RC_Ctl.key.v & 0x8000)  !=  KEY_PRESSED_OFFSET_B)
#define MOUSE_L_NOT_PRESSED    	(RC_Ctl.mouse.press_l != 1)
#define MOUSE_R_NOT_PRESSED    	(RC_Ctl.mouse.press_r != 1)

typedef __packed struct
{
	int16_t ch0;
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	int8_t s1;
	int8_t s2;
}Remote;
typedef __packed struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t last_press_l;
	uint8_t last_press_r;
	uint8_t press_l;
	uint8_t press_r;
}Mouse;	
typedef	__packed struct
{
	uint16_t v;
}Key;

typedef __packed struct
{
	Remote rc;
	Mouse mouse;
	Key key;
}RC_Ctl_t;

extern RC_Ctl_t RC_Ctl;
extern int16_t System_Num;
extern bool RC_IS_READY;

void RC_init(void);
void RemoteDataProcess(uint8_t *pData);

void RC_offline_handler(void);//丢控保护

#endif



