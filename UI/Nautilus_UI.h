/*******************************************************************
  * File Name   : Nautilus_UI
  * Description : UI
  * Author      : 孙羽
  * QQ          ：984464809
  * Telephone   : 15235320302  
********************************************************************
  *
  * Copyright (c) 2022 Nautilus - Wuhan Institute of Technology
  * All rights reserved.
  *

ps: RoboMaster_裁判系统串口协议附录 V1.3
	频率最大为10Hz
*******************************************************************/

#ifndef __NAUTILUS_UI_H
#define __NAUTILUS_UI_H

#include "stm32f4xx.h"
#include "string.h"
#include "usart.h"
#include "CRC8_CRC16.h"
#include "stdarg.h"
#include "referee.h"

#define UI_RADAR_byte       16    // 1+15#define UI_Data_SENTRY_CMD  0x120  // 哨兵自主决策指令
#define UI_Sentry_byte      19    // 4+15
#define REFEREE_ServerID    0x8080  // 裁判系统服务器（用于哨兵和雷达自主决策指令）
#define UI_Data_SENTRY_CMD  0x0120
#define NULL 0

#define UI_Datasame_byte    15
#define UI_Del_byte         17    // 2+15
#define UI_Graph1_byte      30    // 15+15
#define UI_Graph2_byte      45    // 30+15
#define UI_Graph5_byte      90    // 75+15
#define UI_Graph7_byte      120   // 105+15
#define UI_Char_byte        60    // 45+15

/****************************开始标志*********************/
#define UI_SOF 0xA5
/****************************CMD_ID数据********************/
#define UI_CMD_Robo_Exchange 0x0301    
/****************************内容ID数据********************/
#define UI_Data_ID_Del      0x100
#define UI_Data_ID_Draw1    0x101
#define UI_Data_ID_Draw2    0x102
#define UI_Data_ID_Draw5    0x103
#define UI_Data_ID_Draw7    0x104
#define UI_Data_ID_DrawChar 0x110
/****************************红方机器人ID********************/
#define UI_Data_RobotID_RHero      1  //英雄
#define UI_Data_RobotID_REngineer  2  //工程
#define UI_Data_RobotID_RStandard1 3  //步兵
#define UI_Data_RobotID_RStandard2 4  //步兵
#define UI_Data_RobotID_RStandard3 5  //步兵
#define UI_Data_RobotID_RAerial    6  //空中
#define UI_Data_RobotID_RSentry    7  //哨兵
#define UI_Data_RobotID_RRadar     9  //雷达站
/****************************蓝方机器人ID********************/
#define UI_Data_RobotID_BHero      101  //英雄
#define UI_Data_RobotID_BEngineer  102  //工程
#define UI_Data_RobotID_BStandard1 103  //步兵
#define UI_Data_RobotID_BStandard2 104  //步兵
#define UI_Data_RobotID_BStandard3 105  //步兵
#define UI_Data_RobotID_BAerial    106  //空中
#define UI_Data_RobotID_BSentry    107  //哨兵
#define UI_Data_RobotID_BRadar     109  //雷达站
/**************************红方客户端ID**********************/
#define UI_Data_CilentID_RHero      0x0101  //英雄
#define UI_Data_CilentID_REngineer  0x0102  //工程
#define UI_Data_CilentID_RStandard1 0x0103  //步兵
#define UI_Data_CilentID_RStandard2 0x0104  //步兵
#define UI_Data_CilentID_RStandard3 0x0105  //步兵
#define UI_Data_CilentID_RAerial    0x0106  //空中
/***************************蓝方客户端ID*********************/
#define UI_Data_CilentID_BHero      0x0165  //英雄
#define UI_Data_CilentID_BEngineer  0x0166  //工程
#define UI_Data_CilentID_BStandard1 0x0167  //步兵
#define UI_Data_CilentID_BStandard2 0x0168  //步兵
#define UI_Data_CilentID_BStandard3 0x0169  //步兵
#define UI_Data_CilentID_BAerial    0x016A  //空中
/***************************删除操作**********************************/
#define UI_Data_Del_NoOperate 0  //空操作
#define UI_Data_Del_Layer     1  //删除图层
#define UI_Data_Del_ALL       2  //删除所有
/***************************图形配置参数__图形操作********************/
#define UI_Graph_NoOperate   0    //空操作
#define UI_Graph_ADD         1    //增加
#define UI_Graph_Change      2    //修改
#define UI_Graph_Del         3    //删除
/***************************图形配置参数__图形类型********************/
#define UI_Graph_Line      0     //直线
#define UI_Graph_Rectangle 1     //矩形
#define UI_Graph_Circle    2     //整圆
#define UI_Graph_Ellipse   3     //椭圆
#define UI_Graph_Arc       4     //圆弧
#define UI_Graph_Float     5     //浮点型
#define UI_Graph_Int       6     //整形
#define UI_Graph_Char      7     //字符型
/***************************图形配置参数__图形颜色********************/
#define UI_Color_Main         0  //红蓝主色
#define UI_Color_Yellow       1  //黄色
#define UI_Color_Green        2  //绿色
#define UI_Color_Orange       3  //橙色
#define UI_Color_Purplish_red 4  //紫红色
#define UI_Color_Pink         5  //粉色
#define UI_Color_Cyan         6  //青色
#define UI_Color_Black        7  //黑色
#define UI_Color_White        8  //白色


