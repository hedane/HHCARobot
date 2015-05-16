#pragma once

#include "map.h"
#include "robot.h"

//���̲߳���
typedef struct
{
	ROBOTS robots;
	int len;
	MAP map;
	time_t rnd_seed;
	int no;
} LAB_THREAD_PARAM, *PLAB_THREAD_PARAM, *LAB_THREAD_PARAMS;
//���̲߳�����
typedef struct
{
	MAP map;
	int num;
	LAB_THREAD_PARAMS params;
	PLAB_THREAD_PARAM rem_param;
} LAB_THREAD_PARAM_GROUP, *PLAB_THREAD_PARAM_GROUP;

//��1��ʵ��
//int lab_run_once(CMAP map_tpl, ROBOT &robot);
int lab_run_once(MAP map, CGENE gene, STEPS steps);

//�������̲߳�����
extern inline PLAB_THREAD_PARAM_GROUP lab_thread_params_create(const ROBOTS robots, time_t rnd_seed, int core_num, CMAP map_tpl = NULL);
//�ͷŶ��̲߳�����
extern inline void lab_thread_params_destroy(PLAB_THREAD_PARAM_GROUP tpg);
//ʵ���̳߳�ʼ��
void lab_run_more_thread_init(PLAB_THREAD_PARAM_GROUP tpg);
//ʵ���߳��ͷ���Դ
void lab_run_more_thread_free(PLAB_THREAD_PARAM_GROUP tpg);
//��1��ʵ��(���߳�)
void lab_run_more(PLAB_THREAD_PARAM_GROUP tpg);


//���������ļ�ֵ��
typedef struct
{
	long father_id;
	int count;
}RACIAL, *RACIALS, *PRACIAL;
//������������������
extern inline RACIALS lab_racial_create(void);
//�ͷŻ�������������
extern inline void lab_racial_destroy(RACIALS racials);
//���������������ʼ��
void lab_racials_init(RACIALS racials, const ROBOTS robots);
//�����˵÷�����
void lab_robot_sort(ROBOTS robots, RACIALS racials);

//����������ָ������
extern inline PROBOT *lab_top_robots_create(void);
//�ͷŻ�����ָ������
extern inline void lab_top_robots_destroy(PROBOT top_robots[]);
//�����˽������
void lab_robot_evolve(ROBOTS robots, RACIALS racial, PROBOT top_robots[], PROBOT odd_robots[]);
//void lab_robot_evolve(ROBOTS robots, RACIALS racial);
