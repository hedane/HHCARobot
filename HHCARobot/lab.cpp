#include "stdafx.h"
#include "lab.h"

/////////////////////////////////////////////
//跑1次实验
int lab_run_once(CMAP map_tpl, ROBOT &robot)
{
	MAP map = map_create(map_tpl);//拷贝地图
	int res = lab_run_once(map, robot.gene, robot.steps);
	map_destroy(map);//释放资源

	return res;
}
int lab_run_once(MAP map, CGENE gene, STEPS steps)
{
	int x=0, y=0, score=0;

	robot_steps_init(steps);

	for(int i=0;i<gConfig.robot_step_count;i++)
	{
		bool result;//行为结果

		//生成环境编码
		int ecode = map_env_code(map, x, y);
		//根据环境编码从基因中查找策略
		char policy = gene[ecode];

		//随机移动处理
		char action = (policy == ROBOT_ACT_RND) ? (ROBOT_ACT_RND_BASE + rand() % 3) : policy;

		//根据策略执行动作并打分
		switch(action)
		{
			case ROBOT_ACT_NORTH://向北移动
			case ROBOT_ACT_RND_NORTH://随机
				result = map_cell_movable(map, x, y+1);
				result ? y++ : score += gConfig.lab_action_konck_wall;//扣分
				break;
			case ROBOT_ACT_SOUTH://向南移动
			case ROBOT_ACT_RND_SOUTH://随机
				result = map_cell_movable(map, x, y-1);
				result ? y-- : score += gConfig.lab_action_konck_wall;//扣分
				break;
			case ROBOT_ACT_EAST://向东移动
			case ROBOT_ACT_RND_EAST://随机
				result = map_cell_movable(map, x+1, y);
				result ? x++ : score += gConfig.lab_action_konck_wall;//扣分
				break;
			case ROBOT_ACT_WEST://向西移动
			case ROBOT_ACT_RND_WEST://随机
				result = map_cell_movable(map, x-1, y);
				result ? x-- : score += gConfig.lab_action_konck_wall;//扣分
				break;
			case ROBOT_ACT_STAY://不动
				//do_nothing
				result = true;
				break;
			case ROBOT_ACT_CLEAN://捡起垃圾
				result = map_clean(map, x, y);
				if(result) //有垃圾
				{
					score += gConfig.lab_action_trash_ok;//加分
				}
				else //没有垃圾
				{
					score += gConfig.lab_action_trash_fail;//扣分
				}
				break;
			default://异常
				throw action;
				//result = false;
		}

		robot_steps_set(steps, i, action, result);
	}

	return score;//返回分数
}

/////////////////////////////////////////////
//创建多线程参数组
inline PLAB_THREAD_PARAM_GROUP lab_thread_params_create(const ROBOTS robots, time_t rnd_seed, int thread_num, CMAP map_tpl)
{
	//创建地图
	MAP map = map_create(map_tpl);

	//计算线程数量
	int rem = gConfig.lab_more_robot_count;
	int div = 0;

	//根据逻辑CPU数量分配参数
	LAB_THREAD_PARAMS params = NULL;
	if(thread_num > 1)
	{
		div = gConfig.lab_more_robot_count / thread_num;
		rem = gConfig.lab_more_robot_count % thread_num;
		//分配线程参数数组空间
		params = (LAB_THREAD_PARAMS)malloc(sizeof(LAB_THREAD_PARAM) * thread_num);
		//线程参数初始化
		for(int i=0;i<thread_num;i++)
		{
			LAB_THREAD_PARAM &param = params[i];
			param.robots = &robots[div * i]; //数组指针
			param.len = div;	//数组长度
			param.map = map;
			param.rnd_seed = rnd_seed + i + 1;
			param.no = i + 1;
		}
	}

	//单个CPU或多余的部分
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
	//构建返回结构
	PLAB_THREAD_PARAM_GROUP tpg = (PLAB_THREAD_PARAM_GROUP)malloc(sizeof(LAB_THREAD_PARAM_GROUP));
	tpg->map = map;
	tpg->num = thread_num;
	tpg->params = params;
	tpg->rem_param = rem_param;
	return tpg;
}
//释放多线程参数组
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

