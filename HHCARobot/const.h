#pragma once

//最大加载字符串长度
#define MAX_LOADSTRING 100
//最大保存字符串长度
#define MAX_SAVESTRING 256

//地图边长(格子数)
#define MAP_SIDE_NUM	25
//地图里的垃圾数量
#define MAP_TRASH_NUM	250

//机器人基因长度
#define ROBOT_GENE_LENGTH	243
//机器人行动步数
#define ROBOT_STEP_COUNT	400

//机器人进化变异概率(百分比)
#define ROBOT_EVO_VAR_RATE	5
//机器人进化变异片段数
#define ROBOT_EVO_VAR_PIECE	5

//实验中机器人捡起垃圾得分
#define LAB_ACTION_TRASH_OK		10
//实验中机器人捡错垃圾扣分
#define LAB_ACTION_TRASH_FAIL	-1
//实验中机器人撞墙扣分
#define LAB_ACTION_KONCK_WALL	-5

//1代实验的轮数
#define LAB_MORE_COUNT	200
//1代实验的机器人数
#define LAB_MORE_ROBOT_COUNT	1000
//1代实验的机器人排名数
#define LAB_MORE_ROBOT_TOP_COUNT	200
//1代实验的机器人排名同族人数限制
#define LAB_MORE_ROBOT_TOP_RACIAL_LIMIT	50

//复盘次数
#define LAB_RUN_COUNT	1

//历史数据Top机器人数量
#define HISTORY_ROBOT_COUNT	1

typedef struct
{
	//地图边长(格子数)
	int map_side_num;
	//地图里的垃圾数量
	int map_trash_num;

	//机器人基因长度
	int robot_gene_length;
	//机器人行动次数
	int robot_step_count;
	//机器人自增名字ID
	long robot_id_base;
	//机器人自增姓氏ID
	long robot_father_id_base;
	//机器人进化变异概率(百分比)
	int robot_evo_var_rate;
	//机器人进化变异片段数
	int robot_evo_var_piece;

	//实验中机器人捡起垃圾得分
	int lab_action_trash_ok;
	//实验中机器人捡错垃圾扣分
	int lab_action_trash_fail;
	//实验中机器人撞墙扣分
	int lab_action_konck_wall;

	//1代实验的轮数
	int lab_more_count;
	//1代实验的机器人数
	int lab_more_robot_count;
	//1代实验的机器人排名数
	int lab_more_robot_top_count;
	//1代实验的机器人排名同族人数限制
	int lab_more_robot_top_racial_limit;

	//复盘次数
	int lab_run_count;

	//历史数据Top机器人数量
	int history_robot_count;

} CONFIG;

//全局配置
extern CONFIG gConfig;

//打印调试信息
#ifdef _DEBUG
#define DEBUG(fmt, ...)	{\
	char buf[4096];\
	sprintf_s(buf, sizeof(buf), fmt, __VA_ARGS__);\
	size_t n = strlen(buf);\
	buf[n++] = '\r';\
	buf[n++] = '\n';\
	buf[n++] = '\0';\
	OutputDebugString(buf);\
}
#else
#define DEBUG(...) {}
#endif

/*#define WRITE(fmt, ...) {\
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
	if(hConsole) {\
		char buf[4096];\
		sprintf_s(buf, sizeof(buf), fmt, __VA_ARGS__);\
		size_t n = strlen(buf);\
		buf[n++] = '\r';\
		buf[n++] = '\n';\
		buf[n++] = '\0';\
		WriteConsole(hConsole, buf, (DWORD)n, NULL, NULL);\
	}\
}*/

//防止消息阻塞
void DoEvents(void);

//时间戳转换为字符串
void Ts2Str(char *buf, size_t size, time_t ts);
void Ts2Str(char *buf, size_t size, const char *fmt, time_t ts, bool local = true);


//自定义消息
#define WM_TEST_ROUND_NEW		(WM_USER + 0) //实验进入新代 wParam:代数
#define WM_TEST_BATCH_STOP		(WM_USER + 11) //代实验进程结束
