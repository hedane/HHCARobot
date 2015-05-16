#include "stdafx.h"
#include "test.h"
#include "draw.h"
#include "file.h"

PTEST_PARAM gTestParam = NULL;

/////////////////////////////////////////////
//��ʼ��ʵ����Դ
inline void test_init()
{
	gTestParam = (PTEST_PARAM)malloc(sizeof(TEST_PARAM));
	gTestParam->robots = NULL;
	gTestParam->tpg = NULL;
	gTestParam->history = NULL;
	gTestParam->running = false;
}

//�ͷ�ʵ����Դ
inline void test_free()
{
	if(gTestParam != NULL)
	{
		if(gTestParam->tpg != NULL)
		{
			lab_thread_params_destroy(gTestParam->tpg);
		}
		if(gTestParam->robots != NULL)
		{
			robot_destroy(gTestParam->robots, gConfig.lab_more_robot_count);
		}
		if(gTestParam->history != NULL)
		{
			history_destroy(gTestParam->history);
		}
		free(gTestParam);
		gTestParam = NULL;
	}
}

//ʵ���߳���
inline int test_thread_num(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors; //CPU�߼���
}

//�Ƿ�����ʵ��
inline bool test_is_open(void)
{
	return (gTestParam != NULL);
}

/////////////////////////////////////////////
//�½�ʵ��
TEST_RET test_new(HWND hWnd)
{
	if(!file_new(hWnd))
	{
		return TEST_RET_CANCEL;
	}

	test_init();

	//�����������
	time_t ts = time(NULL);
	srand((UINT)ts);

	gTestParam->round = 0;
	gTestParam->ts_start = ts;
	gTestParam->ts_stop = ts;
	gTestParam->ts_total = 0;
	//gTestParam->robots = NULL;
	//gTestParam->tpg = NULL;
	gTestParam->history = history_create();
	//gTestParam->running = false;

	file_changed();
	/*if(!file_save())
	{
		test_free();
		file_close();
		return TEST_RET_FAIL;
	}*/
	return TEST_RET_OK;
}

//�򿪾ɵ�ʵ��
TEST_RET test_open(HWND hWnd)
{
	if(!file_open(hWnd))
	{
		return TEST_RET_CANCEL;
	}

	test_init();
	if(!file_load())
	{
		file_close();
		test_free();
		return TEST_RET_FAIL;
	}

	return TEST_RET_OK;
}

//�ر�ʵ��
void test_close(void)
{
	if(file_is_open())
	{
		file_save();
		file_close();
	}
	test_free();
}


/////////////////////////////////////////////
HANDLE gTestThread = NULL;

