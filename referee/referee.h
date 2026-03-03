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
    RED_HERO = 1,
    RED_ENGINEER = 2,
    RED_STANDARD_1 = 3,
    RED_STANDARD_2 = 4,
    RED_STANDARD_3 = 5,
    RED_AERIAL = 6,
    RED_SENTRY = 7,
    RED_DART = 8,
    RED_RADAR = 9,
    RED_OUTPOST = 10,
    RED_BASE = 11,

    BLUE_HERO = 101,
    BLUE_ENGINEER = 102,
    BLUE_STANDARD_1 = 103,
    BLUE_STANDARD_2 = 104,
    BLUE_STANDARD_3 = 105,
    BLUE_AERIAL = 106,
    BLUE_SENTRY = 107,
    BLUE_DART = 108,
    BLUE_RADAR = 109,
    BLUE_OUTPOST = 110,
    BLUE_BASE = 111,
} robot_id_t;

typedef enum
{
    PROGRESS_UNSTART = 0,
    PROGRESS_PREPARE = 1,
    PROGRESS_SELFCHECK = 2,
    PROGRESS_5sCOUNTDOWN = 3,
    PROGRESS_BATTLE = 4,
    PROGRESS_CALCULATING = 5,
} game_progress_t;

// winner_data
typedef enum
{
    DRAW = 0,
    RED_WIN,
    BLUE_WIN,
} winner_t;

// 机器人扣血原因的枚举
typedef enum
{
    ARMOR_HURTED = 0,
    REFEREE_OFFLINE,
    INITIAL_SPEEDING,
    SHOT_OVER_HEAT,
    CHASSIS_OVER_POWER,
    ARMOR_CRASH,

} HP_deduction_reason_t;

/** 0x0001
 *  比赛状态数据，固定 1Hz 频率发送
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
    uint16_t we_1_robot_HP;
    uint16_t we_2_robot_HP;
    uint16_t we_3_robot_HP;
    uint16_t we_4_robot_HP;
    uint16_t reserved1;
    uint16_t we_7_robot_HP;
    uint16_t we_outpost_HP;
    uint16_t we_base_HP;
	
} game_robot_HP_t;

/** 0x0101
 *  场地事件数据，固定 1Hz 频率发送
 *  服务器→己方全体机器人  */
typedef __packed struct
{
    // bit 0-2：己方补给站状态（3位）
    uint32_t we_depot_front : 1;        // bit0：己方与兑换区不重叠的补给区占领状态
    uint32_t we_depot_inside : 1;       // bit1：己方与兑换区重叠的补给区占领状态
    uint32_t we_recharge_area : 1;      // bit2：己方补给区的占领状态（仅RMUL适用）
    
    // bit 3-6：己方能量机关状态（4位）
    uint32_t we_small_energy_mech : 2;  // bit3-4：小能量机关激活状态（0未激活/1已激活/2正在激活）
    uint32_t we_big_energy_mech : 2;    // bit5-6：大能量机关激活状态（0未激活/1已激活/2正在激活）
    
    // bit 7-10：己方高地占领状态（4位）
    uint32_t we_center_highland : 2;    // bit7-8：中央高地占领状态（1己方占领/2对方占领）
    uint32_t we_keystone_highland : 2;  // bit9-10：梯形高地占领状态（1己方占领/2对方占领）
    
    // bit 11-19：对方飞镖最后一次击中时间（9位，0-420）
    uint32_t last_dart_hit_time : 9;
    
    // bit 20-22：对方飞镖最后一次击中目标（3位）
    uint32_t last_dart_hit_target : 3;  // 0无/1前哨站/2基地固定/3基地随机固定/4基地随机移动/5基地末端移动
    
    // bit 23-29：增益点占领状态（7位）
    uint32_t center_gain_point : 2;     // bit23-24：中心增益点（0未占/1己方/2对方/3双方，仅RMUL）
    uint32_t we_fortress_gain : 2;      // bit25-26：己方堡垒增益点（0未占/1己方/2对方/3双方）
    uint32_t we_outpost_gain : 2;       // bit27-28：己方前哨站增益点（0未占/1己方/2对方）
    uint32_t we_base_gain : 1;          // bit29：己方基地增益点（1已占领）
    
    // bit 30-31：保留位（2位）
    uint32_t reserved : 2;
} event_data_t;

