// robot.h
// 基因：由字符0~6组成的字符串（含结尾'\0'）
//
#pragma once

//基因类型定义
typedef char *GENE;
typedef const char *CGENE;

//行动步骤类型定义
typedef char *STEPS;
typedef const char *CSTEPS;

//机器人类型定义
typedef struct tagROBOT
{
	long id;//名字
	long father_id;//姓氏
	int generation;//代数
	GENE gene;//基因
	long score;//得分
	STEPS steps;//行动步骤
} ROBOT, *PROBOT, *ROBOTS;


//机器人行为定义
#define ROBOT_ACT_NORTH '0' //向北Y+
#define ROBOT_ACT_SOUTH '1' //向南Y-
#define ROBOT_ACT_EAST	'2' //向东X+
#define ROBOT_ACT_WEST	'3' //向西X-
#define ROBOT_ACT_RND	'4' //随机移动
#define ROBOT_ACT_STAY	'5' //不动
#define ROBOT_ACT_CLEAN	'6' //捡垃圾
#define ROBOT_ACT_RND_NORTH 'a' //随机向北Y+
#define ROBOT_ACT_RND_SOUTH 'b' //随机向南Y-
#define ROBOT_ACT_RND_EAST	'c' //随机向东X+
#define ROBOT_ACT_RND_WEST	'd' //随机向西X-
//定义开始标志
#define ROBOT_ACT_BASE	ROBOT_ACT_NORTH
#define ROBOT_ACT_RND_BASE	ROBOT_ACT_RND_NORTH

//机器人行动步骤列定义
#define ROBOT_STEPS			2
#define ROBOT_STEPS_ACTION	0
#define ROBOT_STEPS_RESULT	1

//基因内存大小
extern inline int robot_gene_size(void);

//行动步骤内存大小
extern inline int robot_steps_size(void);

//创建机器人基因
extern inline GENE robot_gene_create(bool make = true);
extern inline GENE robot_gene_create(CGENE tpl);

//创建机器人行动步骤
extern inline STEPS robot_steps_create(CSTEPS tpl = NULL);

//创建机器人
//extern inline PROBOT robot_create(void);
//创建机器人(一批)
extern inline ROBOTS robot_create(int count);

//销毁机器人基因
extern inline void robot_gene_destroy(GENE gene);

//销毁机器人行动步骤
extern inline void robot_steps_destroy(STEPS steps);

//销毁机器人
//extern inline void robot_destroy(PROBOT robot);
extern inline void robot_destroy(ROBOT &robot);
extern inline void robot_destroy(ROBOTS robots, int count);


//机器人计数初始化
extern inline void robot_base_init(void);

//机器人初始化
//extern inline PROBOT robot_init(PROBOT robot, long father_id = 0, int generation = 0, GENE gene = NULL, STEPS steps = NULL);
extern inline void robot_init(ROBOT &robot, long father_id = 0, int generation = 0, GENE gene = NULL, STEPS steps = NULL);


//机器人行动步骤初始化
extern inline void robot_steps_init(STEPS steps);

//读取机器人行动步骤
extern inline char robot_steps_get(CSTEPS steps, int step, int type);
//设置机器人行动步骤
extern inline void robot_steps_set(STEPS steps, int step, char action, bool result);

//生成机器人基因(随机)
GENE robot_gene_make(GENE gene_buf);

//机器人基因交叉进化
int robot_gene_evolve(GENE child1, GENE child2, CGENE parent1, CGENE parent2);
