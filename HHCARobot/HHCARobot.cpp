// HHCARobot.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "HHCARobot.h"
#include "view.h"
#include "draw.h"
#include "test.h"

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Config(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	GeneEdit(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_HHCAROBOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HHCAROBOT));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW/* | CS_DBLCLKS*/;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HHCAROBOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_HHCAROBOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 650, 550, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bMinimized = false; //��־�Ƿ���С��״̬

	switch (message)
	{
	case WM_CREATE:
		//AllocConsole();
		//��ʼ������
		draw_init(hWnd);
		view_init(hWnd);
		view_switch(VIEW_WELCOME, (LONG_PTR)szTitle);
		{//��������ӳ�ʼ��
			time_t ts = time(NULL); DEBUG("ts: %ld", ts);
			srand((UINT)ts);
		}
		break;
	case WM_COMMAND:
		{
			int wmId    = LOWORD(wParam);
			//int wmEvent = HIWORD(wParam);
			// �����˵�ѡ��:
			switch (wmId)
			{
			case IDB_PREV:
				switch(gCurView)
				{
				case VIEW_LAB_LIST:
					if(gTestParam->history->count > 0) //����ʷ��¼ 
					{
						view_switch(VIEW_LAB_STAT_CHART); //ͳ��ͼ��
					}
					break;
				case VIEW_ROBOT_RAND:
					robot_gene_make(gCurGene);
					view_switch(VIEW_ROBOT_RAND_GENE);
					break;
				case VIEW_ROBOT_RAND_GENE:
					robot_gene_make(gCurGene);
					view_refresh();
					break;
				case VIEW_HUMAN_DD:
					view_switch(VIEW_HUMAN_DD_GENE);
					break;
				case VIEW_HUMAN_HD:
					view_switch(VIEW_HUMAN_HD_GENE);
					break;
				case VIEW_HUMAN_ZZ:
					view_switch(VIEW_HUMAN_ZZ_GENE);
					break;
				case VIEW_LAB_ROBOT_GENE:
				case VIEW_LAB_ROBOT_RUN:
				case VIEW_LAB_STAT_CHART:
				case VIEW_LAB_STAT_LIST:
				case VIEW_ROBOT_RAND_RUN:
				case VIEW_HUMAN_DD_RUN:
				case VIEW_HUMAN_HD_RUN:
				case VIEW_HUMAN_ZZ_RUN:
					view_keydown(VK_ESCAPE); //ģ��ESC����
					break;
				default:
					break;
				}
				SetFocus(hWnd);
				break;
			case IDB_NEXT:
				switch(gCurView)
				{
				case VIEW_LAB_LIST:
					if(!test_batch_running())
					{
						view_switch(VIEW_LAB_RUN);
						test_batch_start(hWnd);
					}
					break;
				case VIEW_LAB_RUN:
					if(test_batch_running())
					{
						view_switch(VIEW_LAB_STOP);
						test_batch_stop();
					}
					break;
				case VIEW_LAB_ROBOT_GENE:
					view_switch(VIEW_LAB_ROBOT_RUN);
				case VIEW_LAB_ROBOT_RUN:
					if(gConfig.lab_run_count > 1){view_button_enable(FALSE);}
					test_run(gCurGene);
					if(gConfig.lab_run_count > 1){view_button_enable(TRUE);}
					break;
				case VIEW_LAB_STAT_CHART:
					view_switch(VIEW_LAB_STAT_LIST, gCurStatPage);
					break;
				case VIEW_LAB_STAT_LIST:
					view_switch(VIEW_LAB_STAT_CHART);
					break;
				case VIEW_ROBOT_RAND:
					gCurGenePage = 1;
				case VIEW_ROBOT_RAND_GENE:
					view_switch(VIEW_ROBOT_RAND_RUN);
				case VIEW_ROBOT_RAND_RUN:
					if(gConfig.lab_run_count > 1){view_button_enable(FALSE);}
					test_run(gCurGene);
					if(gConfig.lab_run_count > 1){view_button_enable(TRUE);}
					break;
				case VIEW_HUMAN_DD:
					gCurGenePage = 1;
				case VIEW_HUMAN_DD_GENE:
					view_switch(VIEW_HUMAN_DD_RUN);
				case VIEW_HUMAN_DD_RUN:
					if(gConfig.lab_run_count > 1){view_button_enable(FALSE);}
					test_run(gCurGene);
					if(gConfig.lab_run_count > 1){view_button_enable(TRUE);}
					break;
				case VIEW_HUMAN_HD:
					gCurGenePage = 1;
				case VIEW_HUMAN_HD_GENE:
					view_switch(VIEW_HUMAN_HD_RUN);
				case VIEW_HUMAN_HD_RUN:
					if(gConfig.lab_run_count > 1){view_button_enable(FALSE);}
					test_run(gCurGene);
					if(gConfig.lab_run_count > 1){view_button_enable(TRUE);}
					break;
				case VIEW_HUMAN_ZZ:
					gCurGenePage = 1;
				case VIEW_HUMAN_ZZ_GENE:
					view_switch(VIEW_HUMAN_ZZ_RUN);
				case VIEW_HUMAN_ZZ_RUN:
					if(gConfig.lab_run_count > 1){view_button_enable(FALSE);}
					test_run(gCurGene);
					if(gConfig.lab_run_count > 1){view_button_enable(TRUE);}
					break;
				default:
					break;
				}
				SetFocus(hWnd);
				break;
			case IDM_ROBOT_RAND:
				robot_gene_make(gCurGene);
				view_switch(VIEW_ROBOT_RAND);
				break;
			case IDM_HUMAN_DD:
				memcpy(gCurGene, "262363262161363161262363262262262262262262262262262262060363060161363161060363060060060060060060060060060060262262262262262262262262262060060060060060060060060060262363262161363161262363262262262262262262262262262262363363161161363161363363565", robot_gene_size());
				view_switch(VIEW_HUMAN_DD);
				break;
			case IDM_HUMAN_HD:
				memcpy(gCurGene, "464363262161161161060363060262363262161161161262363262363363060161161161060363060060363060161363161060363060262060262262464262262060262060363060161363161060363060161363262161161161262363262262363262161161161262363262363363161161161161363363565", robot_gene_size());
				view_switch(VIEW_HUMAN_HD);
				break;
			case IDM_HUMAN_ZZ:
				memcpy(gCurGene, "464363060161361161363363060262263262262263262262263262161363161161163161363363060060363060060061161060363060060060062262061162260362260060063060160161061060360060262363262161161161262363262262263262261363162262363262161363161161361161363363565", robot_gene_size());
				view_switch(VIEW_HUMAN_ZZ);
				break;
			case IDM_CONFIG:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIG), hWnd, Config);
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_NEW:
				switch(test_new(hWnd))
				{
				case TEST_RET_OK:
					view_switch(VIEW_LAB_LIST);
					break;
				case TEST_RET_FAIL:
					MessageBox(hWnd, "�½���Ŀʧ�ܣ�", szTitle, MB_OK);
					break;
				default: //cancel
					break;
				}
				break;
			case IDM_OPEN:
				view_switch(VIEW_MESSAGE, (LONG_PTR)"���ڴ���Ŀ�ļ�..."); //DoEvents();
				switch(test_open(hWnd))
				{
				case TEST_RET_OK:
					view_switch(VIEW_LAB_LIST);
					break;
				case TEST_RET_FAIL:
					MessageBox(hWnd, "����Ŀʧ�ܣ�", szTitle, MB_OK);
				default:  //cancel
					view_switch(VIEW_WELCOME, (LONG_PTR)szTitle);
					break;
				}
				break;
			case IDM_CLOSE:
				view_button_enable(FALSE);
				if(test_batch_running())
				{
					draw_text("����ֹͣ����..."); DoEvents();
					test_batch_stop(true);
				}
				test_close();//�ر���Ŀ
				view_switch(VIEW_WELCOME, (LONG_PTR)szTitle);
				break;
			case IDM_EXIT:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			draw_paint(hdc);//����
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_MOUSEMOVE:
		switch(gCurView)
		{
		case VIEW_LAB_LIST:
		case VIEW_LAB_STAT_LIST:
		case VIEW_LAB_ROBOT_GENE:
		case VIEW_ROBOT_RAND_GENE:
		case VIEW_HUMAN_DD_GENE:
		case VIEW_HUMAN_HD_GENE:
		case VIEW_HUMAN_ZZ_GENE:
			draw_table_row_hover(HIWORD(lParam));
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		switch(gCurView)
		{
		case VIEW_LAB_LIST:
			{
				PROBOT robot = draw_test_robot_get(gCurListPage, HIWORD(lParam));
				if(robot != NULL) //���л�����
				{
					memcpy(gCurGene, robot->gene, robot_gene_size());
					view_switch(VIEW_LAB_ROBOT_GENE);
				}
			}
			break;
		case VIEW_LAB_ROBOT_GENE:
		case VIEW_ROBOT_RAND_GENE:
		case VIEW_HUMAN_DD_GENE:
		case VIEW_HUMAN_HD_GENE:
		case VIEW_HUMAN_ZZ_GENE:
			{
				int env;
				char gene = draw_gene_get(gCurGenePage, HIWORD(lParam), env);
				if(gene > 0)
				{
					LPARAM param = MAKELPARAM(gene, env);
					BOOL success = (BOOL)DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_GENE_EDIT), hWnd, GeneEdit, param);
					if(success)
					{
						view_refresh(gCurGenePage);
					}
				}
			}
			break;
		default:
			break;
		}
		break;
	case WM_MOUSEWHEEL: //ģ��Ϊ���·�ҳ
		view_keydown((INT)wParam > 0 ? VK_PRIOR : VK_NEXT);
		break;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE && gCurView == VIEW_LAB_RUN) //ESCֹͣ����
		{
			PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDB_NEXT, BN_CLICKED), NULL);
		}
		view_keydown((int)wParam);
		break;
	case WM_SIZE:
		bMinimized = (wParam == SIZE_MINIMIZED);
		if(!bMinimized)
		{
			SetWindowText(hWnd, szTitle);
		}
		break;
	case WM_TEST_ROUND_NEW: //ʵ������´�
		draw_text("��������� %d �� ...", wParam);
		//draw_stat_runtime(wParam, gTestParam->history);
		if(bMinimized)
		{
			char buf[MAX_LOADSTRING];
			sprintf_s(buf, sizeof(buf), "��%d��:%.1f��", wParam, (double)lParam / gConfig.lab_more_count);
			SetWindowText(hWnd, buf);
		}
		break;
	case WM_TEST_BATCH_STOP: //��ʵ����̽���
		if(gTestParam != NULL) //��ֱֹ�����������ֱ�ӹر���ɵ���ʾ����
		{
			draw_text("���ڱ�������..."); DoEvents();
			view_switch(VIEW_LAB_LIST);
		}
		break;
	case WM_CLOSE:
		view_button_enable(FALSE);
		if(test_batch_running()) //����������
		{
			draw_text("����ֹͣ����..."); DoEvents();
			test_batch_stop(true);
		}
		view_free();
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		test_close();//�ر���Ŀ
		draw_free();//�ͷŻ�ͼ��Դ
		//FreeConsole();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
