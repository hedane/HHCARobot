#pragma once

#include "map.h"
#include "robot.h"
#include "lab.h"
#include "history.h"

//实验返回值
typedef enum
{
	TEST_RET_OK,
	TEST_RET_CANCEL,
	TEST_RET_FAIL,
} TEST_RET;

//实验参数结构
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

//实验线程数
extern inline int test_thread_num(void);
//是否已有实验
extern inline bool test_is_open(void);

//新建实验
TEST_RET test_new(HWND hWnd);
//打开旧的实验
TEST_RET test_open(HWND hWnd);
//关闭实验
void test_close(void);

//跑N代实验是否进行中
extern inline bool test_batch_running(void);
//开始跑N代实验(新线程)
void test_batch_start(HWND hWnd);
//停止跑N代实验(异步)
void test_batch_stop(bool sync = false);

//跑独立N次实验
void test_run(GENE gene, STEPS steps = NULL);
