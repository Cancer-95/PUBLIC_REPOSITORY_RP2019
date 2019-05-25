#include "omron_ctrl.h"

#define DETECT_PERIOD    2     //
#define CONFIRM_TIME     100  //ms

optic_switch_t optic_switch;

void optoelectronic_switch_init(void)
{
  memset(&optic_switch, 0, sizeof(optic_switch_t));
  optic_switch.confirm_time = CONFIRM_TIME / DETECT_PERIOD;
}

/*光电开关延时消抖程序*/
void optoelectronic_switch_task(void)
{
	switch(optic_switch.ctrl_mode)
	{
		case DETECT_ENABLE:
		{
			optic_switch.read[L_SIDE] = HAL_GPIO_ReadPin(OPTIC_SWITCH_L_GPIO_Port,OPTIC_SWITCH_L_Pin);
			optic_switch.read[R_SIDE] = HAL_GPIO_ReadPin(OPTIC_SWITCH_R_GPIO_Port,OPTIC_SWITCH_R_Pin);
				
			if(optic_switch.read[L_SIDE])
			{
				((millis() - optic_switch.react_time[L_SIDE]) > optic_switch.confirm_time ) ? (optic_switch.value[L_SIDE] = 1) : (optic_switch.value[L_SIDE] = 0);
			}
			else
			{
				optic_switch.react_time[L_SIDE] = millis();
				optic_switch.value[L_SIDE] = 0;
			}
			
			if(optic_switch.read[R_SIDE])
			{
				((millis() - optic_switch.react_time[R_SIDE]) > optic_switch.confirm_time ) ? (optic_switch.value[R_SIDE] = 1) : (optic_switch.value[R_SIDE] = 0);
			}
			else
			{
				optic_switch.react_time[R_SIDE] = millis();
				optic_switch.value[R_SIDE] = 0;
			}
		}break;
		
		case FORCE_ONE:
		{
			optic_switch.value[L_SIDE] = 1;
			optic_switch.value[R_SIDE] = 1;	
		}break;
		
		case FORCE_ZERO:
		{
			optic_switch.value[L_SIDE] = 0;
			optic_switch.value[R_SIDE] = 0;	
		}break;
		
		default:break;	
	}
	
}

