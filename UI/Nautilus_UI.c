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
#include "Nautilus_UI.h"

unsigned char UI_Seq;                      //包序号

uint16_t Robot_ID    = UI_Data_RobotID_RStandard1;   // 机器人ID  见头文件
uint16_t Cilent_ID   = UI_Data_CilentID_RStandard1;  // 客户端ID  见头文件

/********************************************删除操作*************************************
**参数：Del_Operate  对应头文件删除操作
			#define UI_Data_Del_NoOperate 0  //空操作
			#define UI_Data_Del_Layer     1  //删除图层
			#define UI_Data_Del_ALL       2  //删除所有
       Del_Layer    要删除的层 取值0-9
*****************************************************************************************/

void UI_Delete(uint8_t Del_Operate,uint8_t Del_Layer)
{
	UI_Data_Delete_t UI_Data_Delete;
	uint8_t data_pack[UI_Del_byte] = {0};
	
	UI_Datahead_t UI_Datahead;
	UI_Datahead.UI_Packhead.SOF = UI_SOF;
	UI_Datahead.UI_Packhead.Data_Length = UI_Del_byte - 9;
	UI_Datahead.UI_Packhead.Seq = UI_Seq;
	memcpy(data_pack, &UI_Datahead.UI_Packhead, 5);
	append_CRC8_check_sum(data_pack,5);
	
	uint16_t CMD_ID;
	CMD_ID = UI_CMD_Robo_Exchange;
	memcpy(data_pack + 5, &CMD_ID, 2);
	
	UI_Datahead.UI_Data_Operate.Data_ID = UI_Data_ID_Del;
	UI_Datahead.UI_Data_Operate.Sender_ID = Robot_ID;
	UI_Datahead.UI_Data_Operate.Receiver_ID = Cilent_ID;
	
	memcpy(data_pack + 7, &UI_Datahead.UI_Data_Operate, 6);
	
	UI_Data_Delete.Delete_Operate = Del_Operate;
	UI_Data_Delete.Layer          = Del_Layer;
	
	memcpy(data_pack + 13, &UI_Data_Delete, UI_Del_byte - UI_Datasame_byte);
	
	append_CRC16_check_sum(data_pack, UI_Del_byte);
	
	HAL_UART_Transmit(&huart6, data_pack, UI_Del_byte,100);
	
	UI_Seq++;      // 包序号+1
}

