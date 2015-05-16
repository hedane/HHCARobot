#pragma once

#include "resource.h"
#include "robot.h"

//VS默认全局变量
//extern HINSTANCE hInst;
//extern TCHAR szTitle[];
//extern TCHAR szWindowClass[];

//当前功能页
typedef enum
{
	VIEW_WELCOME, //欢迎
	VIEW_MESSAGE, //消息
	VIEW_LAB_LIST, //实验列表
	VIEW_LAB_RUN, //实验运行
	VIEW_LAB_STOP, //实验停止
	VIEW_LAB_ROBOT_GENE, //实验单个机器人
	VIEW_LAB_ROBOT_RUN, //实验单个机器人复盘
	VIEW_LAB_STAT_CHART, //实验统计图表
	VIEW_LAB_STAT_LIST, //实验统计列表
	VIEW_ROBOT_RAND, //随机AI
	VIEW_ROBOT_RAND_GENE, //随机AI基因
	VIEW_ROBOT_RAND_RUN, //随机AI复盘
	VIEW_HUMAN_DD, //低级人工AI
	VIEW_HUMAN_DD_GENE, //低级人工AI基因
	VIEW_HUMAN_DD_RUN, //低级人工AI复盘
	VIEW_HUMAN_HD, //中级人工AI
	VIEW_HUMAN_HD_GENE, //中级人工AI基因
	VIEW_HUMAN_HD_RUN, //中级人工AI复盘
	VIEW_HUMAN_ZZ, //高级人工AI
	VIEW_HUMAN_ZZ_GENE, //高级人工AI基因
	VIEW_HUMAN_ZZ_RUN, //高级人工AI复盘
} VIEW;

extern VIEW gCurView;
//extern HMENU gMenuMain;
//extern HWND gBtnPrev;
//extern HWND gBtnNext;
extern GENE gCurGene;
extern int gCurListPage;
extern int gCurGenePage;
extern int gCurStatPage;

//视图初始化
void view_init(HWND hWnd);

//视图释放资源
void view_free(void);

//视图项目菜单使能
//void view_project_menu_enable(bool enable);

//视图按钮文字
void view_button_text(const char *prev = NULL, const char *next = NULL);

//视图按钮使能
void view_button_enable(BOOL enable);

//视图刷新
void view_refresh(LONG_PTR param = NULL);

//视图切换
void view_switch(VIEW view, LONG_PTR param = NULL);

//视图按键
void view_keydown(int key);