//��N��ʵ��(���߳�)
UINT WINAPI test_batch_thread(LPVOID lpParameter)
{
	HWND hWnd = (HWND)lpParameter;
	PROBOT *top_robots = lab_top_robots_create();
	PROBOT *odd_robots = lab_top_robots_create();
	RACIALS racials = lab_racial_create();

	if(gTestParam->robots == NULL)
	{
		//���������һ��������
		ROBOTS robots = robot_create(gConfig.lab_more_robot_count);
		robot_base_init();
		for(int i=0;i<gConfig.lab_more_robot_count;i++)
		{
			robot_init(robots[i]);
		}

		//��ȡʵ���߳���
		int thread_num = test_thread_num();

		//����1���������
		PLAB_THREAD_PARAM_GROUP tpg = lab_thread_params_create(robots, gTestParam->ts_stop, thread_num);

		gTestParam->round = 0;
		gTestParam->robots = robots;
		gTestParam->tpg = tpg;
	}
	else if(gTestParam->round > 0)
	{
		//��ʼ�����������������
		lab_racials_init(racials, gTestParam->robots);
	}

	//ʵ���̳߳�ʼ��
	lab_run_more_thread_init(gTestParam->tpg);

	time_t ts = time(NULL);

	while(gTestParam->running)
	{
		gTestParam->round++;
		DEBUG("round: %d", gTestParam->round);
		//������Ϣ:ʵ������´�
		PostMessage(hWnd, WM_TEST_ROUND_NEW, gTestParam->round, gTestParam->robots[0].score);

		if(gTestParam->round  > 1) //�����˽������
		{
			lab_robot_evolve(gTestParam->robots, racials, top_robots, odd_robots);
		}

		//��1��ʵ��(���߳�)
		lab_run_more(gTestParam->tpg);

		//�����˵÷�����
		lab_robot_sort(gTestParam->robots, racials);

		//��¼������ʷ����
		PHISTORYITEM history_item = history_append(gTestParam->history, gTestParam->round);
		for(int i=0;i<history_item->top_len;i++)
		{
			ROBOT &robot = gTestParam->robots[i];
			ROBOT &top = history_item->top[i];
			top.id = robot.id;
			top.father_id = robot.father_id;
			top.generation = robot.generation;
			top.score = robot.score;
			//top.gene = robot_gene_create(robot.gene);
			//top.steps = robot_steps_create(robot.steps);
		}
	}

	time_t ts_stop = time(NULL);
	DEBUG("total: %ld(s)", ts_stop - ts);

	//ʵ���߳��ͷ�
	lab_run_more_thread_free(gTestParam->tpg);

	gTestParam->ts_stop = ts_stop;
	gTestParam->ts_total += ts_stop - ts; //��ʱ

	//������Ϣ:N��ʵ����̽���
	PostMessage(hWnd, WM_TEST_BATCH_STOP, (WPARAM)ts, (LPARAM)ts_stop);

	//д�ļ�
	file_changed();
	file_save();

	lab_racial_destroy(racials);
	lab_top_robots_destroy(odd_robots);
	lab_top_robots_destroy(top_robots);

	CloseHandle(gTestThread);
	gTestThread = NULL;
	return 0;
}

//��N��ʵ���Ƿ������
inline bool test_batch_running(void)
{
	return (gTestThread != NULL);
}

//��ʼ��N��ʵ��(���߳�)
void test_batch_start(HWND hWnd)
{
	gTestParam->running = true;
	gTestThread = (HANDLE)_beginthreadex(NULL, 0, test_batch_thread, hWnd, 0, NULL);
	//CloseHandle(gTestThread); //������˺���Ͳ���WaitForSingleObject
}

//ֹͣ��N��ʵ��
void test_batch_stop(bool sync)
{
	gTestParam->running = false;
	if(sync)
	{
		/*while(test_batch_running())
		{
			//DoEvents();
			Sleep(0);
		}*/
		if(test_batch_running())
		{
			WaitForSingleObject(gTestThread, INFINITE);
		}
	}
}

/////////////////////////////////////////////
//�ܶ���ʵ��
void test_run(GENE gene, STEPS steps)
{
	MAP map = map_create();
	bool init_steps = (steps == NULL);
	if(init_steps)
	{
		steps = robot_steps_create();
	}
	bool init_gene = (gene == NULL);
	if(init_gene)
	{
		gene = robot_gene_create(true);
		DEBUG("gene: %s", gene);
	}
	assert(gConfig.lab_run_count > 0);

	long total = 0;
	int score, max = INT_MIN, min = INT_MAX;
	for(int i=0;i<gConfig.lab_run_count;i++)
	{
		if(i % 100 == 0)
		{
			DoEvents();
		}

		//������ɵ�ͼ
		map_init(map);
		DEBUG("map: %s", map);

		//��1��ʵ��
		score = lab_run_once(map, gene, steps);

		if(score < min){min =  score;}
		if(score > max){max =  score;}
		total += score;
		DEBUG("score: %d", score);
		DEBUG("steps: %s", steps);
	}

	//������1�εĽ��
	draw_map(map);
	draw_steps(steps, total, gConfig.lab_run_count, max, min);
	DEBUG("score: %.2f    max: %d    min: %d", (float)total / gConfig.lab_run_count, max, min);
	DEBUG("steps: %s", steps);

	//�ͷ���Դ
	if(init_gene)
	{
		robot_gene_destroy(gene);
	}
	if(init_steps)
	{
		robot_steps_destroy(steps);
	}
	map_destroy(map);
}