/************************************************绘制直线*************************************************
**参数：*image Graph_Data_t类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate  图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
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
			   uint32_t End_y)
{
	int i;
	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Line;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = 0;
	image->end_angle   = 0;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	
	image->radius = 0;
	image->end_x = End_x;
	image->end_y = End_y;
}

/************************************************绘制矩形*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
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
					uint32_t End_y)

{
	int i;
	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Rectangle;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = 0;
	image->end_angle   = 0;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->radius = 0;
	image->end_x = End_x;
	image->end_y = End_y;
	
}

/************************************************绘制整圆*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
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
				 uint32_t Graph_Radius)

{
	int i;
	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Circle;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = 0;
	image->end_angle   = 0;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->radius = Graph_Radius;
	image->end_x = 0;
	image->end_y = 0;
}

/************************************************绘制椭圆*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
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
				  uint32_t y_Length)
{
	int i;
	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Ellipse;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = 0;
	image->end_angle   = 0;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;
	image->radius = 0;
	image->end_x = x_Length;
	image->end_y = y_Length;
}

			  
/************************************************绘制圆弧*************************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
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
			  uint32_t y_Length)
{
	int i;

	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Arc;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = Graph_StartAngle;
	image->end_angle = Graph_EndAngle;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;

	image->radius = 0;
	image->end_x = x_Length;
	image->end_y = y_Length;
}

/************************************************绘制浮点型数据*******************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
       Graph_Size     字号
       Graph_Digit    小数位数   0~3
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
				float Graph_Float)
{
	int i;

	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Float;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = Graph_Size;
	image->end_angle = Graph_Digit;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;

	image->graph_FloInt = (int32_t)(Graph_Float*1000);
}

/************************************************绘制整型数据*********************************************
**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
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
				int32_t Graph_Int32)
{
	int i;

	for(i=0;i<3&&imagename[i]!='\0';i++)
		image->graphic_name[i]=imagename[i];
	image->operate_tpye = Graph_Operate;
	image->graphic_tpye = UI_Graph_Int;
	image->layer = Graph_Layer;
	image->color = Graph_Color;
	image->start_angle = Graph_Size;
	image->end_angle = 0;
	
	image->width = Graph_Width;
	image->start_x = Start_x;
	image->start_y = Start_y;

	image->graph_FloInt = Graph_Int32;
}

/************************************************绘制字符型数据*************************************************
浮点数据转字符：
	float Float = X.XXXXXXXX;      // 任意
   char str[Graph_Digit];         // 浮点数字符个数

	sprintf(str, "%.nf", Float);   // n 为所需浮点数输出显示位数  （不写n默认6位小数输出
整数数据转字符：
	int32_t Float = XXXXXXX;       // 任意
   char str[Graph_Digit];         // 整数字符个数

	sprintf(str, "%ndf", Float);   // n 为所需整数型输出显示位数  （不写n默认整数型全输出

**参数：*image Graph_Data类型变量指针，用于存放图形数据
       imagename[3]   图片名称，用于标识更改
       Graph_Operate   图片操作，见头文件
			#define UI_Graph_NoOperate   0    //空操作
			#define UI_Graph_ADD         1    //增加
			#define UI_Graph_Change      2    //修改
			#define UI_Graph_Del         3    //删除
       Graph_Layer    图层0-9
       Graph_Color    图形颜色
			#define UI_Color_Main         0  //红蓝主色
			#define UI_Color_Yellow       1  //黄色
			#define UI_Color_Green        2  //绿色
			#define UI_Color_Orange       3  //橙色
			#define UI_Color_Purplish_red 4  //紫红色
			#define UI_Color_Pink         5  //粉色
			#define UI_Color_Cyan         6  //青色
			#define UI_Color_Black        7  //黑色
			#define UI_Color_White        8  //白色
       Graph_Width    图形线宽
       Graph_Size     字号
       Graph_Digit    字符个数          strlen(Char_Data)
       Start_x、Start_x    开始坐标
       *Char_Data          待发送字符串开始地址
**********************************************************************************************************/

void Char_Draw(String_Data_t *image,
	           char imagename[3],
			   uint32_t Graph_Operate,
			   uint32_t Graph_Layer,
			   uint32_t Graph_Color,
			   uint32_t Graph_Width,
			   uint32_t Graph_Size,
			   uint32_t Graph_Digit,
			   uint32_t Start_x,
			   uint32_t Start_y,
			   char *Char_Data)
{
  int i;
  
  for(i=0;i<3&&imagename[i]!='\0';i++)
     image->Graph_Control.graphic_name[i]=imagename[i];
  image->Graph_Control.graphic_tpye = UI_Graph_Char;
  image->Graph_Control.operate_tpye = Graph_Operate;
  image->Graph_Control.layer = Graph_Layer;
  image->Graph_Control.color = Graph_Color;
  image->Graph_Control.width = Graph_Width;
  image->Graph_Control.start_x = Start_x;
  image->Graph_Control.start_y = Start_y;
  image->Graph_Control.start_angle = Graph_Size;
  image->Graph_Control.end_angle = Graph_Digit;
  
  for(i=0;i<Graph_Digit;i++)
  {
     image->show_Data[i]=*Char_Data;
     Char_Data++;
  }
}

/************************************************UI推送函数（使更改生效）*********************************
**参数： cnt   图形个数   （1、2、5、7）
        ...   图形变量参数

Tips：：该函数只能推送1，2，5，7个图形，其他数目协议未涉及
**********************************************************************************************************/

