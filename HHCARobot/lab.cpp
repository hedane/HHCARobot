#include "stdafx.h"
#include "lab.h"

/////////////////////////////////////////////
//��1��ʵ��
int lab_run_once(CMAP map_tpl, ROBOT &robot)
{
	MAP map = map_create(map_tpl);//������ͼ
	int res = lab_run_once(map, robot.gene, robot.steps);
	map_destroy(map);//�ͷ���Դ

	return res;
}
int lab_run_once(MAP map, CGENE gene, STEPS steps)
{
	int x=0, y=0, score=0;

	robot_steps_init(steps);

	for(int i=0;i<gConfig.robot_step_count;i++)
	{
		bool result;//��Ϊ���

		//���ɻ�������
		int ecode = map_env_code(map, x, y);
		//���ݻ�������ӻ����в��Ҳ���
		char policy = gene[ecode];

		//����ƶ�����
		char action = (policy == ROBOT_ACT_RND) ? (ROBOT_ACT_RND_BASE + rand() % 3) : policy;

		//���ݲ���ִ�ж��������
		switch(action)
		{
			case ROBOT_ACT_NORTH://���ƶ�
			case ROBOT_ACT_RND_NORTH://���
				result = map_cell_movable(map, x, y+1);
				result ? y++ : score += gConfig.lab_action_konck_wall;//�۷�
				break;
			case ROBOT_ACT_SOUTH://�����ƶ�
			case ROBOT_ACT_RND_SOUTH://���
				result = map_cell_movable(map, x, y-1);
				result ? y-- : score += gConfig.lab_action_konck_wall;//�۷�
				break;
			case ROBOT_ACT_EAST://���ƶ�
			case ROBOT_ACT_RND_EAST://���
				result = map_cell_movable(map, x+1, y);
				result ? x++ : score += gConfig.lab_action_konck_wall;//�۷�
				break;
			case ROBOT_ACT_WEST://�����ƶ�
			case ROBOT_ACT_RND_WEST://���
				result = map_cell_movable(map, x-1, y);
				result ? x-- : score += gConfig.lab_action_konck_wall;//�۷�
				break;
			case ROBOT_ACT_STAY://����
				//do_nothing
				result = true;
				break;
			case ROBOT_ACT_CLEAN://��������
				result = map_clean(map, x, y);
				if(result) //������
				{
					score += gConfig.lab_action_trash_ok;//�ӷ�
				}
				else //û������
				{
					score += gConfig.lab_action_trash_fail;//�۷�
				}
				break;
			default://�쳣
				throw action;
				//result = false;
		}

		robot_steps_set(steps, i, action, result);
	}

	return score;//���ط���
}

/////////////////////////////////////////////
//�������̲߳�����
inline PLAB_THREAD_PARAM_GROUP lab_thread_params_create(const ROBOTS robots, time_t rnd_seed, int thread_num, CMAP map_tpl)
{
	//������ͼ
	MAP map = map_create(map_tpl);

	//�����߳�����
	int rem = gConfig.lab_more_robot_count;
	int div = 0;

	//�����߼�CPU�����������
	LAB_THREAD_PARAMS params = NULL;
	if(thread_num > 1)
	{
		div = gConfig.lab_more_robot_count / thread_num;
		rem = gConfig.lab_more_robot_count % thread_num;
		//�����̲߳�������ռ�
		params = (LAB_THREAD_PARAMS)malloc(sizeof(LAB_THREAD_PARAM) * thread_num);
		//�̲߳�����ʼ��
		for(int i=0;i<thread_num;i++)
		{
			LAB_THREAD_PARAM &param = params[i];
			param.robots = &robots[div * i]; //����ָ��
			param.len = div;	//���鳤��
			param.map = map;
			param.rnd_seed = rnd_seed + i + 1;
			param.no = i + 1;
		}
	}

	//����CPU�����Ĳ���
	PLAB_THREAD_PARAM rem_param = NULL;
	if(rem > 0)
	{
		rem_param = (PLAB_THREAD_PARAM)malloc(sizeof(LAB_THREAD_PARAM));
		rem_param->robots = &robots[div * thread_num];
		rem_param->len = rem;
		rem_param->map = map;
		rem_param->rnd_seed = rnd_seed;
		rem_param->no = 0;
	}
	//�������ؽṹ
	PLAB_THREAD_PARAM_GROUP tpg = (PLAB_THREAD_PARAM_GROUP)malloc(sizeof(LAB_THREAD_PARAM_GROUP));
	tpg->map = map;
	tpg->num = thread_num;
	tpg->params = params;
	tpg->rem_param = rem_param;
	return tpg;
}
//�ͷŶ��̲߳�����
inline void lab_thread_params_destroy(PLAB_THREAD_PARAM_GROUP tpg)
{
	if(tpg->map != NULL)
	{
		map_destroy(tpg->map);
	}
	if(tpg->params != NULL)
	{
		free(tpg->params);
	}
	if(tpg->rem_param != NULL)
	{
		free(tpg->rem_param);
	}
	free(tpg);
}

