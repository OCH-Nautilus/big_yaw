/*
 * @Author: 孙羽
 * @Date: 2023-06-05 23:52:39
 * @LastEditors: 周灿
 * @LastEditTime: 2025-07-5 
 * @Telephone: 15235320302
 * @QQ: 984464809
  * @Description: 基于 RoboMaster 裁判系统串口协议附录 V1.9.0（2025.7.5）
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef REFEREE_H
#define REFEREE_H

#include "main.h"

#include "protocol.h"

typedef enum
{
    RED_HERO        = 1,
    RED_ENGINEER    = 2,
    RED_STANDARD_1  = 3,
    RED_STANDARD_2  = 4,
    RED_STANDARD_3  = 5,
    RED_AERIAL      = 6,
    RED_SENTRY      = 7,
    RED_DART        = 8,
    RED_RADAR       = 9,
    RED_OUTPOST     = 10,
    RED_BASE        = 11,
    
    BLUE_HERO       = 101,
    BLUE_ENGINEER   = 102,
    BLUE_STANDARD_1 = 103,
    BLUE_STANDARD_2 = 104,
    BLUE_STANDARD_3 = 105,
    BLUE_AERIAL     = 106,
    BLUE_SENTRY     = 107,
    BLUE_DART        = 108,
    BLUE_RADAR       = 109,
    BLUE_OUTPOST     = 110,
    BLUE_BASE        = 111,
} robot_id_t;

typedef enum
{
    PROGRESS_UNSTART        = 0,
    PROGRESS_PREPARE        = 1,
    PROGRESS_SELFCHECK      = 2,
    PROGRESS_5sCOUNTDOWN    = 3,
    PROGRESS_BATTLE         = 4,
    PROGRESS_CALCULATING    = 5,
} game_progress_t;


//winner_data
typedef enum
{ 
   DRAW = 0,
   RED_WIN,
   BLUE_WIN,
}winner_t;

//机器人扣血原因的枚举
typedef enum
{
  ARMOR_HURTED = 0,
  REFEREE_OFFLINE,
  INITIAL_SPEEDING,
  SHOT_OVER_HEAT,
  CHASSIS_OVER_POWER,
  ARMOR_CRASH,
  
}HP_deduction_reason_t;





/** 0x0001
 *  比赛状态数据，固定 3Hz 频率发送
 *  服务器→全体机器人  */
typedef __packed struct
{
    uint8_t game_type : 4;
    uint8_t game_progress : 4;
    uint16_t stage_remain_time;
	uint64_t SyncTimeStamp;
} game_state_t;

/** 0x0002
 *  比赛结果数据，比赛结束触发发送
 *  服务器→全体机器人  */
typedef __packed struct
{
    uint8_t winner;
} game_result_t;

/** 0x0003
 *  机器人血量数据，固定 3Hz 频率发送
 *  服务器→全体机器人  */
typedef __packed struct
{
	uint16_t red_1_robot_HP;
	uint16_t red_2_robot_HP; 
	uint16_t red_3_robot_HP; 
    uint16_t red_4_robot_HP; 
    uint16_t reserved1; 
    uint16_t red_7_robot_HP; 
    uint16_t red_outpost_HP;
    uint16_t red_base_HP; 
    uint16_t blue_1_robot_HP; 
    uint16_t blue_2_robot_HP; 
    uint16_t blue_3_robot_HP; 
    uint16_t blue_4_robot_HP; 
    uint16_t reserved2; 
    uint16_t blue_7_robot_HP; 
	uint16_t blue_outpost_HP;
	uint16_t blue_base_HP;
} game_robot_HP_t;

/** 0x0101
 *  场地事件数据，固定 3Hz 频率发送  
 *  服务器→己方全体机器人  */
