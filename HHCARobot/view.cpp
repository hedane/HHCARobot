#include "stdafx.h"
#include "view.h"
#include "draw.h"
#include "test.h"

VIEW gCurView = VIEW_WELCOME;
HMENU gMenuMain = NULL;
HWND gBtnPrev = NULL;
HWND gBtnNext = NULL;
GENE gCurGene = NULL;
int gCurListPage = 1;
int gCurGenePage = 1;
int gCurStatPage = 1;

/////////////////////////////////////////////
//��ͼ��ʼ��
void view_init(HWND hWnd)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	RECT tRect;
	GetClientRect(hWnd, &tRect);

	gBtnPrev = CreateWindow("BUTTON", NULL,
		WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
		tRect.right - 66 - 66 - 6, tRect.bottom - 27, 66,  27,
		hWnd, (HMENU)IDB_PREV, // control id
		hInst, NULL);
	gBtnNext = CreateWindow("BUTTON", NULL,
		WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON,
		tRect.right - 66, tRect.bottom - 27, 66,  27,
		hWnd, (HMENU)IDB_NEXT, // control id
		hInst, NULL);
	WPARAM font = (WPARAM)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(gBtnPrev, WM_SETFONT, font, NULL);
	SendMessage(gBtnNext, WM_SETFONT, font, NULL);

	gMenuMain = GetMenu(hWnd);

	gCurGene = robot_gene_create(false);
}

//��ͼ�ͷ���Դ
void view_free(void)
{
	robot_gene_destroy(gCurGene);

	DestroyWindow(gBtnPrev);
	DestroyWindow(gBtnNext);
}

/////////////////////////////////////////////
//��ͼ��Ŀ�˵�ʹ��
void view_project_menu_enable(bool enable)
{
	if(enable)
	{
		EnableMenuItem(gMenuMain, IDM_NEW, MF_DISABLED|MF_GRAYED);
		EnableMenuItem(gMenuMain, IDM_OPEN, MF_DISABLED|MF_GRAYED);
		EnableMenuItem(gMenuMain, IDM_CLOSE, MF_ENABLED);
		EnableMenuItem(gMenuMain, IDM_ROBOT_RAND, MF_DISABLED|MF_GRAYED);
		EnableMenuItem(gMenuMain, IDM_HUMAN_DD, MF_DISABLED|MF_GRAYED);
		EnableMenuItem(gMenuMain, IDM_HUMAN_HD, MF_DISABLED|MF_GRAYED);
		EnableMenuItem(gMenuMain, IDM_HUMAN_ZZ, MF_DISABLED|MF_GRAYED);
	}
	else
	{
		EnableMenuItem(gMenuMain, IDM_NEW, MF_ENABLED);
		EnableMenuItem(gMenuMain, IDM_OPEN, MF_ENABLED);
		EnableMenuItem(gMenuMain, IDM_CLOSE, MF_DISABLED|MF_GRAYED);
		EnableMenuItem(gMenuMain, IDM_ROBOT_RAND, MF_ENABLED);
		EnableMenuItem(gMenuMain, IDM_HUMAN_DD, MF_ENABLED);
		EnableMenuItem(gMenuMain, IDM_HUMAN_HD, MF_ENABLED);
		EnableMenuItem(gMenuMain, IDM_HUMAN_ZZ, MF_ENABLED);
	}
}

//��ͼ��ť����
void view_button_text(const char *prev, const char *next)
{
	if(prev != NULL)
	{
		SendMessage(gBtnPrev, WM_SETTEXT, (WPARAM)NULL, (LPARAM)prev);
	}
	if(next != NULL)
	{
		SendMessage(gBtnNext, WM_SETTEXT, (WPARAM)NULL, (LPARAM)next);
	}
}

//��ͼ��ťʹ��
void view_button_enable(BOOL enable)
{
	EnableWindow(gBtnPrev, enable);
	EnableWindow(gBtnNext, enable);
}


