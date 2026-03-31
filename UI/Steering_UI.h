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
#ifndef __STEERING_UI_H
#define __STEERING_UI_H

#include "Nautilus_UI.h"

void UI_Task(void);
	
void UI_Reset(void);//防止ui丢包，键盘模式下按R进行刷新

void UI_ID_Set(void);
void UI_Cap_Voltage(void);//电容电量
void UI_Gun_Sight(void);//瞄准镜
void UI_Gun_Sight_autoSHOOT(void);//显示自动发弹
void UI_Action_Mode(void);//运动模式显示
//void UI_Hostile_Direction(void);//敌对方向提示
void UI_Anticollition(void);//防撞提示
void UI_Pitch_Angle(void);//pitch轴角度显示
void UI_Fric_Mode(void);
void UI_Shoot_Mode(void);

/**
 * @brief 视觉模式显示
 * @note  display VISION_Mode
 * @param 
 */
extern Graph_Data_t visual_field_4;
extern Graph_Data_t Vision_Mode_1;
void UI_Vision_Mode(void);


extern FloInt_Data_t SC_Vol;

extern FloInt_Data_t Pitch_Angle;

extern Graph_Data_t GUN_Sight_MainAxis;//瞄准镜中轴
extern Graph_Data_t GUN_Sight_LateralAxis_1;//横轴线 1
extern Graph_Data_t GUN_Sight_LateralAxis_2;//横轴线 2
extern Graph_Data_t GUN_Sight_LateralAxis_3;//横轴线 3

//extern Graph_Data_t Hostile_Direction_1;//右斜线"/"
//extern Graph_Data_t Hostile_Direction_2;//左斜线"\"

extern Graph_Data_t Anticollition_1;//3m落点线
extern Graph_Data_t Anticollition_2;
extern Graph_Data_t Anticollition_3;
extern Graph_Data_t Anticollition_4;
extern Graph_Data_t Anticollition_5;//1m落点线
extern Graph_Data_t Anticollition_6;
extern Graph_Data_t Anticollition_7;
extern Graph_Data_t Anticollition_8;

extern Graph_Data_t Action_Mode_1;

extern Graph_Data_t Fric_Mode_1;

extern Graph_Data_t Shoot_Mode_1;

extern Graph_Data_t Pitch_Mode;
void UI_Pitch_Mode(void);
/**
 * @brief 电容电压显示
 * @note  display SuperCap voltage
 * @param 
 */
extern Graph_Data_t SC_Outline_Arc_1;
extern Graph_Data_t SC_Outline_Arc_2;
extern Graph_Data_t SC_Outline_Line_1;
extern Graph_Data_t SC_Outline_Line_2;
extern Graph_Data_t SC_Vol_Arc;
void UI_Cap(void);

Graph_Data_t HI_Outline_Arc_1;
Graph_Data_t HI_Outline_Arc_2;
Graph_Data_t HI_Outline_Line_1;
Graph_Data_t HI_Outline_Line_2;
Graph_Data_t HEAT_UI;
float HEAT_Angle;
void UI_HEAT(void);
/**
 * @brief 特殊运动模式显示
 * @note  Special ACT
 * @param 
 */
extern Graph_Data_t Special_ACT_1;//云台朝向
extern Graph_Data_t Special_ACT_2;//边框圆
extern Graph_Data_t Special_ACT_3;//灯条1/4圆
void UI_Special_ACT(void);
Graph_Data_t UI_ShootNum_1;
Graph_Data_t UI_ShootNum_2;
Graph_Data_t UI_ShootNum_3;
Graph_Data_t UI_ShootNum_4;
Graph_Data_t UI_ShootNum_5;
int32_t ShootNum;
void UI_Shoot_Num(void);

Graph_Data_t shoot_UI;
Graph_Data_t gimbal_UI;
Graph_Data_t chassis_UI;
void UI_Power_Mode(void);

Graph_Data_t Ctrl_Mode_1;
void UI_Ctrl_Mode(void);

Graph_Data_t DM_Mode;
void UI_DM_Mode(void);

Graph_Data_t Shift_Mode;
void UI_Shift_Mode(void);

void UI_Cap_update(void);
void UI_Cap_add(void);

//弹道落点
Graph_Data_t trajectory_3m_1;
Graph_Data_t trajectory_3m_2;
Graph_Data_t trajectory_3m_3;
Graph_Data_t trajectory_3m_4;
Graph_Data_t trajectory_1m_1;
Graph_Data_t trajectory_1m_2;
Graph_Data_t trajectory_1m_3;
Graph_Data_t trajectory_1m_4;

void UI_trajectory_add(void);
void UI_trajectory_update(void);

/*-----------------------------------------整活层-----------------------------------------*/
/**
 * @brief NAUTILUS之LOGO显示
 * @note  
 * @param 
 */
void UI_NAUTILUS_LOGO(void);

/**
 * @brief LOGO的第一部分
 * @note  LOGO_1
 * @param 
 */
extern Graph_Data_t LOGO_1_1;//直线      
extern Graph_Data_t LOGO_1_2;//直线
extern Graph_Data_t LOGO_1_3;//直线
void UI_LOGO_1(void);

/**
 * @brief LOGO的第二部分
 * @note  LOGO_2
 * @param 
 */
