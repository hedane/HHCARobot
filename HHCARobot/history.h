#pragma once

#include "robot.h"

//��ʷ��¼������
typedef struct tagHISTORYITEM
{
	int round;
	ROBOTS top;
	int top_len;
	tagHISTORYITEM *next;
} HISTORYITEM, *PHISTORYITEM;

//��ʷ���ݽṹ
typedef struct
{
	PHISTORYITEM head;
	PHISTORYITEM tail;
	UINT count;
} HISTORY, *PHISTORY;

//������ʷ����
PHISTORY history_create();

//������ʷ����
void history_destroy(PHISTORY history);

//�����ʷ��¼
PHISTORYITEM history_append(PHISTORY history, int round);