typedef __packed struct
{
    /* uint32_t event_type; */
    // bit 0-2：己方补给站状态
    uint8_t we_depot_front : 1;         // 己方与兑换区不重叠的补给区占领状态，1 为已占领
    uint8_t we_depot_inside : 1;        // 己方与兑换区重叠的补给区占领状态，1 为已占领
    uint8_t we_recharge_area : 1;       // 己方补给区的占领状态，1 为已占领（仅 RMUL 适用）
    // bit 3-5：己方能量机关状态
    uint8_t we_small_energy_Mechanism : 1;    // 己方小能量机关的激活状态，1 为已激活
    uint8_t we_big_energy_Mechanism : 1;      // 己方大能量机关的激活状态，1 为已激活
	  // bit 5-8：己方高地占领状态
    uint8_t we_center_highland : 2;  // 己方中央高地的占领状态，1 为被己方占领，2 为被对方占领
    uint8_t we_keystone_highland : 2;   // 己方梯形高地的占领状态，1 为被己方占领，2 为被对方占领
    // bit 9-17 :对方飞镖最后一次击中己方前哨站或基地的时间（0-420，开局默认为 0）
    uint16_t thelast_dart_hit_time : 9;
    // bit 18-20：对方飞镖最后一次击中己方前哨站或基地的具体目标，开局
                //默认为 0，1 为击中前哨站，2 为击中基地固定目标，3 为击中基地随机
                //固定目标，4 为击中基地随机移动目标
    uint8_t thelast_dart_hit_goal : 3;
		// bit 21-22：中心增益点的占领情况，0 `为未被占领，1 为被己方占领，2 为被对方占领，3 为被双方占领。（仅 RMUL 适用）
    uint8_t center_gain : 2;
    //己方堡垒增益点的占领状态，0 为未被占领，1 为被己方占领，2 为被对方占领，3 为被双方占领。
    uint8_t we_fortress;
    //bit 25-31：保留位
    uint8_t reserved1 : 7;  
} event_data_t;


/** 0x0104
 *  裁判警告数据，己方判罚/判负时触发发送
 *  服务器→被处罚方全体机器人  */
typedef __packed struct
{

//己方最后一次受到判罚的等级：
//1：双方黄牌
//2：黄牌
//3：红牌
//4：判负
    uint8_t level;
    
 
    uint8_t offending_robot_id;
    
		uint8_t count;
} referee_warning_t;

/** 0x0105
 *  飞镖发射时间数据，固定 3Hz 频率发送
 *  服务器→己方全体机器人  */
typedef __packed struct
{

//己方飞镖发射剩余时间，单位：秒
	uint8_t dart_remaining_time;
	// bit 0-2：最近一次己方飞镖击中的目标，开局默认为 0，1 为击中前哨站，2 为击中
            //基地固定目标，3 为击中基地随机固定目标，4 为击中基地随机移动目标
	uint8_t  recent_dart_hit_goal : 3;
	// bit 3-5：对方最近被击中的目标累计被击中计数，开局默认为 0，至多为 4
	uint8_t opposing_recent_hit_count : 3;
	// bit 6-7：飞镖此时选定的击打目标，开局默认或未选定/选定前哨站时为 0，选中基
             //地固定目标为 1，选中基地随机固定目标为 2，选中基地随机移动目标为 3
	uint8_t we_selected_hit_goal : 2; 
	//bit 8-15：保留
	uint8_t reserved : 8;
} dart_remaining_time_t;