//int UI_SendGraph(int cnt,...)
//{
//	UI_Datahead_t UI_Datahead;
//	Graph_Data_t imageData;
//	uint16_t CMD_ID;
//	
//	va_list ap;
//	va_start(ap,cnt);
//	
//	UI_Datahead.UI_Packhead.SOF = UI_SOF;
//	// UI_Datahead.UI_Packhead.Data_Length
//	UI_Datahead.UI_Packhead.Seq = UI_Seq;
//	// CRC8
//	
//	CMD_ID = UI_CMD_Robo_Exchange;
//	
//	// UI_Datahead.UI_Data_Operate.Data_ID
//	UI_Datahead.UI_Data_Operate.Sender_ID = Robot_ID;
//	UI_Datahead.UI_Data_Operate.Receiver_ID = Cilent_ID;
//	
//	uint8_t data_pack1[UI_Graph1_byte] = {0};
//	uint8_t data_pack2[UI_Graph2_byte] = {0};
//	uint8_t data_pack5[UI_Graph5_byte] = {0};
//	uint8_t data_pack7[UI_Graph7_byte] = {0};
//	
//	switch(cnt)
//	{
//		case 1:
//			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw1;
//			UI_Datahead.UI_Packhead.Data_Length = UI_Graph1_byte - 9;
//			memcpy(data_pack1, &UI_Datahead.UI_Packhead, 5);
//			append_CRC8_check_sum(data_pack1,5);
//			memcpy(data_pack1 + 5, &CMD_ID, 2);
//			memcpy(data_pack1 + 7, &UI_Datahead.UI_Data_Operate, 6);
//			
//			for(int i = 0;i < cnt;i++)
//			{
//				imageData=va_arg(ap,Graph_Data_t);
//				memcpy(data_pack1+13, &imageData, 15);
//			}
//			
//			append_CRC16_check_sum(data_pack1, UI_Graph1_byte);
//			HAL_UART_Transmit(&huart6, data_pack1, UI_Graph1_byte,100000);
//			
//			break;
//		case 2:
//			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw2;
//			UI_Datahead.UI_Packhead.Data_Length = UI_Graph2_byte - 9;
//			memcpy(data_pack2, &UI_Datahead.UI_Packhead, 5);
//			append_CRC8_check_sum(data_pack2,5);
//			memcpy(data_pack2 + 5, &CMD_ID, 2);
//			memcpy(data_pack2 + 7, &UI_Datahead.UI_Data_Operate, 6);
//			
//			for(int i = 0;i < cnt;i++)
//			{
//				imageData=va_arg(ap,Graph_Data_t);
//				memcpy(data_pack2+13+15*i, &imageData, 15);
//			}
//			
//			append_CRC16_check_sum(data_pack2, UI_Graph2_byte);
//			HAL_UART_Transmit(&huart6, data_pack2, UI_Graph2_byte,100000);
//			
//			break;
//		case 5:
//			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw5;
//			UI_Datahead.UI_Packhead.Data_Length = UI_Graph5_byte - 9;
//			memcpy(data_pack5, &UI_Datahead.UI_Packhead, 5);
//			append_CRC8_check_sum(data_pack5,5);
//			memcpy(data_pack5 + 5, &CMD_ID, 2);
//			memcpy(data_pack5 + 7, &UI_Datahead.UI_Data_Operate, 6);
//			
//			for(int i = 0;i < cnt;i++)
//			{
//				imageData=va_arg(ap,Graph_Data_t);
//				memcpy(data_pack5+13+15*i, &imageData, 15);
//			}
//			
//			append_CRC16_check_sum(data_pack5, UI_Graph5_byte);
//			HAL_UART_Transmit(&huart6, data_pack5, UI_Graph5_byte,100000);
//			
//			break;
//		case 7:
//			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw7;
//			UI_Datahead.UI_Packhead.Data_Length = UI_Graph7_byte - 9;
//			memcpy(data_pack7, &UI_Datahead.UI_Packhead, 5);
//			append_CRC8_check_sum(data_pack7,5);
//			memcpy(data_pack7 + 5, &CMD_ID, 2);
//			memcpy(data_pack7 + 7, &UI_Datahead.UI_Data_Operate, 6);
//			
//			for(int i = 0;i < cnt;i++)
//			{
//				imageData=va_arg(ap,Graph_Data_t);
//				memcpy(data_pack7+13+15*i, &imageData, 15);
//			}
//			
//			append_CRC16_check_sum(data_pack7, UI_Graph7_byte);
//			HAL_UART_Transmit(&huart6, data_pack7, UI_Graph7_byte,100000);
//			
//			break;
//		default:
//			return (-1);
//	}
//	
//	
//	UI_Seq++;
//	return (0);
//}

///************************************************UI推送字符（使更改生效）*********************************
//**参数： image String_Data_t* 数据
//**********************************************************************************************************/