/////////////////////////////////////////////
//��ͼˢ��
void view_refresh(LONG_PTR param)
{
	switch(gCurView)
	{
	case VIEW_WELCOME:
		draw_text("��ӭ����%s!", param);
		break;
	case VIEW_MESSAGE:
		draw_text("%s", param);
		break;
	case VIEW_LAB_LIST:
		gCurListPage = (int)param;
		draw_test(gCurListPage);
		break;
	case VIEW_LAB_RUN:
		draw_text("����������...");
		break;
	case VIEW_LAB_STOP:
		draw_text("����ֹͣ����...");
		break;
	case VIEW_LAB_STAT_CHART:
		draw_stat(gTestParam->history);
		break;
	case VIEW_LAB_STAT_LIST:
		gCurStatPage = (int)param;
		draw_stat_list(gTestParam->history, gCurStatPage);
		break;
	case VIEW_ROBOT_RAND:
		draw_text("����1��<���AI>��%d�������ͼ", gConfig.lab_run_count);
		break;
	case VIEW_HUMAN_DD:
		draw_text("����1��<�����˹�AI>��%d�������ͼ", gConfig.lab_run_count);
		break;
	case VIEW_HUMAN_HD:
		draw_text("����1��<�м��˹�AI>��%d�������ͼ", gConfig.lab_run_count);
		break;
	case VIEW_HUMAN_ZZ:
		draw_text("����1��<�߼��˹�AI>��%d�������ͼ", gConfig.lab_run_count);
		break;
	case VIEW_LAB_ROBOT_GENE:
	case VIEW_ROBOT_RAND_GENE:
	case VIEW_HUMAN_DD_GENE:
	case VIEW_HUMAN_HD_GENE:
	case VIEW_HUMAN_ZZ_GENE:
		gCurGenePage = (int)param;
		draw_gene(gCurGene, gCurGenePage);
		break;
	default:
		break;
	}
}

//��ͼ�л�
void view_switch(VIEW view, LONG_PTR param)
{
	if(view == gCurView) //û��
	{
		view_refresh(param);
		return;
	}

	gCurView = view;
	view_refresh(param);

	BOOL vBtnPrev = FALSE, vBtnNext = FALSE, eBtnAll = FALSE;
	switch(view)
	{
	case VIEW_WELCOME:
		view_project_menu_enable(false);
		break;
	case VIEW_LAB_LIST:
		view_project_menu_enable(true);
		view_button_text("ͳ��ͼ��", "��������");
		if(gTestParam->history->count > 0) //����ʷ��¼ 
		{
			vBtnPrev = TRUE;
		}
		vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_LAB_RUN:
		view_button_text(NULL, "ֹͣ����");
		vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_LAB_STOP:
		//view_button_text(NULL, "ֹͣ����");
		vBtnNext = TRUE;
		eBtnAll = FALSE;
		break;
	case VIEW_LAB_ROBOT_GENE:
		view_button_text("������ҳ", "AI����");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_LAB_ROBOT_RUN:
		view_button_text("���ػ���", "AI����");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_LAB_STAT_CHART:
		view_button_text("������ҳ", "�鿴�б�");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_LAB_STAT_LIST:
		view_button_text("������ҳ", "�鿴ͼ��");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_ROBOT_RAND:
		view_button_text("���ɻ���", "��ʼ����");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_ROBOT_RAND_GENE:
		view_button_text("��������", "��ʼ����");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_HUMAN_DD:
	case VIEW_HUMAN_HD:
	case VIEW_HUMAN_ZZ:
		view_button_text("�鿴����", "��ʼ����");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_HUMAN_DD_GENE:
	case VIEW_HUMAN_HD_GENE:
	case VIEW_HUMAN_ZZ_GENE:
		view_button_text(NULL, "��ʼ����");
		vBtnNext = eBtnAll = TRUE;
		break;
	case VIEW_ROBOT_RAND_RUN:
	case VIEW_HUMAN_DD_RUN:
	case VIEW_HUMAN_HD_RUN:
	case VIEW_HUMAN_ZZ_RUN:
		view_button_text("�鿴����", "��������");
		vBtnPrev = vBtnNext = eBtnAll = TRUE;
		break;
	default:
		break;
	}
	view_button_enable(eBtnAll);
	ShowWindow(gBtnPrev, vBtnPrev ? SW_SHOW : SW_HIDE);
	ShowWindow(gBtnNext, vBtnNext ? SW_SHOW : SW_HIDE);
}

