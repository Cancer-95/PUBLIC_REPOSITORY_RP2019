#include "shoot_ctrl.h"

Tri_moto_t Tri_Moto;
//Fri_whee_t Fri_Whee;

/*反卡弹处理标志位*/
bool flag_tri_stall;
/*
**说明:反卡弹程序只写了*遥控器部分* 效果及格 不过我相信还是有Bug的 相关代码注释后加*****
**卡弹程序的加入实现的效果：
**点射：
**1.拨盘堵转能反转一段时间
**2.反转为开环(或速度环)一段时间后泻力(输出为0)这个时候转子手动转一下感觉是没有力的
**3.狂点发射 目标值能及时反应不叠加 不会在判断堵转标志为产生那一段时间内点太快而产生目标值叠加 具体做法是一有堵转标志位就减掉当前位置与目标位置(这时候可能咱点太快叠加了好些值)中n*格数大小(n取最大值)
**4.不管这个反转停的位置在哪 亦或是我们人手动将拨盘乱掰至任意位置 这时我们再点发射 拨盘的效果是以位置环的形式转动到离当前位置最近的一格的下一格(距离最近的一格的大小往往是一格以内)
**连射:
**1.速度环转 停的位置是格数
**2.遇到堵转反向开环转动一段时间继续速度环连续转 也就是发射不松手 堵也能自己处理
*/

void Shoot_Init()
{
	PIDCKZ_param_clear(&pid_trigger_position);
	PIDCKZ_param_clear(&pid_trigger_speed);
	
	PIDCKZ_param_set(&pid_trigger_position, 8500, 1000, 6.5f, 0.0f, 0.0f);            //拨盘电机位置环
	PIDCKZ_param_set(&pid_trigger_speed, 8500, 1000, 15.0f, 0.0f, 0.0f);	            //拨盘电机速度环
}

void Tri_Spe_Ctrl(int16_t speed)//For running shoot
{
	Tri_Moto.speed_rpm=Trigger_Motor[TRI_SPEED];                                      //当前速度读取
	
	if(!flag_tri_stall) 
	{
		PIDCKZ_Calc(&pid_trigger_speed, Tri_Moto.speed_rpm, speed);    //PID计算(在卡弹标志位开启时不执行 而跑其他数值)*****
	}
	
	Block_Bullet_Handle();                                                            //卡弹的时候强行插入这句话强行反转一段时间
	CAN2_Send(0X1FF,pid_trigger_speed.Out);                                           //速度环PID输出
}

void Tri_Cas_Ctrl(u32 Target_Angle)//For single shoot
{
  Tri_Data_Handle();                                                                //总角度值处理及其读取
	PIDCKZ_Calc(&pid_trigger_position, Tri_Moto.total_angle, Target_Angle);           //外环(位置环)PID计算
	Tri_Spe_Ctrl(pid_trigger_position.Out);                                           //外环输出做输入经速度环PID输出
}

/*
遥控器模式下：S2上拨点射 置中无动作 上拨停留一段时间连射
键盘模式：鼠标左键短按点射 长按超过400ms触发连射
*/
u32 tri_target_angle;//电机的target
u16 speed_stop_offset;//速度环停下时存在的偏差
u16 speed_shoot_count;//触发速度环的计数
	static bool flag_speed_shoot;//触发速度环的flag
