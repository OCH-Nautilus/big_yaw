/*******************************************************************
 * File Name   : Steering_UI
 * Description : 舵轮步兵UI的使用，基于孙老师UI
 * Author      : 刘嘉诚
 * QQ          ：
 * Telephone   : 
********************************************************************
 *
 * Copyright (c) 2022 Nautilus - Wuhan Institute of Technology
 * All rights reserved.
 *

ps: RoboMaster_裁判系统串口协议附录 V1.3
	频率最大为10Hz
*******************************************************************/
#include "Steering_UI.h"
#include "bsp_can.h"
#include "referee.h"
#include "ins_task.h"
#include "cmsis_os.h"
#include "chassis_task.h"
#include "big_gimbal_task.h"
#include "config.h"
#include "CAN_receive.h"
#include "bsp_dwt.h"
#include "bsp_transmit.h"
#include "SuperCAP.h"
void UI_ID_Set(void)
{
	Robot_ID = robot_status.robot_id;
	Cilent_ID = Robot_ID + 0x0100;
}
/******************************************UI修改添加层**********************************/

void UI_chassis_crash_add(void);
void UI_state_add(void);
void UI_chassis_add(void);
void UI_vision_add(void);
void UI_shoot_add(void);
void UI_supercap_add(void);
void UI_state_update(void);
void UI_chassis_update(void);
void UI_vision_update(void);
void UI_shoot_update(void);
void UI_supercap_update(void);
void UI_bullet_add(void);
void UI_bullet_update(void);
void UI_target_add(void);
void UI_target_update(void);
//void UI_Distance_add(void);
//void UI_Distance_update(void);
void UI_Cap_add(void);
void UI_Cap_update(void);
void UI_Pitch_add(void);
void UI_Pitch_update(void);
//void UI_direction_update(void);
void UI_energy_add(void);
void UI_energy_update(void);
extern int32_t motor_current_time[4];

uint32_t UI_time = 0;
float UI_delta = 0;



int qwert=0;
void UI_Task(void)
{
	for(;;)   
	{	
	qwert++;
		UI_ID_Set();
		if(USART_Rx_data.key.bits.Key_G== 1)
		{		
			UI_chassis_crash_add();
			UI_state_add();
			UI_chassis_add();
			UI_vision_add();
			UI_shoot_add();
			UI_supercap_add();
			UI_bullet_add();
			UI_target_add();
//      UI_Distance_add();
			UI_Pitch_add();
			UI_energy_add();
			UI_Cap_add();
			UI_trajectory_add();
			
	UI_SendChars(&state_1);
	osDelay (35);
	UI_SendChars(&state_2);
	osDelay (35);
	UI_SendChars(&state_3);
	osDelay (35);
	UI_SendChars(&state_4);
	osDelay (35);
	UI_SendChars(&state_5);
	osDelay (35);
//	UI_SendGraph( 5,SC_Outline_Arc_1,SC_Outline_Arc_2,SC_Outline_Line_1,SC_Outline_Line_2,SC_Vol_Arc);
//	osDelay (40);	
//	
//	UI_SendGraph(7,direction_1, direction_2,direction_3,bullet_1,bullet_2 ,energy_1,chassis_crash_left);
//	osDelay (40);

	UI_SendGraph( 7,chassis_1,chassis_2,chassis_3,chassis_4,chassis_5,chassis_6,bullet_1);
	osDelay (40);
//	
	UI_SendGraph( 7,vision_1,vision_2,vision_3,vision_4,vision_5,vision_6,vision_7);
	osDelay (40);
	UI_SendGraph( 5,SC_Vol_Arc,trajectory_1m_1,trajectory_1m_2,trajectory_1m_3,trajectory_1m_4);
	osDelay (40);	
	UI_SendGraph(5,trajectory_3m_1,trajectory_3m_2,trajectory_3m_3,trajectory_3m_4,trajectory_1m_1);
	osDelay (40);	
	UI_SendGraph(1,chassis_crash_left);
	osDelay (40);	
	UI_SendGraph(1,chassis_crash_right);
	osDelay (40);	
//	UI_SendGraph( 7,target_1,target_2,target_3,target_4,target_5,target_6,target_7);
//	osDelay (40);
//	
////	UI_SendGraph( 7,shoot_1,shoot_2,shoot_3,shoot_4,shoot_5,target_8,Distance);
	UI_SendGraph( 5,shoot_1,shoot_2,shoot_3,shoot_4,shoot_5);
	osDelay (40);
//	
//	UI_SendGraph( 7,supercap_1,supercap_2,supercap_3,supercap_4,bullet_4,chassis_crash_right);
//	osDelay (40);
	

//	
		} else {
			UI_state_update();
			UI_chassis_update();
			UI_vision_update();
			UI_shoot_update();
//			UI_supercap_update();
//			UI_bullet_update();
////		  UI_Distance_update();
			UI_Pitch_update();
////			UI_direction_update();
//			UI_energy_update();
		  UI_Cap_update();
			UI_trajectory_update();
//			UI_target_update();
//		 Sentry_cmd_update();
	UI_SendChars(&state_1);
	osDelay (35);
	UI_SendChars(&state_2);
	osDelay (35);
	UI_SendChars(&state_3);
	osDelay (35);
	UI_SendChars(&state_4);
	osDelay (35);		
	UI_SendChars(&state_5);
	osDelay (35);		
	UI_SendGraph( 7,chassis_1,chassis_2,chassis_3,chassis_4,chassis_5,chassis_6,bullet_1);
	osDelay (40);	
	UI_SendGraph( 7,shoot_1,shoot_2,shoot_3,shoot_4,shoot_5,Pitch_Angle_1,SC_Vol_Arc);
	osDelay (40);
	UI_SendGraph( 7,vision_1,vision_2,vision_3,vision_4,vision_5,vision_6,vision_7);
	osDelay (40);	
	UI_SendGraph(5,trajectory_3m_1,trajectory_3m_2,trajectory_3m_3,trajectory_3m_4,trajectory_1m_1);
	osDelay (40);		
	UI_SendGraph(5,SC_Vol_Arc,trajectory_1m_1,trajectory_1m_2,trajectory_1m_3,trajectory_1m_4);
	osDelay (40);	

//	UI_SendGraph( 7,shoot_1,shoot_2,shoot_3,shoot_4,shoot_5,direction_3,SC_Vol_Arc);
//	osDelay (40);		
//				
//	UI_SendGraph( 7,bullet_1,bullet_2,bullet_3,bullet_4,target_1,target_2,target_3);
//	osDelay (40);	

////	UI_SendGraph( 7,supercap_1,supercap_2,supercap_3,supercap_4,supercap_4,Distance,target_4);
//	UI_SendGraph( 7,supercap_1,supercap_2,supercap_3,supercap_4,supercap_4,supercap_4,target_4);
//	osDelay (40);
//  UI_SendSentry(&Sentry_cmd);
//	osDelay (40);
		}
	  	UI_delta = DWT_GetDeltaT(&UI_time);
	}
}

