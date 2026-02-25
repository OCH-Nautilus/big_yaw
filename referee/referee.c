/*
 * @Author: 孙羽
 * @Date: 2023-06-05 23:52:39
 * @LastEditors: 周灿
 * @LastEditTime: 2025-7-5 
 * @Telephone: 15235320302
 * @QQ: 984464809
 * @Description: 基于 RoboMaster 裁判系统串口协议附录 V1.9.0（2025.7.5）
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */

#include "referee.h"
#include "string.h"
#include "stdio.h"
#include "CRC8_CRC16.h"
#include "protocol.h"

// 裁判系统数据
frame_header_struct_t referee_receive_header;
frame_header_struct_t referee_send_header;
/** 0x0001
 *  比赛状态数据，固定 3Hz 频率发送
 *  服务器→全体机器人  */
game_state_t game_state;
/** 0x0002
 *  比赛结果数据，比赛结束触发发送
 *  服务器→全体机器人  */
game_result_t game_result;
/** 0x0003
 *  机器人血量数据，固定 3Hz 频率发送
 *  服务器→全体机器人  */
game_robot_HP_t game_robot_HP;
/** 0x0101
 *  场地事件数据，固定 3Hz 频率发送  
 *  服务器→己方全体机器人  */
event_data_t event_data;
///** 0x0102
// *  补给站动作标识数据，补给站弹丸释放时触发发送
// *  服务器→己方全体机器人  */
//ext_supply_projectile_action_t supply_projectile_action;
/** 0x0104
 *  裁判警告数据，己方判罚/判负时触发发送
 *  服务器→被处罚方全体机器人  */
referee_warning_t referee_warning;
/** 0x0105
 *  飞镖发射时间数据，固定 3Hz 频率发送
 *  服务器→己方全体机器人  */
dart_remaining_time_t dart_remaining_time;
/** 0x0201
 *  机器人性能体系数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
robot_status_t robot_status;
/** 0x0202
 *  实时功率热量数据，固定 50Hz 频率发送
 *  主控模块→对应机器人  */
power_heat_data_t power_heat_data;
/** 0x0203
 *  机器人位置数据，固定 10Hz 频率发送
 *  主控模块→对应机器人  */
robot_pos_t robot_pos;
/** 0x0204
 *  机器人增益数据，固定 3Hz 频率发送
 *  服务器→对应机器人  */
buff_t buff;
///** 0x0205
// *  空中支援时间数据，固定 10Hz 频率发送
// *  服务器→己方空中机器人  */
//air_support_data_t air_support_data;
/** 0x0206
 *  伤害状态数据，伤害发生后发送
 *  主控模块→对应机器人  */
hurt_data_t hurt_data;
uint8_t if_refresh;  // TODO
/** 0x0207
 *  实时射击数据，弹丸发射后发送
 *  主控模块→对应机器人  */
shoot_data_t shoot_data;
/** 0x0208
 *  允许发弹量，固定 10Hz 频率发送
 *  服务器→己方英雄、步兵、哨兵、空中机器人  */
projectile_allowance_t projectile_allowance;
/** 0x0209
 *  机器人 RFID 状态，固定 3Hz 频率发送
 *  服务器→己方装有 RFID 模块的机器人  */
rfid_status_t rfid_status;
/** 0x020A
 *  飞镖选手端指令数据，飞镖闸门上线后固定 10Hz 频率发送
 *  服务器→己方飞镖机器人  */
dart_client_cmd_t dart_client_cmd;
/** 0x020B
 *  地面机器人位置数据，固定 1Hz 频率发送
 *  服务器→己方哨兵机器人  */
ground_robot_position_t ground_robot_position;
/** 0x020C
 *  雷达标记进度数据，固定 1Hz 频率发送
 *  服务器→己方雷达机器人  */
radar_mark_data_t radar_mark_data;
/** 0x020D
 *  哨兵自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方哨兵机器人  */
sentry_info_t sentry_info;
/** 0x020E
 *  雷达自主决策信息同步，固定以1Hz 频率发送
 *  服务器→己方雷达机器人  */
radar_info_t radar_info;
/** 0x0301
 *  机器人交互数据，发送方触发发送，频率上限为 10Hz  */
robot_interaction_data_t robot_interaction_data;
/** 0x0302
 *  自定义控制器与机器人交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端图传连接的机器人  */
custom_robot_data_t custom_robot_data;
/** 0x0303
 *  选手端小地图交互数据，选手端触发发送
 *  选手端点击→服务器→发送方选择的己方机器人  */
map_command_t map_command;
uint8_t if_update=0;  // TODO
/** 0x0304
 *  键鼠遥控数据，固定 30Hz 频率发送
 *  客户端→选手端图传连接的机器人  */
remote_control_t remote_control;
/** 0x0305
 *  选手端小地图接收雷达数据，频率上限为 10Hz
 *  雷达→服务器→己方所有选手端  */