typedef __packed struct
{
   uint8_t SOF;                    //起始字节,固定0xA5
   uint16_t Data_Length;           //帧数据长度
   uint8_t Seq;                    //包序号
   uint8_t CRC8;                   //CRC8校验值
} UI_Packhead_t;             //帧头

typedef __packed struct
{
   uint16_t Data_ID;               //内容ID
   uint16_t Sender_ID;             //发送者ID
   uint16_t Receiver_ID;           //接收者ID
} UI_Data_Operate_t;         //操作定义帧

typedef __packed struct
{
   uint8_t Delete_Operate;         //图形操作
   uint8_t Layer;                  //图层数
} UI_Data_Delete_t;          //删除图层帧

typedef __packed struct
{ 
   uint8_t graphic_name[3]; 
   uint32_t operate_tpye:3; 
   uint32_t graphic_tpye:3; 
   uint32_t layer:4; 
   uint32_t color:4; 
   uint32_t start_angle:9;
   uint32_t end_angle:9;
   uint32_t width:10; 
   uint32_t start_x:11; 
   uint32_t start_y:11;
   int32_t graph_FloInt;              //浮点数据,整型数据
} FloInt_Data_t;


typedef __packed struct
{ 
	uint8_t graphic_name[3]; 
	uint32_t operate_tpye:3; 
	uint32_t graphic_tpye:3; 
	uint32_t layer:4; 
	uint32_t color:4; 
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10; 
	uint32_t start_x:11; 
	uint32_t start_y:11;
	uint32_t radius:10; 
	uint32_t end_x:11; 
	uint32_t end_y:11;              //图形数据
} Graph_Data_t;


typedef __packed struct
{
   Graph_Data_t Graph_Control;
   uint8_t show_Data[30];
} String_Data_t;                    //打印字符串数据

typedef __packed struct
{
	UI_Packhead_t UI_Packhead;
	// cmd_id
	UI_Data_Operate_t UI_Data_Operate;
	// 内容数据段
	uint16_t frame_tail;
} UI_Datahead_t;

typedef __packed struct
{
	uint8_t if_revive:1;    /* 哨兵机器人是否确认复活 */
													// 0 表示哨兵机器人确认不复活，即使此时哨兵的复活读条已经完成
													// 1 表示哨兵机器人确认复活，若复活读条完成将立即复活
	uint8_t if_immediately_revive:1;    /* 哨兵机器人是否确认兑换立即复活 */
																			// 0 表示哨兵机器人确认不兑换立即复活
																			// 1 表示哨兵机器人确认兑换立即复活，若此时哨兵机器人
																			// 符合兑换立即复活的规则要求，则会立即消耗金币兑换立
																			// 即复活
	uint16_t exchange_projectile_num:11;    /* 哨兵将要兑换的发弹量值，开局为 0，修改此值后，哨兵在补血点即可兑换允许发弹量 */
																					// 此值的变化需要单调递增，否则视为不合法
	uint8_t remote_exchange_projectile_count:4;    /* 哨兵远程兑换发弹量的请求次数，开局为 0，修改此值即可请求远程兑换发弹量 */
																								 // 此值的变化需要单调递增且每次仅能增加 1，否则视为不合法
	uint8_t remote_exchange_HP_count:4;    /* 哨兵远程兑换血量的请求次数，开局为 0，修改此值即可请求远程兑换血量 */
																				 // 此值的变化需要单调递增且每次仅能增加 1，否则视为不合法
	/* 在哨兵发送该子命令时，服务器将按照从相对低位到相对高位的原则依次处理这些指令，直至全部成功或不能处理为止 */
	uint16_t reserve:11;    // 保留
} Sentry_cmd_t;      // 哨兵自主决策指令


/********************************************删除操作*************************************
**参数：Del_Operate  对应头文件删除操作
        Del_Layer    要删除的层 取值0-9
*****************************************************************************************/

void UI_Delete(uint8_t Del_Operate,uint8_t Del_Layer);

/************************************************绘制直线*************************************************
**参数：*image Graph_Data_t类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate  图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Width    图形线宽
        Start_x、Start_y    开始坐标
        End_x、End_y   结束坐标
**********************************************************************************************************/

void Line_Draw(Graph_Data_t *image,
	           char imagename[3],
			   uint32_t Graph_Operate,
			   uint32_t Graph_Layer,
			   uint32_t Graph_Color,
			   uint32_t Graph_Width,
			   uint32_t Start_x,
			   uint32_t Start_y,
			   uint32_t End_x,
			   uint32_t End_y);
			   
/************************************************绘制矩形*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Width    图形线宽
        Start_x、Start_y    开始坐标
        End_x、End_y   结束坐标（对顶角坐标）
**********************************************************************************************************/