/** 0x0104
 *  裁判警告数据，己方判罚/判负时触发发送
 *  服务器→被处罚方全体机器人  */
typedef __packed struct
{

    // 己方最后一次受到判罚的等级：
    // 1：双方黄牌
    // 2：黄牌
    // 3：红牌
    // 4：判负
    uint8_t level;

    uint8_t offending_robot_id;

    uint8_t count;
} referee_warning_t;

/** 0x0105
 *  飞镖发射时间数据，固定 1Hz 频率发送
 *  服务器→己方全体机器人
 *  注：结构体总长度 = 1字节（剩余时间） + 2字节（飞镖信息） = 3字节，与协议表格完全匹配
 */
typedef __packed struct
{
    // 字节偏移0，大小1：己方飞镖发射剩余时间（单位：秒）
    uint8_t dart_remaining_time;

    // 字节偏移1，大小2（uint16_t）：飞镖信息位域（bit0~15）
    uint16_t recent_dart_hit_goal : 3;      // bit0-2：最近一次己方飞镖击中的目标
                                            // 0=默认/未击中 1=击中前哨站 2=击中基地固定目标,3=击中基地随机固定目标 4=击中基地随机移动目标 5=击中基地末端移动目标
    uint16_t opposing_recent_hit_count : 3; // bit3-5：对方最近被击中目标的累计击中次数（0~4）
    uint16_t we_selected_hit_goal : 3;      // bit6-8：飞镖当前选定的击打目标
                                            // 0=未选定/选定前哨站 1=选中基地固定目标 2=选中基地随机固定目标,3=选中基地随机移动目标 | 4=选中基地末端移动目标
    uint16_t reserved : 7;                  // bit9-15：保留位
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
    uint16_t shooter_barrel_cooling_value;
    uint16_t shooter_barrel_heat_limit;
    uint16_t chassis_power_limit;
    uint8_t power_management_gimbal_output : 1;
    uint8_t power_management_chassis_output : 1;
    uint8_t power_management_shooter_output : 1;

} robot_status_t;

/** 0x0202
 *  实时功率热量数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
    uint16_t reserved1;
    uint16_t reserved2;
    float reserved3;
    uint16_t buffer_energy;
    uint16_t shooter_17mm_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;
} power_heat_data_t;

/** 0x0203
 *  机器人位置数据，固定 1Hz 频率发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
    float x;
    float y;
    float angle;
} robot_pos_t;

/** 0x0204
 *  机器人增益数据，固定 3Hz 频率发送
 *  服务器→对应机器人
 */
typedef __packed struct
{
    uint8_t recovery_buff;      // 字节偏移0，大小1：机器人回血增益（百分比）
    uint16_t cooling_buff;      // 字节偏移1，大小2：射击热量冷却增益具体值（修正为uint16_t，匹配协议）
    uint8_t defence_buff;       // 字节偏移3，大小1：机器人防御增益（百分比）
    uint8_t vulnerability_buff; // 字节偏移4，大小1：机器人负防御增益（百分比）
    uint16_t attack_buff;       // 字节偏移5，大小2：机器人攻击增益（百分比）
    // 字节偏移7，大小1：剩余能量值反馈
    __packed union
    {
        uint8_t remaining_energy;
        __packed struct
        {
            uint8_t energy_ge125 : 1; // bit0：剩余能量≥125%时为1
            uint8_t energy_ge100 : 1; // bit1：剩余能量≥100%时为1
            uint8_t energy_ge50 : 1;  // bit2：剩余能量≥50%时为1
            uint8_t energy_ge30 : 1;  // bit3：剩余能量≥30%时为1
            uint8_t energy_ge15 : 1;  // bit4：剩余能量≥15%时为1
            uint8_t energy_ge5 : 1;   // bit5：剩余能量≥5%时为1
            uint8_t energy_ge1 : 1;   // bit6：剩余能量≥1%时为1
            uint8_t reserve : 1;      // bit7：保留位（≥50%时为1，即0x80）
        } energy_bits;
    } energy_info;
} buff_t;

/** 0x0206
 *  伤害状态数据，伤害发生后发送
 *  主控模块→对应机器人  */
