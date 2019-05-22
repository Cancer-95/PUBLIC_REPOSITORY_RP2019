#include "get_bomb.h"

get_bomb_t get_bomb;

void get_bomb_param_init(void)
{
	lifter.ctrl_mode = LIFTER_ZERO;     //¿ò¼ÜÂıÂı½µÏÂÀ´	

	kb.bomb_claw_enable = DISABLE;           //È¡µ¯×¦×Ó
//	SUPPORTING_WHEEL_PULL;                   //¸¨ÖúÖ§³ÅÍÈÊÕ»Ø
	BOMB_CLAW_CATCH;                         //È¡µ¯×¦×ÓºÏÉÏ
	BOMB_EJECT_PULL;                         //µ¯ÉäÊÕ»Ø
	
	BOMB_CLAW_MOVE_LEFT;                     //
//	BOMB_CLAW_MOVE_MID;                      //È¡µ¯×¦×ÓÆ½ÒÆÖÁÖĞ
	BOMB_CLAW_ROLL_ZERO;                     //È¡µ¯×¦×Ó·­×ª¹éÁã	
//	BOMB_CLAW_ROLL_ENTIRE; 
	BOMB_CLAW_PULL;                          //È¡µ¯×¦×ÓÊÕ»Ø

	get_bomb.once_triple = 0;							
	flag_bomb_claw_triple = 0;               //¹ØµôÈıÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
	
	get_bomb.once_penta = 0;							
	flag_bomb_claw_penta = 0;                //¹ØµôÎåÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
	
	vision.enable = DISABLE;
//	vision.aim = HAVENT;
//	get_bomb.vision_assist = 0;	
	flag_bomb_claw_vision = 0;               //¹ØµôÊÓ¾õÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
	
	get_bomb.scan = 0;	
  get_bomb.flag_scan_failed = NO;	
	get_bomb.scan_finished = NO;	
	optic_switch.value_record	= ENABLE;	
  get_bomb.scan_delay_record = ENABLE;	
	flag_bomb_claw_scan = 0;                 //¹ØµôÉ¨ÃèÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
	
	get_bomb.turn_around_record = ENABLE;    //µºÏÂÈ¡µ¯µôÍ·Ê¹ÄÜ
	get_bomb.turn_around_enable = ENABLE;
	
	get_bomb.flag_push_aim_single = HAVENT;    //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½	
	get_bomb.flag_push_aim_multiple = HAVENT;  //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½
	get_bomb.flag_push_aim_scan = HAVENT;      //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½
	
	get_bomb.get_pour_step = 0;              //È¡µ¯µ¹µ¯Ä£Ê½step¹éÁã	
}

bool flag_get_bomb_init = 1;

bool flag_push_aim_single_init = 1;
bool flag_push_aim_multiple_init = 1;
bool flag_push_aim_scan_init = 1;
bool flag_scan_safe_delay = 0;//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼ä

bool flag_eject_push_Time_record = 1;
bool flag_bomb_claw_record = 0;  