void Trigger_Remote_Control()
{
	static bool flag_single_shot;//保证点射的flag
	static bool flag_feed_offset;//是否进行位置补偿的flag

	
	Tri_Data_Handle();//这个函数原本放在Tri_Cas_Ctrl()中最后还是把它放在所有程序开头
	if(RC_Ctl.rc.s1 == RC_SW_DOWN)//键盘模式
	{
		if(MOUSE_L_PRESSED)//鼠标左键按下
		{
			speed_shoot_count ++;//触发速度环计数
			if(speed_shoot_count > 420)//420ms
			{
				flag_speed_shoot = 1;//速度环trigger置1触发速度环
				speed_shoot_count = 0;//计数清零
			}	
			
			flag_feed_offset = 0;   //点射就不需要补偿了
			if(flag_single_shot)		//让下面的目标值的累加只执行一次
			{	
				Shooter.Shoot_Time1 = millis();//记录按下的时间便于与裁判系统记录的Time2相减得出发射延时
				tri_target_angle += Bomb_Angle;	//点射固定Target值
				flag_single_shot = 0;//让上面的目标值的累加只执行一次
			}		
		}
		else if(MOUSE_L_NOT_PRESSED)//松掉鼠标左键
		{
			flag_speed_shoot = 0;//速度环trigger清零
			speed_shoot_count = 0;//计数清零
			flag_single_shot = 1;
			if(flag_feed_offset)     //位置补偿
			{
				tri_target_angle = Tri_Moto.total_angle;//这句话很重要 放的地方也很重要 用于连射模式下记录拨盘位置 不能置于下面if连射模式中
				speed_stop_offset = Tri_Moto.total_angle % Bomb_Angle;//计算余数
				tri_target_angle += (Bomb_Angle - speed_stop_offset);//target值补偿
				flag_feed_offset = 0;//这个if只执行一次
			}
		}
		if(flag_speed_shoot)//键盘模式下触发了连射
		{
	  	flag_feed_offset = 1;         //连射时拨杆回中需要补偿
	  	Tri_Spe_Ctrl(3000);           //给定速度环连射
		}
		else Tri_Cas_Ctrl(tri_target_angle);	//除连射外执行串级
	}
	
	else//非键盘模式
	{
		switch(RC_Ctl.rc.s2)
		{
	/*S2拨杆上拨*/
			case RC_SW_UP:
			{
					speed_shoot_count ++;//触发速度环计数
					if(speed_shoot_count > 420)//420ms
					{
						flag_speed_shoot = 1;//速度环trigger置1触发速度环
						speed_shoot_count = 0;//计数清零
					}				
					flag_feed_offset = 0;  //点射就不需要补偿了

					if(flag_single_shot)		//让下面的目标值的累加只执行一次
					{			
		        if(flag_tri_stall) 
						{
							tri_target_angle -= (tri_target_angle - Tri_Moto.total_angle) / Bomb_Angle * Bomb_Angle;						
						}
						/*上面这句话是为了减掉所有误点而使目标值叠加上的值 若将其至于下面累加那句话的后面 效果将会是 电机泻力之后再拨拨杆会就近跑到最近的一格 而不是最近一格的下一格******/
						
						Shooter.Shoot_Time1 = millis();//记录按下的时间便于与裁判系统记录的Time2相减得出发射延时
						tri_target_angle += Bomb_Angle;	//点射固定Target值
						flag_single_shot = 0;//让上面的目标值的累加只执行一次	
					}				
					flag_tri_stall = 0;//老实说这个标志位的清零不知道为什么放这里*****
			}break;
	/*S2拨杆回中*/				 
			case RC_SW_MID:
			{
				flag_speed_shoot = 0;//速度环trigger清零
			  speed_shoot_count = 0;//计数清零
				flag_single_shot = 1;
				if(flag_feed_offset)     //位置补偿
				{
					tri_target_angle = Tri_Moto.total_angle;//这句话很重要 放的地方也很重要 用于连射模式下记录拨盘位置 不能置于下面if连射模式中
					speed_stop_offset = Tri_Moto.total_angle % Bomb_Angle;//计算余数
					tri_target_angle += (Bomb_Angle - speed_stop_offset);//target值补偿
					flag_feed_offset = 0;
				}
			}break;
		
			default:					
			break;//Default do nothing
		}
	/*连射标志位生效*/
		if(flag_speed_shoot)
		{
	  	flag_feed_offset = 1;         //连射时拨杆回中需要补偿
	  	flag_tri_stall= 0;            //让后面的速度环中的PID计算能够执行*****
			Tri_Spe_Ctrl(3000);           //给定速度环连射
		}
		else Tri_Cas_Ctrl(tri_target_angle);	//除连射外执行串级

	}
		
}

