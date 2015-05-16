#pragma once

#include "map.h"
#include "robot.h"
#include "lab.h"
#include "history.h"

//ʵ�鷵��ֵ
typedef enum
{
	TEST_RET_OK,
	TEST_RET_CANCEL,
	TEST_RET_FAIL,
} TEST_RET;

//ʵ������ṹ
typedef struct
{
	int round;
	time_t ts_start;
	time_t ts_stop;
	time_t ts_total;
	ROBOTS robots;
	PLAB_THREAD_PARAM_GROUP tpg;
	PHISTORY history;
	bool running;
} TEST_PARAM, *PTEST_PARAM;

extern PTEST_PARAM gTestParam;

//ʵ���߳���
extern inline int test_thread_num(void);
//�Ƿ�����ʵ��
extern inline bool test_is_open(void);

//�½�ʵ��
TEST_RET test_new(HWND hWnd);
//�򿪾ɵ�ʵ��
TEST_RET test_open(HWND hWnd);
//�ر�ʵ��
void test_close(void);

//��N��ʵ���Ƿ������
extern inline bool test_batch_running(void);
//��ʼ��N��ʵ��(���߳�)
void test_batch_start(HWND hWnd);
//ֹͣ��N��ʵ��(�첽)
void test_batch_stop(bool sync = false);

//�ܶ���N��ʵ��
void test_run(GENE gene, STEPS steps = NULL);