//int asds=0;
//Sentry_cmd_t Sentry_cmd;
//void Sentry_cmd_update()
//{
//	if(rc_ctrl.keyboard.flag_V)
//	{
//		asds++;
//	 Sentry_Cmd_Fill(&Sentry_cmd,0 , 0, 50, 0);
//	}

//}


/**
* @brief 根据裁判系统信息变更ui发送ID
* @note  Change send ID
* @param 
*/
//void UI_ID_Set(void)
//{
//	Robot_ID = robot_status.robot_id;
//	Cilent_ID = Robot_ID + 0x0100;
//}

//底盘防撞UI，图层1
Graph_Data_t chassis_crash_left;
Graph_Data_t chassis_crash_right;
int uuii=0;
void UI_chassis_crash_add(void)
{
uuii++;
	Line_Draw( &chassis_crash_left, "99", 1, 1, 3, 3, 212, 35, 594, 365 );
	Line_Draw( &chassis_crash_right, "100", 1, 1, 3, 3, 1337, 342, 1738, 24 );
}

char *C_state="C";
char *G_state="G";
char *S_state="S";
char *K_state="K";
char *Z_state="Z";
String_Data_t state_1;
String_Data_t state_2;
String_Data_t state_3;
String_Data_t state_4;
String_Data_t state_5;

//状态UI，图层1
void UI_state_add(void)
{
	char *C_state="C";
	char *G_state="G";
	char *S_state="S";
	char *K_state="K";
	char *Z_state="Z";

	Char_Draw( &state_1, "101", 1, 1, 8, 5, 35, 1, 150, 607, C_state );
	Char_Draw( &state_2, "102", 1, 1, 8, 5, 35, 1, 210, 607, G_state );
	Char_Draw( &state_3, "103", 1, 1, 8, 5, 35, 1, 270, 607, S_state );
	Char_Draw( &state_4, "104", 1, 1, 8, 5, 35, 1, 330, 607, K_state );
	Char_Draw( &state_5, "105", 1, 1, 8, 5, 35, 1, 390, 607, Z_state );
//	UI_SendChars(&state_1);
//	osDelay (1);
//	UI_SendChars(&state_2);
//	osDelay (1);
//	UI_SendChars(&state_3);
//	osDelay (1);
//	UI_SendChars(&state_4);
//	osDelay (1);
}