//多线程运行标志
bool gLabThreadsRunning = false;
HANDLE *gLabThreads = NULL; //线程句柄
HANDLE *gLabThreadsEvents = NULL; //线程(启动=0|结束>0)事件
long gLabThreadsRunningCount = 0; //正在跑任务的数量

//多线程入口函数
UINT WINAPI lab_run_more_thread(LPVOID lpParameter)
{
	LAB_THREAD_PARAM &param = *(PLAB_THREAD_PARAM)lpParameter;

	//随机数种子
	srand((UINT)param.rnd_seed);

	//逐个跑实验
	while(gLabThreadsRunning)
	{
		//主线程不检查
		if(param.no > 0)
		{
			//等待任务开始
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

			//跑1次实验
			int score = lab_run_once(param.map, robot);
			//DEBUG("id: X%ld.M%ld.D%d\tscore: %ld\tgene: %s", robot.father_id, robot.id, robot.generation, score, robot.gene);

			//加得分
			robot.score += score;
		}
		//主线程直接跳出
		if(param.no <= 0)
		{
			break;
		}
		//标志任务结束
		//WRITE("thread %d finish", param.no);
		if(InterlockedDecrement(&gLabThreadsRunningCount) <= 0)
		{   //最后一个，发信号
			SetEvent(gLabThreadsEvents[0]);
		}
	}

	return 0;
}

//实验线程初始化/释放/运行检查
void lab_run_more_thread_init(PLAB_THREAD_PARAM_GROUP tpg)
{
	gLabThreadsRunning = true;
	if(tpg->params != NULL)
	{
		gLabThreads = (HANDLE*)malloc(sizeof(HANDLE) * tpg->num);
		gLabThreadsEvents = (HANDLE*)malloc(sizeof(HANDLE) * (tpg->num + 1));
		gLabThreadsEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL); //结束事件
		gLabThreadsRunningCount = 0;
		for(int i=0;i<tpg->num;i++)
		{
			gLabThreadsEvents[i+1] = CreateEvent(NULL, FALSE, FALSE, NULL); //开始事件
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
		//主线程等待各线程结束
		WaitForMultipleObjects(tpg->num, gLabThreads, TRUE, INFINITE);

		//关闭句柄
		for(int i=0;i<tpg->num;i++)
		{
			CloseHandle(gLabThreads[i]);
			CloseHandle(gLabThreadsEvents[i+1]);
		}
		CloseHandle(gLabThreadsEvents[0]);

		//释放资源
		free(gLabThreads);
		free(gLabThreadsEvents);
		//gLabThreads = NULL;
		//gLabThreadsEvents = NULL;
		//gLabThreadsRunningCount = 0;
	}
}

//跑1代实验(多线程)
void lab_run_more(PLAB_THREAD_PARAM_GROUP tpg)
{
	//逐个在N个地图上跑实验
	for(int m=0;m<gConfig.lab_more_count;m++)
	{
		//随机生成地图
		map_init(tpg->map);
		//DEBUG("map %d: %s", m+1, tpg->map);

		//在该地图上分线程跑这批机器人
		if(tpg->params != NULL)
		{
			//WRITE("main set ready");
			gLabThreadsRunningCount = tpg->num;
			for(int i=1;i<=tpg->num;i++)
			{
				SetEvent(gLabThreadsEvents[i]);
			}
		}
		if(tpg->rem_param != NULL) //主线程处理多余的部分
		{
			lab_run_more_thread(tpg->rem_param);
		}

		if(tpg->params != NULL)
		{
			//主线程等待各线程执行结束
			//WRITE("main wait: %ld", gLabThreadsRunningCount);
			WaitForSingleObject(gLabThreadsEvents[0], INFINITE);
			//WRITE("main wait ok: %ld", gLabThreadsRunningCount);
		}
	}
}