typedef __packed struct
{
    uint8_t armor_id : 4;

    // 0：装甲模块被弹丸攻击导致扣血
    // 1：裁判系统重要模块离线导致扣血
    // 5：装甲模块受到撞击导致扣血
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
    // 剩余金币数量
    uint16_t remaining_gold_coin;
    // 堡垒增益点提供的储备 17mm 弹丸允许发弹量
    uint16_t projectile_allowance_fortress;
} projectile_allowance_t;

/** 0x0209
 *  机器人 RFID 状态，固定 3Hz 频率发送
 *  服务器→己方装有 RFID 模块的机器人
 *  注：结构体总长度 = 4字节（rfid_status） + 1字节（rfid_status_2） = 5字节，与协议表格完全匹配
 */
typedef __packed struct
{
    // 字节偏移0，4字节：32位增益点状态位域（bit0~bit31）
    __packed union
    {
        uint32_t rfid_status;
        __packed struct
        {
            uint32_t we_base_gain_point : 1;                        // bit0: 己方基地增益点
            uint32_t we_central_highland_gain_point : 1;            // bit1: 己方中央高地增益点
            uint32_t enemy_central_highland_gain_point : 1;         // bit2: 对方中央高地增益点
            uint32_t we_keystone_heights_gain_point : 1;            // bit3: 己方梯形高地增益点
            uint32_t enemy_keystone_heights_gain_point : 1;         // bit4: 对方梯形高地增益点
            uint32_t we_slope_gain_point_front : 1;                 // bit5: 己方飞坡前增益点
            uint32_t we_slope_gain_point_behind : 1;                // bit6: 己方飞坡后增益点
            uint32_t opposing_slope_gain_point_front : 1;           // bit7: 对方飞坡前增益点
            uint32_t opposing_slope_gain_point_behind : 1;          // bit8: 对方飞坡后增益点
            uint32_t we_big_step_gain_point_below : 1;              // bit9: 己方中央高地下方大台阶
            uint32_t we_big_step_gain_point_above : 1;              // bit10: 己方中央高地上方大台阶
            uint32_t opposing_big_step_gain_point_below : 1;        // bit11: 对方中央高地下方大台阶
            uint32_t opposing_big_step_gain_point_above : 1;        // bit12: 对方中央高地上方大台阶
            uint32_t we_small_step_gain_point_below : 1;            // bit13: 己方公路下方小台阶
            uint32_t we_small_step_gain_point_above : 1;            // bit14: 己方公路上方小台阶
            uint32_t opposing_small_step_gain_point_below : 1;      // bit15: 对方公路下方小台阶
            uint32_t opposing_small_step_gain_point_above : 1;      // bit16: 对方公路上方小台阶
            uint32_t we_fortress_gain_points : 1;                   // bit17: 己方堡垒增益点
            uint32_t we_outpost_gain_points : 1;                    // bit18: 己方前哨站增益点
            uint32_t we_depot_gain_point : 1;                       // bit19: 己方非重叠补给区
            uint32_t we_depot_overlap_point : 1;                    // bit20: 己方重叠补给区
            uint32_t we_large_resource_island_gain_point : 1;       // bit21: 己方大资源岛（装配增益点）
            uint32_t opposing_large_resource_island_gain_point : 1; // bit22: 对方大资源岛（装配增益点）
            uint32_t central_gain_point : 1;                        // bit23: 中心增益点（仅RMUL适用）
            uint32_t enemy_fortress_gain_points : 1;                // bit24: 对方堡垒增益点
            uint32_t enemy_outpost_gain_points : 1;                 // bit25: 对方前哨站增益点
            uint32_t we_tunnel_road_below : 1;                      // bit26: 己方隧道-公路下方
            uint32_t we_tunnel_road_mid : 1;                        // bit27: 己方隧道-公路中间
            uint32_t we_tunnel_road_above : 1;                      // bit28: 己方隧道-公路上方
            uint32_t we_tunnel_keystone_low : 1;                    // bit29: 己方隧道-梯形高地低处
            uint32_t we_tunnel_keystone_mid : 1;                    // bit30: 己方隧道-梯形高地中间
            uint32_t we_tunnel_keystone_high : 1;                   // bit31: 己方隧道-梯形高地高处
        } status_bits;
    } rfid_info;

    // 字节偏移4，1字节：8位增益点状态位域（bit0~bit5）
    __packed union
    {
        uint8_t rfid_status_2;
        __packed struct
        {
            uint8_t enemy_tunnel_road_below : 1;    // bit0: 对方隧道-公路下方
            uint8_t enemy_tunnel_road_mid : 1;      // bit1: 对方隧道-公路中间
            uint8_t enemy_tunnel_road_above : 1;    // bit2: 对方隧道-公路上方
            uint8_t enemy_tunnel_keystone_low : 1;  // bit3: 对方隧道-梯形高地低处
            uint8_t enemy_tunnel_keystone_mid : 1;  // bit4: 对方隧道-梯形高地中间
            uint8_t enemy_tunnel_keystone_high : 1; // bit5: 对方隧道-梯形高地高处
            uint8_t reserved : 2;                   // bit6~7: 保留位
        } status2_bits;
    } rfid_info2;
} rfid_status_t;

