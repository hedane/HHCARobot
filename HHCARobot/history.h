#pragma once

#include "robot.h"

//历史记录单链表
typedef struct tagHISTORYITEM
{
	int round;
	ROBOTS top;
	int top_len;
	tagHISTORYITEM *next;
} HISTORYITEM, *PHISTORYITEM;

//历史数据结构
typedef struct
{
	PHISTORYITEM head;
	PHISTORYITEM tail;
	UINT count;
} HISTORY, *PHISTORY;

//创建历史数据
PHISTORY history_create();

//销毁历史数据
void history_destroy(PHISTORY history);

//添加历史记录
PHISTORYITEM history_append(PHISTORY history, int round);
