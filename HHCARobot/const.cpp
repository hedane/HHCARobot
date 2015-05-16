#include "stdafx.h"
//#include "const.h"

//ȫ������
CONFIG gConfig = {
	MAP_SIDE_NUM,
	MAP_TRASH_NUM,

	ROBOT_GENE_LENGTH,
	ROBOT_STEP_COUNT,
	0,
	0,
	ROBOT_EVO_VAR_RATE,
	ROBOT_EVO_VAR_PIECE,

	LAB_ACTION_TRASH_OK,
	LAB_ACTION_TRASH_FAIL,
	LAB_ACTION_KONCK_WALL,

	LAB_MORE_COUNT,
	LAB_MORE_ROBOT_COUNT,
	LAB_MORE_ROBOT_TOP_COUNT,
	LAB_MORE_ROBOT_TOP_RACIAL_LIMIT,

	LAB_RUN_COUNT,

	HISTORY_ROBOT_COUNT,
};

//��ֹ��Ϣ����
void DoEvents(void)
{
    MSG msg;
    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
    {
        DispatchMessage(&msg);
        TranslateMessage(&msg);
    }
}

//ʱ���ת��Ϊ�ַ���
void Ts2Str(char *buf, size_t size, time_t ts)
{
	tm p;
	gmtime_s(&p, &ts);
	sprintf_s(buf, size, "%02d:%02d:%02d", p.tm_yday * 24 + p.tm_hour, p.tm_min, p.tm_sec);  
}
void Ts2Str(char *buf, size_t size, const char *fmt, time_t ts, bool local)
{
	tm p;
	if(local)
	{
		localtime_s(&p, &ts);
	}
	else
	{
		gmtime_s(&p, &ts);
	}
    strftime(buf, size, fmt, &p);  
}