/** 0x020A
 *  飞镖选手端指令数据，飞镖闸门上线后固定 3Hz 频率发送
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
 *  服务器→己方雷达机器人
 */
typedef __packed struct
{
    // 字节偏移0，2字节：16位标记进度位域（bit0~bit15）
    __packed union
    {
        uint16_t mark_progress; // 整体读取
        __packed struct
        {
            uint16_t enemy_hero_vulnerable : 1;      // bit0: 对方1号英雄易伤（标记进度≥100为1，<100为0）
            uint16_t enemy_engineer_vulnerable : 1;  // bit1: 对方2号工程易伤
            uint16_t enemy_infantry3_vulnerable : 1; // bit2: 对方3号步兵易伤
            uint16_t enemy_infantry4_vulnerable : 1; // bit3: 对方4号步兵易伤
            uint16_t enemy_air_special_mark : 1;     // bit4: 对方空中机器人特殊标识
            uint16_t enemy_sentry_vulnerable : 1;    // bit5: 对方哨兵易伤
            uint16_t own_hero_special_mark : 1;      // bit6: 己方1号英雄特殊标识（≥50为1，<50为0）
            uint16_t own_engineer_special_mark : 1;  // bit7: 己方2号工程特殊标识
            uint16_t own_infantry3_special_mark : 1; // bit8: 己方3号步兵特殊标识
            uint16_t own_infantry4_special_mark : 1; // bit9: 己方4号步兵特殊标识
            uint16_t own_air_special_mark : 1;       // bit10: 己方空中机器人特殊标识
            uint16_t own_sentry_special_mark : 1;    // bit11: 己方哨兵特殊标识
            uint16_t reserved : 4;                   // bit12~15: 保留位
        } mark_bits;
    } mark_info;
} radar_mark_data_t;

/** 0x020D
 *  哨兵自主决策信息同步，固定以1Hz频率发送
 *  服务器→己方哨兵机器人
 */
typedef __packed struct
{
    // 字节偏移0，4字节：32位哨兵核心信息（bit0~bit31）
    __packed union
    {
        uint32_t sentry_core_info; // sentry_info 整体4字节读取（用于校验/传输）
        __packed struct
        {
            uint32_t sentry_exchange_shoot_num : 11;     // bit0-10：非远程兑换的发弹量
            uint32_t sentry_exchange_shoot_time : 4;     // bit11-14：远程兑换发弹次数
            uint32_t sentry_exchange_HP_time : 4;        // bit15-18：远程兑换血量次数
            uint32_t sentry_if_free_revive : 1;          // bit19：可确认免费复活（1=是）
            uint32_t sentry_if_instant_revive : 1;       // bit20：可兑换立即复活（1=是）
            uint32_t sentry_instant_revive_economy : 10; // bit21-30：立即复活所需金币
            uint32_t reserved_core : 1;                  // bit31：保留位
        } core_bits;
    } core_info;

    // 字节偏移4，2字节：16位哨兵状态信息（bit0~bit15）
    __packed union
    {
        uint16_t sentry_state_info; //sentry_info_2 整体2字节读取（用于校验/传输）
        __packed struct
        {
            uint16_t sentry_if_out_fight : 1;          // bit0：脱战状态（1=脱战）
            uint16_t we_17mm_remain_exchange_num : 11; // bit1-11：17mm发弹剩余可兑换数
            uint16_t sentry_posture : 2;               // bit12-13：姿态（1=进攻/2=防御/3=移动）
            uint16_t energy_mechanism_activatable : 1; // bit14：能量机关可激活（1=可激活）
            uint16_t reserved_state : 1;               // bit15：保留位
        } state_bits;
    } state_info;
} sentry_info_t;