/** 0x0201
 *  机器人性能体系数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
   uint8_t robot_id;
   uint8_t robot_level;
	 uint16_t current_HP; 
   uint16_t maximum_HP;
	 uint16_t shooter_barrel_cooling_value;//机器人射击热量每秒冷却值
	 uint16_t shooter_barrel_heat_limit;//机器人射击热量上限 
	 uint16_t chassis_power_limit;
	 uint8_t power_management_gimbal_output : 1;
   uint8_t power_management_chassis_output : 1; 
   uint8_t power_management_shooter_output : 1;
	
} robot_status_t;

/** 0x0202
 *  实时功率热量数据，固定 50Hz 频率发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
    uint16_t reserved1;
    uint16_t reserved2;
     float reserved3;
    uint16_t buffer_energy;
    uint16_t shooter_17mm_1_barrel_heat;//第1个17mm发射机构的射击热量
    uint16_t shooter_17mm_2_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;

} power_heat_data_t;

/** 0x0203
 *  机器人位置数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
    float x;
    float y;
    float angle;
} robot_pos_t;

/** 0x0204
 *  机器人增益数据，固定 3Hz 频率发送
 *  服务器→对应机器人  */
typedef __packed struct
{
    uint8_t recovery_buff;
    uint8_t cooling_buff;
    uint8_t defence_buff;
	  uint8_t vulnerability_buff;//机器人负防御增益
    uint16_t attack_buff;
    
    uint8_t remaining_energy_greater_50 : 1;
    uint8_t remaining_energy_greater_30 : 1;
    uint8_t remaining_energy_greater_15 : 1;
    uint8_t remaining_energy_greater_5 : 1;
    uint8_t remaining_energy_greater_1 : 1;
    uint8_t reserved :3;
} buff_t;


/** 0x0206
 *  伤害状态数据，伤害发生后发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
    uint8_t armor_id : 4;
    
    //0：装甲模块被弹丸攻击导致扣血
    //1：裁判系统重要模块离线导致扣血
    //5：装甲模块受到撞击导致扣血
    uint8_t HP_deduction_reason : 4;
} hurt_data_t;

/** 0x0207
 *  实时射击数据，弹丸发射后发送
 *  主控模块→对应机器人  */
typedef __packed struct
{

    //    1：17mm 弹丸
    //    2：42mm 弹丸
    uint8_t bullet_type;
    uint8_t shooter_number;
    uint8_t launching_frequency;
    float initial_speed;
	  
} shoot_data_t;

/** 0x0208
 *  允许发弹量，固定 10Hz 频率发送
 *  服务器→己方英雄、步兵、哨兵、空中机器人  */
typedef __packed struct
{
    uint16_t projectile_allowance_17mm;
    uint16_t projectile_allowance_42mm;
    //剩余金币数量
    uint16_t remaining_gold_coin;
    //堡垒增益点提供的储备 17mm 弹丸允许发弹量
    uint16_t projectile_allowance_fortress;
} projectile_allowance_t;

/** 0x0209
 *  机器人 RFID 状态，固定 3Hz 频率发送
 *  服务器→己方装有 RFID 模块的机器人  */
