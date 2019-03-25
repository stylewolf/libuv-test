//
// Created by Administrator on 2019/3/5.
//
#pragma once
#ifndef ENGINE_BUS_JSON_CMD_H
#define ENGINE_BUS_JSON_CMD_H
/**
 * 连接命令
 */
extern const char* cmd_connect;
/**
 * 就绪
 */
extern const char* cmd_ready;
extern const char* cmd_list_processes;
extern const char* cmd_disconnect;
/**
 * 数据库命令
 */
extern const char* cmd_db_write;
extern const char* cmd_db_read;

extern const char* json_name_cmd;
/**
 * 激活哪个监听方法
 */
extern const char* json_name_fire;
extern const char* json_name_msg;
extern const char* json_name_error_code;
extern const char* json_name_process;
extern const char* json_name_cmd_key;

extern const char* json_name_sql;
extern const char* json_name_datas;
extern const char* json_name_param;
extern const char* json_name_param_type;
extern const char* json_name_param_value;

extern const char* json_name_param_int;
extern const char* json_name_param_double;
extern const char* json_name_param_string;

extern const char* json_name_column_int;
extern const char* json_name_column_real;

extern const char* json_name_client_id;
extern const char* json_name_processes;

#endif //ENGINE_BUS_JSON_CMD_H