//void UI_SendChars(String_Data_t *image)
//{
//	UI_Datahead_t UI_Datahead;
//	uint8_t data_pack[60] = {0};
//	uint16_t CMD_ID;
//	
//	UI_Datahead.UI_Packhead.SOF = UI_SOF;
//	UI_Datahead.UI_Packhead.Seq = UI_Seq;
//	UI_Datahead.UI_Packhead.Data_Length = UI_Char_byte - 9;
//	
//	memcpy(data_pack, &UI_Datahead.UI_Packhead, 5);
//	
//	append_CRC8_check_sum(data_pack, 5);
//	
//	CMD_ID = 0x0301;
//	
//	memcpy(data_pack + 5, &CMD_ID, 2);
//	
//	
//	UI_Datahead.UI_Data_Operate.Data_ID = UI_Data_ID_DrawChar;
//	UI_Datahead.UI_Data_Operate.Sender_ID = Robot_ID;
//	UI_Datahead.UI_Data_Operate.Receiver_ID = Cilent_ID;
//	
//	memcpy(data_pack + 7, &UI_Datahead.UI_Data_Operate, 6);
//	
//	memcpy(data_pack + 13, &image->Graph_Control, 15);
//	
//	memcpy(data_pack + 28, image->show_Data, 30);
//	
//	append_CRC16_check_sum(data_pack, UI_Char_byte);
//	
//	HAL_UART_Transmit(&huart6, data_pack, UI_Char_byte, 100000);
//	
//	UI_Seq++;
//}
HAL_StatusTypeDef success = HAL_TIMEOUT;
static volatile uint8_t tx_complete = 1;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
   if (huart == &huart6) {
       tx_complete = 1;
   }
}
int UI_SendGraph(int cnt,...)
{
	while (!tx_complete) {  // 等待上一次传输完成
       // 可添加超时机制
   }
   tx_complete = 0;  // 设置忙碌状态
	UI_Datahead_t UI_Datahead;
	Graph_Data_t imageData;
	uint16_t CMD_ID;
	
	va_list ap;
	va_start(ap,cnt);
	
	UI_Datahead.UI_Packhead.SOF = UI_SOF;
	// UI_Datahead.UI_Packhead.Data_Length
	UI_Datahead.UI_Packhead.Seq = UI_Seq;
	// CRC8
	
	CMD_ID = UI_CMD_Robo_Exchange;
	
	// UI_Datahead.UI_Data_Operate.Data_ID
	UI_Datahead.UI_Data_Operate.Sender_ID = Robot_ID;
	UI_Datahead.UI_Data_Operate.Receiver_ID = Cilent_ID;
	
static	uint8_t data_pack1[UI_Graph1_byte] = {0};
static	uint8_t data_pack2[UI_Graph2_byte] = {0};
static	uint8_t data_pack5[UI_Graph5_byte] = {0};
static	uint8_t data_pack7[UI_Graph7_byte] = {0};
	
	switch(cnt)
	{
		case 1:
			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw1;
			UI_Datahead.UI_Packhead.Data_Length = UI_Graph1_byte - 9;
			memcpy(data_pack1, &UI_Datahead.UI_Packhead, 5);
			append_CRC8_check_sum(data_pack1,5);
			memcpy(data_pack1 + 5, &CMD_ID, 2);
			memcpy(data_pack1 + 7, &UI_Datahead.UI_Data_Operate, 6);
			
			for(int i = 0;i < cnt;i++)
			{
				imageData=va_arg(ap,Graph_Data_t);
				memcpy(data_pack1+13, &imageData, 15);
			}
			
			  append_CRC16_check_sum(data_pack1, UI_Graph1_byte);
//			success = HAL_UART_Transmit_DMA(&huart6, data_pack1, UI_Graph1_byte); 
			//HAL_UART_Transmit(&huart6, data_pack1, UI_Graph1_byte,100000);
				//HAL_UART_Transmit_DMA(&huart6, data_pack1, UI_Graph1_byte);
			//usart6_tx_dma_enable(data_pack1, UI_Graph1_byte);
			HAL_UART_Transmit_DMA(&huart6, data_pack1, UI_Graph1_byte);
			break;
		case 2:
			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw2;
			UI_Datahead.UI_Packhead.Data_Length = UI_Graph2_byte - 9;
			memcpy(data_pack2, &UI_Datahead.UI_Packhead, 5);
			append_CRC8_check_sum(data_pack2,5);
			memcpy(data_pack2 + 5, &CMD_ID, 2);
			memcpy(data_pack2 + 7, &UI_Datahead.UI_Data_Operate, 6);
			
			for(int i = 0;i < cnt;i++)
			{
				imageData=va_arg(ap,Graph_Data_t);
				memcpy(data_pack2+13+15*i, &imageData, 15);
			}
			
			append_CRC16_check_sum(data_pack2, UI_Graph2_byte);
			HAL_UART_Transmit_DMA(&huart6, data_pack2, UI_Graph2_byte); 
//			HAL_UART_Transmit(&huart6, data_pack2, UI_Graph2_byte,100000);
			
			break;
		case 5:
			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw5;
			UI_Datahead.UI_Packhead.Data_Length = UI_Graph5_byte - 9;
			memcpy(data_pack5, &UI_Datahead.UI_Packhead, 5);
			append_CRC8_check_sum(data_pack5,5);
			memcpy(data_pack5 + 5, &CMD_ID, 2);
			memcpy(data_pack5 + 7, &UI_Datahead.UI_Data_Operate, 6);
			
			for(int i = 0;i < cnt;i++)
			{
				imageData=va_arg(ap,Graph_Data_t);
				memcpy(data_pack5+13+15*i, &imageData, 15);
			}
			
			append_CRC16_check_sum(data_pack5, UI_Graph5_byte);
			HAL_UART_Transmit_DMA(&huart6, data_pack5, UI_Graph5_byte); 
//			HAL_UART_Transmit(&huart6, data_pack5, UI_Graph5_byte,100000);
			
			break;
		case 7:
			UI_Datahead.UI_Data_Operate.Data_ID=UI_Data_ID_Draw7;
			UI_Datahead.UI_Packhead.Data_Length = UI_Graph7_byte - 9;
			memcpy(data_pack7, &UI_Datahead.UI_Packhead, 5);
			append_CRC8_check_sum(data_pack7,5);
			memcpy(data_pack7 + 5, &CMD_ID, 2);
			memcpy(data_pack7 + 7, &UI_Datahead.UI_Data_Operate, 6);
			
			for(int i = 0;i < cnt;i++)
			{
				imageData=va_arg(ap,Graph_Data_t);
				memcpy(data_pack7+13+15*i, &imageData, 15);
			}
			
			append_CRC16_check_sum(data_pack7, UI_Graph7_byte);
		success =	HAL_UART_Transmit_DMA(&huart6, data_pack7, UI_Graph7_byte); 
//			HAL_UART_Transmit_DMA(&huart6, (uint8_t *)&data_pack7, UI_Graph7_byte);  
//			HAL_UART_Transmit(&huart6, data_pack7, UI_Graph7_byte,100000);
			
			break;
		default:
			return (-1);
	}
	
	
	UI_Seq++;
	return (0);
}