/////////////////////////////////////////////
//机器人种族数组
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
		if(k == 0) //没有了
		{
			break;
		}
		if(k == father_id) //找到了
		{
			racial = &racials[m];
			break;
		}
		m++;
	}
	if(racial == NULL) //没找到
	{
		racial = &racials[m];
		racial->father_id = father_id; //创建新的
	}
	racial->count++; //数量+1
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

//机器人得分排序
inline int lab_robot_sort_compare(const void *a, const void *b)
{
	return (((PROBOT)b)->score - ((PROBOT)a)->score);  //从大到小排序
}
void lab_robot_sort(ROBOTS robots, RACIALS racials)
{
	qsort(robots, gConfig.lab_more_robot_count, sizeof(ROBOT), lab_robot_sort_compare);
	//前20%同姓族群最多不能大于50人
	int n = 0;
	memset(racials, 0, lab_racial_size());
	for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
	{
		const ROBOT &robot = robots[i];
		PRACIAL racial = lab_racials_counter(racials, robot.father_id);
		if(racial->count > gConfig.lab_more_robot_top_racial_limit) //超过人数限制了
		{
			//整体移动
			ROBOT r = robots[i];//复制
			memcpy(&robots[i], &robots[i+1], sizeof(ROBOT) * (gConfig.lab_more_robot_count - i - 1)); //后面位置的前移
			robots[gConfig.lab_more_robot_count - 1] = r; //复制到末尾
			if(n++ >= gConfig.lab_more_robot_count - i) //防止死循环
			{
				break;
			}
			i--;//这个位置再来一次
		}
	}
}


/////////////////////////////////////////////
//机器人指针数组，每个单元存放1个TOP机器人的指针，便于进化处理中随机打乱（而不会打乱指向的源数组）
inline PROBOT *lab_top_robots_create()
{
	return (PROBOT *)malloc(sizeof(PROBOT) * gConfig.lab_more_robot_top_count);
}
inline void lab_top_robots_destroy(PROBOT top_robots[])
{
	free(top_robots);
}
//机器人交叉进化
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
		int piece_pos = robot_gene_evolve(robot1.gene, robot2.gene, robot3.gene, robot4.gene);//基因进化
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
//TOP200内的繁殖，同姓之间繁殖优先展开，如果同姓是奇数，余下的才随机匹配繁殖
void lab_robot_evolve(ROBOTS robots, RACIALS racials, PROBOT top_robots[], PROBOT odd_robots[])
{
	int oc = 0, index = gConfig.lab_more_robot_top_count;
	for(int ri=0;ri<gConfig.lab_more_robot_top_count;ri++) //种族循环
	{
		long key = racials[ri].father_id;
		if(key == 0) //没有了
		{
			break;
		}

		//从前20%取出该族的族人Top
		int tc = 0;
		for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
		{
			if(robots[i].father_id == key)
			{
				top_robots[tc] = &robots[i];
				tc++;
			}
		}

		if(tc % 2 == 1) //奇数个，取出最后一个备用
		{
			tc--; //计数减去多余那个
			odd_robots[oc] = top_robots[tc];
			oc++;
		}

		//打乱Top机器人的指针
		for (int i=0; i<tc; i++)
		{
			int r = rand() % tc;

			PROBOT t = top_robots[i];
			top_robots[i] = top_robots[r];
			top_robots[r] = t;
		}

		//Top繁殖到中20%
		index = lab_robot_evolve(robots, index, top_robots, tc);
	}
	if(oc > 0)
	{
		//奇数Top繁殖到中20%
		index = lab_robot_evolve(robots, index, odd_robots, oc);
	}

	//前20%得分清零
	for(int i=0;i<gConfig.lab_more_robot_top_count;i++)
	{
		robots[i].score = 0;
	}

	//剩下的重新初始化基因
	for(int i=index;i<gConfig.lab_more_robot_count;i++)
	{
		//基因重新生成，但存储地址不变
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
