#pragma once

#include "map.h"
#include "robot.h"

//多线程参数
typedef struct
{
	ROBOTS robots;
	int len;
	MAP map;
	time_t rnd_seed;
	int no;
} LAB_THREAD_PARAM, *PLAB_THREAD_PARAM, *LAB_THREAD_PARAMS;
//多线程参数组
typedef struct
{
	MAP map;
	int num;
	LAB_THREAD_PARAMS params;
	PLAB_THREAD_PARAM rem_param;
} LAB_THREAD_PARAM_GROUP, *PLAB_THREAD_PARAM_GROUP;

//跑1次实验
//int lab_run_once(CMAP map_tpl, ROBOT &robot);
int lab_run_once(MAP map, CGENE gene, STEPS steps);

//创建多线程参数组
extern inline PLAB_THREAD_PARAM_GROUP lab_thread_params_create(const ROBOTS robots, time_t rnd_seed, int core_num, CMAP map_tpl = NULL);
//释放多线程参数组
extern inline void lab_thread_params_destroy(PLAB_THREAD_PARAM_GROUP tpg);
//实验线程初始化
void lab_run_more_thread_init(PLAB_THREAD_PARAM_GROUP tpg);
//实验线程释放资源
void lab_run_more_thread_free(PLAB_THREAD_PARAM_GROUP tpg);
//跑1代实验(多线程)
void lab_run_more(PLAB_THREAD_PARAM_GROUP tpg);


//种族数量的键值对
typedef struct
{
	long father_id;
	int count;
}RACIAL, *RACIALS, *PRACIAL;
//创建机器人种族数组
extern inline RACIALS lab_racial_create(void);
//释放机器人种族数组
extern inline void lab_racial_destroy(RACIALS racials);
//机器人种族数组初始化
void lab_racials_init(RACIALS racials, const ROBOTS robots);
//机器人得分排序
void lab_robot_sort(ROBOTS robots, RACIALS racials);

//创建机器人指针数组
extern inline PROBOT *lab_top_robots_create(void);
//释放机器人指针数组
extern inline void lab_top_robots_destroy(PROBOT top_robots[]);
//机器人交叉进化
void lab_robot_evolve(ROBOTS robots, RACIALS racial, PROBOT top_robots[], PROBOT odd_robots[]);
//void lab_robot_evolve(ROBOTS robots, RACIALS racial);
