#include "stdafx.h"
#include "history.h"

//创建历史数据
PHISTORY history_create()
{
	PHISTORY p = (PHISTORY)malloc(sizeof(HISTORY));
	p->count = 0;
	p->head = NULL;
	p->tail = NULL;
	return p;
}

//销毁历史数据
void history_destroy(PHISTORY history)
{
	PHISTORYITEM cur = history->head;
	while(cur != NULL)
	{
		PHISTORYITEM next = cur->next;
		robot_destroy(cur->top, cur->top_len);
		free(cur);
		cur = next;
	}
	free(history);
}

//添加历史记录
PHISTORYITEM history_append(PHISTORY history, int round)
{
	PHISTORYITEM last = (PHISTORYITEM)malloc(sizeof(HISTORYITEM));
	last->round = round;
	last->top = robot_create(gConfig.history_robot_count);
	last->top_len = gConfig.history_robot_count;
	last->next = NULL;

	for(int i=0;i<last->top_len;i++)
	{
		last->top[i].gene = NULL;
		last->top[i].steps = NULL;
	}

	if(history->head == NULL)
	{
		history->head = last;
		history->tail = last;
		history->count = 1;
	}
	else
	{
		history->tail->next = last;
		history->tail = last;
		history->count++;
	}

	return last;
}
