#pragma once

//�������ַ�������
#define MAX_LOADSTRING 100
//��󱣴��ַ�������
#define MAX_SAVESTRING 256

//��ͼ�߳�(������)
#define MAP_SIDE_NUM	25
//��ͼ�����������
#define MAP_TRASH_NUM	250

//�����˻��򳤶�
#define ROBOT_GENE_LENGTH	243
//�������ж�����
#define ROBOT_STEP_COUNT	400

//�����˽����������(�ٷֱ�)
#define ROBOT_EVO_VAR_RATE	5
//�����˽�������Ƭ����
#define ROBOT_EVO_VAR_PIECE	5

//ʵ���л����˼��������÷�
#define LAB_ACTION_TRASH_OK		10
//ʵ���л����˼�������۷�
#define LAB_ACTION_TRASH_FAIL	-1
//ʵ���л�����ײǽ�۷�
#define LAB_ACTION_KONCK_WALL	-5

//1��ʵ�������
#define LAB_MORE_COUNT	200
//1��ʵ��Ļ�������
#define LAB_MORE_ROBOT_COUNT	1000
//1��ʵ��Ļ�����������
#define LAB_MORE_ROBOT_TOP_COUNT	200
//1��ʵ��Ļ���������ͬ����������
#define LAB_MORE_ROBOT_TOP_RACIAL_LIMIT	50

//���̴���
#define LAB_RUN_COUNT	1

//��ʷ����Top����������
#define HISTORY_ROBOT_COUNT	1

typedef struct
{
	//��ͼ�߳�(������)
	int map_side_num;
	//��ͼ�����������
	int map_trash_num;

	//�����˻��򳤶�
	int robot_gene_length;
	//�������ж�����
	int robot_step_count;
	//��������������ID
	long robot_id_base;
	//��������������ID
	long robot_father_id_base;
	//�����˽����������(�ٷֱ�)
	int robot_evo_var_rate;
	//�����˽�������Ƭ����
	int robot_evo_var_piece;

	//ʵ���л����˼��������÷�
	int lab_action_trash_ok;
	//ʵ���л����˼�������۷�
	int lab_action_trash_fail;
	//ʵ���л�����ײǽ�۷�
	int lab_action_konck_wall;

	//1��ʵ�������
	int lab_more_count;
	//1��ʵ��Ļ�������
	int lab_more_robot_count;
	//1��ʵ��Ļ�����������
	int lab_more_robot_top_count;
	//1��ʵ��Ļ���������ͬ����������
	int lab_more_robot_top_racial_limit;

	//���̴���
	int lab_run_count;

	//��ʷ����Top����������
	int history_robot_count;

} CONFIG;

//ȫ������
extern CONFIG gConfig;

//��ӡ������Ϣ
#ifdef _DEBUG
#define DEBUG(fmt, ...)	{\
	char buf[4096];\
	sprintf_s(buf, sizeof(buf), fmt, __VA_ARGS__);\
	size_t n = strlen(buf);\
	buf[n++] = '\r';\
	buf[n++] = '\n';\
	buf[n++] = '\0';\
	OutputDebugString(buf);\
}
#else
#define DEBUG(...) {}
#endif

/*#define WRITE(fmt, ...) {\
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
	if(hConsole) {\
		char buf[4096];\
		sprintf_s(buf, sizeof(buf), fmt, __VA_ARGS__);\
		size_t n = strlen(buf);\
		buf[n++] = '\r';\
		buf[n++] = '\n';\
		buf[n++] = '\0';\
		WriteConsole(hConsole, buf, (DWORD)n, NULL, NULL);\
	}\
}*/

//��ֹ��Ϣ����
void DoEvents(void);

//ʱ���ת��Ϊ�ַ���
void Ts2Str(char *buf, size_t size, time_t ts);
void Ts2Str(char *buf, size_t size, const char *fmt, time_t ts, bool local = true);


//�Զ�����Ϣ
#define WM_TEST_ROUND_NEW		(WM_USER + 0) //ʵ������´� wParam:����
#define WM_TEST_BATCH_STOP		(WM_USER + 11) //��ʵ����̽���
