#include "stdafx.h"
#include "robot.h"

/////////////////////////////////////////////
//基因内存大小
inline int robot_gene_size(void)
{
	return (gConfig.robot_gene_length + 1);
}

//行动内存大小
inline int robot_steps_size(void)
{
	return (gConfig.robot_step_count * ROBOT_STEPS + 1);
}

//创建机器人基因
inline GENE robot_gene_create(bool make)
{
	GENE ret = (GENE)malloc(robot_gene_size());
	if(make)
	{
		return robot_gene_make(ret);
	}
	ret[0] = '\0';
	return ret;
}
inline GENE robot_gene_create(CGENE tpl)
{
	if(tpl == NULL)
	{
		return robot_gene_create();
	}
	return (GENE)memcpy(malloc(robot_gene_size()), tpl, robot_gene_size());
}

//创建机器人行动步骤
inline STEPS robot_steps_create(CSTEPS tpl)
{
	STEPS ret = (STEPS)malloc(robot_steps_size());
	if(tpl != NULL)
	{
		return (STEPS)memcpy(ret, tpl, robot_steps_size());
	}
	ret[0] = '\0';
	return ret;
}

//创建机器人
inline PROBOT robot_create(void)
{
	return (PROBOT)malloc(sizeof(ROBOT));
}

//创建机器人(一批)
inline ROBOTS robot_create(int count)
{
	return (ROBOTS)malloc(sizeof(ROBOT) * count);
}

//销毁机器人基因
inline void robot_gene_destroy(GENE gene)
{
	free(gene);
}

//销毁机器人行动步骤
inline void robot_steps_destroy(STEPS steps)
{
	free(steps);
}

//销毁机器人
/*inline void robot_destroy(PROBOT robot)
{
	robot_destroy(*robot);
	free(robot);
}*/
inline void robot_destroy(ROBOT &robot)
{
	if(robot.gene != NULL)
	{
		free(robot.gene);
	}
	if(robot.steps != NULL)
	{
		free(robot.steps);
	}
}
inline void robot_destroy(ROBOTS robots, int count)
{
	for(int i=0;i<count;i++)
	{
		robot_destroy(robots[i]);
	}
	free(robots);
}


/////////////////////////////////////////////
//机器人计数初始化
inline void robot_base_init(void)
{
	gConfig.robot_id_base = 0;
	gConfig.robot_father_id_base = 0;
}

//机器人初始化
/*inline PROBOT robot_init(PROBOT robot, long father_id, int generation, GENE gene, STEPS steps)
{
	robot_init(*robot, father_id, generation, gene, steps);
	return robot;
}*/
inline void robot_init(ROBOT &robot, long father_id, int generation, GENE gene, STEPS steps)
{
	robot.id = ++gConfig.robot_id_base;
	robot.father_id = father_id > 0 ? father_id : ++gConfig.robot_father_id_base;
	robot.generation = generation > 0 ? generation : 1;
	robot.gene = gene == NULL ? robot_gene_create() : gene; //随机生成基因
	robot.steps = steps == NULL ? robot_steps_create() : steps; //分配行动空间
	robot.score = 0;
}


/////////////////////////////////////////////
//机器人行动步骤初始化
inline void robot_steps_init(STEPS steps)
{
	steps[gConfig.robot_step_count * ROBOT_STEPS] = '\0';
}

//读取机器人行动步骤
inline char robot_steps_get(CSTEPS steps, int step, int type)
{
	return steps[ROBOT_STEPS * step + type];
}

//设置机器人行动步骤
inline void robot_steps_set(STEPS steps, int step, char action, bool result)
{
	steps[ROBOT_STEPS * step + ROBOT_STEPS_ACTION] = action;
	steps[ROBOT_STEPS * step + ROBOT_STEPS_RESULT] = '0' + result; //数字0~1组成
}


/////////////////////////////////////////////
//生成机器人基因片段(随机)
inline void robot_gene_piece_make(char *piece, int len)
{
	for(int i=0;i<len;i++)
	{
		piece[i] = '0' + rand() % 7; //数字0~6组成
	}
}
//生成机器人基因(随机)
GENE robot_gene_make(GENE gene_buf)
{
	robot_gene_piece_make(gene_buf, gConfig.robot_gene_length);
	gene_buf[gConfig.robot_gene_length] = '\0';
	return gene_buf;
}

//机器人基因进化(单个)
inline void robot_gene_evolve_one(GENE child, CGENE parent1, CGENE parent2, int pos)
{
	memcpy(child, parent1, pos);
	memcpy(&child[pos], &parent2[pos], gConfig.robot_gene_length - pos);
	//随机变异(X%概率)
	if(gConfig.robot_evo_var_rate >= rand() % 100)
	{
		pos = rand() % (gConfig.robot_gene_length - gConfig.robot_evo_var_piece);//随机位置
		robot_gene_piece_make(&child[pos], gConfig.robot_evo_var_piece);
	}
}
//机器人基因交叉进化
int robot_gene_evolve(GENE child1, GENE child2, CGENE parent1, CGENE parent2)
{
	//交叉进化
	int pos = 1 + rand() % (gConfig.robot_gene_length - 1);//[1, gene_len)
	robot_gene_evolve_one(child1, parent1, parent2, pos);
	robot_gene_evolve_one(child2, parent2, parent1, pos);
	return pos;//返回基因片段分裂位置
}