void Block_Bullet_Handle(void)//卡弹处理
{
  static bool stall_flag = 0;
	static uint32_t stall_count = 0;
	static uint32_t stall_inv_count = 0;
			
  if (pid_trigger_speed.Out >= 7500 && abs(Trigger_Motor[TRI_SPEED]) < 20 )
  {
    if (stall_flag == 0) stall_count ++;
  }
  else stall_count = 0;
  
  if (stall_count >= 250)          //0.25s
  {
    stall_flag = 1;
    stall_count = 0;	
		flag_tri_stall = 1;//卡弹标志位在堵转一定时间内开启 清零则是在拨杆上拨之时*****（保险到处都加）
  }
  
  if (stall_flag == 1)
  {
    stall_inv_count++;
		flag_tri_stall = 1;//卡弹标志位在堵转一定时间内开启 清零则是在拨杆上拨之时*****（保险到处都加）
    if (stall_inv_count >= 250)   //0.25s
    {
      stall_flag = 0;
      stall_inv_count = 0;
			pid_trigger_speed.Out = 0;
    } 
		else if(flag_speed_shoot) 
		{
			PIDCKZ_Calc(&pid_trigger_speed, Tri_Moto.speed_rpm, -1500);     //连射时的反转为速度环(当然了这俩可以一样的反转方式)
//			pid_trigger_speed.Out = -1500;
		}
		else pid_trigger_speed.Out = -500;                                //点射时的反转为开环(当然了这俩可以一样的反转方式)
  }

}


int16_t Open_Angle =  60;//舵机开盖角度
int16_t Close_Angle = 27;//舵机关闭角度
int16_t Close_Count = 0;//关闭计时
int16_t BBox_Open_Flag = 0;
int16_t Reload_Flag = 0;
void Arsenal_Control()//弹仓开合控制
{
	if((RC_Ctl.rc.s1 == RC_SW_UP && RC_Ctl.rc.s2 == RC_SW_UP && Reload_Flag == 0) || KEY_R_PRESSED)//S1S2拨杆都置于上才触发//键盘模式松手后才开始计时2s后关闭
	{			
		Reload_Flag = 1;//确保Flag以及计数只赋值一次
		BBox_Open_Flag = 1;
    Close_Count = 0;		
	}
	else if(BBox_Open_Flag)
	{
		/*几秒内云台强制水平+开弹仓盖*/
		Pitch_Target[MEC] = Pitch_M_MID;
		Pitch_Target[GYO] = Pitch_G_MID;
		Close_Count ++;
			  
		if((abs(Motor_angle[PIT] - Pitch_M_MID) <= 100)||(abs(imu.pit) <= 10)) Servo_Ctrl(Open_Angle);//约束条件中不加陀螺仪那个不知道为什么陀螺仪模式下抬头到水平不能开弹仓
			
		if(Close_Count > 2000) BBox_Open_Flag = 0;
	}
	else Servo_Ctrl(Close_Angle);
	
	if(RC_Ctl.rc.s2 != RC_SW_UP) Reload_Flag = 0;
	

//int16_t Servo_Ramp = 2;//舵机斜坡坡度
//int16_t Servo_Aim;//舵机目标
//int16_t Servo_Real;//舵机实时

//if(Servo_Real < Servo_Aim)//斜坡函数 Cancer表示并不知道这个斜坡函数是用来干什么的
//{
//	Servo_Real += Servo_Ramp;
//	Servo_Real = constrain(Servo_Real, Servo_Real, Servo_Aim);
//}
//else if(Servo_Real > Servo_Aim)
//{
//	Servo_Real -= Servo_Ramp;
//	Servo_Real = constrain(Servo_Real, Servo_Real, Servo_Aim);
//}	
//	Servo_Ctrl(Servo_Real);
}