void UI_state_update(void)
{

	if(Report_IF_Chassis_work() == false)
		Char_Draw( &state_1, "101", 2, 1, 8, 5, 35, 1, 150, 607, C_state );
	else
		Char_Draw( &state_1, "101", 2, 1, 3, 5, 35, 1, 150, 607, C_state );
	
	if(Report_IF_Gimbal_work() == false)
		Char_Draw( &state_2, "102", 2, 1, 8, 5, 35, 1, 210, 607, G_state );
	else
		Char_Draw( &state_2, "102", 2, 1, 3, 5, 35, 1, 210, 607, G_state );
	
	if(Report_IF_ArmorBooster_work() == false)
		Char_Draw( &state_3, "103", 2, 1, 8, 5, 35, 1, 270, 607, S_state );
	else
		Char_Draw( &state_3, "103", 2, 1, 3, 5, 35, 1, 270, 607, S_state );
	
	if(USART_Rx_data.mode.bits.controls_mode==CONTROL_RC_CTRL)
		Char_Draw( &state_4, "104", 2, 1, 8, 5, 35, 1, 330, 607, K_state );
	else
		Char_Draw( &state_4, "104", 2, 1, 3, 5, 35, 1, 330, 607, K_state );
	
	if(USART_Rx_data.flag.bits.down_over_flag&&USART_Rx_data.mode.bits.gimbal_mode==GIMBAL_FOLD)
		Char_Draw( &state_5, "105", 2, 1, 3, 5, 35, 1, 390, 607, Z_state );
	else if(USART_Rx_data.flag.bits.down_over_flag==0&&USART_Rx_data.mode.bits.gimbal_mode==GIMBAL_FOLD)
		Char_Draw( &state_5, "105", 2, 1, 5, 5, 35, 1, 390, 607, Z_state );
	else
		Char_Draw( &state_5, "105", 2, 1, 8, 5, 35, 1, 390, 607, Z_state );
}
//弹道
Graph_Data_t trajectory_3m_1;
Graph_Data_t trajectory_3m_2;
Graph_Data_t trajectory_3m_3;
Graph_Data_t trajectory_3m_4;
Graph_Data_t trajectory_1m_1;
Graph_Data_t trajectory_1m_2;
Graph_Data_t trajectory_1m_3;
Graph_Data_t trajectory_1m_4;

void UI_trajectory_add(void)
{
	Line_Draw(&trajectory_3m_1,"348",1,1,8,3,944,557,1010,557);
	Line_Draw(&trajectory_3m_2,"349",1,1,8,3,1010,557,1010,500);
	Line_Draw(&trajectory_3m_3,"350",1,1,8,3,944,500,1010,500);
	Line_Draw(&trajectory_3m_4,"351",1,1,8,3,944,557,944,500);
	
	Line_Draw(&trajectory_1m_1,"352",1,1,8,3,964,577,1050,577);
	Line_Draw(&trajectory_1m_2,"353",1,1,8,3,1050,577,1050,480);
	Line_Draw(&trajectory_1m_3,"354",1,1,8,3,964,480,1050,480);
	Line_Draw(&trajectory_1m_4,"355",1,1,8,3,964,577,964,480);
}

void UI_trajectory_update(void)
{
	if(USART_Rx_data.mode.bits.shoot_mode==SHOOT_IDLE&&USART_Rx_data.flag.bits.stuck_state==1)
	{
		Line_Draw(&trajectory_3m_1,"348",2,1,8,3,944,557,1010,557);
		Line_Draw(&trajectory_3m_2,"349",2,1,8,3,1010,557,1010,500);
		Line_Draw(&trajectory_3m_3,"350",2,1,8,3,944,500,1010,500);
		Line_Draw(&trajectory_3m_4,"351",2,1,8,3,944,557,944,500);
		
		Line_Draw(&trajectory_1m_1,"352",2,1,8,3,964,577,1050,577);
		Line_Draw(&trajectory_1m_2,"353",2,1,8,3,1050,577,1050,480);
		Line_Draw(&trajectory_1m_3,"354",2,1,8,3,964,480,1050,480);
		Line_Draw(&trajectory_1m_4,"355",2,1,8,3,964,577,964,480);
	}
	else if(USART_Rx_data.mode.bits.shoot_mode!=SHOOT_IDLE&&USART_Rx_data.flag.bits.stuck_state==1)
	{
		Line_Draw(&trajectory_3m_1,"348",2,1,3,3,944,557,1010,557);
		Line_Draw(&trajectory_3m_2,"349",2,1,3,3,1010,557,1010,500);
		Line_Draw(&trajectory_3m_3,"350",2,1,3,3,944,500,1010,500);
		Line_Draw(&trajectory_3m_4,"351",2,1,3,3,944,557,944,500);
		
		Line_Draw(&trajectory_1m_1,"352",2,1,3,3,964,577,1050,577);
		Line_Draw(&trajectory_1m_2,"353",2,1,3,3,1050,577,1050,480);
		Line_Draw(&trajectory_1m_3,"354",2,1,3,3,964,480,1050,480);
		Line_Draw(&trajectory_1m_4,"355",2,1,3,3,964,577,964,480);
	}
	else
	{
		Line_Draw(&trajectory_3m_1,"348",2,1,5,3,944,557,1010,557);
		Line_Draw(&trajectory_3m_2,"349",2,1,5,3,1010,557,1010,500);
		Line_Draw(&trajectory_3m_3,"350",2,1,5,3,944,500,1010,500);
		Line_Draw(&trajectory_3m_4,"351",2,1,5,3,944,557,944,500);
		
		Line_Draw(&trajectory_1m_1,"352",2,1,5,3,964,577,1050,577);
		Line_Draw(&trajectory_1m_2,"353",2,1,5,3,1050,577,1050,480);
		Line_Draw(&trajectory_1m_3,"354",2,1,5,3,964,480,1050,480);
		Line_Draw(&trajectory_1m_4,"355",2,1,5,3,964,577,964,480);
	}
	
}

//底盘UI，图层1
Graph_Data_t chassis_1;
Graph_Data_t chassis_2;
Graph_Data_t chassis_3;
Graph_Data_t chassis_4;
Graph_Data_t chassis_5;
Graph_Data_t chassis_6;

void UI_chassis_add(void)
{
	//左前箭头
	Line_Draw( &chassis_1, "111", 1, 1, 8, 4, 1514, 588, 1495, 629 );
	Line_Draw( &chassis_2, "112", 1, 1, 8, 4, 1476, 588, 1495, 629 );
	Line_Draw( &chassis_3, "113", 1, 1, 8, 5, 1495, 540, 1495, 630 );
	
	//右后箭头
	Line_Draw( &chassis_4, "114", 1, 1, 8, 4, 1542, 540, 1561, 581 );
	Line_Draw( &chassis_5, "115", 1, 1, 8, 4, 1542, 540, 1523, 581 );
	Line_Draw( &chassis_6, "116", 1, 1, 8, 4, 1542, 539, 1542, 630 );
	
//	UI_SendGraph( 7,chassis_1,chassis_2,chassis_3,chassis_4,chassis_5,chassis_6,chassis_6);
//	osDelay (10);
}

void UI_chassis_update(void)
{
	//左前箭头
	Line_Draw( &chassis_1, "111", 2, 1, 3, 4, 1514, 588, 1495, 629 );
	Line_Draw( &chassis_2, "112", 2, 1, 3, 4, 1476, 588, 1495, 629 );
	Line_Draw( &chassis_3, "113", 2, 1, 3, 5, 1495, 540, 1495, 630 );
	
	//右后箭头
	Line_Draw( &chassis_4, "114", 2, 1, 3, 4, 1542, 540, 1561, 581 );
	Line_Draw( &chassis_5, "115", 2, 1, 3, 4, 1542, 540, 1523, 581 );
	Line_Draw( &chassis_6, "116", 2, 1, 3, 4, 1542, 539, 1542, 630 );
	
	if(USART_Rx_data.mode.bits.chassis_mode==CHASSIS_TOP)
	{
		if(USART_Rx_data.flag.bits.top_mode==0)
		{
			Line_Draw( &chassis_1, "111", 2, 1, 3, 4, 1514, 588, 1495, 629 );
			Line_Draw( &chassis_2, "112", 2, 1, 3, 4, 1476, 588, 1495, 629 );
			Line_Draw( &chassis_3, "113", 2, 1, 3, 5, 1495, 540, 1495, 630 );
			Line_Draw( &chassis_4, "114", 2, 1, 3, 4, 1542, 540, 1561, 581 );
			Line_Draw( &chassis_5, "115", 2, 1, 3, 4, 1542, 540, 1523, 581 );
			Line_Draw( &chassis_6, "116", 2, 1, 3, 4, 1542, 539, 1542, 630 );
		}
		else
		{
			Line_Draw( &chassis_1, "111", 2, 1, 8, 4, 1514, 588, 1495, 629 );
			Line_Draw( &chassis_2, "112", 2, 1, 8, 4, 1476, 588, 1495, 629 );
			Line_Draw( &chassis_3, "113", 2, 1, 8, 5, 1495, 540, 1495, 630 );
			Line_Draw( &chassis_4, "114", 2, 1, 8, 4, 1542, 540, 1561, 581 );
			Line_Draw( &chassis_5, "115", 2, 1, 8, 4, 1542, 540, 1523, 581 );
			Line_Draw( &chassis_6, "116", 2, 1, 8, 4, 1542, 539, 1542, 630 );
		}
	}
	else 
		{
		if(CHASSIS.front_set_num == 0){
			Line_Draw( &chassis_1, "111", 2, 1, 8, 4, 1514, 588, 1495, 629 );
			Line_Draw( &chassis_2, "112", 2, 1, 8, 4, 1476, 588, 1495, 629 );
			Line_Draw( &chassis_3, "113", 2, 1, 8, 5, 1495, 540, 1495, 630 );
			Line_Draw( &chassis_4, "114", 2, 1, 3, 4, 1542, 540, 1561, 581 );
			Line_Draw( &chassis_5, "115", 2, 1, 3, 4, 1542, 540, 1523, 581 );
			Line_Draw( &chassis_6, "116", 2, 1, 3, 4, 1542, 539, 1542, 630 );
		}else{
			Line_Draw( &chassis_1, "111", 2, 1, 3, 4, 1514, 588, 1495, 629 );
			Line_Draw( &chassis_2, "112", 2, 1, 3, 4, 1476, 588, 1495, 629 );
			Line_Draw( &chassis_3, "113", 2, 1, 3, 5, 1495, 540, 1495, 630 );
			Line_Draw( &chassis_4, "114", 2, 1, 8, 4, 1542, 540, 1561, 581 );
			Line_Draw( &chassis_5, "115", 2, 1, 8, 4, 1542, 540, 1523, 581 );
			Line_Draw( &chassis_6, "116", 2, 1, 8, 4, 1542, 539, 1542, 630 );
		}
	}
	
//	UI_SendGraph( 7,chassis_1,chassis_2,chassis_3,chassis_4,chassis_5,chassis_6,chassis_6);
//	osDelay (10);
	
}

//视觉UI，图层1
Graph_Data_t vision_1;
Graph_Data_t vision_2;
Graph_Data_t vision_3;
Graph_Data_t vision_4;

Graph_Data_t vision_5;

Graph_Data_t vision_6;
Graph_Data_t vision_7;