typedef __packed struct
{
    /* uint32_t rfid_status; */
    uint8_t we_base_gain_point : 1;                           // 己方基地增益点
    uint8_t we_central_highland_gain_point : 1;              // 己方中央高地增益点
    uint8_t enemy_central_highland_gain_point : 1;           // 对方中央高地增益点
    uint8_t we_keystone_heights_gain_point : 1;              // 己方梯形高地增益点
    uint8_t enemy_keystone_heights_gain_point : 1;           // 对方梯形高地增益点
    uint8_t we_slope_gain_point_front : 1 ;                   // 己方飞坡增益点（靠近己方一侧飞坡前）
    uint8_t we_slope_gain_point_behind : 1 ;                  // 己方飞坡增益点（靠近己方一侧飞坡后）
    uint8_t opposing_slope_gain_point_front : 1 ;             // 对方飞坡增益点（靠近对方一侧飞坡前）
    uint8_t opposing_slope_gain_point_behind : 1 ;            // 对方飞坡增益点（靠近对方一侧飞坡后）
    uint8_t we_big_step_gain_point_below         :1;              // 己方地形跨越增益点（ 中央高地下方,大台阶）
    uint8_t we_big_step_gain_point_above         :1;              // 己方地形跨越增益点（中央高地上方,大台阶）
    uint8_t opposing_big_step_gain_point_below         :1;        // 对方地形跨越增益点（ 中央高地下方,大台阶）
    uint8_t opposing_big_step_gain_point_above         :1;        // 对方地形跨越增益点（中央高地上方,大台阶）
    uint8_t we_small_step_gain_point_below         :1;              // 己方地形跨越增益点（ 公路下方,小台阶）
    uint8_t we_small_step_gain_point_above         :1;              // 己方地形跨越增益点（公路上方,小台阶）
    uint8_t opposing_small_step_gain_point_below         :1;        // 对方地形跨越增益点（ 公路下方,小台阶）
    uint8_t opposing_small_step_gain_point_above         :1;        // 对方地形跨越增益点（公路上方,小台阶）
    uint8_t we_fortress_gain_points : 1 ;                      // 己方堡垒增益点
    uint8_t we_outpost_gain_points : 1 ;                       // 己方前哨站增益点
    uint8_t we_depot_gain_point : 1;                           //己方与兑换区不重叠的补给区/RMUL 补给区
    uint8_t we_depot_overlap_point: 1;                         //己方与兑换区重叠的补给区  兑换站
    uint8_t  we_large_resource_island_gain_point : 1 ;        // 己方大资源岛增益点
    uint8_t  opposing_large_resource_island_gain_point : 1 ;  // 对方大资源岛增益点
    uint8_t  central_gain_point : 1 ;  // 中心增益点（仅 RMUL 适用）
    uint8_t enemy_fortress_gain_points :1; //对方堡垒增益点
    uint8_t reserved :7;
 
} rfid_status_t;

/** 0x020A
 *  飞镖选手端指令数据，飞镖闸门上线后固定 10Hz 频率发送
 *  服务器→己方飞镖机器人  */
typedef __packed struct
{

//    当前飞镖发射站的状态：
//    1：关闭
//    2：正在开启或者关闭中
//    0：已经开启
	 uint8_t dart_launch_opening_status;
	 uint8_t reserved;
	 uint16_t target_change_time;
	 uint16_t latest_launch_cmd_time;
} dart_client_cmd_t;

/** 0x020B
 *  地面机器人位置数据，固定 1Hz 频率发送
 *  服务器→己方哨兵机器人  */
typedef __packed struct
{
    float hero_x;
    float hero_y;
    float engineer_x;
    float engineer_y;
    float standard_3_x;
    float standard_3_y;
    float standard_4_x;
    float standard_4_y;
    float reserved1;
    float reserved2;
} ground_robot_position_t;

/** 0x020C
 *  雷达标记进度数据，固定 1Hz 频率发送
 *  服务器→己方雷达机器人  */
typedef __packed struct
{
    uint8_t mark_hero_progress;
    uint8_t mark_engineer_progress;
    uint8_t mark_standard_3_progress;
    uint8_t mark_standard_4_progress;
    uint8_t mark_sentry_progress;
} radar_mark_data_t;

/** 0x020D
 *  哨兵自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方哨兵机器人  */