extern Graph_Data_t LOGO_2_1;//直线
extern Graph_Data_t LOGO_2_2;//直线
extern Graph_Data_t LOGO_2_3;//直线
extern Graph_Data_t LOGO_2_4;//圆弧
void UI_LOGO_2(void);

/**
 * @brief LOGO的第三部分
 * @note  LOGO_3
 * @param 
 */
extern Graph_Data_t LOGO_3_1;//直线
extern Graph_Data_t LOGO_3_2;//直线
extern Graph_Data_t LOGO_3_3;//直线
extern Graph_Data_t LOGO_3_4;//圆弧
void UI_LOGO_3(void);

/**
 * @brief LOGO的第四部分
 * @note  LOGO_4
 * @param 
 */
extern Graph_Data_t LOGO_4_1;//直线
extern Graph_Data_t LOGO_4_2;//直线
extern Graph_Data_t LOGO_4_3;//直线
extern Graph_Data_t LOGO_4_4;//圆弧
void UI_LOGO_4(void);

/**
 * @brief LOGO的第五部分
 * @note  LOGO_5
 * @param 
 */
extern Graph_Data_t LOGO_5_1;//直线
extern Graph_Data_t LOGO_5_2;//直线
extern Graph_Data_t LOGO_5_3;//直线
extern Graph_Data_t LOGO_5_4;//圆弧
void UI_LOGO_5(void);

/**
 * @brief LOGO的第六部分
 * @note  LOGO_6
 * @param 
 */
extern Graph_Data_t LOGO_6_1;//直线
extern Graph_Data_t LOGO_6_2;//直线
extern Graph_Data_t LOGO_6_3;//直线
extern Graph_Data_t LOGO_6_4;//圆弧
void UI_LOGO_6(void);

/**
 * @brief LOGO的第七部分
 * @note  LOGO_7
 * @param 
 */
extern Graph_Data_t LOGO_7_1;//直线
extern Graph_Data_t LOGO_7_2;//直线
extern Graph_Data_t LOGO_7_3;//直线
extern Graph_Data_t LOGO_7_4;//圆弧
void UI_LOGO_7(void);

/**
 * @brief LOGO的第八部分
 * @note  LOGO_8
 * @param 
 */
extern Graph_Data_t LOGO_8_1;//正圆
void UI_LOGO_8(void);

/**
 * @brief LOGO的第九部分
 * @note  LOGO_9
 * @param 
 */
extern String_Data_t LOGO_9_1;
extern String_Data_t LOGO_9_2;
extern String_Data_t LOGO_9_3;
extern String_Data_t LOGO_9_4;
extern String_Data_t LOGO_9_5;
extern String_Data_t LOGO_9_6;
extern String_Data_t LOGO_9_7;
extern String_Data_t LOGO_9_8;
extern char *Name;
void UI_LOGO_9(void);


/****************************************************************************修改*****************************************************************************/
extern Graph_Data_t chassis_crash_left;
extern Graph_Data_t chassis_crash_right;

extern String_Data_t state_1;
extern String_Data_t state_2;
extern String_Data_t state_3;
extern String_Data_t state_4;
extern String_Data_t state_5;

extern Graph_Data_t chassis_1;
extern Graph_Data_t chassis_2;
extern Graph_Data_t chassis_3;
extern Graph_Data_t chassis_4;
extern Graph_Data_t chassis_5;
extern Graph_Data_t chassis_6;

extern Graph_Data_t vision_1;
extern Graph_Data_t vision_2;
extern Graph_Data_t vision_3;
extern Graph_Data_t vision_4;

extern Graph_Data_t vision_5;

extern Graph_Data_t vision_6;
extern Graph_Data_t vision_7;

extern Graph_Data_t shoot_1;
extern Graph_Data_t shoot_2;
extern Graph_Data_t shoot_3;
extern Graph_Data_t shoot_4;
extern Graph_Data_t shoot_5;

extern Graph_Data_t supercap_1;
extern Graph_Data_t supercap_2;
extern Graph_Data_t supercap_3;
extern Graph_Data_t supercap_4;

extern FloInt_Data_t bullet_1;
extern FloInt_Data_t bullet_2;
extern FloInt_Data_t bullet_3;
extern FloInt_Data_t bullet_4;

extern Graph_Data_t target_1;//3m弹道框
extern Graph_Data_t target_2;
extern Graph_Data_t target_3;
extern Graph_Data_t target_4;

extern Graph_Data_t target_5;//1m弹道框
extern Graph_Data_t target_6;
extern Graph_Data_t target_7;
extern Graph_Data_t target_8;

//extern FloInt_Data_t Distance;

extern Graph_Data_t SC_Outline_Arc_1;
extern Graph_Data_t SC_Outline_Arc_2;
extern Graph_Data_t SC_Outline_Line_1;
extern Graph_Data_t SC_Outline_Line_2;
extern Graph_Data_t SC_Vol_Arc;

extern FloInt_Data_t Pitch_Angle_1;

extern Graph_Data_t direction_1;//云台朝向
extern Graph_Data_t direction_2;//边框圆
extern Graph_Data_t direction_3;//灯条1/4圆

extern FloInt_Data_t energy_1;



extern Sentry_cmd_t Sentry_cmd;
//extern void Sentry_cmd_update();




#endif