void UI_vision_add(void)
{
	Line_Draw( &vision_1, "121", 1, 1, 7, 5, 1600, 582, 1665, 631 );
	Line_Draw( &vision_2, "122", 1, 1, 7, 5, 1600, 583, 1665, 534 );
	Line_Draw( &vision_3, "123", 1, 1, 7, 5, 1725, 582, 1664, 631 );
	Line_Draw( &vision_4, "124", 1, 1, 7, 5, 1725, 583, 1664, 534 );
	
	Rectangle_Draw(&vision_5,"125",1, 2, 8, 3, 1638, 558, 1690, 610 );
	
	Line_Draw( &vision_6, "126", 1, 0, 8, 5, 1640, 586, 1688, 586 );//横
	Line_Draw( &vision_7, "127", 1, 0, 8, 5, 1665, 560, 1665, 608 );//竖	

//	UI_SendGraph( 7,vision_1,vision_2,vision_3,vision_4,vision_5,vision_6,vision_7);
//	osDelay (10);
	
}

void UI_vision_update(void)
{
	if(motor_current_time[FL]>2000)  Line_Draw( &vision_1, "121", 2, 1, 8, 5, 1600, 582, 1665, 631 );
	else  Line_Draw( &vision_1, "121", 2, 1, 7, 5, 1600, 582, 1665, 631 );

	if(motor_current_time[RL]>2000 )  Line_Draw( &vision_2, "122",2, 1, 8, 5, 1600, 583, 1665, 534 );
	else  Line_Draw( &vision_2, "122", 2, 1, 7, 5, 1600, 583, 1665, 534 );
	
	if(motor_current_time[FR]>2000 )  Line_Draw( &vision_3, "123", 2, 1, 8, 5, 1725, 582, 1664, 631 );
	else  Line_Draw( &vision_3, "123", 2, 1, 7, 5, 1725, 582, 1664, 631 );
	
	if(motor_current_time[RR]>2000 )  Line_Draw( &vision_4, "124", 2, 1, 8, 5, 1725, 583, 1664, 534 );
	else  Line_Draw( &vision_4, "124", 2, 1, 7, 5, 1725, 583, 1664, 534 );
	
	
	if(USART_Rx_data.mode.bits.vision_mode!=VISION_CLOSE&&USART_Rx_data.flag.bits.IF_DISCERN==1)  
	{	
		Rectangle_Draw(&vision_5,"125",2, 2, 3, 3, 1638, 558, 1690, 610 );
		if(USART_Rx_data.mode.bits.vision_mode==VISION_ARMOR){
		
		}else if(USART_Rx_data.mode.bits.vision_mode == VISION_SMALL_BUFF )
		{
		  Line_Draw( &vision_6, "126", 2, 0, 8, 5, 1640, 586, 1688, 586 );//横
			Line_Draw( &vision_7, "127", 2, 0, 3, 5, 1665, 560, 1665, 608 );//竖	
			Line_Draw( &vision_7, "127", 2, 0, 8, 5, 1665, 560, 1665, 608 );//竖	
		}
		else if
		(USART_Rx_data.mode.bits.vision_mode == VISION_BIG_BUFF)
		{
			Line_Draw( &vision_6, "126", 2, 0, 3, 5, 1640, 586, 1688, 586 );//横
			Line_Draw( &vision_7, "127", 2, 0, 3, 5, 1665, 560, 1665, 608 );//竖	
		}
	}
	else
	{
		Rectangle_Draw(&vision_5,"125",2, 2, 8, 3, 1638, 558, 1690, 610 );
		Line_Draw( &vision_6, "126", 2, 0, 8, 5, 1640, 586, 1688, 586 );//横
		Line_Draw( &vision_7, "127", 2, 0, 8, 5, 1665, 560, 1665, 608 );//竖	
		
	}
	
//	UI_SendGraph( 7,vision_1,vision_2,vision_3,vision_4,vision_5,vision_6,vision_7);
//	osDelay (10);
}

//发射UI，图层1
Graph_Data_t shoot_1;
Graph_Data_t shoot_2;
Graph_Data_t shoot_3;
Graph_Data_t shoot_4;
Graph_Data_t shoot_5;

void UI_shoot_add(void)
{
	Line_Draw( &shoot_1, "131", 1, 1, 8, 5, 1480, 462, 1578, 462 );//上
	Line_Draw( &shoot_2, "132", 1, 1, 8, 5, 1480, 392, 1578, 392 );//下
	
	Line_Draw( &shoot_3, "133", 1, 1, 8, 4, 1530, 461, 1530, 392 );//中
	
	Line_Draw( &shoot_4, "134", 1, 1, 8, 4, 1529, 461, 1480, 392 );//左
	Line_Draw( &shoot_5, "135", 1, 1, 8, 4, 1530, 461, 1579, 392 );//右
	
//	UI_SendGraph( 5,shoot_1,shoot_2,shoot_3,shoot_4,shoot_5);
//	osDelay (5);
}

