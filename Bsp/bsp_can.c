#include "bsp_can.h"
#include "CAN_receive.h"
int zz=0;
void can_filter_init(void)
{
	zz++;
    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
	
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	  
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
    can_filter_st.SlaveStartFilterBank = 14;
    can_filter_st.FilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
    
    
   
}

/**
 * @brief 滤波器初始化函数
* @param 
*/

void can1_user_init(void)
{
  CAN_FilterTypeDef  can_filter;

  can_filter.FilterBank = 0;                       // filter 0   过滤器组选择，通常设置为0
  can_filter.FilterMode =  CAN_FILTERMODE_IDMASK;  // mask mode   过滤模式选择。常见的有CAN_FILTERMODE_IDMASK和CAN_FILTERMODE_IDLIST。前者表示根据ID进行掩码过滤，后者表示根据ID列表进行过滤
  can_filter.FilterScale = CAN_FILTERSCALE_32BIT;  //过滤器ID的大小
  can_filter.FilterIdHigh = 0;                     //过滤器匹配的ID的高位和低位值
  can_filter.FilterIdLow  = 0;
  can_filter.FilterMaskIdHigh = 0;                //ID掩码的高位和低位值
  can_filter.FilterMaskIdLow  = 0;                // set mask 0 to receive all can id
  can_filter.FilterFIFOAssignment = CAN_RX_FIFO0; // assign to fifo0  指定接收数据的FIFO（First In First Out）通道
  can_filter.FilterActivation = ENABLE;           // enable can filter  过滤器激活方式
  can_filter.SlaveStartFilterBank  = 14;          // only meaningful in dual can mode    选择从CAN实例的启动滤波器组
  
  HAL_CAN_ConfigFilter(&hcan1, &can_filter);        // init can filter
  HAL_CAN_Start(&hcan1);                          // start can1
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); // enable can1 rx interrupt 启用FIFO中断
}

void can2_user_init(void)
{
  CAN_FilterTypeDef  can_filter;
 
  can_filter.FilterBank = 14;                       // filter 0   通过FIFO1通道，必须从14开始选择，才能激活FIFO1的IDMASK模式
  can_filter.FilterMode =  CAN_FILTERMODE_IDMASK;  // mask mode
  can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter.FilterIdHigh = 0;
  can_filter.FilterIdLow  = 0;
  can_filter.FilterMaskIdHigh = 0;
  can_filter.FilterMaskIdLow  = 0;                // set mask 0 to receive all can id
  can_filter.FilterFIFOAssignment = CAN_RX_FIFO1; // assign to fifo0
  can_filter.FilterActivation = ENABLE;           // enable can filter
  can_filter.SlaveStartFilterBank  = 14;          // only meaningful in dual can mode
   
  HAL_CAN_ConfigFilter(&hcan2, &can_filter);        // init can filter
	
  HAL_CAN_Start(&hcan2);                          // start can1
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING); // enable can2 rx interrupt
}


 HAL_StatusTypeDef HAL_StatusTypeDef_t; 
void set_motor_current( CAN_HandleTypeDef *hcan,uint32_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4 )
{
	
  CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
	uint32_t    CAN_TX_MAILBOX;  //CAN_TX_MAILBOX0
    
  tx_header.StdId = id_range;       //
  tx_header.IDE   = CAN_ID_STD;     //标准ID，
  tx_header.RTR   = CAN_RTR_DATA;   //选择帧为数据帧  
  tx_header.DLC   = 8;              //帧数据长度

  tx_data[0] = (v1>>8)&0xff;
  tx_data[1] =    (v1)&0xff;
  tx_data[2] = (v2>>8)&0xff;
  tx_data[3] =    (v2)&0xff;
  tx_data[4] = (v3>>8)&0xff;
  tx_data[5] =    (v3)&0xff;
  tx_data[6] = (v4>>8)&0xff;
  tx_data[7] =    (v4)&0xff;
	
  HAL_StatusTypeDef_t=HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,&CAN_TX_MAILBOX); 
}

//MIT 控制模式发送函数

HAL_StatusTypeDef HAL_StatusTypeDef_t;
int oooo;
void ctrl_motor(CAN_HandleTypeDef* hcan,uint16_t id, float _pos, float _vel,float _KP, float _KD, float _torq)//_pos位置 _vel速度 _torq转距
{
uint16_t pos_tmp,vel_tmp,kp_tmp,kd_tmp,tor_tmp;
	pos_tmp = float_to_uint(_pos, P_MIN, P_MAX, 16);
	vel_tmp = float_to_uint(_vel, V_MIN, V_MAX, 12);
	kp_tmp  = float_to_uint(_KP, KP_MIN, KP_MAX, 12);
	kd_tmp  = float_to_uint(_KD, KD_MIN, KD_MAX, 12);
	tor_tmp = float_to_uint(_torq, T_MIN, T_MAX, 12);
	
	CAN_TxHeaderTypeDef tx_header;
	uint8_t             tx_data[8];
	
	uint32_t	pTxMailbox;

	tx_header.StdId = id
	;
	tx_header.IDE   = CAN_ID_STD;
	tx_header.RTR   = CAN_RTR_DATA;
	tx_header.DLC   = 8;
	
	tx_data[0] = (pos_tmp >> 8);
	tx_data[1] = pos_tmp;
	tx_data[2] = (vel_tmp >> 4);
	tx_data[3] = ((vel_tmp&0xF)<<4)|(kp_tmp>>8);
	tx_data[4] = kp_tmp;
	tx_data[5] = (kd_tmp >> 4);
	tx_data[6] = ((kd_tmp&0xF)<<4)|(tor_tmp>>8);
	tx_data[7] = tor_tmp;
	
	HAL_StatusTypeDef_t=HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,&pTxMailbox); 
	if(HAL_StatusTypeDef_t==HAL_ERROR)
	{
		oooo++;
	}
}
//位置速度模式
void ctrl_motor_pv(CAN_HandleTypeDef* hcan,uint16_t id, float _pos, float _vel)//_pos是弧度制，_vel从1开始给（定值）
{
	uint8_t *pbuf,*vbuf;
  pbuf=(uint8_t*)&_pos;
  vbuf=(uint8_t*)&_vel;
	
	CAN_TxHeaderTypeDef tx_header;
	uint8_t             tx_data[8];
	
	uint32_t	pTxMailbox;
	tx_header.StdId = id;
	tx_header.IDE   = CAN_ID_STD;
	tx_header.RTR   = CAN_RTR_DATA;
	tx_header.DLC   = 0x08;
	
	tx_data[0] =*pbuf;
	tx_data[1] =*(pbuf+1);
	tx_data[2] =*(pbuf+2);
	tx_data[3] =*(pbuf+3);
	tx_data[4] =*vbuf;
	tx_data[5] =*(vbuf+1);
	tx_data[6] =*(vbuf+2);
	tx_data[7] =*(vbuf+3);
	
	
	HAL_StatusTypeDef_t=HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,&pTxMailbox);
}

void damiao_init(CAN_HandleTypeDef* hcan,uint16_t id)//4310使能
{
	CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
	uint32_t    CAN_TX_MAILBOX;  //CAN_TX_MAILBOX0
		   
  tx_header.StdId = id;       //
  tx_header.IDE   = CAN_ID_STD;     //标准ID，
  tx_header.RTR   = CAN_RTR_DATA;   //选择帧为数据帧  
  tx_header.DLC   = 8;              //帧数据长度
	
	tx_data[0]=tx_data[1]=tx_data[2]=tx_data[3]=
	tx_data[4]=tx_data[5]=tx_data[6]=0xFF;
	tx_data[7]=0xFC;
	HAL_StatusTypeDef_t=HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,&CAN_TX_MAILBOX); 
}

void damiao_exit(CAN_HandleTypeDef* hcan,uint16_t id)//4310失能
{
	CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
	uint32_t    CAN_TX_MAILBOX;  //CAN_TX_MAILBOX0
		   
  tx_header.StdId = id;       //
  tx_header.IDE   = CAN_ID_STD;     //标准ID，
  tx_header.RTR   = CAN_RTR_DATA;   //选择帧为数据帧  
  tx_header.DLC   = 8;              //帧数据长度
	
	tx_data[0]=tx_data[1]=tx_data[2]=tx_data[3]=
	tx_data[4]=tx_data[5]=tx_data[6]=0xFF;
	tx_data[7]=0xFD;
	HAL_StatusTypeDef_t=HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,&CAN_TX_MAILBOX); 
}

void damiao_record(CAN_HandleTypeDef* hcan,uint16_t id)
{
	CAN_TxHeaderTypeDef tx_header;
  uint8_t             tx_data[8];
	uint32_t    CAN_TX_MAILBOX;  //CAN_TX_MAILBOX0
		   
  tx_header.StdId = id;       //
  tx_header.IDE   = CAN_ID_STD;     //标准ID，
  tx_header.RTR   = CAN_RTR_DATA;   //选择帧为数据帧  
  tx_header.DLC   = 8;              //帧数据长度
	
	tx_data[0]=tx_data[1]=tx_data[2]=tx_data[3]=
	tx_data[4]=tx_data[5]=tx_data[6]=0xFF;
	tx_data[7]=0xFE;
	HAL_StatusTypeDef_t=HAL_CAN_AddTxMessage(hcan, &tx_header, tx_data,&CAN_TX_MAILBOX); 
}