typedef __packed struct
{

//字节1数据//
	// bit 0-10：除远程兑换外，哨兵成功兑换的发弹量，开局为 0，在哨兵成功兑
              //换一定发弹量后，该值将变为哨兵成功兑换的发弹量值。
	  uint16_t sentry_exchange_shoot_num : 11;
	// bit 11-14：哨兵成功远程兑换发弹量的次数，开局为 0，在哨兵成功远程兑
              //换发弹量后，该值将变为哨兵成功远程兑换发弹量的次数
	  uint16_t sentry_exchange_shoot_time : 4;
	// bit 15-18：哨兵成功远程兑换血量的次数，开局为 0，在哨兵成功远程兑换
              //血量后，该值将变为哨兵成功远程兑换血量的次数。
	  uint16_t sentry_exchange_HP_time : 4;
  // bit 19：哨兵机器人当前是否可以确认免费复活，可以确认免费复活时值为
              //1，否则为 0。
    uint8_t sentry_if_free_revive :1;
  //bit 20：哨兵机器人当前是否可以兑换立即复活，可以兑换立即复活时值为
              //1，否则为 0。
    uint8_t sentry_if_instant_revive :1;
  //bit 21-30：哨兵机器人当前若兑换立即复活需要花费的金币数。
    uint16_t sentry_instant_revive_economy:10;
    
	// bit 30-31：保留
	  uint8_t reserve : 1;
    
//字节2数据//
//  bit 0：哨兵当前是否处于脱战状态，处于脱战状态时为 1，否则为 0。
     uint8_t sentry_if_out_fight :1;
//  bit 1-11：队伍 17mm 允许发弹量的剩余可兑换数。
    uint16_t we_17mm_remain_exchange_num :11;
    
    uint8_t reserved1 :4;
    
    
}sentry_info_t;

/** 0x020E
 *  雷达自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方雷达机器人  */
typedef __packed struct
{
 // bit 0-1：雷达是否拥有触发双倍易伤的机会，开局为 0，数值为雷达拥有触发双倍易伤的机会，至多为 2
	uint8_t trigger_double_vulnerability : 2;
 // bit 2：对方是否正在被触发双倍易伤 0：对方未被触发双倍易伤 1：对方正在被触发双倍易伤
	uint8_t if_opposing_trigger_double_vulnerability : 1;
 // bit 3-7：保留
	uint8_t reserve : 5;
} radar_info_t;


//机器人交互数据通过常规链路发送，其数据段包含一个统一的数据段头结构。数据段头结构包括内容 ID、
//发送者和接收者的 ID、内容数据段。机器人交互数据包的总长不超过 127 个字节，减去 frame_header、
//cmd_id 和 frame_tail 的 9 个字节以及数据段头结构的 6 个字节，故机器人交互数据的内容数据段最大
//为 112 个字节。
//每 1000 毫秒，英雄、工程、步兵、空中机器人、飞镖能够接收数据的上限为 3720 字节，雷达和哨兵机器
//人能够接收数据的上限为 5120 字节。
//由于存在多个内容 ID，但整个 cmd_id 上行频率最大为 30Hz，请合理安排带宽。
/** 0x0301
 *  机器人交互数据，发送方触发发送，频率上限为 10Hz  */
typedef __packed struct
{
	uint16_t data_cmd_id;
	uint16_t sender_ID;
	uint16_t receiver_ID;
	uint8_t  data[];
} robot_interaction_data_t;

/** 0x0302
 *  自定义控制器与机器人交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端图传连接的机器人  */
typedef __packed struct
{
    uint8_t data[30];
} custom_robot_data_t;

/** 0x0303
 *  选手端小地图交互数据，选手端触发发送
 *  选手端点击→服务器→发送方选择的己方机器人  */
typedef __packed struct
{
	float target_position_x;
	float target_position_y;
	uint8_t cmd_keyboard;
	uint8_t target_robot_ID;
	uint16_t cmd_source;
} map_command_t;

/** 0x0304
 *  键鼠遥控数据，固定 30Hz 频率发送
 *  客户端→选手端图传连接的机器人  */
typedef __packed struct
{
    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;
    int8_t left_button_down;
    int8_t right_button_down;
    
    

    uint16_t W_key : 1;
    uint16_t S_key : 1;
    uint16_t A_key : 1;
    uint16_t D_key : 1;
    uint16_t Shift_key : 1;
    uint16_t Ctrl_key : 1;
    uint16_t Q_key : 1;
    uint16_t E_key : 1;
    uint16_t R_key : 1;
    uint16_t F_key : 1;
    uint16_t G_key : 1;
    uint16_t Z_key : 1;
    uint16_t X_key : 1;
    uint16_t C_key : 1;
    uint16_t V_key : 1;
    uint16_t B_key : 1;

//    uint16_t keyboard_value;
    
    uint16_t reserved;
} remote_control_t;