/** 0x020E
 *  雷达自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方雷达机器人  */
typedef __packed struct
{
    // bit 0-1：雷达拥有触发双倍易伤的机会（0~2）
    uint8_t trigger_double_vulnerability : 2;
    // bit 2：对方是否正在被触发双倍易伤（0=否/1=是）
    uint8_t if_opposing_trigger_double_vulnerability : 1;
    // bit 3-4：己方加密等级（开局1，最高3）
    uint8_t own_encryption_level : 2;
    // bit 5：当前是否可以修改密钥（1=可修改）
    uint8_t if_can_modify_key : 1;
    // bit 6-7：保留位
    uint8_t reserve : 2;
} radar_info_t;

// 机器人交互数据通过常规链路发送，其数据段包含一个统一的数据段头结构。数据段头结构包括内容 ID、
// 发送者和接收者的 ID、内容数据段。机器人交互数据包的总长不超过 127 个字节，减去 frame_header、
// cmd_id 和 frame_tail 的 9 个字节以及数据段头结构的 6 个字节，故机器人交互数据的内容数据段最大
// 为 112 个字节。
// 每 1000 毫秒，英雄、工程、步兵、空中机器人、飞镖能够接收数据的上限为 3720 字节，雷达和哨兵机器
// 人能够接收数据的上限为 5120 字节。
// 由于存在多个内容 ID，但整个 cmd_id 上行频率最大为 30Hz，请合理安排带宽。
/** 0x0301
 *  机器人交互数据，发送方触发发送，频率上限为 10Hz  */
typedef __packed struct
{
    uint16_t data_cmd_id;
    uint16_t sender_ID;
    uint16_t receiver_ID;
    uint8_t data[];
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
	
		uint16_t reserved1;
		uint16_t reserved2;
	
    uint16_t sentry_position_x;
    uint16_t sentry_position_y;

} map_robot_data_t;

/** 0x0306
 *  自定义控制器与选手端交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端  */
typedef __packed struct
{

    // 键盘键值：
    //  bit 0-7：按键 1 键值
    //  bit 8-15：按键 2 键值
    uint8_t key1_value;
    uint8_t key2_value;

    uint16_t x_position : 12;
    uint16_t mouse_left : 4;
    uint16_t y_position : 12;
    uint16_t mouse_right : 4;
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

} robot_custom_data_t;

/** 0x0310
 *  机器人发送给自定义客户端的数据，频率上限为 50Hz
 *  己方机器人 → 图传链路 → 对应操作手选手端连接的自定义客户端*/
typedef __packed struct
{
    uint8_t data[300]; // 字节偏移0-299：300字节自定义数据区
} robot_custom_data_2_t;

/** 0x0311
 *  自定义客户端发送给机器人的自定义指令，频率上限为 75Hz
 *  对应操作手选手端连接的自定义客户端 → 图传链路 → 己方机器人 */
typedef __packed struct
{
    uint8_t data[30]; // 字节偏移0-29：30字节自定义指令区，可承载操作指令、参数配置等自定义数据
} robot_custom_data_3_t;

/** 0x0A01
 *  对方机器人的位置坐标，频率上限为 10Hz
 *  信号发射源 → 雷达（雷达无线链路） */
typedef __packed struct
{
    uint16_t enemy_hero_x;      // 字节偏移0-1：对方英雄机器人位置x轴坐标，单位：cm
    uint16_t enemy_hero_y;      // 字节偏移2-3：对方英雄机器人位置y轴坐标，单位：cm
    uint16_t enemy_engineer_x;  // 字节偏移4-5：对方工程机器人位置x轴坐标，单位：cm
    uint16_t enemy_engineer_y;  // 字节偏移6-7：对方工程机器人位置y轴坐标，单位：cm
    uint16_t enemy_infantry3_x; // 字节偏移8-9：对方3号步兵机器人位置x轴坐标，单位：cm
    uint16_t enemy_infantry3_y; // 字节偏移10-11：对方3号步兵机器人位置y轴坐标，单位：cm
    uint16_t enemy_infantry4_x; // 字节偏移12-13：对方4号步兵机器人位置x轴坐标，单位：cm
    uint16_t enemy_infantry4_y; // 字节偏移14-15：对方4号步兵机器人位置y轴坐标，单位：cm
    uint16_t enemy_air_x;       // 字节偏移16-17：对方空中机器人位置x轴坐标，单位：cm
    uint16_t enemy_air_y;       // 字节偏移18-19：对方空中机器人位置y轴坐标，单位：cm
    uint16_t enemy_sentry_x;    // 字节偏移20-21：对方哨兵机器人位置x轴坐标，单位：cm
    uint16_t enemy_sentry_y;    // 字节偏移22-23：对方哨兵机器人位置y轴坐标，单位：cm
} enemy_robot_position_t;