void UI_shoot_update(void)
{
	// if(GIMBAL.IF_PT_OVER != 1)
	// 	Line_Draw( &shoot_1, "131", 2, 1, 3, 5, 1480, 462, 1578, 462 );//无平头模式
	// else 
	// 	Line_Draw( &shoot_1, "131", 2, 1, 8, 5, 1480, 462, 1578, 462 );//平头模式
	
	//卡弹
	if(USART_Rx_data.flag.bits.stuck_state == STUCK_ERR)
		Line_Draw( &shoot_2, "132", 2, 1, 3, 5, 1480, 392, 1578, 392 );//下
	else
		Line_Draw( &shoot_2, "132", 2, 1, 8, 5, 1480, 392, 1578, 392 );//下
	
	// if(rc_ctrl.keyboard.key_V==1)
	// 	Line_Draw( &shoot_3, "133", 2, 1, 3, 4, 1530, 461, 1530, 392 );//中
	// else
	// 	Line_Draw( &shoot_3, "133", 2, 1, 8, 4, 1530, 461, 1530, 392 );//中
	
	if(USART_Rx_data.flag.bits.shoot_l == 0)
		Line_Draw( &shoot_4, "134", 2, 1, 3, 4, 1529, 461, 1480, 392 );//左
	else 
		Line_Draw( &shoot_4, "134", 2, 1, 8, 4, 1529, 461, 1480, 392 );//左
	
	if(USART_Rx_data.flag.bits.shoot_r == 0)
		Line_Draw( &shoot_5, "135", 2, 1, 3, 4, 1530, 461, 1579, 392 );//右
	else
		Line_Draw( &shoot_5, "135", 2, 1, 8, 4, 1530, 461, 1579, 392 );//右
	
//	UI_SendGraph( 5,shoot_1,shoot_2,shoot_3,shoot_4,shoot_5);
//	osDelay (5);
}

//电容UI，图层1
Graph_Data_t supercap_1;
Graph_Data_t supercap_2;
Graph_Data_t supercap_3;
Graph_Data_t supercap_4;

void UI_supercap_add(void)
{
	Line_Draw( &supercap_1, "141", 1, 1, 8, 5, 1618, 437, 1652, 486 );
	Line_Draw( &supercap_2, "142", 1, 1, 8, 5, 1641, 378, 1680, 438 );
	Line_Draw( &supercap_3, "143", 1, 1, 8, 5, 1619, 436, 1679, 436 );
	
	Line_Draw( &supercap_4, "144", 1, 2, 8, 5, 1684, 380, 1609, 486 );//划线
	
//	UI_SendGraph( 5,supercap_1,supercap_2,supercap_3,supercap_4,supercap_4);
//	osDelay (5);
}

void UI_supercap_update(void)
{
//	if(rc_ctrl.keyboard.key_Shift==0&&rc_ctrl.keyboard.key_F==0)
//	{
//		Line_Draw( &supercap_1, "141", 2, 1, 8, 5, 1618, 437, 1652, 486 );
//		Line_Draw( &supercap_2, "142", 2, 1, 8, 5, 1641, 378, 1680, 438 );
//		Line_Draw( &supercap_3, "143", 2, 1, 8, 5, 1619, 436, 1679, 436 );
//	}else{
//		Line_Draw( &supercap_1, "141", 2, 1, 3, 5, 1618, 437, 1652, 486 );
//		Line_Draw( &supercap_2, "142", 2, 1, 3, 5, 1641, 378, 1680, 438 );
//		Line_Draw( &supercap_3, "143", 2, 1, 3, 5, 1619, 436, 1679, 436 );	
//	}
	
	// if(SuperCAP.Mode_C !=0)
	// {
	// 	Line_Draw( &supercap_1, "141", 2, 1, 3, 5, 1618, 437, 1652, 486 );
	// 	Line_Draw( &supercap_2, "142", 2, 1, 3, 5, 1641, 378, 1680, 438 );
	// 	Line_Draw( &supercap_3, "143", 2, 1, 3, 5, 1619, 436, 1679, 436 );	
	// }
	// else
	// {
	//  		Line_Draw( &supercap_1, "141", 2, 1, 8, 5, 1618, 437, 1652, 486 );
	// 	  Line_Draw( &supercap_2, "142", 2, 1, 8, 5, 1641, 378, 1680, 438 );
	// 	  Line_Draw( &supercap_3, "143", 2, 1, 8, 5, 1619, 436, 1679, 436 );
	// }
	
	
	// if(SuperCAP.Mode_C==0&&SuperCAP.C_Vol>=10.5f)
	// 	Line_Draw( &supercap_4, "144", 2, 2, 4, 5, 1684, 380, 1609, 486 );//划线
	// else
	// 	Line_Draw( &supercap_4, "144", 2, 2, 8, 5, 1684, 380, 1609, 486 );//划线
	
//	UI_SendGraph( 5,supercap_1,supercap_2,supercap_3,supercap_4,supercap_4);
//	osDelay (5);
}

//剩余弹丸UI层，图层1
FloInt_Data_t bullet_1;
FloInt_Data_t bullet_2;
FloInt_Data_t bullet_3;
FloInt_Data_t bullet_4;

void UI_bullet_add(void)
{
	Int32_Draw(&bullet_1, "151",1,1,7,25,3,1296,80,0);
	Int32_Draw(&bullet_2, "152",1,1,7,25,3,1321,80,0);
	Int32_Draw(&bullet_3, "153",1,1,7,25,3,1346,80,0);
	Int32_Draw(&bullet_4, "154",1,1,7,25,3,1371,80,0);
	
//	UI_SendGraph( 5,bullet_1,bullet_2,bullet_3,bullet_4,bullet_4);
//	osDelay (5);
	
}