//���߳����б�־
bool gLabThreadsRunning = false;
HANDLE *gLabThreads = NULL; //�߳̾��
HANDLE *gLabThreadsEvents = NULL; //�߳�(����=0|����>0)�¼�
long gLabThreadsRunningCount = 0; //���������������

//���߳���ں���
UINT WINAPI lab_run_more_thread(LPVOID lpParameter)
{
	LAB_THREAD_PARAM &param = *(PLAB_THREAD_PARAM)lpParameter;

	//���������
	srand((UINT)param.rnd_seed);

	//�����ʵ��
	while(gLabThreadsRunning)
	{
		//���̲߳����
		if(param.no > 0)
		{
			//�ȴ�����ʼ
			//WRITE("thread %d wait", param.no);
			WaitForSingleObject(gLabThreadsEvents[param.no], INFINITE);
			if(!gLabThreadsRunning)
			{
				break;
			}
			//WRITE("thread %d start", param.no);
		}
		for(int i=0;i<param.len;i++)
		{
			ROBOT &robot = param.robots[i];

			//��1��ʵ��
			int score = lab_run_once(param.map, robot);
			//DEBUG("id: X%ld.M%ld.D%d\tscore: %ld\tgene: %s", robot.father_id, robot.id, robot.generation, score, robot.gene);

			//�ӵ÷�
			robot.score += score;
		}
		//���߳�ֱ������
		if(param.no <= 0)
		{
			break;
		}
		//��־�������
		//WRITE("thread %d finish", param.no);
		if(InterlockedDecrement(&gLabThreadsRunningCount) <= 0)
		{   //���һ�������ź�
			SetEvent(gLabThreadsEvents[0]);
		}
	}

	return 0;
}

//ʵ���̳߳�ʼ��/�ͷ�/���м��
void lab_run_more_thread_init(PLAB_THREAD_PARAM_GROUP tpg)
{
	gLabThreadsRunning = true;
	if(tpg->params != NULL)
	{
		gLabThreads = (HANDLE*)malloc(sizeof(HANDLE) * tpg->num);
		gLabThreadsEvents = (HANDLE*)malloc(sizeof(HANDLE) * (tpg->num + 1));
		gLabThreadsEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL); //�����¼�
		gLabThreadsRunningCount = 0;
		for(int i=0;i<tpg->num;i++)
		{
			gLabThreadsEvents[i+1] = CreateEvent(NULL, FALSE, FALSE, NULL); //��ʼ�¼�
			gLabThreads[i] = (HANDLE)_beginthreadex(NULL, 0, lab_run_more_thread, &tpg->params[i], 0, NULL);
		}
	}
}
void lab_run_more_thread_free(PLAB_THREAD_PARAM_GROUP tpg)
{
	gLabThreadsRunning = false;
	if(tpg->params != NULL)
	{
		for(int i=1;i<=tpg->num;i++)
		{
			SetEvent(gLabThreadsEvents[i]);
		}
		//���̵߳ȴ����߳̽���
		WaitForMultipleObjects(tpg->num, gLabThreads, TRUE, INFINITE);

		//�رվ��
		for(int i=0;i<tpg->num;i++)
		{
			CloseHandle(gLabThreads[i]);
			CloseHandle(gLabThreadsEvents[i+1]);
		}
		CloseHandle(gLabThreadsEvents[0]);

		//�ͷ���Դ
		free(gLabThreads);
		free(gLabThreadsEvents);
		//gLabThreads = NULL;
		//gLabThreadsEvents = NULL;
		//gLabThreadsRunningCount = 0;
	}
}