/** 0x0A02
 *  对方机器人的血量信息，频率上限为 10Hz
 *  信号发射源 → 雷达（雷达无线链路）
 */
typedef __packed struct
{
    uint16_t enemy_hero1_hp;     // 字节偏移0-1：对方1号英雄机器人血量（未上场/被罚下时为0）
    uint16_t enemy_engineer2_hp; // 字节偏移2-3：对方2号工程机器人血量
    uint16_t enemy_infantry3_hp; // 字节偏移4-5：对方3号步兵机器人血量
    uint16_t enemy_infantry4_hp; // 字节偏移6-7：对方4号步兵机器人血量
    uint16_t reserved;           // 字节偏移8-9：保留位
    uint16_t enemy_sentry7_hp;   // 字节偏移10-11：对方7号哨兵机器人血量
} enemy_robot_hp_t;

/** 0x0A03
 *  对方机器人的剩余发弹量信息，频率上限为 10Hz
 *  信号发射源 → 雷达（雷达无线链路）
 */
typedef __packed struct
{
    uint16_t enemy_hero1_ammo;     // 字节偏移0-1：对方1号英雄机器人允许发弹量
    uint16_t enemy_infantry3_ammo; // 字节偏移2-3：对方3号步兵机器人允许发弹量（含堡垒提供的储备允许发弹量）
    uint16_t enemy_infantry4_ammo; // 字节偏移4-5：对方4号步兵机器人允许发弹量
    uint16_t enemy_air6_ammo;      // 字节偏移6-7：对方6号空中机器人允许发弹量
    uint16_t enemy_sentry7_ammo;   // 字节偏移8-9：对方7号哨兵机器人允许发弹量
} enemy_robot_ammo_t;

/** 0x0A04
 *  对方资源与场地状态数据，用于己方感知对方金币储备与场地控制权
 *  服务器→己方机器人（雷达无线链路传输）
 */
typedef __packed struct enemy_resource_field_struct
{
    uint16_t enemy_remaining_coins; // 0-1：对方剩余金币数
    uint16_t enemy_total_coins;     // 2-3：对方累计总金币数

    // 字节偏移4-7（4字节）：场地增益点占领状态位域
    __packed union
    {
        uint32_t field_status; // 整体位域容器
        __packed struct
        {
            uint32_t enemy_supply_area : 1;            // bit0：对方补给区占领状态
            uint32_t enemy_central_highland : 2;       // bit1-2：对方中央高地占领状态（1=对方/2=己方）
            uint32_t enemy_trapezoid_highland : 1;     // bit3：对方梯形高地占领状态（1=对方）
            uint32_t enemy_fortress_gain : 2;          // bit4-5：对方堡垒增益点占领状态（0=未占/1=对方/2=己方/3=双方）
            uint32_t enemy_outpost_gain : 2;           // bit6-7：对方前哨站增益点占领状态（0=未占/1=对方/2=己方）
            uint32_t enemy_base_gain : 1;              // bit8：对方基地增益点占领状态（1=对方）
            uint32_t enemy_tunnel_flyslope_front : 1;  // bit9：对方飞坡前隧道增益点状态（1=对方）
            uint32_t enemy_tunnel_flyslope_back : 1;   // bit10：对方飞坡后隧道增益点状态（1=对方）
            uint32_t enemy_tunnel_flyslope_front2 : 1; // bit11：对方飞坡前隧道增益点状态（1=对方）
            uint32_t enemy_tunnel_flyslope_back2 : 1;  // bit12：对方飞坡后隧道增益点状态（1=对方）
            uint32_t enemy_highland_interact : 1;      // bit13：对方高地交互模块状态（1=对方）
            uint32_t enemy_flyslope_interact : 1;      // bit14：对方飞坡交互模块状态（1=对方）
            uint32_t enemy_road_interact : 1;          // bit15：对方公路交互模块状态（1=对方）
            uint32_t reserve : 16;                     // bit16-31：保留位
        } status_bits;
    } field_info;
} enemy_resource_field_t;