uint32_t integer,integer_lost;
uint32_t gw,sw,bw,qw;
void UI_bullet_update(void)
{	
	integer=Report_Shoot_NUM();
		qw=integer/1000;
		bw=integer/100%10;
		sw=integer/10%100;
		gw=integer%10;
		
		Int32_Draw(&bullet_1, "151",2,1,7,25,3,1296,80,qw);
		Int32_Draw(&bullet_2, "152",2,1,7,25,3,1321,80,bw);
		Int32_Draw(&bullet_3, "153",2,1,7,25,3,1346,80,sw);
		Int32_Draw(&bullet_4, "154",2,1,7,25,3,1371,80,gw);
		
//		UI_SendGraph( 5,bullet_1,bullet_2,bullet_3,bullet_4,bullet_4);
//		osDelay (5);
	integer_lost=integer;
}


//弹道落点
Graph_Data_t target_1;//3m弹道框
Graph_Data_t target_2;
Graph_Data_t target_3;
Graph_Data_t target_4;

Graph_Data_t target_5;//1m弹道框
Graph_Data_t target_6;
Graph_Data_t target_7;
Graph_Data_t target_8;
void UI_target_add(void)
{
	
	//3m线-小步兵
	Line_Draw( &target_1, "161", 1, 2, 1, 2, 956, 540, 956, 505 );//操作1(添加), 图层2, 颜色1(黄色), 线宽2
	Line_Draw( &target_2, "162", 1, 2, 1, 2, 920, 540, 920, 505 );
	Line_Draw( &target_3, "163", 1, 2, 1, 2, 920, 540, 956, 500 );//3m
	Line_Draw( &target_4, "164", 1, 2, 1, 2, 920, 505, 956, 505 );//3m
	
	//1m线-小步兵
	Line_Draw( &target_5, "165", 1, 2, 1, 2, 1055, 605, 1055, 465 );//操作1(添加), 图层2, 颜色1(黄色), 线宽2
	Line_Draw( &target_6, "166", 1, 2, 1, 2, 885, 605, 885, 465 );
	Line_Draw( &target_7, "167", 1, 2, 1, 2, 885, 605, 1055, 605 );//1m
	Line_Draw( &target_8, "168", 1, 2, 1, 2, 885, 465, 1055, 465 );//1m
	
//	UI_SendGraph( 7,target_1,target_2,target_3,target_4,target_5,target_6,target_7);
//	osDelay (10);
//	UI_SendGraph( 1,target_8);
//	osDelay (1);
	
}

void UI_target_update(void)
{
	
	//3m线-小步兵
	// if(motor[SHOOT_LEFT].speed_rpm<-6000&&motor[SHOOT_RIGHT].speed_rpm>6000)
	// {
	// 	Line_Draw( &target_1, "161", 2, 2, 4, 2, 956, 560, 956, 525 );//操作1(添加), 图层2, 颜色1(黄色), 线宽2
	// 	Line_Draw( &target_2, "162", 2, 2, 4, 2, 920, 560, 920, 525 );
	// 	Line_Draw( &target_3, "163", 2, 2, 4, 2, 920, 560, 956, 560 );//3m
	// 	Line_Draw( &target_4, "164", 2, 2, 4, 2, 920, 525, 956, 525 );//3m
	// }
	// else
	// 	{
	// 			Line_Draw( &target_1, "161", 2, 2, 1, 2, 956, 560, 956, 525 );//操作1(添加), 图层2, 颜色1(黄色), 线宽2
	// 	    Line_Draw( &target_2, "162", 2, 2, 1, 2, 920, 560, 920, 525 );
	// 	    Line_Draw( &target_3, "163", 2, 2, 1, 2, 920, 560, 956, 560 );//3m
	// 	    Line_Draw( &target_4, "164", 2, 2, 1, 2, 920, 525, 956, 525 );//3m
	//   }
		
		
	
//	UI_SendGraph( 7,target_1,target_2,target_3,target_4,target_5,target_6,target_7);
//	osDelay (10);
//	UI_SendGraph( 1,target_8);
//	osDelay (1);
	
}



//目标距离显示
//FloInt_Data_t Distance;
//float Distance_Last;
//void UI_Distance_add(void)
//{
//	Float_Draw(&Distance, "171", 1, 6, 6, 40, 2, 3, 200, 700, diatance);//电容电压//图层6 字号40 线宽3 颜色6(青色
////	UI_SendGraph( 1,SC_Vol);
////	osDelay (1);
//}

//void UI_Distance_update(void)
//{
//					 //		if(rc_ctrl.keyboard.key_R ==1)
////			vision_mode = VISION_FIX;
////		else if(rc_ctrl.keyboard.key_R ==0)
////		vision_mode = VISION_MOVE;
//	if(IF_DISCERN() == 1)
//		Float_Draw(&Distance, "171", 2, 6, 6, 40, 2, 3, 200, 700, diatance);//修改P
//	else
//		Float_Draw(&Distance, "171", 2, 6, 6, 40, 2, 3, 200, 700, 0);//修改P
////		UI_SendGraph( 1,SC_Vol);
////		osDelay (1);
//	
//	
//}