//��1��ʵ��(���߳�)
void lab_run_more(PLAB_THREAD_PARAM_GROUP tpg)
{
	//�����N����ͼ����ʵ��
	for(int m=0;m<gConfig.lab_more_count;m++)
	{
		//������ɵ�ͼ
		map_init(tpg->map);
		//DEBUG("map %d: %s", m+1, tpg->map);

		//�ڸõ�ͼ�Ϸ��߳�������������
		if(tpg->params != NULL)
		{
			//WRITE("main set ready");
			gLabThreadsRunningCount = tpg->num;
			for(int i=1;i<=tpg->num;i++)
			{
				SetEvent(gLabThreadsEvents[i]);
			}
		}
		if(tpg->rem_param != NULL) //���̴߳������Ĳ���
		{
			lab_run_more_thread(tpg->rem_param);
		}

		if(tpg->params != NULL)
		{
			//���̵߳ȴ����߳�ִ�н���
			//WRITE("main wait: %ld", gLabThreadsRunningCount);
			WaitForSingleObject(gLabThreadsEvents[0], INFINITE);
			//WRITE("main wait ok: %ld", gLabThreadsRunningCount);
		}
	}
}

/////////////////////////////////////////////
//��������������
inline int lab_racial_size(void)
{
	return sizeof(RACIAL) * gConfig.lab_more_robot_top_count;
}
inline RACIALS lab_racial_create(void)
{
	return (RACIALS)malloc(lab_racial_size());
}
inline void lab_racial_destroy(RACIALS racials)
{
	free(racials);
}
inline PRACIAL lab_racials_counter(RACIALS racials, long father_id)
{
	PRACIAL racial = NULL;
	int m = 0;
	while(m < gConfig.lab_more_robot_top_count)
	{
		long k = racials[m].father_id;
		if(k == 0) //û����
		{
			break;
		}
		if(k == father_id) //�ҵ���
		{
			racial = &racials[m];
			break;
		}
		m++;
	}
	if(racial == NULL) //û�ҵ�
	{
		racial = &racials[m];
		racial->father_id = father_id; //�����µ�
	}
	racial->count++; //����+1
	return racial;
}
void lab_racials_init(RACIALS racials, const ROBOTS robots)
{
	memset(racials, 0, lab_racial_size());
	for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
	{
		lab_racials_counter(racials, robots[i].father_id);
	}
}

//�����˵÷�����
inline int lab_robot_sort_compare(const void *a, const void *b)
{
	return (((PROBOT)b)->score - ((PROBOT)a)->score);  //�Ӵ�С����
}
void lab_robot_sort(ROBOTS robots, RACIALS racials)
{
	qsort(robots, gConfig.lab_more_robot_count, sizeof(ROBOT), lab_robot_sort_compare);
	//ǰ20%ͬ����Ⱥ��಻�ܴ���50��
	int n = 0;
	memset(racials, 0, lab_racial_size());
	for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
	{
		const ROBOT &robot = robots[i];
		PRACIAL racial = lab_racials_counter(racials, robot.father_id);
		if(racial->count > gConfig.lab_more_robot_top_racial_limit) //��������������
		{
			//�����ƶ�
			ROBOT r = robots[i];//����
			memcpy(&robots[i], &robots[i+1], sizeof(ROBOT) * (gConfig.lab_more_robot_count - i - 1)); //����λ�õ�ǰ��
			robots[gConfig.lab_more_robot_count - 1] = r; //���Ƶ�ĩβ
			if(n++ >= gConfig.lab_more_robot_count - i) //��ֹ��ѭ��
			{
				break;
			}
			i--;//���λ������һ��
		}
	}
}


