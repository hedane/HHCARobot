#pragma once

#include "resource.h"
#include "robot.h"

//VSĬ��ȫ�ֱ���
//extern HINSTANCE hInst;
//extern TCHAR szTitle[];
//extern TCHAR szWindowClass[];

//��ǰ����ҳ
typedef enum
{
	VIEW_WELCOME, //��ӭ
	VIEW_MESSAGE, //��Ϣ
	VIEW_LAB_LIST, //ʵ���б�
	VIEW_LAB_RUN, //ʵ������
	VIEW_LAB_STOP, //ʵ��ֹͣ
	VIEW_LAB_ROBOT_GENE, //ʵ�鵥��������
	VIEW_LAB_ROBOT_RUN, //ʵ�鵥�������˸���
	VIEW_LAB_STAT_CHART, //ʵ��ͳ��ͼ��
	VIEW_LAB_STAT_LIST, //ʵ��ͳ���б�
	VIEW_ROBOT_RAND, //���AI
	VIEW_ROBOT_RAND_GENE, //���AI����
	VIEW_ROBOT_RAND_RUN, //���AI����
	VIEW_HUMAN_DD, //�ͼ��˹�AI
	VIEW_HUMAN_DD_GENE, //�ͼ��˹�AI����
	VIEW_HUMAN_DD_RUN, //�ͼ��˹�AI����
	VIEW_HUMAN_HD, //�м��˹�AI
	VIEW_HUMAN_HD_GENE, //�м��˹�AI����
	VIEW_HUMAN_HD_RUN, //�м��˹�AI����
	VIEW_HUMAN_ZZ, //�߼��˹�AI
	VIEW_HUMAN_ZZ_GENE, //�߼��˹�AI����
	VIEW_HUMAN_ZZ_RUN, //�߼��˹�AI����
} VIEW;

extern VIEW gCurView;
//extern HMENU gMenuMain;
//extern HWND gBtnPrev;
//extern HWND gBtnNext;
extern GENE gCurGene;
extern int gCurListPage;
extern int gCurGenePage;
extern int gCurStatPage;

//��ͼ��ʼ��
void view_init(HWND hWnd);

//��ͼ�ͷ���Դ
void view_free(void);

//��ͼ��Ŀ�˵�ʹ��
//void view_project_menu_enable(bool enable);

//��ͼ��ť����
void view_button_text(const char *prev = NULL, const char *next = NULL);

//��ͼ��ťʹ��
void view_button_enable(BOOL enable);

//��ͼˢ��
void view_refresh(LONG_PTR param = NULL);

//��ͼ�л�
void view_switch(VIEW view, LONG_PTR param = NULL);

//��ͼ����
void view_keydown(int key);