/** 0x0305
 *  选手端小地图接收雷达数据，频率上限为 10Hz
 *  雷达→服务器→己方所有选手端  */
typedef __packed struct
{
	uint16_t hero_position_x; 
 uint16_t hero_position_y; 
 uint16_t engineer_position_x; 
 uint16_t engineer_position_y; 
 uint16_t infantry_3_position_x; 
 uint16_t infantry_3_position_y; 
 uint16_t infantry_4_position_x; 
 uint16_t infantry_4_position_y; 
 uint16_t infantry_5_position_x; 
 uint16_t infantry_5_position_y; 
 uint16_t sentry_position_x; 
 uint16_t sentry_position_y;
  
} map_robot_data_t;

/** 0x0306
 *  自定义控制器与选手端交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端  */
typedef __packed struct
{

  //键盘键值：
  // bit 0-7：按键 1 键值
  // bit 8-15：按键 2 键值
    uint8_t key1_value;
    uint8_t key2_value;
    
    
    uint16_t x_position:12;
    uint16_t mouse_left:4;
    uint16_t y_position:12;
    uint16_t mouse_right:4;
    uint16_t reserved;
} custom_client_data_t;

/** 0x0307
 *  选手端小地图接收哨兵数据，频率上限为 1Hz
 *  哨兵→己方云台手选手端  */
typedef __packed struct
{
    uint8_t intention;
    uint16_t start_position_x;
    uint16_t start_position_y;
    int8_t delta_x[49];
    int8_t delta_y[49];
	  uint16_t sender_id;
} map_data_t;

/** 0x0308
 *  选手端小地图接收机器人数据，频率上限为 3Hz 
 *  己方机器人→己方选手端  */
typedef __packed struct
{
		uint16_t sender_id;
		uint16_t receiver_id;
		uint8_t user_data[30];
} custom_info_t;
/** 0x0309
 *  自定义控制器接收机器人数据，频率上限为 10Hz
 *  己方机器人→对应操作手选手端连接的自定义控制器 自定义数据  */
typedef __packed struct
{
  uint8_t data[30];
  
}robot_custom_data_t;
// 裁判系统数据
extern frame_header_struct_t referee_receive_header;
extern frame_header_struct_t referee_send_header;
/** 0x0001
 *  比赛状态数据，固定 3Hz 频率发送
 *  服务器→全体机器人  */
extern game_state_t game_state;
/** 0x0002
 *  比赛结果数据，比赛结束触发发送
 *  服务器→全体机器人  */
extern game_result_t game_result;
/** 0x0003
 *  机器人血量数据，固定 3Hz 频率发送
 *  服务器→全体机器人  */
extern game_robot_HP_t game_robot_HP;
/** 0x0101
 *  场地事件数据，固定 3Hz 频率发送  
 *  服务器→己方全体机器人  */
extern event_data_t event_data;
/** 0x0104

 *  裁判警告数据，己方判罚/判负时触发发送
 *  服务器→被处罚方全体机器人  */
extern referee_warning_t referee_warning;
/** 0x0105
 *  飞镖发射时间数据，固定 3Hz 频率发送
 *  服务器→己方全体机器人  */
extern dart_remaining_time_t dart_remaining_time;
/** 0x0201
 *  机器人性能体系数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
extern robot_status_t robot_status;
/** 0x0202
 *  实时功率热量数据，固定 50Hz 频率发送
 *  主控模块→对应机器人  */
extern power_heat_data_t power_heat_data;
/** 0x0203
 *  机器人位置数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
extern robot_pos_t robot_pos;
/** 0x0204
 *  机器人增益数据，固定 3Hz 频率发送
 *  服务器→对应机器人  */
extern buff_t buff;
/** 0x0205
 *  空中支援时间数据，固定 10Hz 频率发送
 *  服务器→己方空中机器人  */
//extern air_support_data_t air_support_data;
/** 0x0206
 *  伤害状态数据，伤害发生后发送
 *  主控模块→对应机器人  */
extern hurt_data_t hurt_data;
extern uint8_t if_refresh;  // TODO
/** 0x0207
 *  实时射击数据，弹丸发射后发送
 *  主控模块→对应机器人  */
extern shoot_data_t shoot_data;
/** 0x0208
 *  允许发弹量，固定 10Hz 频率发送
 *  服务器→己方英雄、步兵、哨兵、空中机器人  */
extern projectile_allowance_t projectile_allowance;
/** 0x0209
 *  机器人 RFID 状态，固定 3Hz 频率发送
 *  服务器→己方装有 RFID 模块的机器人  */
extern rfid_status_t rfid_status;
/** 0x020A
 *  飞镖选手端指令数据，飞镖闸门上线后固定 10Hz 频率发送
 *  服务器→己方飞镖机器人  */
extern dart_client_cmd_t dart_client_cmd;
/** 0x020B
 *  地面机器人位置数据，固定 1Hz 频率发送
 *  服务器→己方哨兵机器人  */
extern ground_robot_position_t ground_robot_position;
/** 0x020C
 *  雷达标记进度数据，固定 1Hz 频率发送
 *  服务器→己方雷达机器人  */
extern radar_mark_data_t radar_mark_data;
 /** 0x020D
 *  哨兵自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方哨兵机器人  */
extern sentry_info_t sentry_info;
/** 0x020E
 *  雷达自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方雷达机器人  */
extern radar_info_t radar_info;
/** 0x0301
 *  机器人交互数据，发送方触发发送，频率上限为 10Hz  */
extern robot_interaction_data_t robot_interaction_data;
/** 0x0302
 *  自定义控制器与机器人交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端图传连接的机器人  */
extern custom_robot_data_t custom_robot_data;
/** 0x0303
 *  选手端小地图交互数据，选手端触发发送
 *  选手端点击→服务器→发送方选择的己方机器人  */
extern map_command_t map_command;
extern uint8_t if_update;  // TODO
/** 0x0304
 *  键鼠遥控数据，固定 30Hz 频率发送
 *  客户端→选手端图传连接的机器人  */
extern remote_control_t remote_control;
/** 0x0305
 *  选手端小地图接收雷达数据，频率上限为 10Hz
 *  雷达→服务器→己方所有选手端  */
extern map_robot_data_t map_robot_data;
/** 0x0306
 *  自定义控制器与选手端交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端  */
extern custom_client_data_t custom_client_data;
/** 0x0307
 *  选手端小地图接收哨兵数据，频率上限为 1Hz
 *  哨兵/半自动控制机器人→对应操作手选手端  */
extern map_data_t map_data;
/** 0x0308
 *  选手端小地图接收机器人数据，频率上限为 3Hz 
 *  己方机器人→己方选手端  */
extern custom_info_t custom_info;
/** 0x0309
 *  自定义控制器接收机器人数据，频率上限为 10Hz
 *  己方机器人→对应操作手选手端连接的自定义控制器  */
extern robot_custom_data_t robot_custom_data;
extern void init_referee_struct_data(void);
extern void referee_data_solve(uint8_t *frame);

extern void get_chassis_power_and_buffer(fp32 *power, fp32 *buffer);

extern uint8_t get_robot_id(void);
extern uint16_t SHOOT_NUM_1 ;
extern uint16_t SHOOT_NUM_2 ;
extern float speed_gun_1;
extern float speed_gun_2;
uint32_t Report_Shoot_NUM(void);

extern void get_shoot_heat0_limit_and_heat0(uint16_t *heat0_limit, uint16_t *heat0);
extern void get_shoot_heat1_limit_and_heat1(uint16_t *heat1_limit, uint16_t *heat1);
#endif