/////////////////////////////////////////////
//������ָ�����飬ÿ����Ԫ���1��TOP�����˵�ָ�룬���ڽ���������������ң����������ָ���Դ���飩
inline PROBOT *lab_top_robots_create()
{
	return (PROBOT *)malloc(sizeof(PROBOT) * gConfig.lab_more_robot_top_count);
}
inline void lab_top_robots_destroy(PROBOT top_robots[])
{
	free(top_robots);
}
//�����˽������
int lab_robot_evolve(ROBOTS robots, int index, PROBOT top_robots[], int top_cnt)
{
	const int gene_half_len = gConfig.robot_gene_length / 2;
	for(int i=0;i<top_cnt;i+=2)
	{
		ROBOT &robot1 = robots[index];
		ROBOT &robot2 = robots[index + 1];
		ROBOT &robot3 = *top_robots[i];
		ROBOT &robot4 = *top_robots[i + 1];

		robot1.id = gConfig.robot_id_base++;
		robot2.id = gConfig.robot_id_base++;
		int piece_pos = robot_gene_evolve(robot1.gene, robot2.gene, robot3.gene, robot4.gene);//�������
		if(piece_pos > gene_half_len)
		{
			robot1.father_id = robot3.father_id;
			robot1.generation = robot3.generation + 1;
			robot2.father_id = robot4.father_id;
			robot2.generation = robot4.generation + 1;
		}
		else
		{
			robot1.father_id = robot4.father_id;
			robot1.generation = robot4.generation + 1;
			robot2.father_id = robot3.father_id;
			robot2.generation = robot3.generation + 1;
		}
		//float rate1 = (float)piece_pos / gConfig.robot_gene_length;
		//float rate2 = 1 - rate1;
		robot1.score = 0; //(int)(robot3.score * rate1 + robot4.score * rate2);
		robot2.score = 0; //(int)(robot3.score * rate2 + robot4.score * rate1);

		index += 2;
	}
	return index;
}
//TOP200�ڵķ�ֳ��ͬ��֮�䷱ֳ����չ�������ͬ�������������µĲ����ƥ�䷱ֳ
void lab_robot_evolve(ROBOTS robots, RACIALS racials, PROBOT top_robots[], PROBOT odd_robots[])
{
	int oc = 0, index = gConfig.lab_more_robot_top_count;
	for(int ri=0;ri<gConfig.lab_more_robot_top_count;ri++) //����ѭ��
	{
		long key = racials[ri].father_id;
		if(key == 0) //û����
		{
			break;
		}

		//��ǰ20%ȡ�����������Top
		int tc = 0;
		for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
		{
			if(robots[i].father_id == key)
			{
				top_robots[tc] = &robots[i];
				tc++;
			}
		}

		if(tc % 2 == 1) //��������ȡ�����һ������
		{
			tc--; //������ȥ�����Ǹ�
			odd_robots[oc] = top_robots[tc];
			oc++;
		}

		//����Top�����˵�ָ��
		for (int i=0; i<tc; i++)
		{
			int r = rand() % tc;

			PROBOT t = top_robots[i];
			top_robots[i] = top_robots[r];
			top_robots[r] = t;
		}

		//Top��ֳ����20%
		index = lab_robot_evolve(robots, index, top_robots, tc);
	}
	if(oc > 0)
	{
		//����Top��ֳ����20%
		index = lab_robot_evolve(robots, index, odd_robots, oc);
	}

	//ǰ20%�÷�����
	for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
	{
		robots[i].score = 0;
	}

	//ʣ�µ����³�ʼ������
	for(int i=index;i<gConfig.lab_more_robot_count;i++)
	{
		//�����������ɣ����洢��ַ����
		ROBOT &robot = robots[i];
		robot_init(robot, 0, 0, robot_gene_make(robot.gene), robot.steps);
	}
}
/*void lab_robot_evolve(ROBOTS robots, RACIALS racial)
{
	PROBOT *top_robots = lab_top_robots_create();
	PROBOT *odd_robots = lab_top_robots_create();

	lab_robot_evolve(robots, racial, top_robots, odd_robots);

	lab_top_robots_destroy(odd_robots);
	lab_top_robots_destroy(top_robots);
}*/