#ifdef _WIN64
#define ABOUT_PLATFORM "(x64)"
#else
#define ABOUT_PLATFORM "(x86)"
#endif
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_ABOUT_PLATFORM, ABOUT_PLATFORM);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//�������öԻ���
INT_PTR CALLBACK Config(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	static BOOL readonly = false;
	switch (message)
	{
	case WM_INITDIALOG:
		{
			assert(gConfig.lab_more_robot_count > 0);
			SetDlgItemInt(hDlg, IDE_CFG_SIDE_NUM, gConfig.map_side_num, false);
			SetDlgItemInt(hDlg, IDE_CFG_TRASH_NUM, gConfig.map_trash_num, false);
			SetDlgItemInt(hDlg, IDE_CFG_GENE_LENGTH, gConfig.robot_gene_length, false);
			SetDlgItemInt(hDlg, IDE_CFG_STEP_COUNT, gConfig.robot_step_count, false);
			SetDlgItemInt(hDlg, IDE_CFG_ACT_TRASH_OK, gConfig.lab_action_trash_ok, false);
			SetDlgItemInt(hDlg, IDE_CFG_ACT_TRASH_FAIL, -gConfig.lab_action_trash_fail, false);
			SetDlgItemInt(hDlg, IDE_CFG_ACT_KONCK_WALL, -gConfig.lab_action_konck_wall, false);
			SetDlgItemInt(hDlg, IDE_CFG_MORE_COUNT, gConfig.lab_more_count, false);
			SetDlgItemInt(hDlg, IDE_CFG_ROBOT_COUNT, gConfig.lab_more_robot_count, false);
			SetDlgItemInt(hDlg, IDE_CFG_TOP_COUNT, gConfig.lab_more_robot_top_count * 100 / gConfig.lab_more_robot_count, false);
			SetDlgItemInt(hDlg, IDE_CFG_TOP_RACIAL_LIMIT, gConfig.lab_more_robot_top_racial_limit, false);
			SetDlgItemInt(hDlg, IDE_CFG_VAR_RATE, gConfig.robot_evo_var_rate, false);
			SetDlgItemInt(hDlg, IDE_CFG_VAR_PIECE, gConfig.robot_evo_var_piece, false);
			SetDlgItemInt(hDlg, IDE_CFG_RUN_COUNT, gConfig.lab_run_count, false);
			readonly = (gTestParam != NULL && gTestParam->robots != NULL);
			for(int i=IDE_CFG_ID_START;i<=IDE_CFG_ID_STOP;i++)
			{
				SendDlgItemMessage(hDlg, i, EM_SETREADONLY, readonly, 0);
			}
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK: //����
			{
				UINT value;
				BOOL success;
				if(!readonly)
				{
					value = GetDlgItemInt(hDlg, IDE_CFG_SIDE_NUM, &success, false);
					if(success && value > 0){ gConfig.map_side_num = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_TRASH_NUM, &success, false);
					if(success && value >= 0 && gConfig.map_trash_num <= gConfig.map_side_num * gConfig.map_side_num){ gConfig.map_trash_num = value; }
					/*value = GetDlgItemInt(hDlg, IDE_CFG_GENE_LENGTH, &success, false);
					if(success && value > 0){ gConfig.robot_gene_length = value; }*/
					value = GetDlgItemInt(hDlg, IDE_CFG_STEP_COUNT, &success, false);
					if(success && value > 0){ gConfig.robot_step_count = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_ACT_TRASH_OK, &success, false);
					if(success && value >= 0){ gConfig.lab_action_trash_ok = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_ACT_TRASH_FAIL, &success, false);
					if(success && value >= 0){ gConfig.lab_action_trash_fail = -(int)value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_ACT_KONCK_WALL, &success, false);
					if(success && value >= 0){ gConfig.lab_action_konck_wall = -(int)value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_MORE_COUNT, &success, false);
					if(success && value > 0){ gConfig.lab_more_count = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_ROBOT_COUNT, &success, false);
					if(success && value > 0){ gConfig.lab_more_robot_count = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_TOP_COUNT, &success, false);
					if(success && value > 0 && value <= 100){ gConfig.lab_more_robot_top_count = gConfig.lab_more_robot_count * value / 100; }
					value = GetDlgItemInt(hDlg, IDE_CFG_TOP_RACIAL_LIMIT, &success, false);
					if(success && value > 0 && (int)value <= gConfig.lab_more_robot_top_count){ gConfig.lab_more_robot_top_racial_limit = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_VAR_RATE, &success, false);
					if(success && value >= 0 && value <= 100){ gConfig.robot_evo_var_rate = value; }
					value = GetDlgItemInt(hDlg, IDE_CFG_VAR_PIECE, &success, false);
					if(success && value > 0 && (int)value < gConfig.robot_gene_length){ gConfig.robot_evo_var_piece = value; }
				}
				value = GetDlgItemInt(hDlg, IDE_CFG_RUN_COUNT, &success, false);
				if(success && value > 0){ gConfig.lab_run_count = value; }
			}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		default:
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//����༭�Ի���
INT_PTR CALLBACK GeneEdit(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	static int env, gene;
	switch (message)
	{
	case WM_INITDIALOG:
		{
			env = HIWORD(lParam);
			gene = LOWORD(lParam) - ROBOT_ACT_BASE;
			if(gene < 0 || gene > 6)
			{
				return (INT_PTR)FALSE;
			}

			//��������
			char north, east, south, west, center;
			map_env_decode(env, north, east, south, west, center);

			SetDlgItemInt(hDlg, IDC_GEDT_ENV, env + 1, false);
			SetDlgItemText(hDlg, IDE_GEDT_N, draw_env_get(north));
			SetDlgItemText(hDlg, IDE_GEDT_E, draw_env_get(east));
			SetDlgItemText(hDlg, IDE_GEDT_S, draw_env_get(south));
			SetDlgItemText(hDlg, IDE_GEDT_W, draw_env_get(west));
			SetDlgItemText(hDlg, IDE_GEDT_C, draw_env_get(center));
			SendDlgItemMessage(hDlg, IDE_GEDT_C, EM_SETREADONLY, 1, 0);

			char *list[7] = {"0-����","1-����","2-����","3-����","4-����","5-����","6-ʰȡ"};
			for(int i=0;i<7;i++)
			{
				SendDlgItemMessage(hDlg, IDC_GEDT_ACT, CB_ADDSTRING, NULL, (LPARAM)list[i]);
			}
			SendDlgItemMessage(hDlg, IDC_GEDT_ACT, CB_SETCURSEL, (WPARAM)gene, NULL);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				 char g = (char)SendDlgItemMessage(hDlg, IDC_GEDT_ACT, CB_GETCURSEL, 0, 0);
				 if(g != gene) //�б��
				 {
					 gCurGene[env] = g + ROBOT_ACT_BASE;
				 }
			}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}