/************************************************UI推送字符（使更改生效）*********************************
**参数： image String_Data_t* 数据
**********************************************************************************************************/
int yui=0;
void UI_SendChars(String_Data_t *image)
{
	UI_Datahead_t UI_Datahead;
static	uint8_t data_pack[60] = {0};
	uint16_t CMD_ID;
	
	UI_Datahead.UI_Packhead.SOF = UI_SOF;
	UI_Datahead.UI_Packhead.Seq = UI_Seq;
	UI_Datahead.UI_Packhead.Data_Length = UI_Char_byte - 9;
	
	memcpy(data_pack, &UI_Datahead.UI_Packhead, 5);
	
	append_CRC8_check_sum(data_pack, 5);
	
	CMD_ID = 0x0301;
	
	memcpy(data_pack + 5, &CMD_ID, 2);
	
	
	UI_Datahead.UI_Data_Operate.Data_ID = UI_Data_ID_DrawChar;
	UI_Datahead.UI_Data_Operate.Sender_ID = Robot_ID;
	UI_Datahead.UI_Data_Operate.Receiver_ID = Cilent_ID;
	
	memcpy(data_pack + 7, &UI_Datahead.UI_Data_Operate, 6);
	
	memcpy(data_pack + 13, &image->Graph_Control, 15);
	
	memcpy(data_pack + 28, image->show_Data, 30);
	
	append_CRC16_check_sum(data_pack, UI_Char_byte);
	
	
	yui=HAL_UART_Transmit_DMA(&huart6, (uint8_t *)data_pack, UI_Char_byte);  
//	HAL_UART_Transmit(&huart6, data_pack, UI_Char_byte, 100000);
	
	UI_Seq++;
}

int wew;
/*************************************************哨兵自主决策指令推送函数**********************************
**参数： cmd Sentry_cmd_t* 数据
**********************************************************************************************************/
HAL_StatusTypeDef HAL_StatusType_t;
  uint8_t data_sentry_pack[UI_Sentry_byte] = {0};
	 Sentry_cmd_t Sentry_cmd_send;
void UI_SendSentry(Sentry_cmd_t *cmd)
{
 wew++;

   UI_Datahead_t UI_Datahead;
   
   uint16_t CMD_ID;
   
   UI_Datahead.UI_Packhead.SOF = UI_SOF;
   UI_Datahead.UI_Packhead.Seq = UI_Seq;
   UI_Datahead.UI_Packhead.Data_Length = UI_Sentry_byte - 9;
   
   memcpy(data_sentry_pack, &UI_Datahead.UI_Packhead, 5);
   
   append_CRC8_check_sum(data_sentry_pack, 5);
   
   CMD_ID = 0x0301;
   
   memcpy(data_sentry_pack + 5, &CMD_ID, 2);
   

   
   UI_Datahead.UI_Data_Operate.Data_ID = UI_Data_SENTRY_CMD;
   UI_Datahead.UI_Data_Operate.Sender_ID = Robot_ID;
   UI_Datahead.UI_Data_Operate.Receiver_ID = REFEREE_ServerID;
   
   memcpy(data_sentry_pack + 7, &UI_Datahead.UI_Data_Operate, 6);
   
   memcpy(data_sentry_pack + 13, &Sentry_cmd_send, 4);
   
   append_CRC16_check_sum(data_sentry_pack, UI_Sentry_byte);
   
HAL_StatusType_t  = HAL_UART_Transmit_DMA(&huart6,data_sentry_pack,UI_Sentry_byte);
 
// =     HAL_UART_Transmit(&huart6, data_pack, UI_Sentry_byte, 100);
   
   UI_Seq++;
}




/********************************************填充哨兵自主决策相关指令***************************************
**参数：*cmd Sentry_cmd_t类型变量指针，用于存放哨兵自主决策指令
       IF_Revive                  哨兵机器人是否确认复活
           #define SENTRY_NO_REVIVE     0                //确认不复活
           #define SENTRY_YES_REVIVE    1                //确认复活
       IF_Immediately_Revive      哨兵机器人是否确认兑换立即复活
           #define SENTRY_NO_IMMEDIATELY_REVIVE     0    //确认不兑换立即复活
           #define SENTRY_YES_IMMEDIATELY_REVIVE    1    //确认兑换立即复活
       Exchange_Projectile_Num    哨兵将要兑换的发弹量值
       IF_Remote_Exchange_HP      哨兵是否进行远程兑换血量
           #define SENTRY_NO_REMOTE_EXCHANGE_HP     0    //确认不进行远程兑换血量
           #define SENTRY_YES_REMOTE_EXCHANGE_HP    1    //确认进行远程兑换血量
PS:需在完成 哨兵自主决策相关指令初始化Sentry_Cmd_Fill_Init 后进行调用
**********************************************************************************************************/

//void Sentry_Cmd_Fill(Sentry_cmd_t *cmd,
//                     uint8_t IF_Revive,
//                     uint8_t IF_Immediately_Revive,
//                     uint16_t Exchange_Projectile_Num,
//                     uint8_t IF_Remote_Exchange_HP)
//{
//    cmd->if_revive = IF_Revive;
//    
//    cmd->if_immediately_revive = IF_Immediately_Revive;
//    if (Exchange_Projectile_Num)    // 需要兑换发弹量值
//    {
//        cmd->exchange_projectile_num += Exchange_Projectile_Num;
//    }
//		
//      if(Tx_Vision_Message.MyColor == CONST__BLUE)
//  {
//    Robot_ID=UI_Data_RobotID_RStandard1;
//  }
//  else Robot_ID=UI_Data_RobotID_BStandard1;
//    
//    cmd->remote_exchange_projectile_count=0;//远程兑换弹丸为0
//    if (IF_Remote_Exchange_HP)      // 进行远程兑换血量
//        cmd->remote_exchange_HP_count ++;
//}