map_robot_data_t map_robot_data;
/** 0x0306
 *  自定义控制器与选手端交互数据，发送方触发发送，频率上限为 30Hz
 *  自定义控制器→选手端  */
custom_client_data_t custom_client_data;
/** 0x0307
 *  选手端小地图接收哨兵数据，频率上限为 1Hz
 *  哨兵/半自动控制机器人→对应操作手选手端  */
map_data_t map_data;
/** 0x0308
 *  选手端小地图接收机器人数据，频率上限为 3Hz 
 *  己方机器人→己方选手端  */
custom_info_t custom_info;
/** 0x0309
 *  自定义控制器接收机器人数据，频率上限为 10Hz
 *  己方机器人→对应操作手选手端连接的自定义控制器  */
robot_custom_data_t robot_custom_data;
 
 


void init_referee_struct_data(void)
{
    memset(&referee_receive_header, 0, sizeof(frame_header_struct_t));
    memset(&referee_send_header, 0, sizeof(frame_header_struct_t));

	
    memset(&game_state, 0, sizeof(game_state_t));          // 0x0001
    memset(&game_result, 0, sizeof(game_result_t));        // 0x0002
    memset(&game_robot_HP, 0, sizeof(game_robot_HP_t));    // 0x0003

    memset(&event_data, 0, sizeof(event_data_t));                                   // 0x0101
    memset(&referee_warning, 0, sizeof(referee_warning_t ));                        // 0x0104
		memset(&dart_remaining_time,0,sizeof(dart_remaining_time_t ));				    // 0x0105

    memset(&robot_status, 0, sizeof(robot_status_t));                   // 0x0201
    memset(&power_heat_data, 0, sizeof(power_heat_data_t));             // 0x0202
    memset(&robot_pos, 0, sizeof(robot_pos_t));                         // 0x0203
    memset(&buff, 0, sizeof(buff_t));                                   // 0x0204
    memset(&hurt_data, 0, sizeof(hurt_data_t));                         // 0x0206
    memset(&shoot_data, 0, sizeof(shoot_data_t));                       // 0x0207
    memset(&projectile_allowance, 0, sizeof(projectile_allowance_t));   // 0x0208
		memset(&rfid_status, 0, sizeof(rfid_status_t));                     // 0x0209
		memset(&dart_client_cmd, 0, sizeof(dart_client_cmd_t));             // 0x020A
		memset(&ground_robot_position, 0, sizeof(ground_robot_position_t)); // 0x020B
		memset(&radar_mark_data, 0, sizeof(radar_mark_data_t));             // 0x020C
		memset(&sentry_info, 0, sizeof(sentry_info_t));                     // 0x020D
		memset(&radar_info, 0, sizeof(radar_info_t));                       // 0x020E
		
		memset(&robot_interaction_data, 0, sizeof(robot_interaction_data_t));    // 0x0301
		memset(&custom_robot_data, 0, sizeof(custom_robot_data_t));              // 0x0302
		memset(&map_command, 0, sizeof(map_command_t));                          // 0x0303
		memset(&remote_control, 0, sizeof(remote_control_t));                    // 0x0304
		memset(&map_robot_data, 0, sizeof(map_robot_data_t));                    // 0x0305
		memset(&custom_client_data, 0, sizeof(custom_client_data_t));            // 0x0306	
		memset(&map_data, 0, sizeof(map_data_t));                                // 0x0307	
		memset(&custom_info, 0, sizeof(custom_info_t));                          // 0x0308	
    memset(&robot_custom_data,0,sizeof(robot_custom_data_t));                // 0x0309
}
uint16_t SHOOT_NUM_1 = 0, SHOOT_NUM_2 = 0;
float speed_gun_1;
float speed_gun_2;
uint8_t buff_test[40];int asd;
uint8_t plplk;
void referee_data_solve(uint8_t *frame)
{
    uint16_t cmd_id = 0;

    uint8_t index = 0;

    memcpy(&referee_receive_header, frame, sizeof(frame_header_struct_t));

    index += sizeof(frame_header_struct_t);

    memcpy(&cmd_id, frame + index, sizeof(uint16_t));
    index += sizeof(uint16_t);
	
		asd = sizeof(sentry_info_t );
    switch (cmd_id)
    {
        case GAME_STATE_CMD_ID:        // 0x0001
        {
            memcpy(&game_state, frame + index, sizeof(game_state_t));
        }
        break;
        case GAME_RESULT_CMD_ID:       // 0x0002
        {
            memcpy(&game_result, frame + index, sizeof(game_result_t ));
        }
        break;
        case GAME_ROBOT_HP_CMD_ID:     // 0x0003
        {
            memcpy(&game_robot_HP, frame + index, sizeof(game_robot_HP_t));
        }
        break;


        case FIELD_EVENTS_CMD_ID:               // 0x0101
        {
            memcpy(&event_data, frame + index, sizeof(event_data_t ));
        }
        break;
//        break;
        case REFEREE_WARNING_CMD_ID:            // 0x0104
        {
            memcpy(&referee_warning, frame + index, sizeof(referee_warning_t ));
        }
				break;
				case DART_REMAINING_TIME_CMD_ID:        // 0x0105
				{
					memcpy(&dart_remaining_time, frame + index, sizeof(dart_remaining_time_t ));
				}
				break;
		
		
        case ROBOT_STATE_CMD_ID:                // 0x0201
        {
            memcpy(&robot_status, frame + index, sizeof(robot_status_t));
        }
        break;
        case POWER_HEAT_DATA_CMD_ID:            // 0x0202
        {
            memcpy(&power_heat_data, frame + index, sizeof(power_heat_data_t));
        }
        break;
        case ROBOT_POS_CMD_ID:                  // 0x0203
        {
            memcpy(&robot_pos, frame + index, sizeof(robot_pos_t));
        }
        break;
        case BUFF_MUSK_CMD_ID:                  // 0x0204
        {
            memcpy(&buff, frame + index, sizeof(buff_t));
        }
        break;
//        break;
        case ROBOT_HURT_CMD_ID:                 // 0x0206
        {
            memcpy(&hurt_data, frame + index, sizeof(hurt_data_t));
            if_refresh = 1;
            plplk++;
        }
        break;
        case SHOOT_DATA_CMD_ID:                 // 0x0207
        {
            memcpy(&shoot_data, frame + index, sizeof(shoot_data_t));

           SHOOT_NUM_1++;
           speed_gun_1 = shoot_data.initial_speed ;
             
        }
        break;
        case BULLET_REMAINING_CMD_ID:           // 0x0208
        {
            memcpy(&projectile_allowance, frame + index, sizeof(projectile_allowance_t));
        }
        break;
		case RFID_STATUS_CMD_ID:                // 0x0209
		{
			memcpy(&rfid_status, frame+index, sizeof(rfid_status_t));
		}
		break;
		case DART_CLIENT_CMD_ID:                // 0x020A
		{
			memcpy(&dart_client_cmd, frame+index, sizeof(dart_client_cmd_t));
		}
		break;
		case GROUND_ROBOT_POSITION_CMD_ID:      // 0x020B
		{
			memcpy(&ground_robot_position, frame+index, sizeof(ground_robot_position_t));
		}
		break;
		case RADAR_MARK_DATA_CMD_ID:            // 0x020C
		{
			memcpy(&radar_mark_data, frame+index, sizeof(radar_mark_data_t));
		}
		break;
		case SENTRY_INFO_CMD_ID:                // 0x020D
		{
			memcpy(&sentry_info, frame+index, sizeof(sentry_info_t));
								

		}
		break;
		case RADAR_INFO_CMD_ID:           		  // 0x020E
		{
			memcpy(&radar_info, frame+index, sizeof(radar_info_t));
		}
		break;
				
		
        case STUDENT_INTERACTIVE_DATA_CMD_ID:   // 0x0301
        {
            memcpy(&robot_interaction_data, frame+index, sizeof(robot_interaction_data_t));
        }
        break;
		case CUSTOM_ROBOT_DATA_CMD_ID:          // 0x0302
		{
			memcpy(&custom_robot_data, frame+index, sizeof(custom_robot_data_t));
		}
		break;
		case MAP_COMMAND_CMD_ID:                // 0x0303
		{
			memcpy(&map_command, frame+index, sizeof(map_command_t));
			if_update = 1;
		}
		break;
		case REMOTE_CONTROL_CMD_ID:             // 0x0304
		{
			memcpy(&remote_control, frame+index, sizeof(remote_control_t));
		}
		break;
        case MAP_ROBOT_DATA_CMD_ID:             // 0x0305
        {
            memcpy(&map_robot_data, frame+index, sizeof(map_robot_data_t));
        }
		break;
		case CUSTOM_CLIENT_DATA_CMD_ID:         // 0x0306
        {
            memcpy(&custom_client_data, frame+index, sizeof(custom_client_data_t));
        }
		break;
	    case MAP_DATA_CMD_ID:            // 0x0307
        {
            memcpy(&map_data, frame+index, sizeof(map_data_t));
        }
		break;
	    case CUSTOM_INFO_CMD_ID:               // 0x0308
        {
            memcpy(&custom_info, frame+index, sizeof(custom_info_t));
        }
		break;
    
      case ROBOT_CUSTOM_DATA_CMD_ID:              // 0x309
        {
          memcpy(&robot_custom_data, frame+index, sizeof(robot_custom_data_t));
        }
    break;
        default:
        {
            break;
        }
    }
}
int now_speed=0,last_speed=0;
/*反馈打弹数量*/
uint32_t Report_Shoot_NUM(void)
{
//    if(rc_ctrl.keyboard.key_B == 1)
//    {
//        SHOOT_NUM_1 = 0;
//    }
	now_speed=shoot_data.initial_speed;
	if(now_speed!=last_speed)
		SHOOT_NUM_1++;
	last_speed=now_speed;
	return SHOOT_NUM_1;
}