void get_bomb_ctrl(void)
{
	/*Èô³õÊ¼»¯¾ÍY_CLAW_HALF»á³öÎÊÌâ ¹Ê·ÅÔÚÕâÀï³õÊ¼»¯*/	
	if(flag_get_bomb_init)
	{
		get_bomb_param_init();
		BOMB_CLAW_MOVE_MID;                      //È¡µ¯×¦×ÓÆ½ÒÆÖÁÖĞ
		BOMB_CLAW_ROLL_ENTIRE; 
		
	  flag_push_aim_single_init = 1;
	  flag_push_aim_multiple_init = 1;
		flag_push_aim_scan_init = 1;
		flag_scan_safe_delay = 0;
		
		flag_get_bomb_init = 0;
	}

	keyboard_get_bomb_handler();//keyboard	
	
  if(get_bomb.get_bomb_enable == ENABLE)	
	{
//		get_bomb.vision_assist = ENABLE;//È«³ÌÔÊĞíÊÓ¾õ¶ÔÎ»
		
		switch(get_bomb.ctrl_mode)
		{	
			case STAND_BY_MODE://¹éÖĞ´ıÃüÄ£Ê½
			{		
				get_bomb.turn_around_record = ENABLE;   //µºÏÂÈ¡µ¯µôÍ·Ê¹ÄÜ
				get_bomb.turn_around_enable = ENABLE;
				
		    chassis.ass_mode_enable = DISABLE; 	    //¹Øµô³µÎ²Ä£Ê½  
				chassis.force_push_in_enable = DISABLE; //¹ØµôÍÆ½øÄ£Ê½
				chassis.ctrl_mode = CHASSIS_FOLLOW;	    //µ×ÅÌÍÓÂİÒÇÄ£Ê½
				
				flag_bomb_claw_CCTV = 0;
				flag_bomb_claw_CCTV_switch = 0;
				
				flag_eject_push_Time_record = 1;
				get_bomb.have_finished_once_get_bomb = NO;
				
				flag_push_aim_single_init = 1;
				flag_push_aim_multiple_init = 1;
				flag_push_aim_scan_init = 1;
				flag_scan_safe_delay = 0;
				
				get_bomb.once_triple = 0;							
	      flag_bomb_claw_triple = 0;               //¹ØµôÈıÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
				get_bomb.once_penta = 0;							
	      flag_bomb_claw_penta = 0;                //¹ØµôÎåÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
//				get_bomb.vision_assist = DISABLE;
				vision.enable = DISABLE;
//				vision.aim = HAVENT;
//				get_bomb.vision_assist = 0;	
				flag_bomb_claw_vision = 0;               //¹ØµôÊÓ¾õÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
			  get_bomb.scan = 0;	
				get_bomb.flag_scan_failed = NO;
        get_bomb.scan_finished = NO;	
        optic_switch.value_record	= ENABLE;	
				get_bomb.scan_delay_record = ENABLE;			
		    flag_bomb_claw_scan = 0;             //¹ØµôÉ¨ÃèÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
				/*ĞŞÕıÖĞĞÄÖµYÖá*/	
				if(bomb_claw.target_half[Y] > bomb_claw.Y_target[M])
				{
					bomb_claw.from_claw_entire[Y_axis] = YES;
					bomb_claw.from_claw_zero[Y_axis] = NO;
				}	
				else if(bomb_claw.target_half[Y] < bomb_claw.Y_target[M])
				{
					bomb_claw.from_claw_entire[Y_axis] = NO;
					bomb_claw.from_claw_zero[Y_axis] = YES;
				}		
				bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
				/*ĞŞÕı¿ò¼ÜÄ¿±êÖµ*/ 
				if(lifter.target_bomb_bit[L] != lifter.target_bomb_bit_normal[L] || 
					 lifter.target_bomb_bit[R] != lifter.target_bomb_bit_normal[R])		
				{
					lifter.target_bomb_bit[L] = lifter.target_bomb_bit_normal[L];
					lifter.target_bomb_bit[R] = lifter.target_bomb_bit_normal[R];
					if((lifter.total_angle[L] > lifter.target_bomb_bit[L]) && 
						 (lifter.total_angle[R] < lifter.target_bomb_bit[R]))
					{
						from_lifter_zero = YES;
						from_lifter_bomb_entire = NO; 			
					}
					else if((lifter.total_angle[L] < lifter.target_bomb_bit[L]) && 
									(lifter.total_angle[R] > lifter.target_bomb_bit[R]))
					{
						from_lifter_zero = NO;
						from_lifter_bomb_entire = YES; 			
					}
				}		
				/*ĞŞÕıÆ½Ì¨×óÓÒÄ¿±êÖµ*/
				if(bomb_claw.target_entire[Y] !=  bomb_claw.Y_target[R] ||
 				   bomb_claw.target_zero[Y] != bomb_claw.Y_target[L])
				{
					bomb_claw.target_entire[Y] =  bomb_claw.Y_target[R];
					bomb_claw.target_zero[Y] = bomb_claw.Y_target[L];
				}
				
				
				kb.bomb_claw_enable = DISABLE;    	 //È¡µ¯×¦×²²»¿É¿Ø	 ¹ØµôÁË°´ctrl+EÊÇÃ»ÓĞÊ²Ã´ÓÃµÄ
				
//	      SUPPORTING_WHEEL_PULL;               //¸¨ÖúÖ§³ÅÍÈÊÕ»Ø				
				BOMB_CLAW_CATCH;                   	 //È¡µ¯×¦×ÓºÏÉÏ
				BOMB_EJECT_PULL;                     //µ¯ÉäÊÕ»Ø

        BOMB_CLAW_MOVE_MID;               	 //È¡µ¯×¦×ÓÆ½ÒÆÖÁÖĞ
        BOMB_CLAW_ROLL_ENTIRE;               //È¡µ¯×¦×Ó·­×ª¹éÁã			
				BOMB_CLAW_PULL;                   	 //È¡µ¯×¦×ÓÊÕ»Ø		
				
				GIM_SERVO_ZERO;                      //ÔÆÌ¨Ç°Ãæ×ªµ½×îÇ°Ãæ

				get_bomb.flag_push_aim_single   = HAVENT;     //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½	
				get_bomb.flag_push_aim_multiple = HAVENT;   //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½	
				get_bomb.flag_push_aim_scan     = HAVENT;       //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½
				
				get_bomb.box_num = 0;	
				
				get_bomb.get_pour_step = 0;          //È¡µ¯µ¹µ¯Ä£Ê½step¹éÁã
					
//        if(servo_get_bomb_delay_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼äÈÃ¶æ»ú×ª»ØÀ´ÏÈ
//				{					
//					servo_get_bomb_delay_time = millis();
//					servo_get_bomb_delay_record = 0;
//				}
//				if(millis() - servo_get_bomb_delay_time > 800)
//				{
				if((abs(bomb_claw.total_angle[L] - bomb_claw.target_entire[L]) < 100) || 
					 (abs(bomb_claw.total_angle[R] - bomb_claw.target_entire[R]) < 100))	//×¦×Ó·­¹ıÀ´ÁË ¼ÙÏëÆø¸××ÜÊÇÒª±È×¦×Ó·­µÃ¿ìµÄ				
				{
					get_bomb.get_bomb_enable = DISABLE; //ÈÃÕâ¸öcaseÅÜÒ»´Î¾ÍÍË³öÁË
					lifter.ctrl_mode = LIFTER_ZERO;     //¿ò¼ÜÂıÂı½µÏÂÀ´
				}										
//				}		
			}break;
					
			case PUSH_AIM_SINGLE_MODE://µ¥ÏäÄ£Ê½ÏÂµÄÍÆ³ö¶Ô×¼Ä£Ê½
			{	
				if((climb_tube.location == ON_GROUND) && 
					 (climb_tube.ctrl_mode == PLAIN_MODE) &&
				    chassis.ctrl_mode == CHASSIS_FOLLOW &&
				    get_bomb.turn_around_enable == ENABLE)//µºÏÂÏÈµô¸öÍ·
				{
					if(get_bomb.turn_around_record)
					{
						get_bomb.turn_around_time = millis();
						flag_turn_around = 1;
						chassis.position_temp = imu.yaw;
						ramp_init(&turn_around_ramp, TUR_ARO_TIME/INFO_GET_PERIOD);		
						get_bomb.turn_around_record = DISABLE;
					}
					else if((!flag_turn_around) && ((millis() - get_bomb.turn_around_time) > 1200))//µôÍ·Íê³ÉÇ°±£³ÖÍÓÂİÒÇÄ£Ê½ ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼ä¾¡Á¿ÈÃËü×ªµ½Î»
					{
						chassis.ass_mode_enable = ENABLE;               //¿ªÆô³µÎ²Ä£Ê½
						chassis.force_push_in_enable = ENABLE;          //¿ªÆôÍÆ½øÄ£Ê½
						if(vision.enable == DISABLE)
						{
							chassis.ctrl_mode = CHASSIS_SEPARATE;         //µ×ÅÌ»úĞµÄ£Ê½
						}							
					}
				}
	      else //µºÉÏ¸Ã¸ÉÂï¸ÉÂï
				{
					chassis.ass_mode_enable = ENABLE;               //¿ªÆô³µÎ²Ä£Ê½
					chassis.force_push_in_enable = ENABLE;          //¿ªÆôÍÆ½øÄ£Ê½
					if(vision.enable == DISABLE)
					{
						chassis.ctrl_mode = CHASSIS_SEPARATE;         //µ×ÅÌ»úĞµÄ£Ê½
					}							
				}					
	
		  	{
				  /*ĞŞÕıÖĞĞÄÖµYÖá*/	
					if(bomb_claw.target_half[Y] > bomb_claw.Y_target[M])
					{
						bomb_claw.from_claw_entire[Y_axis] = YES;
						bomb_claw.from_claw_zero[Y_axis] = NO;
					}	
					else if(bomb_claw.target_half[Y] < bomb_claw.Y_target[M])
					{
						bomb_claw.from_claw_entire[Y_axis] = NO;
						bomb_claw.from_claw_zero[Y_axis] = YES;
					}		
					bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
					if(!vision.enable)
					{
						/*ĞŞÕı¿ò¼ÜÄ¿±êÖµ*/ 
						if(lifter.target_bomb_bit[L] != lifter.target_bomb_bit_normal[L] || 
							 lifter.target_bomb_bit[R] != lifter.target_bomb_bit_normal[R])		
						{
							lifter.target_bomb_bit[L] = lifter.target_bomb_bit_normal[L];
							lifter.target_bomb_bit[R] = lifter.target_bomb_bit_normal[R];
							if((lifter.total_angle[L] > lifter.target_bomb_bit[L]) && 
								 (lifter.total_angle[R] < lifter.target_bomb_bit[R]))
							{
								from_lifter_zero = YES;
								from_lifter_bomb_entire = NO; 			
							}
							else if((lifter.total_angle[L] < lifter.target_bomb_bit[L]) && 
											(lifter.total_angle[R] > lifter.target_bomb_bit[R]))
							{
								from_lifter_zero = NO;
								from_lifter_bomb_entire = YES; 			
							}	
						}					
					}
					/*ĞŞÕıÆ½Ì¨×óÓÒÄ¿±êÖµ*/
					if(bomb_claw.target_entire[Y] !=  bomb_claw.Y_target[R] || 
						 bomb_claw.target_zero[Y] != bomb_claw.Y_target[L])
					{
						bomb_claw.target_entire[Y] =  bomb_claw.Y_target[R];
						bomb_claw.target_zero[Y] = bomb_claw.Y_target[L];
					}			
					
	//				servo_get_bomb_delay_record = 1;
	//				servo_get_bomb_scan_delay_record = 1;
					
					get_bomb.once_triple = 0;							
					flag_bomb_claw_triple = 0;               //¹ØµôÈıÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
					
					get_bomb.once_penta = 0;							
					flag_bomb_claw_penta = 0;                //¹ØµôÎåÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
					
	//				vision.enable = DISABLE;
	//				vision.aim = HAVENT;
	//				get_bomb.vision_assist = 0;	
	//				flag_bomb_claw_vision = 0;               //¹ØµôÊÓ¾õÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
					
					get_bomb.scan = 0;	
					get_bomb.flag_scan_failed = NO;
					optic_switch.value_record	= ENABLE;		
					get_bomb.scan_delay_record = ENABLE;						
					get_bomb.scan_finished = NO;				
					flag_bomb_claw_scan = 0;             //¹ØµôÉ¨ÃèÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
					
					get_bomb.flag_push_aim_single = HAVENT;     //»¹Ã»Íê³Éµ¥ÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½
					get_bomb.flag_push_aim_multiple = HAVENT;   //»¹Ã»Íê³ÉÈıÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½
					get_bomb.flag_push_aim_scan = HAVENT;       //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½
					
					BOMB_CLAW_MOVE_MID;                 //Ç¿ĞĞÒÆµ½ÖĞ¼ä
					get_bomb.get_pour_step = 0;         //ºóÃæµÄÈ¡µ¯²½ÖèÇåÁã		
					
					if(flag_push_aim_single_init)
					{
						BOMB_CLAW_ROLL_HALF;//Ç¿ÖÆ·­×ª×¦×ÓÖÁHALF		

//						if((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 100) || 
//							 (abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 100))
//						{
							GIM_SERVO_SIDE;
//						}						
						
						if((abs(bomb_claw.total_angle[L] - bomb_claw.target_half[L]) < 100) || 
							 (abs(bomb_claw.total_angle[R] - bomb_claw.target_half[R]) < 100))			
						{
	//						if(climb_tube.location == ON_ISLAND)
	//						{
	//					     BOMB_CLAW_PUSH;
	//						}
	//						else BOMB_CLAW_PULL;					
							
							BOMB_CLAW_PULL;

							BOMB_EJECT_PULL;//µ¯ÉäÊÕ»Ø
							BOMB_CLAW_LOOSE;//Ç¿ÖÆÕÅ¿ª×¦×Ó 
							
							if(flag_eject_push_Time_record && get_bomb.have_finished_once_get_bomb == NO)
							{
								get_bomb.eject_push_Time = millis();//¼ÇÂ¼µ¯ÉäµÄÊ±¿Ìms	
								flag_eject_push_Time_record = 0;
							}
							else if((millis() - get_bomb.eject_push_Time > 300) && 
								      (millis() - get_bomb.eject_push_Time < 600)&&
						          	get_bomb.have_finished_once_get_bomb == NO)//ms
							{
								BOMB_EJECT_PUSH;
							}
							else if(millis() - get_bomb.eject_push_Time > 900)//ms
							{
								BOMB_EJECT_PULL;
								flag_push_aim_single_init = 0;
								flag_push_aim_multiple_init = 1;
								flag_push_aim_scan_init = 1;
								flag_scan_safe_delay = 0;
								get_bomb_clear_record = 0;//ÍË³ö¾À´íÖØÀ´µÄ¿ØÖÆ							
							}					
						}
					}
					
					if((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 100) ||
						 (abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 100))//Ì§Éıµ½Ä¿±ê½Ç¶È
					{ /*¿ò¼ÜÌ§µ½ÁËÎ»ÖÃ²ÅÖ´ĞĞÉì³ö¶Ô×¼*/	
							kb.bomb_claw_enable = ENABLE;//È¡µ¯×¦×Ó¿É¿Ø								
					
//						if((flag_bomb_claw_catch) && ((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100)))							
						if((flag_bomb_claw_catch) && (bomb_claw.zero_correct == FINISHED))
						{				
							if(get_bomb.catch_confirm_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼äÈÃ×¦×Ó¼Ğ×¡
							{					
								get_bomb.catch_confirm_delay_time = millis();
								get_bomb.catch_confirm_record = DISABLE;
							}
							else if(millis() - get_bomb.catch_confirm_delay_time > 125)
							{
								get_bomb.flag_push_aim_single = FINISHED;//±ê¼ÇÍê³ÉÁËÍÆ³ö¶Ô×¼Ä£Ê½
								get_bomb.catch_confirm_record = ENABLE;	
							}
						}	
						else get_bomb.catch_confirm_record = ENABLE;			
					}
					else lifter.ctrl_mode = BOMB_LIFTER_BIT;//¿ò¼ÜÌ§ÉıÒ»µã
				}					
																		
			}break;
						
			case PUSH_AIM_MULTIPLE_MODE://¶àÏäÄ£Ê½ÏÂµÄÍÆ³ö¶Ô×¼Ä£Ê½
			{					
				
				chassis.ass_mode_enable = ENABLE;                //¿ªÆô³µÎ²Ä£Ê½
				chassis.force_push_in_enable = ENABLE;           //¿ªÆôÍÆ½øÄ£Ê½
				if(vision.enable == DISABLE)
				{
					chassis.ctrl_mode = CHASSIS_SEPARATE;          //µ×ÅÌ»úĞµÄ£Ê½
				}
				
			
//				vision.enable = DISABLE;
//				vision.aim = HAVENT;				
//				get_bomb.vision_assist = 0;	
//	      flag_bomb_claw_vision = 0;                  //¹ØµôÊÓ¾õÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
				get_bomb.scan = 0;	
				get_bomb.flag_scan_failed = NO;
				optic_switch.value_record	= ENABLE;			
				get_bomb.scan_delay_record = ENABLE;	
        get_bomb.scan_finished = NO;								
		    flag_bomb_claw_scan = 0;                    //¹ØµôÉ¨ÃèÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
					
//				if(get_bomb.once_triple)
//				{
//					get_bomb.once_penta = 0;							
//					flag_bomb_claw_penta = 0;
//				}
//				else if(get_bomb.once_penta)
//				{
//					get_bomb.once_triple = 0;	
//					flag_bomb_claw_triple = 0;
//				}
				
				get_bomb.flag_push_aim_single = HAVENT;     //»¹Ã»Íê³Éµ¥ÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½	
				get_bomb.flag_push_aim_multiple = HAVENT;   //»¹Ã»Íê³ÉÈıÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½
				get_bomb.flag_push_aim_scan = HAVENT;       //»¹Ã»Íê³ÉÈıÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½

				if(get_bomb.box_num == 3 || get_bomb.box_num == 4)//ÎåÏäÖ®ÖĞµÄ×îºóµÚ¶şÏä¿ªÊ¼
				{ /*Ç¿ĞĞ±ê¼ÇÎªÍÆ³ö*/
					BOMB_CLAW_PUSH;
					flag_bomb_claw_record = flag_bomb_claw_push;
					/*ĞŞ¸ÄÆ½Ì¨×óÓÒÄ¿±êÖµ*/
					bomb_claw.target_entire[Y] = (bomb_claw.Y_target[R] + bomb_claw.Y_target[M])/2;
	        bomb_claw.target_zero[Y] = (bomb_claw.Y_target[L] + bomb_claw.Y_target[M])/2;				
				}
				
				
				switch(get_bomb.box_num)
				{
					case 0:
					{					
						BOMB_CLAW_MOVE_MID;
						get_bomb.get_pour_step = 0;//ºóÃæµÄÈ¡µ¯²½ÖèÇåÁã	
						
						if(flag_push_aim_multiple_init)
						{
							flag_push_aim_multiple_init = 0;
							flag_push_aim_single_init = 1;
							flag_push_aim_scan_init = 1;
							flag_scan_safe_delay = 0;
							
							BOMB_CLAW_LOOSE;//Ç¿ÖÆÕÅ¿ª×¦×Ó
							BOMB_CLAW_ROLL_HALF;//Ç¿ÖÆ·­×ª×¦×ÓÖÁHALF	
							
//							/*ÔÚµºÉÏµÄÊ±ºòÄ¬ÈÏÈ¡µ¯×¦×ÓÍÆ³ö µºÏÂµÄÊ±ºòÄ¬ÈÏ²»ÍÆ³ö*/					
//							if(climb_tube.location == ON_ISLAND) 
//							{
//								BOMB_CLAW_PUSH;
//							}							
//							else BOMB_CLAW_PULL;
							BOMB_CLAW_PULL;
						}
									
						if((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 100) || 
							 (abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 100))//Ì§Éıµ½Ä¿±ê½Ç¶È
						{ /*¿ò¼ÜÌ§µ½ÁËÎ»ÖÃ²ÅÖ´ĞĞÉì³ö¶Ô×¼*/							  
							kb.bomb_claw_enable = ENABLE;//È¡µ¯×¦×Ó¿É¿Ø								
						  
							if(get_bomb.once_penta) flag_bomb_claw_push = 0;//È¡ÎåÏäµÄÊ±ºòµÚÒ»ÏäÊÇ²»ÍÆ³öÈ¥µÄ
							
//							if((flag_bomb_claw_catch) && 
//								((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || 
//							   (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100)))	
              if((flag_bomb_claw_catch) && (bomb_claw.zero_correct == FINISHED))					
							{
							  flag_bomb_claw_record = flag_bomb_claw_push;//¼ÇÂ¼ÊÇ°´ÏÂÁËÍÆ³ö¼üÂğ£¿
								
								if(get_bomb.catch_confirm_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼äÈÃ×¦×Ó¼Ğ×¡
								{					
									get_bomb.catch_confirm_delay_time = millis();
									get_bomb.catch_confirm_record = DISABLE;
								}
								else if(millis() - get_bomb.catch_confirm_delay_time > 125)
								{
									get_bomb.flag_push_aim_multiple = FINISHED; //±ê¼ÇÍê³ÉÁËÍÆ³ö¶Ô×¼Ä£Ê½
								  get_bomb.catch_confirm_record = ENABLE;	
								}
						  }							
              else get_bomb.catch_confirm_record = ENABLE;							
						}
						else lifter.ctrl_mode = BOMB_LIFTER_BIT;//¿ò¼ÜÌ§ÉıÒ»µã										
					}break;
					
					case 1:
					case 3:
					{
						chassis.ctrl_mode = CHASSIS_STOP;      //µ×ÅÌËÙ¶È»·Ëø×¡
						
						kb.bomb_claw_enable = ENABLE;//È¡µ¯×¦×Ó²»¿É¿Ø

//						BOMB_CLAW_MOVE_LEFT;							
//						if(abs(bomb_claw.target_zero[Y] - bomb_claw.total_angle[Y]) < 100)//È¡µ¯×¦×ÓÅ²µ½ÁËÎ»ÖÃ
					  BOMB_CLAW_MOVE_RIGHT;							
						if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100)//È¡µ¯×¦×ÓÅ²µ½ÁËÎ»ÖÃ
						{
							BOMB_CLAW_ROLL_ZERO;
//							if((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || 
//								 (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100))//È¡µ¯×¦×Ó·­×ªµ½ÁËÎ»ÖÃ
							if(bomb_claw.zero_correct == FINISHED)
							{
								BOMB_CLAW_CATCH;                             //È¡µ¯×¦×Ó×¥È¡
								if(get_bomb.catch_confirm_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼äÈÃ×¦×Ó¼Ğ×¡
								{					
									get_bomb.catch_confirm_delay_time = millis();
									get_bomb.catch_confirm_record = DISABLE;
								}
								else if(millis() - get_bomb.catch_confirm_delay_time > 125)
								{
									get_bomb.flag_push_aim_multiple = FINISHED;  //±ê¼ÇÍê³ÉÁËÍÆ³ö¶Ô×¼Ä£Ê½
								  get_bomb.catch_confirm_record = ENABLE;
								}
							}
						}
					}break;
						
					case 2:
					case 4:
					{
						chassis.ctrl_mode = CHASSIS_STOP;      //µ×ÅÌËÙ¶È»·Ëø×¡
						
						kb.bomb_claw_enable = ENABLE;//È¡µ¯×¦×Ó²»¿É¿Ø										

						BOMB_CLAW_MOVE_LEFT;							
						if(abs(bomb_claw.target_zero[Y] - bomb_claw.total_angle[Y]) < 100)//È¡µ¯×¦×ÓÅ²µ½ÁËÎ»ÖÃ
//						BOMB_CLAW_MOVE_RIGHT;							
//						if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100)//È¡µ¯×¦×ÓÅ²µ½ÁËÎ»ÖÃ
						{
							BOMB_CLAW_ROLL_ZERO;
//							if((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || 
//								 (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100))//È¡µ¯×¦×Ó·­×ªµ½ÁËÎ»ÖÃ
							if(bomb_claw.zero_correct == FINISHED)
							{
								BOMB_CLAW_CATCH;                             //È¡µ¯×¦×Ó×¥
								if(get_bomb.catch_confirm_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼äÈÃ×¦×Ó¼Ğ×¡
								{					
									get_bomb.catch_confirm_delay_time = millis();
									get_bomb.catch_confirm_record = DISABLE;
								}
								else if(millis() - get_bomb.catch_confirm_delay_time > 125)
								{
									get_bomb.flag_push_aim_multiple = FINISHED;  //±ê¼ÇÍê³ÉÁËÍÆ³ö¶Ô×¼Ä£Ê½
							  	get_bomb.catch_confirm_record = ENABLE;
								}
							}
						}
					}break;
					
					
					default: break;					
				}											
																		
			}break;
							
			case PUSH_AIM_SCAN_MODE://É¨ÃèÄ£Ê½ÏÂµÄÍÆ³ö¶Ô×¼Ä£Ê½
			{	
				
			  chassis.ass_mode_enable = ENABLE;               //¿ªÆô³µÎ²Ä£Ê½
				chassis.force_push_in_enable = ENABLE;          //¿ªÆôÍÆ½øÄ£Ê½
				if(vision.enable == DISABLE)
				{
					chassis.ctrl_mode = CHASSIS_STOP;             //µ×ÅÌËÙ¶È»·Ëø×¡
				}
				
				
				get_bomb.once_triple = 0;							
	      flag_bomb_claw_triple = 0;           //¹ØµôÈıÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
					
				get_bomb.once_penta = 0;							
				flag_bomb_claw_penta = 0;            //¹ØµôÎåÏäÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
//				vision.enable = DISABLE;
//				vision.aim = HAVENT;
//				get_bomb.vision_assist = 0;	
//	      flag_bomb_claw_vision = 0;               //¹ØµôÊÓ¾õÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½
				
				get_bomb.flag_push_aim_single = HAVENT;     //»¹Ã»Íê³Éµ¥ÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½	
				get_bomb.flag_push_aim_multiple = HAVENT;   //»¹Ã»Íê³ÉÈıÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½
				get_bomb.flag_push_aim_scan = HAVENT;       //»¹Ã»Íê³ÉÈıÏäÄ£Ê½ÏÂÍÆ³ö¶Ô×¼Ä£Ê½
				
				get_bomb.get_pour_step = 0;         //ºóÃæµÄÈ¡µ¯²½ÖèÇåÁã	
				
				if(flag_bomb_claw_push)//Èç¹û°´shift+RÖ®Ç°×¦×ÓÊÇÉì³öÈ¥µÄ ×ö¸ö±ê¼ÇÒ»»áÑÓÊ±Ò»ÏÂ
				{
					BOMB_CLAW_PULL;//×¦×ÓÊÕ»ØÀ´
					get_bomb.scan_safe_Time = millis();//¼ÇÂ¼µ¯ÉäµÄÊ±¿Ìms
					flag_scan_safe_delay = 1;//ÑÓÊ±Ê¹ÄÜ¿ªÆğÀ´ ¸øµãÊ±¼äÎÒ×¦×ÓÉì»ØÀ´
				}
				if(flag_scan_safe_delay)
				{
					if(millis() - get_bomb.scan_safe_Time > 1000)//ms
					{
					  flag_scan_safe_delay = 0;
					}
				}
				else
				{
					if(flag_push_aim_scan_init)
					{
//					  if(servo_get_bomb_scan_delay_record)
//						{
//							servo_get_bomb_scan_delay_time = millis();
//							servo_get_bomb_scan_delay_record = 0;
//							GIM_SERVO_BACK;
//						}	
//						else if(millis() - servo_get_bomb_scan_delay_time > 500)
//						{						
							flag_push_aim_multiple_init = 1;
							flag_push_aim_single_init = 1;
							
							BOMB_CLAW_LOOSE;//Ç¿ÖÆÕÅ¿ª×¦×Ó
							BOMB_CLAW_ROLL_HALF;//Ç¿ÖÆ·­×ª×¦×ÓÖÁHALF	
							
							
							lifter.target_bomb_bit[L] = lifter.target_bomb_bit_scan[L];
							lifter.target_bomb_bit[R] = lifter.target_bomb_bit_scan[R];	
							from_lifter_zero = NO;
							from_lifter_bomb_entire = YES; //ĞŞÕı¿ò¼ÜÄ¿±êÖµ

							lifter.ctrl_mode = BOMB_LIFTER_BIT;//¿ò¼ÜÌ§ÉıÒ»µã(ĞŞ¸Ä)
							
							if(((abs(bomb_claw.total_angle[L] - bomb_claw.target_half[L]) < 100) || 
									(abs(bomb_claw.total_angle[R] - bomb_claw.target_half[R]) < 100)) && 
								 ((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 50) || 
									(abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 50)))			
							{	
								if(get_bomb.scan_delay_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼ä ÈÃÅĞ¶Ï¸ü×¼
								{
									get_bomb.scan_delay_time = millis();
									get_bomb.scan_delay_record = 0;
								}
								else if(millis() - get_bomb.scan_delay_time > 333)
								{
									if(optic_switch.value_record)//ÅĞ¶Ïí¡µ½ÄÄÀïÈ¥ÁË
									{
										optic_switch.value_start[M] = optic_switch.value[M];
										optic_switch.value_record = DISABLE;
									}												
									if(climb_tube.location == ON_ISLAND)
									{
										if(optic_switch.value_start[M])
										{
											/*í¡µ½Ïä×ÓÉÏ*/			
											if((optic_switch.value[M] == 1) && (get_bomb.scan_finished == NO)) //¼ì²âµ½Ïä×Ó
											{
												BOMB_CLAW_MOVE_RIGHT;                //Ç¿ĞĞÒÆµ½ÓÒ±ß	
												if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100)
												{
													get_bomb.flag_scan_failed = YES;  //±ê¼ÇÉ¨ÃèÊ§°
													get_bomb.scan_finished = YES;
												}													
											}
											else//µÚÒ»´Î·­×ªĞŞ¸Ätarget_mid
											{
												bomb_claw.target_half[Y] = bomb_claw.total_angle[Y] - 21000;	
												if((bomb_claw.target_half[Y] > bomb_claw.target_entire[Y]) ||
													 (bomb_claw.target_half[Y] < bomb_claw.target_zero[Y]) ||
												    get_bomb.flag_scan_failed)
												{
													bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
													get_bomb.flag_scan_failed = YES;  //±ê¼ÇÉ¨ÃèÊ§°Ü
												}	
												
												get_bomb.scan_finished = YES;       //±ê¼ÇÉ¨ÃèÍê³É
												BOMB_CLAW_MOVE_MID;                 //Ç¿ĞĞÒÆµ½ÖĞ¼ä
												flag_push_aim_scan_init = 0;
											}								
										}
										else
										{
											/*í¡µ½·ìÀïÁË*/
											if((optic_switch.value[M] == 0) && (get_bomb.scan_finished == NO))
											{
												BOMB_CLAW_MOVE_RIGHT;                //Ç¿ĞĞÒÆµ½ÓÒ±ß															
												if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100)
												{
													get_bomb.flag_scan_failed = YES;  //±ê¼ÇÉ¨ÃèÊ§°Ü
													get_bomb.scan_finished = YES;
												}												
											}
											else //µÚÒ»´Î·­×ªĞŞ¸Ätarget_mid
											{
												bomb_claw.target_half[Y] = bomb_claw.total_angle[Y] + 16000;
												if((bomb_claw.target_half[Y] > bomb_claw.target_entire[Y]) ||
													 (bomb_claw.target_half[Y] < bomb_claw.target_zero[Y]) ||
												    get_bomb.flag_scan_failed)
												{
													bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
													get_bomb.flag_scan_failed = YES;  //±ê¼ÇÉ¨ÃèÊ§°Ü
												}	
												/*ÏÂÃæÁ½ÌõÓï¾äÊÇÎªÁËËÙ¶È¿Éµ÷*/
												bomb_claw.from_claw_entire[Y_axis] = NO;
												bomb_claw.from_claw_zero[Y_axis] = YES;									

												get_bomb.scan_finished = YES;       //±ê¼ÇÉ¨ÃèÍê³É
												BOMB_CLAW_MOVE_MID;                 //Ç¿ĞĞÒÆµ½ÖĞ¼ä
												flag_push_aim_scan_init = 0;
											}						
										}
									}
									else if(climb_tube.location == ON_GROUND)
									{
										if(optic_switch.value_start[M] == 0)
										{
											/*Ö»ÓĞí¡µ½Ïä×ÓÉÏ Á½µ²°åÖ®¼ä²ÅÄÜ´¥·¢µ¥ÏäÉ¨Ãè*/			
											if((optic_switch.value[M] == 0) && (get_bomb.scan_finished == NO)) //¼ì²âµ½Ïä×Ó
											{
												BOMB_CLAW_MOVE_RIGHT;                //Ç¿ĞĞÒÆµ½ÓÒ±ß				
												if(abs(bomb_claw.target_entire[Y] - bomb_claw.total_angle[Y]) < 100)
												{
													get_bomb.flag_scan_failed = YES;  //±ê¼ÇÉ¨ÃèÊ§°Ü
													get_bomb.scan_finished = YES;
												}												
											}
											else //µÚÒ»´Î·­×ªĞŞ¸Ätarget_mid
											{
												bomb_claw.target_half[Y] = bomb_claw.total_angle[Y] - 8500;	
												if((bomb_claw.target_half[Y] > bomb_claw.target_entire[Y]) ||
													 (bomb_claw.target_half[Y] < bomb_claw.target_zero[Y]) ||
												    get_bomb.flag_scan_failed)
												{
													bomb_claw.target_half[Y] = bomb_claw.Y_target[M];
													get_bomb.flag_scan_failed = YES;  //±ê¼ÇÉ¨ÃèÊ§°Ü
												}	
												
												get_bomb.scan_finished = YES;       //±ê¼ÇÉ¨ÃèÍê³É
												BOMB_CLAW_MOVE_MID;                 //Ç¿ĞĞÒÆµ½ÖĞ¼ä
												flag_push_aim_scan_init = 0;
											}								
										}
										else get_bomb_clear_record = 1;//Æô¶¯ÍË³öÖØÀ´³ÌĞò
									}							
								}
							}							
//						}
					}	
				}
				
        if(get_bomb.scan_finished)				
				{		
		      /*ĞŞÕı¿ò¼ÜÄ¿±êÖµ*/ 
          if(lifter.target_bomb_bit[L] != lifter.target_bomb_bit_normal[L] || 
						 lifter.target_bomb_bit[R] != lifter.target_bomb_bit_normal[R])		
	       	{					
						lifter.target_bomb_bit[L] = lifter.target_bomb_bit_normal[L];
						lifter.target_bomb_bit[R] = lifter.target_bomb_bit_normal[R];
						if((lifter.total_angle[L] > lifter.target_bomb_bit[L]) && 
							 (lifter.total_angle[R] < lifter.target_bomb_bit[R]))
						{
							from_lifter_zero = YES;
							from_lifter_bomb_entire = NO; 			
						}
						else if((lifter.total_angle[L] < lifter.target_bomb_bit[L]) && 
										(lifter.total_angle[R] > lifter.target_bomb_bit[R]))
						{
							from_lifter_zero = NO;
							from_lifter_bomb_entire = YES; 			
						}
					}
					
					if((abs(lifter.target_bomb_bit[L] - lifter.total_angle[L]) < 100) || 
						 (abs(lifter.target_bomb_bit[R] - lifter.total_angle[R]) < 100))
					{
					  kb.bomb_claw_enable = ENABLE;//È¡µ¯×¦×Ó¿É¿Ø	
            GIM_SERVO_SIDE;						

						if(climb_tube.location == ON_ISLAND && !get_bomb.flag_scan_failed)
						{
							 BOMB_CLAW_PUSH;
						}
//						else BOMB_CLAW_PULL;

//						if((flag_bomb_claw_catch) && 
//						((abs(bomb_claw.target_zero[L] - bomb_claw.total_angle[L]) < 100) || 
//						 (abs(bomb_claw.target_zero[R] - bomb_claw.total_angle[R]) < 100)))							
						if((flag_bomb_claw_catch) &&(bomb_claw.zero_correct == FINISHED))
						{	
							if(get_bomb.catch_confirm_record)//ÑÓÊ±Ò»Ğ¡¶ÎÊ±¼äÈÃ×¦×Ó¼Ğ×¡
							{					
								get_bomb.catch_confirm_delay_time = millis();
								get_bomb.catch_confirm_record = DISABLE;
							}
							else if(millis() - get_bomb.catch_confirm_delay_time > 125)
							{
								get_bomb.flag_push_aim_scan = FINISHED;//±ê¼ÇÍê³ÉÁËÍÆ³ö¶Ô×¼Ä£Ê½
								get_bomb.catch_confirm_record = ENABLE;
							}
						}					
					}	
					
				}		
				
			}break;				
					
			case GET_POUR_MODE://È¡µ¯µ¹µ¯Ä£Ê½
			{		
			  chassis.ass_mode_enable = ENABLE;      //¿ªÆô³µÎ²Ä£Ê½
				chassis.force_push_in_enable = ENABLE; //¿ªÆôÍÆ½øÄ£Ê½
				chassis.ctrl_mode = CHASSIS_STOP;      //µ×ÅÌËÙ¶È»·Ëø×¡
				vision.enable = DISABLE;
				flag_bomb_claw_vision = 0;		
				
				if(get_bomb.get_pour_step == 0)//step 0
				{	 
					kb.bomb_claw_enable = DISABLE;        //È¡µ¯×¦×Ó²»¿É¿Ø	
					lifter.ctrl_mode = BOMB_LIFTER_ENTIRE;//¿ò¼ÜÌ§Éı°Î³öÏä×Ó	
					if((abs(lifter.target_bomb_entire[L] - lifter.total_angle[L]) < 100) || 
						 (abs(lifter.target_bomb_entire[R] - lifter.total_angle[R]) < 100))//Ì§Éıµ½Ä¿±ê½Ç¶È
					{ /*¿ò¼ÜÌ§µ½ÁËÎ»ÖÃ²ÅÖ´ĞĞµ¹µ¯*/
						BOMB_CLAW_PULL;                    //È¡µ¯×¦×ÓÊÕ»Ø
						/*ĞŞÕıÖĞĞÄÖµ*/	
						if(bomb_claw.target_half[Y] > bomb_claw.Y_target[M])
						{
							bomb_claw.from_claw_entire[Y_axis] = YES;
							bomb_claw.from_claw_zero[Y_axis] = NO;
						}	
						else if(bomb_claw.target_half[Y] < bomb_claw.Y_target[M])
						{
							bomb_claw.from_claw_entire[Y_axis] = NO;
							bomb_claw.from_claw_zero[Y_axis] = YES;
						}													
						bomb_claw.target_half[Y] = bomb_claw.Y_target[M];  
						
						BOMB_CLAW_MOVE_MID;                //È¡µ¯×¦×ÓÒÆµ½ÖĞ¼ä
						BOMB_CLAW_ROLL_BIT;                //È¡µ¯×¦×Ó·­×ªµ½BIT
						get_bomb.get_pour_step ++;
					}		
				}							
						
			  else if(get_bomb.get_pour_step == 1)//step 1
				{
//					if((abs(bomb_claw.target_half[L] - bomb_claw.total_angle[L]) < 100) || 
//						 (abs(bomb_claw.target_half[R] - bomb_claw.total_angle[R]) < 100))							
//					{
					lifter.ctrl_mode = BOMB_LIFTER_BIT;//¿ò¼Ü»Øµ½È¡µ¯ÄÇ¸öÎ»ÖÃ							
//					}
							
					if((abs(bomb_claw.target_bit[L] - bomb_claw.total_angle[L]) < 100) || 
						 (abs(bomb_claw.target_bit[R] - bomb_claw.total_angle[R]) < 100))							
					{										
//						BOMB_CLAW_LOOSE;                    //È¡µ¯×¦×ÓËÉ¿ª
						get_bomb.claw_loose_Time = millis();//¼ÇÂ¼×¦×ÓËÉ¿ªµÄÊ±¿Ìms
						get_bomb.get_pour_step ++;
					}								
				}
						
				else if(get_bomb.get_pour_step == 2)//step 2
				{
					if((millis() - get_bomb.claw_loose_Time > 200) && 
						 (millis() - get_bomb.claw_loose_Time < 350))//ms
					{
						BOMB_CLAW_LOOSE;                    //È¡µ¯×¦×ÓËÉ¿ª
					}
					else if((millis() - get_bomb.claw_loose_Time > 350) && 
						      (millis() - get_bomb.claw_loose_Time < 500))//ms
					{
						BOMB_CLAW_ROLL_HALF;                //
					}					
					else if(millis() - get_bomb.claw_loose_Time > 500)//ms
					{
						BOMB_EJECT_PUSH;                    //µ¯ÉäÍÆ³ö
						get_bomb.eject_push_Time = millis();//¼ÇÂ¼µ¯ÉäµÄÊ±¿Ìms
						get_bomb.get_pour_step ++;
					}
				}
						
				else if(get_bomb.get_pour_step == 3)//step 3
				{
					if(millis() - get_bomb.eject_push_Time > 300)//ms
					{
						get_bomb.get_pour_step = 0;//È¡µ¯²½ÖèÇåÁã		
						get_bomb.have_finished_once_get_bomb = YES;
						
						get_bomb.flag_push_aim_single = HAVENT;    //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½											
						get_bomb.flag_push_aim_multiple = HAVENT;  //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½	
						get_bomb.flag_push_aim_scan = HAVENT;      //»¹Ã»Íê³ÉÍÆ³ö¶Ô×¼Ä£Ê½		
						
						BOMB_EJECT_PULL;        //µ¯ÉäÊÕ»Ø								
						flag_bomb_claw_roll = 0;//Ê¹ÍË³öºóÔÙ½øÈëPUH_AIMºóENABLEÊ±×¦×ÓÄÜ×Ô¶¯HALF
								
						/*ÔÚµºÉÏµÄÊ±ºòÄ¬ÈÏÈ¡µ¯×¦×ÓÍÆ³ö µºÏÂµÄÊ±ºòÄ¬ÈÏ²»ÍÆ³ö*/
//						if(((climb_tube.location == ON_ISLAND) || (flag_bomb_claw_record  == 1)) && (get_bomb.box_num != 2))//µºÉÏ»òÊÇ¼ÇÂ¼ÁËÍÆ³ö
						if(flag_bomb_claw_record  == 1 && get_bomb.box_num != 2)//µºÉÏ»òÊÇ¼ÇÂ¼ÁËÍÆ³ö
						{
							flag_bomb_claw_push = 1;		
						}				
						else//µºÏÂ 
						{
							flag_bomb_claw_push = 0;//µ¥ÏäÄ£Ê½ÏÂÄ¬ÈÏ²»ÍÆ³ö	
              flag_bomb_claw_record  = 0;							
						}
						
						
						if(get_bomb.once_triple)/*ÈıÏä¶¼È¡ÍêÖ®ºóÔÙ¹ØµôÈıÏäÄ£Ê½*/
						{							
							if(get_bomb.box_num < 2) 
							{
								get_bomb.box_num ++;
							}
							else 
							{
								get_bomb.box_num = 0;	
								
                get_bomb.once_triple = 0;	
                flag_bomb_claw_triple = 0; //¹ØµôÈıÏäÄ£Ê½								
							}						
						}		
						else if(get_bomb.once_penta)/*ÎåÏä¶¼È¡ÍêÖ®ºóÔÙ¹ØµôÎåÏäÄ£Ê½*/
						{							
							if(get_bomb.box_num < 4) 
							{
								get_bomb.box_num ++;
							}
							else 
							{
								get_bomb.box_num = 0;	
								
                get_bomb.once_penta = 0;	
                flag_bomb_claw_penta = 0; //¹ØµôÎåÏäÄ£Ê½		

								flag_bomb_claw_push = 0;//µ¥ÏäÄ£Ê½ÏÂÄ¬ÈÏ²»ÍÆ³ö	
								flag_bomb_claw_record  = 0;									
							}						
						}	
						
            /*¹ØÓÚÉ¨ÃèµÄÒ»Ğ©¶«Î÷*/						
//						if(get_bomb.scan)
//						{		
							get_bomb.scan = 0;	
						  get_bomb.flag_scan_failed = NO;
						  optic_switch.value_record	= ENABLE;	
              get_bomb.scan_delay_record = ENABLE;							
	            get_bomb.scan_finished = NO;	
              flag_bomb_claw_scan = 0; //¹ØµôÉ¨ÃèÄ£Ê½		
//						}
						
					}					
				}
					
			}break;
				
//			case VISION_ASSIST_MODE://ÊÓ¾õ¶ÔÎ»Ä£Ê½
//			{								
//				vision.enable = ENABLE;
//				if(vision.aim == FINISHED)
//				{
//					vision.enable = DISABLE;
//					vision.aim = HAVENT;
//					get_bomb.vision_assist = 0;	
//					flag_bomb_claw_vision = 0;               //¹ØµôÊÓ¾õÄ£Ê½ ÈÃÏÂ´Î°´ctrl+R Ä¬ÈÏÊÇµ¥ÏäÄ£Ê½		
//				}	

//			}break;			
					
			case SUSPEND_MODE://×èÈûÌ¬Ä£Ê½
			{	
        //do nothing 				
			}break;
			default: break;		
		}		
	}
	
}

void get_bomb_task(void)
{
//	if(bomb_claw.ALL_reset)
//	{
		get_bomb_ctrl();
//	}
}