void Rectangle_Draw(Graph_Data_t *image,
	                char imagename[3],
					uint32_t Graph_Operate,
					uint32_t Graph_Layer,
					uint32_t Graph_Color,
					uint32_t Graph_Width,
					uint32_t Start_x,
					uint32_t Start_y,
					uint32_t End_x,
					uint32_t End_y);

/************************************************绘制整圆*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Width    图形线宽
        Start_x、Start_y    圆心坐标
        Graph_Radius  图形半径
**********************************************************************************************************/
					
void Circle_Draw(Graph_Data_t *image,
	             char imagename[3],
			     uint32_t Graph_Operate,
				 uint32_t Graph_Layer,
				 uint32_t Graph_Color,
				 uint32_t Graph_Width,
				 uint32_t Start_x,
				 uint32_t Start_y,
				 uint32_t Graph_Radius);

/************************************************绘制椭圆*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Width    图形线宽
        Start_y,Start_y    圆心坐标
        x_Length,y_Length   x,y方向上半轴长度
**********************************************************************************************************/

void Ellipse_Draw(Graph_Data_t *image,
	              char imagename[3],
				  uint32_t Graph_Operate,
				  uint32_t Graph_Layer,
				  uint32_t Graph_Color,
				  uint32_t Graph_Width,
				  uint32_t Start_x,
				  uint32_t Start_y,
				  uint32_t x_Length,
				  uint32_t y_Length);
				 
/************************************************绘制圆弧*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Width    图形线宽
        Graph_StartAngle,Graph_EndAngle    开始，终止角度   0~360
        Start_y,Start_y    圆心坐标
        x_Length,y_Length   x,y方向上轴长，参考椭圆
**********************************************************************************************************/

void Arc_Draw(Graph_Data_t *image,
	          char imagename[3],
			  uint32_t Graph_Operate,
			  uint32_t Graph_Layer,
			  uint32_t Graph_Color,
			  uint32_t Graph_Width,
			  uint32_t Graph_StartAngle,
			  uint32_t Graph_EndAngle,
			  uint32_t Start_x,
			  uint32_t Start_y,
			  uint32_t x_Length,
			  uint32_t y_Length);

/************************************************绘制浮点型数据*******************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Size     字号
        Graph_Digit    小数位数
        Graph_Width    图形线宽
        Start_x、Start_x    开始坐标
        Graph_Float   要显示的变量 float 32位浮点数
**********************************************************************************************************/
      
void Float_Draw(FloInt_Data_t *image,
	            char imagename[3],
			    uint32_t Graph_Operate,
				uint32_t Graph_Layer,
				uint32_t Graph_Color,
				uint32_t Graph_Size,
				uint32_t Graph_Digit,
				uint32_t Graph_Width,
				uint32_t Start_x,
				uint32_t Start_y,
				float Graph_Float);
			   
/************************************************绘制整型数据*********************************************
整型数据范围： 0~447

**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Size     字号
        Graph_Width    图形线宽
        Start_x、Start_x    开始坐标
        Graph_Int32   要显示的变量 int32_t 32位整型数
**********************************************************************************************************/
      
void Int32_Draw(FloInt_Data_t *image,
	            char imagename[3],
			    uint32_t Graph_Operate,
				uint32_t Graph_Layer,
				uint32_t Graph_Color,
				uint32_t Graph_Size,
				uint32_t Graph_Width,
				uint32_t Start_x,
				uint32_t Start_y,
				int32_t Graph_Int32);
				
/************************************************绘制字符型数据*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
        imagename[3]   图片名称，用于标识更改
        Graph_Operate   图片操作，见头文件
        Graph_Layer    图层0-9
        Graph_Color    图形颜色
        Graph_Width    图形线宽
        Graph_Size     字号
        Graph_Digit    字符个数
        Start_x、Start_x    开始坐标
        *Char_Data          待发送字符串开始地址
**********************************************************************************************************/

void Char_Draw(String_Data_t *image,
	           char imagename[3],
			   uint32_t Graph_Operate,
			   uint32_t Graph_Layer,
			   uint32_t Graph_Color,
			   uint32_t Graph_Size,
			   uint32_t Graph_Digit,
			   uint32_t Graph_Width,
			   uint32_t Start_x,
			   uint32_t Start_y,
			   char *Char_Data);

						 
/************************************************UI推送函数（使更改生效）*********************************
**参数： cnt   图形个数   （1、2、5、7）
         ...   图形变量参数

Tips：：该函数只能推送1，2，5，7个图形，其他数目协议未涉及
**********************************************************************************************************/

int UI_SendGraph(int cnt,...);

/************************************************UI推送字符（使更改生效）*********************************
**参数： image String_Data_t* 数据
**********************************************************************************************************/

void UI_SendChars(String_Data_t *image);
void UI_SendSentry(Sentry_cmd_t *cmd);
void Sentry_Cmd_Fill(Sentry_cmd_t *cmd,
                     uint8_t IF_Revive,
                     uint8_t IF_Immediately_Revive,
                     uint16_t Exchange_Projectile_Num,
                     uint8_t IF_Remote_Exchange_HP);
extern uint16_t Robot_ID;
extern uint16_t Cilent_ID;

	
#endif /*__ UITask_H */