/**
* @brief 电容电压显示
* @note  display SuperCap voltage
* @param 
*/
Graph_Data_t SC_Outline_Arc_1;
Graph_Data_t SC_Outline_Arc_2;
Graph_Data_t SC_Outline_Line_1;
Graph_Data_t SC_Outline_Line_2;
Graph_Data_t SC_Vol_Arc;
float CAP_Vol_Angle;


void UI_Cap_add(void)
{
       Arc_Draw(&SC_Outline_Arc_1, "711", 1, 6, 1, 1, 45, 135, 960, 540, 301, 301);//操作1 创建，图层6，颜色1黄色，线宽1，起始角度，终点角度，圆心（960，540）,xy半径
       Arc_Draw(&SC_Outline_Arc_2, "712", 1, 6, 1, 1, 45, 135, 960, 540, 295, 295);//操作1 创建，图层6，颜色1黄色，线宽1，起始角度，终点角度，圆心（960，540）,xy半径
       Line_Draw(&SC_Outline_Line_1, "713", 1, 6, 1, 1, 1168, (1080-331), 1173, (1080-326) );//操作1 创建，图层6，颜色1黄色，线宽1，
       Line_Draw(&SC_Outline_Line_2, "714", 1, 6, 1, 1, 1168, (1080-747), 1173, (1080-753) );//操作1 创建，图层6，颜色1黄色，线宽1，
				Arc_Draw(&SC_Vol_Arc, "715", 1, 6, 5, 5, 45, 135, 960, 540, 298, 298);//操作1 创建，图层6，颜色5粉色，线宽5，起始角度，终点角度，圆心（960，540）,xy半径
//				UI_SendGraph( 5,SC_Outline_Arc_1,SC_Outline_Arc_2,SC_Outline_Line_1,SC_Outline_Line_2,SC_Vol_Arc);
//				osDelay (5);
   
}
void UI_Cap_update(void)
{
   
//    CAP_Vol_Angle = 135.0f - SuperCAP.C_Vol*SuperCAP.C_Vol*0.17f;//0.5f*7.9f/2090.0f=0.00189f-----0.00189f*90.0f=0.17f  //容值为7.9f，2090为总能量
	  CAP_Vol_Angle = 135.0f - SuperCAP.cap_v*SuperCAP.cap_v*0.17f;//0.5f*7.9f/2090.0f=0.00189f-----0.00189f*90.0f=0.17f  //容值为7.9f，2090为总能量
   if(CAP_Vol_Angle>=134)
       CAP_Vol_Angle = 134;//等于135时ui会变成一个整圆，防止这种情况
   if(SuperCAP.cap_v>=16)
       Arc_Draw(&SC_Vol_Arc, "715", 2, 6, 6, 5, CAP_Vol_Angle, 135, 960, 540, 298, 298);//操作2 更新，图层6，颜色2绿色，线宽5，起始角度，终点角度，圆心（960，540）,xy半径
   if(SuperCAP.cap_v<16)
       Arc_Draw(&SC_Vol_Arc, "715", 2, 6, 5, 5, CAP_Vol_Angle, 135, 960, 540, 298, 298);//操作2 更新，图层6，颜色5粉色，线宽5，起始角度，终点角度，圆心（960，540）,xy半径
//			UI_SendGraph( 1,SC_Vol_Arc);
//		  osDelay (1);
}
//pitch角度
FloInt_Data_t Pitch_Angle_1;
double Pitch_last;
void UI_Pitch_add(void)
{
	Float_Draw(&Pitch_Angle_1, "181", 1, 7, 3, 15, 1, 3, 1050, 540, INS.Pitch);//PITCH角度//图层7 字号15 小数位数1 线宽3 颜色3(橙色)
//	UI_SendGraph( 1,Pitch_Angle_1);
//	osDelay (1);
}



void UI_Pitch_update(void)
{
	if((INS.Pitch-Pitch_last)<=0.1 || (INS.Pitch-Pitch_last)>=-0.1){
		if(USART_Rx_data.mode.bits.shoot_mode ==SHOOT_IDLE)
			Float_Draw(&Pitch_Angle_1, "181", 2, 7, 6, 15, 1, 3, 1050, 540, INS.Pitch);//有变化则修改,青色
		else Float_Draw(&Pitch_Angle_1, "181", 2, 7, 5, 15, 1, 3, 1050, 540, INS.Pitch);//有变化则修改,粉色
	}
//	UI_SendGraph( 1,Pitch_Angle_1);
//	osDelay (1);
	
	Pitch_last = INS.Pitch;
}





//剩余能量
FloInt_Data_t energy_1;

void UI_energy_add(void)
{
	Int32_Draw(&energy_1, "191",1,1,7,25,3,530,136,0);
//	UI_SendGraph(1,energy_1 );
//	osDelay (1);
}

void UI_energy_update(void)
{

	int32_t energy,energy_lost;
	energy=0;//Report_Remaining_Energy();
	if(energy!=energy_lost){
		Int32_Draw(&energy_1, "191",2,1,7,25,3,530,136,energy);	
//		UI_SendGraph(1,energy_1 );
//		osDelay (1);
	}
	
	energy_lost=energy;
}