bool Trigger_ecd_Record = 1;
void Tri_Data_Handle()//该函数最好置于最短的时间戳 以防记不到圈数 置于2ms都会出问题
{
	if(Trigger_ecd_Record)
	{
		Trigger_ecd_Record = 0;
		Tri_Moto.offset_ecd = Trigger_Motor[TRI_ANGLE];  //只记录一次初始偏差
	}
	
	Tri_Moto.las_ecd = Tri_Moto.cur_ecd;
	Tri_Moto.cur_ecd = Trigger_Motor[TRI_ANGLE];
	
	if(Tri_Moto.cur_ecd - Tri_Moto.las_ecd > 4096)
	{
		Tri_Moto.round_cnt--;
	}
	else if(Tri_Moto.cur_ecd - Tri_Moto.las_ecd < -4096)
	{
		Tri_Moto.round_cnt++;
	}
	
	Tri_Moto.total_ecd = Tri_Moto.round_cnt * 8192 + Tri_Moto.cur_ecd - Tri_Moto.offset_ecd;
  Tri_Moto.total_angle = Tri_Moto.total_ecd / ENCODER_ANGLE_RATIO;//度数换算 最后出来的东西是转子的转动总角度
}
 

void Fric_Ctrl(uint16_t pwm_L, uint16_t pwm_R)//摩擦轮的控制
{
	FRIC_L_PWM = 1000 + pwm_L;
	FRIC_R_PWM = 1000 + pwm_R;
}

void Servo_Ctrl( int16_t pwm )//弹仓舵机的控制
{
	  pwm = abs(pwm);

	  SERVO_PWM = pwm;
}

int16_t fric_pwm_set=620;
int16_t fric_pwm = 0;
void Shoot_Remote_Ctrl()
{
	Arsenal_Control();
	switch(fric_mode)
	{
		case FRIC_WHEEL_RUN://摩擦轮开
		{	
			if(fric_pwm < fric_pwm_set)//斜坡PWM启动摩擦轮
			{
				fric_pwm++;
			}		
			Fric_Ctrl((int16_t)fric_pwm,(int16_t)fric_pwm);
			
			Trigger_Remote_Control();	//拨盘的控制 只有在摩擦轮开的时候才有！
		}break;

		case FRIC_WHEEL_OFF://摩擦轮关
		{	
			fric_pwm = 0;//清零
			Fric_Ctrl((int16_t)fric_pwm,(int16_t)fric_pwm);
			pid_trigger_position.Out = 0;
			pid_trigger_speed.Out = 0;

		}break;
		
		default://全不输出																																															
		{
			fric_pwm = 0;
			Fric_Ctrl((int16_t)fric_pwm,(int16_t)fric_pwm);
			pid_trigger_position.Out = 0;
			pid_trigger_speed.Out = 0;
		}break;
	}
}


////u32 Target_Temp;
//void Tri_Ctrl_Position(u32 Target_Angle)//位置环(外环)计算以及输出
//{
//	Tri_Data_Handle();
//	
//	/*这是一个尝试单级位置环控制拨盘电机写的谐波函数 事实证明没有用的放弃了*/
////	if((Target_Temp < Target_Angle)&&(Target_Temp - Tri_Moto.total_angle) <= (13870 / ENCODER_ANGLE_RATIO)) 
////	{
////		Target_Temp += (1387 / ENCODER_ANGLE_RATIO);
////		if(Target_Temp > Target_Angle) Target_Temp = Target_Angle;
////	}	
////	else if((Target_Temp > Target_Angle)&&(Target_Temp - Tri_Moto.total_angle) >= (-13870 / ENCODER_ANGLE_RATIO))
////	{
////		Target_Temp -= (1387 / ENCODER_ANGLE_RATIO);
////		if(Target_Temp < Target_Angle) Target_Temp = Target_Angle;
////	}	
//	
//	PIDCKZ_Calc(&pid_trigger_position, Tri_Moto.total_angle, Target_Angle);
//	Tri_Ctrl_Speed(pid_trigger_position.Out);
//}
//void Tri_Ctrl_Speed(int16_t Target_Speed)//速度环(内环)
//{
//	Tri_Moto.speed_rpm=Trigger_Motor[TRI_SPEED];
//	PIDCKZ_Calc(&pid_trigger_speed, Tri_Moto.speed_rpm, Target_Speed);    //PID计算
//	CAN2_Send(0X1FF,pid_trigger_speed.Out);
//}