//��ͼ����
void view_keydown(int key)
{
	switch(key)
	{
	case VK_ESCAPE: //ESC����
		switch(gCurView)
		{
		case VIEW_LAB_ROBOT_GENE: //���򷵻�ʵ��
			view_switch(VIEW_LAB_LIST, gCurListPage);
			break;
		case VIEW_LAB_ROBOT_RUN: //���̷��ػ���
			view_switch(VIEW_LAB_ROBOT_GENE, gCurGenePage);
			break;
		case VIEW_LAB_STAT_CHART: //ͳͼ����ʵ��
			view_switch(VIEW_LAB_LIST, gCurListPage);
			break;
		case VIEW_LAB_STAT_LIST: //ͳ����ʵ��
			view_switch(VIEW_LAB_LIST, gCurListPage);
			break;
		case VIEW_ROBOT_RAND_RUN: //���̷��ػ���
			view_switch(VIEW_ROBOT_RAND_GENE, gCurGenePage);
			break;
		case VIEW_HUMAN_DD_RUN: //���̷��ػ���
			view_switch(VIEW_HUMAN_DD_GENE, gCurGenePage);
			break;
		case VIEW_HUMAN_HD_RUN: //���̷��ػ���
			view_switch(VIEW_HUMAN_HD_GENE, gCurGenePage);
			break;
		case VIEW_HUMAN_ZZ_RUN: //���̷��ػ���
			view_switch(VIEW_HUMAN_ZZ_GENE, gCurGenePage);
			break;
		default:
			break;
		}
		break;
	case VK_HOME: //��ҳ
	case VK_LEFT:
		switch(gCurView)
		{
		case VIEW_LAB_LIST:
		case VIEW_LAB_ROBOT_GENE:
		case VIEW_LAB_STAT_LIST:
		case VIEW_ROBOT_RAND_GENE:
		case VIEW_HUMAN_DD_GENE:
		case VIEW_HUMAN_HD_GENE:
		case VIEW_HUMAN_ZZ_GENE:
			view_refresh();
			break;
		default:
			break;
		}
		break;
	case VK_PRIOR: //��ҳ
	case VK_UP:
		switch(gCurView)
		{
		case VIEW_LAB_LIST:
			view_refresh(gCurListPage - 1);
			break;
		case VIEW_LAB_ROBOT_GENE:
		case VIEW_ROBOT_RAND_GENE:
		case VIEW_HUMAN_DD_GENE:
		case VIEW_HUMAN_HD_GENE:
		case VIEW_HUMAN_ZZ_GENE:
			view_refresh(gCurGenePage - 1);
			break;
		case VIEW_LAB_STAT_LIST:
			view_refresh(gCurStatPage - 1);
			break;
		default:
			break;
		}
		break;
	case VK_NEXT: //��ҳ
	case VK_DOWN:
		switch(gCurView)
		{
		case VIEW_LAB_LIST:
			view_refresh(gCurListPage + 1);
			break;
		case VIEW_LAB_ROBOT_GENE:
		case VIEW_ROBOT_RAND_GENE:
		case VIEW_HUMAN_DD_GENE:
		case VIEW_HUMAN_HD_GENE:
		case VIEW_HUMAN_ZZ_GENE:
			view_refresh(gCurGenePage + 1);
			break;
		case VIEW_LAB_STAT_LIST:
			view_refresh(gCurStatPage + 1);
			break;
		default:
			break;
		}
		break;
	case VK_END: //ĩҳ
	case VK_RIGHT:
		switch(gCurView)
		{
		case VIEW_LAB_LIST:
		case VIEW_LAB_ROBOT_GENE:
		case VIEW_LAB_STAT_LIST:
		case VIEW_ROBOT_RAND_GENE:
		case VIEW_HUMAN_DD_GENE:
		case VIEW_HUMAN_HD_GENE:
		case VIEW_HUMAN_ZZ_GENE:
			view_refresh(INT_MAX);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