/**
 * @brief 单个敌方机器人增益数据子结构体（英雄/工程/步兵/哨兵共用）
 */
typedef __packed struct
{
    uint8_t recovery_buff;      // n+0：回血增益（百分比，如10表示每秒恢复血量上限10%）
    uint16_t cooling_buff;      // n+1：射击热量冷却增益具体值（直接值，如x表示热量冷却增加x/s）
    uint8_t defence_buff;       // n+3：防御增益（百分比，如50表示50%防御增益）
    uint8_t vulnerability_buff; // n+4：负防御增益（百分比，如30表示-30%防御增益）
    uint16_t attack_buff;       // n+5：攻击增益（百分比，如50表示50%攻击增益）
} enemy_robot_buff_t;

/** 0x0A05
 *  对方机器人增益与哨兵姿态数据，用于己方评估敌方战力与战术意图
 *  服务器→己方机器人（雷达无线链路传输）
 */
typedef __packed struct
{
    enemy_robot_buff_t enemy_hero_buff;      // 0-6：对方英雄机器人增益数据
    enemy_robot_buff_t enemy_engineer_buff;  // 7-13：对方工程机器人增益数据
    enemy_robot_buff_t enemy_infantry3_buff; // 14-20：对方3号步兵机器人增益数据
    enemy_robot_buff_t enemy_infantry4_buff; // 21-27：对方4号步兵机器人增益数据
    enemy_robot_buff_t enemy_sentry_buff;    // 28-34：对方哨兵机器人增益数据
    uint8_t enemy_sentry_posture;            // 35：对方哨兵机器人当前姿态（1=进攻/2=防御/3=移动）
} enemy_robot_buff_all_t;

/** 0x0A06
 *  对方雷达ASCII标识数据，用于雷达链路的身份识别与校验
 *  服务器→己方机器人（雷达无线链路传输）
 */
typedef __packed struct
{
    uint8_t ascii_data[6];  // 字节偏移0-5：6字节ASCII编码的字母或数字字符串
} enemy_radar_ascii_info_t;

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
// extern air_support_data_t air_support_data;
/** 0x0206
 *  伤害状态数据，伤害发生后发送
 *  主控模块→对应机器人  */
extern hurt_data_t hurt_data;
extern uint8_t if_refresh; // TODO
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

 /** 0x0310（机器人端）
 *  机器人→自定义客户端的自定义数据（图传链路),频率上限为 50Hz
 *  机器人向自定义客户端发送最大300字节的自定义信息
 */
 extern robot_custom_data_2_t  robot_custom_data_2; 
 /** 0x0311
 *  自定义客户端→机器人的自定义指令（图传链路）,频率上限为 75Hz
 *  自定义客户端向机器人下发最大30字节的自定义控制指令
 */
extern robot_custom_data_3_t robot_custom_data_3;

/** 0x0A01
 *  对方机器人位置数据，用于己方感知敌方坐标
 *  服务器→己方机器人（通常通过雷达无线链路传输）
 */
extern enemy_robot_position_t enemy_robot_position;
/** 0x0A02
 *  对方机器人血量数据，用于己方评估敌方战力状态
 *  服务器→己方机器人（雷达无线链路传输）
 */
extern enemy_robot_hp_t enemy_robot_hp;
/** 0x0A03
 *  对方机器人允许发弹量数据，用于己方预判敌方攻击强度
 *  服务器→己方机器人（雷达无线链路传输）
 */
extern enemy_robot_ammo_t enemy_robot_ammo;
/** 0x0A04
 *  对方资源与场地状态数据，用于己方感知对方金币储备与场地控制权
 *  服务器→己方机器人（雷达无线链路传输）
 */
extern enemy_resource_field_t enemy_resource_field;
/** 0x0A05
 *  对方机器人增益与哨兵姿态数据，用于己方评估敌方战力与战术意图
 *  服务器→己方机器人（雷达无线链路传输）
 */
extern enemy_robot_buff_all_t enemy_robot_buff_all;
/** 0x0A06
 *  对方雷达ASCII标识数据，用于雷达链路的身份识别与校验
 *  服务器→己方机器人（雷达无线链路传输）
 */
extern enemy_radar_ascii_info_t enemy_radar_ascii_info;


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
