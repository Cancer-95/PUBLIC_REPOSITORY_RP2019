#ifndef __RELAY_H__
#define __RELAY_H__

#include "system.h"

/*抱杆爪子控制*/
#define TUBE_CLAW_CATCH                          \
{                                                \
  GPIO_SetBits(TUBE_CLAW_CL_GPIO_Port,           \
	             TUBE_CLAW_CL_Pin);                \
	GPIO_SetBits(TUBE_CLAW_CL_BACKUP_GPIO_Port,    \
               TUBE_CLAW_CL_BACKUP_Pin);         \
	flag_tube_claw_catch = 1;                      \
}                                                \

#define TUBE_CLAW_LOOSE                          \
{                                                \
	GPIO_ResetBits(TUBE_CLAW_CL_GPIO_Port,         \
	               TUBE_CLAW_CL_Pin);              \
	GPIO_ResetBits(TUBE_CLAW_CL_BACKUP_GPIO_Port,  \
	               TUBE_CLAW_CL_BACKUP_Pin);       \
	flag_tube_claw_catch = 0;                      \
}                                                \

/*取弹爪子抓取控制*/
#define BOMB_CLAW_CATCH                          \
{                                                \
  GPIO_ResetBits(BOMB_CLAW_CL_GPIO_Port,         \
	               BOMB_CLAW_CL_Pin);              \
	GPIO_ResetBits(BOMB_CLAW_CL_BACKUP_GPIO_Port,  \
	               BOMB_CLAW_CL_BACKUP_Pin);       \
	flag_bomb_claw_catch = 1;                      \
}                                                \

#define BOMB_CLAW_LOOSE                          \
{                                                \
	GPIO_SetBits(BOMB_CLAW_CL_GPIO_Port,           \
	             BOMB_CLAW_CL_Pin);                \
	GPIO_SetBits(BOMB_CLAW_CL_BACKUP_GPIO_Port,    \
	             BOMB_CLAW_CL_BACKUP_Pin);         \
	flag_bomb_claw_catch = 0;                      \
}                                                \

/*弹射控制*/
#define BOMB_EJECT_PUSH         	               \
{                               	               \
	GPIO_SetBits(BOMB_EJECT_PP_GPIO_Port,          \
	             BOMB_EJECT_PP_Pin);               \
	GPIO_SetBits(BOMB_EJECT_PP_BACKUP_GPIO_Port,   \
	             BOMB_EJECT_PP_BACKUP_Pin);        \
}                               	               \

#define BOMB_EJECT_PULL                          \
{                                                \
	GPIO_ResetBits(BOMB_EJECT_PP_GPIO_Port,        \
	               BOMB_EJECT_PP_Pin);             \
	GPIO_ResetBits(BOMB_EJECT_PP_BACKUP_GPIO_Port, \
	               BOMB_EJECT_PP_BACKUP_Pin);      \
}                                                \

/*取弹爪子推拉控制*/
#define BOMB_CLAW_PUSH                           \
{                                                \
  GPIO_SetBits(BOMB_CLAW_PP_GPIO_Port,           \
	             BOMB_CLAW_PP_Pin);                \
	GPIO_SetBits(BOMB_CLAW_PP_BACKUP_GPIO_Port,    \
	             BOMB_CLAW_PP_BACKUP_Pin);         \
	flag_bomb_claw_push = 1;                       \
}                                                \

#define BOMB_CLAW_PULL                           \
{                                                \
	GPIO_ResetBits(BOMB_CLAW_PP_GPIO_Port,         \
	               BOMB_CLAW_PP_Pin);              \
	GPIO_ResetBits(BOMB_CLAW_PP_BACKUP_GPIO_Port,  \
	               BOMB_CLAW_PP_BACKUP_Pin);       \
	flag_bomb_claw_push = 0;        						   \
}                                                \

/*拖车钩子上下控制*/
#define TRAILER_HOOK_PUSH                        \
{                                                \
  GPIO_SetBits(TRAILER_HOOK_PP_GPIO_Port,        \
	             TRAILER_HOOK_PP_Pin);             \
	GPIO_SetBits(TRAILER_HOOK_PP_BACKUP_GPIO_Port, \
	             TRAILER_HOOK_PP_BACKUP_Pin);      \
	flag_trailer_hook = 1;                         \
}                                                \

#define TRAILER_HOOK_PULL                          \
{                                                  \
	GPIO_ResetBits(TRAILER_HOOK_PP_GPIO_Port,        \
	               TRAILER_HOOK_PP_Pin);             \
	GPIO_ResetBits(TRAILER_HOOK_PP_BACKUP_GPIO_Port, \
	               TRAILER_HOOK_PP_BACKUP_Pin);      \
	flag_trailer_hook = 0;                           \
}                                                  \


/*视野切换控制*/
#define CCTV_FEED_RESCUE                           \
{                                                  \
	flag_mirror_CCTV = 0;                            \
	GPIO_SetBits(THE_THIRD_EYE_GPIO_Port,            \
	             THE_THIRD_EYE_Pin);                 \
	GPIO_SetBits(THE_THIRD_EYE_BACKUP_GPIO_Port,     \
	             THE_THIRD_EYE_BACKUP_Pin);          \
	GPIO_ResetBits(CCTV_SWITCH_GPIO_Port,            \
	               CCTV_SWITCH_Pin);                 \
	GPIO_ResetBits(CCTV_SWITCH_BACKUP_GPIO_Port,     \
	               CCTV_SWITCH_BACKUP_Pin);          \
}

#define CCTV_GET_BOMB                             \
{                                                 \
  flag_mirror_CCTV =1;	                          \
	GPIO_SetBits(CCTV_SWITCH_GPIO_Port,             \
	             CCTV_SWITCH_Pin);                  \
	GPIO_SetBits(CCTV_SWITCH_BACKUP_GPIO_Port,      \
	             CCTV_SWITCH_BACKUP_Pin);           \
	GPIO_ResetBits(THE_THIRD_EYE_GPIO_Port,         \
	               THE_THIRD_EYE_Pin);              \
	GPIO_ResetBits(THE_THIRD_EYE_BACKUP_GPIO_Port,  \
	               THE_THIRD_EYE_BACKUP_Pin);       \
}

#define CCTV_CLIMB_TUBE                          \
{                                                \
	flag_mirror_CCTV =1;	                         \
  GPIO_ResetBits(CCTV_SWITCH_GPIO_Port,          \
	               CCTV_SWITCH_Pin);               \
	GPIO_ResetBits(CCTV_SWITCH_BACKUP_GPIO_Port,   \
	               CCTV_SWITCH_BACKUP_Pin);        \
	GPIO_ResetBits(THE_THIRD_EYE_GPIO_Port,        \
	               THE_THIRD_EYE_Pin);             \
	GPIO_ResetBits(THE_THIRD_EYE_BACKUP_GPIO_Port, \
	               THE_THIRD_EYE_BACKUP_Pin);      \
}  

extern bool flag_mirror_CCTV;//镜像摄像头标记
extern bool flag_bomb_claw_CCTV_switch;

void relay_init(void);
void CCTV_switch_task(void);

#endif 
