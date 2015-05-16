#include "stdafx.h"
#include "draw.h"
#include "file.h"
#include "test.h"
#include "view.h"

HWND hDrawWnd;
RECT gDrawClientRect;
HBITMAP gDrawBitmap;
HDC gDrawBmpMemDC = NULL;

int gTableListLineHeight = 0;
int gTableListPosTop = 0;
int gTableListPosBottom = 0;

/////////////////////////////////////////////
//����ģ���ʼ��
void draw_init(HWND hWnd)
{
	hDrawWnd = hWnd;
	GetClientRect(hDrawWnd, &gDrawClientRect);

	HDC hdc = GetDC(hDrawWnd);
	gDrawBitmap = CreateCompatibleBitmap(hdc, gDrawClientRect.right-gDrawClientRect.left, gDrawClientRect.bottom-gDrawClientRect.top);
	gDrawBmpMemDC = CreateCompatibleDC(hdc);
	ReleaseDC(hDrawWnd, hdc);

	SelectObject(gDrawBmpMemDC, gDrawBitmap);
	//SetBkMode(gDrawBmpMemDC, TRANSPARENT);
	SetBkColor(gDrawBmpMemDC, RGB(0, 0, 0));
	SetTextColor(gDrawBmpMemDC, RGB(255, 255, 255));
	SelectObject(gDrawBmpMemDC, GetStockObject(WHITE_PEN));
	SelectObject(gDrawBmpMemDC, GetStockObject(BLACK_BRUSH));
	SelectObject(gDrawBmpMemDC, GetStockObject(OEM_FIXED_FONT));
}

//����ģ��OnPaint�¼�
void draw_paint(HDC &hdc)
{
	BitBlt(hdc, gDrawClientRect.left, gDrawClientRect.top, gDrawClientRect.right, gDrawClientRect.bottom, gDrawBmpMemDC, 0, 0, SRCCOPY);
}

//���ƽ���ˢ��
void draw_refresh(void)
{
	InvalidateRect(hDrawWnd, &gDrawClientRect, FALSE);
}

//����ģ���ͷ���Դ
void draw_free(void)
{
	DeleteObject(gDrawBitmap);
	DeleteDC(gDrawBmpMemDC);
}

/////////////////////////////////////////////
//����
void draw_clear(bool refresh)
{
	FillRect(gDrawBmpMemDC, &gDrawClientRect, NULL);
	if(refresh)
	{
		draw_refresh();
	}
	gTableListPosTop = 0;
	gTableListPosBottom = 0;
}

//����
void draw_line(int x1, int y1, int x2, int y2)
{
	MoveToEx(gDrawBmpMemDC, x1, y1, NULL);
	LineTo(gDrawBmpMemDC, x2, y2);
}

//�����ı�
void draw_text(const char* fmt, ...)
{
	draw_clear(false);

	va_list args;
	va_start(args, fmt);
	char buf[MAX_LOADSTRING];
	vsprintf_s(buf, fmt, args);
	va_end(args);

	RECT rect = gDrawClientRect;
	DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &rect,
		DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	//TextOut(hdc, 0, r, buf, strlen(buf));
	//DEBUG(buf);

	draw_refresh();
}

/////////////////////////////////////////////
//������
void draw_map_trush(int x, int y, const bool cross = false)
{
	x = DRAW_MAP_CELL_X(x);
	y = DRAW_MAP_CELL_Y(y);
	int x0 = x + DRAW_MAP_TRUSH_MARGIN;
	int y0 = y + DRAW_MAP_TRUSH_MARGIN;
	Ellipse(gDrawBmpMemDC,
		DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
		DRAW_MAP_COORD_X(x0+DRAW_MAP_CELL_LEN_X-DRAW_MAP_TRUSH_MARGIN),
		DRAW_MAP_COORD_Y(y0+DRAW_MAP_CELL_LEN_Y-DRAW_MAP_TRUSH_MARGIN)
	);
	if(cross) //�������
	{
		x0 += DRAW_MAP_CROSS_OFFSET;
		y0 += DRAW_MAP_CROSS_OFFSET;
		const int x1 = x + DRAW_MAP_CELL_LEN_X - DRAW_MAP_TRUSH_MARGIN - DRAW_MAP_CROSS_OFFSET;
		const int y1 = y + DRAW_MAP_CELL_LEN_Y - DRAW_MAP_TRUSH_MARGIN - DRAW_MAP_CROSS_OFFSET;
		draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
			DRAW_MAP_COORD_X(x1), DRAW_MAP_COORD_Y(y1));
		draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y1),
			DRAW_MAP_COORD_X(x1), DRAW_MAP_COORD_Y(y0));
	}
}

//����ͼ
void draw_map(CMAP map)
{
	//��ͼ
	draw_clear(false);
	for(int i=0;i<=gConfig.map_side_num;i++)
	{
		draw_line(DRAW_MAP_MARGIN_X, DRAW_MAP_CELL_Y(i), DRAW_MAP_MARGIN_X+DRAW_MAP_WIDTH, DRAW_MAP_CELL_Y(i));
		draw_line(DRAW_MAP_CELL_X(i), DRAW_MAP_MARGIN_Y, DRAW_MAP_CELL_X(i), DRAW_MAP_MARGIN_Y+DRAW_MAP_HEIGHT);
	}
	//����
	HGDIOBJ hBrushOld = SelectObject(gDrawBmpMemDC, GetStockObject(GRAY_BRUSH));//��ˢ
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, GetStockObject(NULL_PEN));//�ձ�
	for(int x=0;x<gConfig.map_side_num;x++)
	{
		for(int y=0;y<gConfig.map_side_num;y++)
		{
			if(map_cell(map, x, y) == MAP_CELL_TRUSH)//������
			{
				for(int i=0;i<=gConfig.map_side_num;i++)
				{
					draw_map_trush(x, y);
				}
			}
		}
	}
	SelectObject(gDrawBmpMemDC, hPenOld);
	SelectObject(gDrawBmpMemDC, hBrushOld);
}

//��Ȧ
void draw_steps_circle(int x, int y)
{
	x = DRAW_MAP_CELL_X(x);
	y = DRAW_MAP_CELL_Y(y);
	int x0 = x + DRAW_MAP_CIRCLE_X;
	int y0 = y + DRAW_MAP_CIRCLE_Y;
	int x1 = x + DRAW_MAP_CELL_LEN_X - DRAW_MAP_CIRCLE_X;
	int y1 = y + DRAW_MAP_CELL_LEN_Y - DRAW_MAP_CIRCLE_Y;
	Ellipse(gDrawBmpMemDC,
		DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
		DRAW_MAP_COORD_X(x1), DRAW_MAP_COORD_Y(y1)
	);
}

//���ж�����
void draw_steps(CSTEPS steps, long score, int count, int max, int min)
{
	//�÷�
	char buf[MAX_LOADSTRING];
	if(count > 1)
	{
		if(max == INT_MAX || min == INT_MIN)
		{
			sprintf_s(buf, "����: %d  �ܷ�: %ld  ƽ��: %.2f", count, score, (float)score / count);
		}
		else
		{
			sprintf_s(buf, "����: %d  ƽ��: %.2f  ���: %d  ���: %d", count, (float)score / count, max, min);
		}
	}
	else
	{
		sprintf_s(buf, "�÷�: %ld", score);
	}
	TextOut(gDrawBmpMemDC, DRAW_MAP_MARGIN_X, DRAW_MAP_HEIGHT+DRAW_MAP_MARGIN_Y+DRAW_MAP_SCORE_Y, buf, (int)strlen(buf));
	//��Ϊ
	int x=0, y=0, x0, y0;
	HPEN hPenRed = CreatePen(PS_SOLID, 0,  RGB(255,0,0)); //����
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, hPenRed);
	HGDIOBJ hBrushOld = SelectObject(gDrawBmpMemDC, GetStockObject(NULL_BRUSH));//��ˢ
	for(int i=0;i<gConfig.robot_step_count;i++)
	{
		char action = robot_steps_get(steps, i, ROBOT_STEPS_ACTION);
		bool result = robot_steps_get(steps, i, ROBOT_STEPS_RESULT) != '0';
		switch(action)
		{
		case ROBOT_ACT_NORTH://���ƶ�
		case ROBOT_ACT_RND_NORTH://���
			x0 = DRAW_MAP_CELL_X(x) + DRAW_MAP_CELL_HALF_X;
			y0 = DRAW_MAP_CELL_Y(y) + DRAW_MAP_CELL_HALF_Y;
			if(result)
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0+DRAW_MAP_CELL_LEN_Y));
				y++;
			}
			else
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0+DRAW_MAP_CELL_HALF_Y));
			}
			break;
		case ROBOT_ACT_SOUTH://�����ƶ�
		case ROBOT_ACT_RND_SOUTH://���
			x0 = DRAW_MAP_CELL_X(x) + DRAW_MAP_CELL_HALF_X;
			y0 = DRAW_MAP_CELL_Y(y) + DRAW_MAP_CELL_HALF_Y;
			if(result)
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0-DRAW_MAP_CELL_LEN_Y));
				y--;
			}
			else
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0-DRAW_MAP_CELL_HALF_Y));
			}
			break;
		case ROBOT_ACT_EAST://���ƶ�
		case ROBOT_ACT_RND_EAST://���
			x0 = DRAW_MAP_CELL_X(x) + DRAW_MAP_CELL_HALF_X;
			y0 = DRAW_MAP_CELL_Y(y) + DRAW_MAP_CELL_HALF_Y;
			if(result)
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0+DRAW_MAP_CELL_LEN_X), DRAW_MAP_COORD_Y(y0));
				x++;
			}
			else
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0+DRAW_MAP_CELL_HALF_X), DRAW_MAP_COORD_Y(y0));
			}
			break;
		case ROBOT_ACT_WEST://�����ƶ�
		case ROBOT_ACT_RND_WEST://���
			x0 = DRAW_MAP_CELL_X(x) + DRAW_MAP_CELL_HALF_X;
			y0 = DRAW_MAP_CELL_Y(y) + DRAW_MAP_CELL_HALF_Y;
			if(result)
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0-DRAW_MAP_CELL_LEN_X), DRAW_MAP_COORD_Y(y0));
				x--;
			}
			else
			{
				draw_line(DRAW_MAP_COORD_X(x0), DRAW_MAP_COORD_Y(y0),
					DRAW_MAP_COORD_X(x0-DRAW_MAP_CELL_HALF_X), DRAW_MAP_COORD_Y(y0));
			}
			break;
		case ROBOT_ACT_STAY://����
			SelectObject(gDrawBmpMemDC, GetStockObject(BLACK_BRUSH));//��ˢ
			draw_steps_circle(x, y);//��Ȧ
			break;
		case ROBOT_ACT_CLEAN://��������
			if(result) //�ɹ�
			{
				SelectObject(gDrawBmpMemDC, GetStockObject(LTGRAY_BRUSH));//����ˢ
				draw_map_trush(x, y);
			}
			else //ʧ��
			{
				SelectObject(gDrawBmpMemDC, GetStockObject(NULL_BRUSH));//��ˢ
				draw_map_trush(x, y, true);
			}
			break;
		}//end switch
	}//end for

	//�����յ�
	HBRUSH hBrushRed = CreateSolidBrush(RGB(255,0,0));//��ˢ
	SelectObject(gDrawBmpMemDC, hBrushRed);
	draw_steps_circle(0, 0);//���
	draw_steps_circle(x, y);//�յ�

	//�ͷ���Դ
	SelectObject(gDrawBmpMemDC, hBrushOld);
	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hBrushRed);
	DeleteObject(hPenRed);

	draw_refresh();
}

/////////////////////////////////////////////
//����ҳ��
int draw_calc_page(const int total, const int rows, int &page, int &start, int &end)
{
	int pages = total / rows;
	if(total % rows > 0){pages++;}
	if(page < 1){page = 1;}
	if(page > pages){page = pages;}
	start = rows * (page - 1);
	end = rows * page;
	if(end > total){end = total;}
	return pages;
}

//�����б��и���̬
void draw_table_row_hover(int y)
{
	static int oldBottom = -1;

	if(gTableListLineHeight == 0 || y < gTableListPosTop || y >= gTableListPosBottom)
	{
		if(oldBottom >= 0) //�����ɵ�
		{
			int left = gDrawClientRect.left + DRAW_TEST_MARGIN - 1;
			int right = gDrawClientRect.right - DRAW_TEST_MARGIN;
			HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, GetStockObject(BLACK_PEN));
			draw_line(left, oldBottom, right, oldBottom);
			SelectObject(gDrawBmpMemDC, hPenOld);
			RECT rect = {left, oldBottom, right, oldBottom+1};
			InvalidateRect(hDrawWnd, &rect, FALSE);
			oldBottom = -1;
		}
		return;
	}
	int top = gTableListPosTop + (y - gTableListPosTop) / gTableListLineHeight * gTableListLineHeight;
	int bottom = top + gTableListLineHeight - 1;
	if(oldBottom == bottom) //λ��û��
	{
		return;
	}
	int left = gDrawClientRect.left + DRAW_TEST_MARGIN - 1;
	int right = gDrawClientRect.right - DRAW_TEST_MARGIN;

	//�����ɵ�
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, GetStockObject(BLACK_PEN));
	if(oldBottom >= 0){draw_line(left, oldBottom, right, oldBottom);}
	//���µ�
	HPEN hPenRed = CreatePen(PS_SOLID, 0,  RGB(128,0,0)); //�����
	SelectObject(gDrawBmpMemDC, hPenRed);
	draw_line(left, bottom, right, bottom);

	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hPenRed);

	RECT rect = {left, min(bottom, oldBottom), right, max(bottom, oldBottom)+1};
	InvalidateRect(hDrawWnd, &rect, FALSE);

	//��¼����
	oldBottom = bottom;
}

//����ʵ����
void draw_test_table(int offset, RECT &rect, int column, const char *t1 = NULL, const char *v1 = NULL, const char *t2 = NULL, const char *v2 = NULL, const char *t3 = NULL, const char *v3 = NULL)
{
	int height = 0;
	const int left = rect.left;
	const int t_column = column + offset;
	const int v_column = column - offset;

	rect.top++;
	//rect.right = left + t_column;
	if(t1){height = DrawText(gDrawBmpMemDC, t1, (int)strlen(t1), &rect, DT_SINGLELINE);}
	rect.left += t_column; //rect.right += v_column;
	if(v1){height = DrawText(gDrawBmpMemDC, v1, (int)strlen(v1), &rect, DT_SINGLELINE);}
	rect.left += v_column; //rect.right += t_column;
	if(t2){height = DrawText(gDrawBmpMemDC, t2, (int)strlen(t2), &rect, DT_SINGLELINE);}
	rect.left += t_column; //rect.right += v_column;
	if(v2){height = DrawText(gDrawBmpMemDC, v2, (int)strlen(v2), &rect, DT_SINGLELINE);}
	rect.left += v_column; //rect.right += t_column;
	if(t3){height = DrawText(gDrawBmpMemDC, t3, (int)strlen(t3), &rect, DT_SINGLELINE);}
	rect.left += t_column; //rect.right += v_column;
	if(v3){height = DrawText(gDrawBmpMemDC, v3, (int)strlen(v3), &rect, DT_SINGLELINE);}
	rect.left = left;
	if(height > 0)
	{
		rect.top += height + 1;
		draw_line(rect.left, rect.top, rect.right, rect.top);
		gTableListLineHeight = height + 2; //��¼�и�
	}
	else
	{
		rect.top--;
	}
}

//����ʵ����Ŀ�ļ�
void draw_filename(RECT &rect)
{
	int column = (rect.right - rect.left) / 6;
	draw_test_table(20, rect, column, "��Ŀ�ļ�", gFileName);
}

//����ʵ�����ò���
void draw_config(RECT &rect)
{
	char v1[MAX_LOADSTRING], v2[MAX_LOADSTRING]/*, v3[MAX_LOADSTRING]*/;
	int column = (rect.right - rect.left) / 6;

	/*sprintf_s(v1, sizeof(v1), "%d", gConfig.map_side_num);
	sprintf_s(v2, sizeof(v2), "%d", gConfig.map_trash_num);
	draw_test_table(20, rect, column, "��ͼ�߸���", v1, "��ͼ������", v2);
	*/
	/*sprintf_s(v1, sizeof(v1), "%d", gConfig.robot_gene_length);
	sprintf_s(v2, sizeof(v2), "%d", gConfig.robot_step_count);
	draw_test_table(20, rect, column, "���򳤶�", v1, "�ж�����", v2);
	*/
	sprintf_s(v1, sizeof(v1), "%ld", gConfig.robot_id_base);
	sprintf_s(v2, sizeof(v2), "%ld", gConfig.robot_father_id_base);
	draw_test_table(20, rect, column, "��������ID", v1, "��������ID", v2);

	/*sprintf_s(v1, sizeof(v1), "%d", gConfig.lab_action_trash_ok);
	sprintf_s(v2, sizeof(v2), "%d", gConfig.lab_action_trash_fail);
	sprintf_s(v3, sizeof(v3), "%d", gConfig.lab_action_konck_wall);
	draw_test_table(20, rect, column, "���������÷�", v1, "��������÷�", v2, "�ƶ�ײǽ�÷�", v3);
	*/
	/*sprintf_s(v1, sizeof(v1), "%d", gConfig.lab_more_count);
	sprintf_s(v2, sizeof(v2), "%d", gConfig.lab_more_robot_count);
	draw_test_table(20, rect, column, "ʵ������", v1, "ʵ���������", v2);
	*/
	/*sprintf_s(v1, sizeof(v1), "%d", gConfig.lab_more_robot_top_count);
	sprintf_s(v2, sizeof(v2), "%d", gConfig.lab_more_robot_top_racial_limit);
	sprintf_s(v3, sizeof(v3), "%d", gConfig.lab_more_robot_count - gConfig.lab_more_robot_top_count * 2);
	draw_test_table(20, rect, column, "������������", v1, "������ͬ������", v2, "������������", v3);
	*/
	/*sprintf_s(v1, sizeof(v1), "%d%%", gConfig.robot_evo_var_rate);
	sprintf_s(v2, sizeof(v2), "%d", gConfig.robot_evo_var_piece);
	draw_test_table(20, rect, column, "�����������", v1, "����Ƭ�γ���", v2);
	*/
}

//����ʵ�����
void draw_test_param(RECT &rect)
{
	if(gTestParam == NULL)
	{
		return;
	}

	char v1[MAX_LOADSTRING], v2[MAX_LOADSTRING], v3[MAX_LOADSTRING];
	int column = (rect.right - rect.left) / 6;

	int thread_num;
	if(gTestParam->tpg)
	{
		thread_num = gTestParam->tpg->num;
	}
	else
	{
		thread_num = test_thread_num();
	}
	sprintf_s(v1, sizeof(v1), "%d", gTestParam->round);
	Ts2Str(v2, sizeof(v2), gTestParam->ts_total);
	sprintf_s(v3, sizeof(v3), "%d", thread_num);
	draw_test_table(20, rect, column, "ʵ�����", v1, "�ۻ���ʱ", v2, "�߳���", v3);

	Ts2Str(v1, sizeof(v1), "%Y-%m-%d %H:%M:%S", gTestParam->ts_start);
	Ts2Str(v2, sizeof(v2), "%Y-%m-%d %H:%M:%S", gTestParam->ts_stop);
	draw_test_table(20, rect, column, "��ʼʱ��", v1, NULL, "����ʱ��", v2);
}

//����ʵ��������б�
void draw_test_robots(RECT &rect, int &page)
{
	if(gTestParam == NULL || gTestParam->robots == NULL)
	{
		return;
	}
	assert(gConfig.lab_more_count > 0);

	char v1[MAX_LOADSTRING], v2[MAX_LOADSTRING], v3[MAX_LOADSTRING], v4[MAX_LOADSTRING];
	int column = (rect.right - rect.left) / 6;

	draw_test_table(20, rect, column, "����", "���", NULL, NULL, "�ܷ�", "ƽ����");
	int start, end;
	int pages = draw_calc_page(gConfig.lab_more_robot_count, DRAW_TEST_ROBOTS_ROWS, page, start, end);
	gTableListPosTop = rect.top; //��¼��ʼλ��
	for(int i=start;i<end;i++)
	{
		ROBOT &robot = gTestParam->robots[i];
		sprintf_s(v1, sizeof(v1), "%d", i+1);
		sprintf_s(v2, sizeof(v2), "X%ld.M%ld.D%d", robot.father_id, robot.id, robot.generation);
		sprintf_s(v3, sizeof(v3), "%ld", robot.score);
		sprintf_s(v4, sizeof(v4), "%.2f", (float)robot.score / gConfig.lab_more_count);
		draw_test_table(20, rect, column, v1, v2, NULL, NULL, v3, v4);
	}
	gTableListPosBottom = rect.top; //��¼����λ��

	sprintf_s(v1, sizeof(v1), "%d/%d", page, pages);
	TextOut(gDrawBmpMemDC, rect.left, rect.top + DRAW_PAGE_NO_Y, v1, (int)strlen(v1));
}

//����ʵ������
void draw_test(int &page)
{
	RECT rect = gDrawClientRect;
	rect.left += DRAW_TEST_MARGIN;
	rect.top += DRAW_TEST_MARGIN;
	rect.right -= DRAW_TEST_MARGIN;
	rect.bottom -= DRAW_TEST_MARGIN;

	draw_clear(false);

	HPEN hPenRed = CreatePen(PS_SOLID, 0,  RGB(128,0,0)); //�����
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, hPenRed);

	draw_filename(rect);
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);

	rect.top++;
	draw_config(rect);
	//rect.top++;
	//draw_line(rect.left, rect.top, rect.right, rect.top);

	rect.top++;
	draw_test_param(rect);
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);

	rect.top++;
	draw_test_robots(rect, page);
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);

	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hPenRed);

	draw_refresh();
}

//��ȡʵ��������ָ������Ļ�����
PROBOT draw_test_robot_get(int &page, int y)
{
	if(gTestParam == NULL || gTestParam->robots == NULL
		|| gTableListLineHeight == 0 || y < gTableListPosTop || y >= gTableListPosBottom)
	{
		return NULL;
	}
	int start, end;
	draw_calc_page(gConfig.lab_more_robot_count, DRAW_TEST_ROBOTS_ROWS, page, start, end);
	int i = start + (y - gTableListPosTop) / gTableListLineHeight;
	return &gTestParam->robots[i];
}

/////////////////////////////////////////////
//����ʵ��ͳ�������б�
void draw_stat_list(PHISTORY history, int &page)
{
	RECT rect = gDrawClientRect;
	rect.left += DRAW_STAT_LIST_MARGIN;
	rect.top += DRAW_STAT_LIST_MARGIN;
	rect.right -= DRAW_STAT_LIST_MARGIN;
	rect.bottom -= DRAW_STAT_LIST_MARGIN;

	char v1[MAX_LOADSTRING], v2[MAX_LOADSTRING], v3[MAX_LOADSTRING], v4[MAX_LOADSTRING], v5[MAX_LOADSTRING], v6[MAX_LOADSTRING];
	int column = (rect.right - rect.left) / 6;

	draw_clear(false);

	HPEN hPenRed = CreatePen(PS_SOLID, 0,  RGB(128,0,0)); //�����
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, hPenRed);

	draw_test_table(10, rect, column, "����", "�Ӵ�", "����", "����", "����", "ƽ����");
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);
	rect.top++;

	int start, end;
	int pages = draw_calc_page(history->count, DRAW_STAT_LIST_ROWS, page, start, end);
	gTableListPosTop = rect.top; //��¼��ʼλ��

	PHISTORYITEM p = history->head;
	int i = 0;
	while(p != NULL)
	{
		if(i >= start && i < end)
		{
			ROBOT &robot = p->top[0];
			sprintf_s(v1, sizeof(v1), "%d", p->round);
			sprintf_s(v2, sizeof(v2), "%d", robot.generation);
			sprintf_s(v3, sizeof(v3), "%.2f", (float)p->round / robot.generation);
			sprintf_s(v4, sizeof(v4), "X%ld", robot.father_id);
			sprintf_s(v5, sizeof(v5), "M%ld", robot.id);
			sprintf_s(v6, sizeof(v6), "%.2f", (float)robot.score / gConfig.lab_more_count);
			draw_test_table(10, rect, column, v1, v2, v3, v4, v5, v6);
		}

		i++;
		p = p->next;
	}
	gTableListPosBottom = rect.top; //��¼����λ��
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);

	sprintf_s(v1, sizeof(v1), "%d/%d", page, pages);
	TextOut(gDrawBmpMemDC, rect.left, rect.top + DRAW_PAGE_NO_Y, v1, (int)strlen(v1));

	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hPenRed);

	draw_refresh();
}

//����ʵ��ͳ������ͼ��
void draw_stat_chart(PHISTORY history, RECT &rect, int max_round, int min_round, double max_value, double min_value, int type)
{
	int x = type == 0 ? rect.left : rect.right;
	if(type < 2)
	{
		draw_line(x, rect.top, x, rect.bottom);
	}

	int h = (rect.bottom - rect.top) / DRAW_STAT_TAG_NUM;
	x += type == 0 ? -DRAW_STAT_TAG_LEN : DRAW_STAT_TAG_LEN;
	int y = rect.bottom;
	double value = (max_value - min_value) / DRAW_STAT_TAG_NUM;
	char buf[MAX_LOADSTRING];
	for(int i=0;i<=DRAW_STAT_TAG_NUM;i++)
	{
		if(type < 2)
		{
			draw_line(x, y, type == 0 ? rect.left : rect.right, y);
		}
		sprintf_s(buf, sizeof(buf), "%.1f", min_value +  value * i);
		switch(type)
		{
		case 0:
			{
				RECT r = {gDrawClientRect.left, y - h, x, y};
				DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
			}
			break;
		case 1:
			{
				RECT r = {x, y - h, gDrawClientRect.right, y};
				DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
			}
			break;
		default:
			if(i > 0)
			{
				RECT r = {x, y, gDrawClientRect.right, y + h};
				DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_LEFT | DT_TOP | DT_SINGLELINE);
			}
			break;
		}
		y -= h;
	}

	PHISTORYITEM p = history->head;
	int height = rect.bottom - rect.top;
	int width = rect.right - rect.left;
	const int cr = type == 0 ? 4 : 3;
	int i = 0;
	while(p != NULL)
	{
		x = rect.left + width * (p->round - min_round) / (max_round - min_round);
		switch(type)
		{
		case 0:
			y = rect.bottom - (int)(height * (p->top[0].generation - min_value) / (max_value - min_value));
			break;
		case 1:
			value = (double)p->round / p->top[0].generation;
			y = rect.bottom - (int)(height * (value - min_value) / (max_value - min_value));
			break;
		default:
			value = (double)p->top[0].score / gConfig.lab_more_count;
			y = rect.bottom - (int)(height * (value - min_value) / (max_value - min_value));
			break;
		}
		if(i > 0)
		{
			LineTo(gDrawBmpMemDC, x, y);
		}
		else
		{
			MoveToEx(gDrawBmpMemDC, x, y, NULL);
			Ellipse(gDrawBmpMemDC, x-cr, y-cr, x+cr, y+cr);
		}

		i++;
		p = p->next;
	}
	if(i > 1)
	{
		Ellipse(gDrawBmpMemDC, x-cr, y-cr, x+cr, y+cr);
	}

	switch(type)
	{
	case 0:
		{
			RECT r = {rect.left, gDrawClientRect.top, rect.right/3, rect.top};
			sprintf_s(buf, sizeof(buf), "%s", "���� �Ӵ�");
			DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
		break;
	case 1:
		{
			RECT r = {rect.right/3, gDrawClientRect.top, rect.right*2/3, rect.top};
			sprintf_s(buf, sizeof(buf), "%s", "���� ����");
			DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		break;
	default:
		{
			RECT r = {rect.right*2/3, gDrawClientRect.top, rect.right, rect.top};
			sprintf_s(buf, sizeof(buf), "%s", "���� ƽ����");
			DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		break;
	}
}
void draw_stat(PHISTORY history)
{
	RECT rect = gDrawClientRect;
	rect.left += DRAW_STAT_MARGIN;
	rect.top += DRAW_STAT_MARGIN;
	rect.right -= DRAW_STAT_MARGIN;
	rect.bottom -= DRAW_STAT_MARGIN;

	draw_clear(false);

	//�ȼ��������Сֵ
	PHISTORYITEM p = history->head;
	int max_round = INT_MIN;
	int min_round = INT_MAX;
	double max_gen = INT_MIN;
	double min_gen = INT_MAX;
	double max_ratio = INT_MIN;
	double min_ratio = INT_MAX;
	double max_score = INT_MIN;
	double min_score = INT_MAX;
	while(p != NULL)
	{
		if(p->round > max_round){max_round = p->round;}
		if(p->round < min_round){min_round = p->round;}
		if(p->top_len > 0)
		{
			if(p->top[0].generation > max_gen){max_gen = (double)p->top[0].generation;}
			if(p->top[0].generation < min_gen){min_gen = (double)p->top[0].generation;}
			double value = (double)p->round / p->top[0].generation;
			if(value > max_ratio){max_ratio = value;}
			if(value < min_ratio){min_ratio = value;}
			value = (double)p->top[0].score / gConfig.lab_more_count;
			if(value > max_score){max_score = value;}
			if(value < min_score){min_score = value;}
		}
		p = p->next;
	}
	max_round++;
	min_round--;
	max_gen++;
	min_gen--;
	max_ratio++;
	min_ratio--;
	max_score++;
	min_score--;

	//��������������
	draw_line(rect.left, rect.bottom, rect.right, rect.bottom);
	int w = (rect.right - rect.left) / DRAW_STAT_TAG_NUM;
	int x = rect.left;
	int y = rect.bottom + DRAW_STAT_TAG_LEN;
	double value = (double)(max_round - min_round) / DRAW_STAT_TAG_NUM;
	char buf[MAX_LOADSTRING];
	for(int i=0;i<=DRAW_STAT_TAG_NUM;i++)
	{
		draw_line(x, rect.bottom, x, y);
		int v = (int)(min_round + value * i);
		sprintf_s(buf, sizeof(buf), "%d", v);
		int h = 0;
		if(v > 100000 && i % 2 == 1)
		{
			h = DRAW_STAT_TAG_LEN * 2;
		}
		TextOut(gDrawBmpMemDC, x-DRAW_STAT_TAG_LEN*2, y + h, buf, (int)strlen(buf));
		x += w;
	}
	RECT r = {rect.left, rect.bottom + DRAW_STAT_TAG_LEN * 3, rect.right, gDrawClientRect.bottom};
	sprintf_s(buf, sizeof(buf), "%s", "����");
	DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	//�����Ӵ�
	COLORREF hRed = RGB(255,0,0); //����
	HPEN hPenRed = CreatePen(PS_SOLID, 0,  hRed);
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, hPenRed);
	COLORREF hOldColor = SetTextColor(gDrawBmpMemDC, hRed);
	draw_stat_chart(history, rect, max_round, min_round, max_gen, min_gen, 0);

	//���ƴ���
	COLORREF hBlue = RGB(0,0,255); //����
	HPEN hPenBlue = CreatePen(PS_SOLID, 0,  hBlue);
	SelectObject(gDrawBmpMemDC, hPenBlue);
	SetTextColor(gDrawBmpMemDC, hBlue);
	draw_stat_chart(history, rect, max_round, min_round, max_ratio, min_ratio, 1);

	//���Ƶ÷�
	COLORREF hSilver = RGB(96,96,96); //����
	HPEN hPenSilver = CreatePen(PS_SOLID, 0,  hSilver);
	SelectObject(gDrawBmpMemDC, hPenSilver);
	SetTextColor(gDrawBmpMemDC, hSilver);
	draw_stat_chart(history, rect, max_round, min_round, max_score, min_score, 2);

	SetTextColor(gDrawBmpMemDC, hOldColor);
	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hPenRed);
	DeleteObject(hPenBlue);
	DeleteObject(hPenSilver);

	draw_refresh();
}
/*
void draw_stat_runtime(int round, PHISTORY history)
{
	RECT rect = gDrawClientRect;
	rect.left += DRAW_STAT_MARGIN;
	rect.right -= DRAW_STAT_MARGIN;
	rect.bottom -= DRAW_STAT_MARGIN;
	rect.top += DRAW_STAT_MARGIN;

	draw_clear(false);

	//��������������
	int max_round = rect.right - rect.left - 2; //1������1��
	int start_round = (history->count > 0 ? history->tail->round : 0) / max_round * max_round;
	if(start_round == max_round)
	{
		max_round += start_round;
	}

	char buf[MAX_LOADSTRING];
	int x = rect.left;
	int y = rect.bottom + DRAW_STAT_TAG_LEN;
	for(;x<rect.right;x+=100)
	{
		draw_line(x, rect.bottom, x, y);
		sprintf_s(buf, sizeof(buf), "%d", start_round + x - rect.left);
		TextOut(gDrawBmpMemDC, x, y, buf, (int)strlen(buf));
	}
	sprintf_s(buf, sizeof(buf), "��������� %d ��...", round);
	RECT r = {rect.left, rect.bottom + DRAW_STAT_TAG_LEN*2, rect.right, gDrawClientRect.bottom};
	DrawText(gDrawBmpMemDC, buf, (int)strlen(buf), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	draw_line(rect.left, rect.bottom, rect.right, rect.bottom);

	//��ȡ�������ʷ��¼
	HISTORY history_draw = {NULL, NULL, 0};
	PHISTORYITEM p = history->head;
	while(p != NULL)
	{
		if(p->round >= start_round)
		{
			if(history_draw.count == 0)
			{
				history_draw.head = p;
			}
			history_draw.tail = p;
			history_draw.count++;
		}
		p = p->next;
	}

	//�����Ӵ�
	COLORREF hRed = RGB(255,0,0); //����
	HPEN hPenRed = CreatePen(PS_SOLID, 0,  hRed);
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, hPenRed);
	COLORREF hOldColor = SetTextColor(gDrawBmpMemDC, hRed);
	draw_stat_chart(&history_draw, rect, max_round, start_round, max_round, 1, 0);

	//���ƴ���
	COLORREF hBlue = RGB(0,0,255); //����
	HPEN hPenBlue = CreatePen(PS_SOLID, 0,  hBlue);
	SelectObject(gDrawBmpMemDC, hPenBlue);
	SetTextColor(gDrawBmpMemDC, hBlue);
	draw_stat_chart(&history_draw, rect, max_round, start_round, max_round, 0, 1);

	//���Ƶ÷�
	COLORREF hSilver = RGB(96,96,96); //����
	HPEN hPenSilver = CreatePen(PS_SOLID, 0,  hSilver);
	SelectObject(gDrawBmpMemDC, hPenSilver);
	SetTextColor(gDrawBmpMemDC, hSilver);
	draw_stat_chart(&history_draw, rect, max_round, start_round, 1500, 0, 2);

	SetTextColor(gDrawBmpMemDC, hOldColor);
	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hPenRed);
	DeleteObject(hPenBlue);
	DeleteObject(hPenSilver);

	draw_refresh();
}
*/

/////////////////////////////////////////////
//��ȡ��������
inline const char *draw_env_get(char code)
{
	switch(code)
	{
	case MAP_CELL_NULL: //��
		return "��";
	case MAP_CELL_TRUSH: //����
		return "��";
	case MAP_CELL_WALL: //ǽ
		return "ǽ";
	default:
		assert(0);
		return "��";
	}
}

//���ƻ���
void draw_gene(CGENE gene, int &page)
{
	RECT rect = gDrawClientRect;
	rect.left += DRAW_GENE_MARGIN;
	rect.top += DRAW_GENE_MARGIN;
	rect.right -= DRAW_GENE_MARGIN;
	rect.bottom -= DRAW_GENE_MARGIN;
	char v1[MAX_LOADSTRING], v2[MAX_LOADSTRING], v3[MAX_LOADSTRING], v4[MAX_LOADSTRING], *v5 = NULL;
	int column = (rect.right - rect.left) / 6;

	draw_clear(false);

	HPEN hPenRed = CreatePen(PS_SOLID, 0,  RGB(128,0,0)); //�����
	HGDIOBJ hPenOld = SelectObject(gDrawBmpMemDC, hPenRed);

	//��ͷ
	COLORREF hOldColor = SetTextColor(gDrawBmpMemDC, RGB(255,0,0)); //����
	draw_test_table(0, rect, column, "����", "��-��-��-��-��", NULL, "����", "����", "�ж�");
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);
	rect.top++;
	SetTextColor(gDrawBmpMemDC, hOldColor);

	//����
	int start, end;
	int pages = draw_calc_page(gConfig.robot_gene_length, DRAW_GENE_ROWS, page, start, end);
	gTableListPosTop = rect.top; //��¼��ʼλ��
	for(int env=start;env<end;env++)
	{
		//��������
		sprintf_s(v1, sizeof(v1), "%d", env + 1);

		//�����������
		char north, east, south, west, center;
		map_env_decode(env, north, east, south, west, center);
		sprintf_s(v2, sizeof(v2),
			"%s-%s-%s-%s-%s",
			draw_env_get(north),
			draw_env_get(east),
			draw_env_get(south),
			draw_env_get(west),
			draw_env_get(center)
		);
		
		//�������
		char code = gene[env];
		sprintf_s(v3, sizeof(v3), "%c", code);

		//����������
		int score = 0; //����: 0һ�� 1��� 2�ܺ�
		switch(code)
		{
		case ROBOT_ACT_NORTH:
			sprintf_s(v4, sizeof(v4), "%s", "����");
			score = (north == MAP_CELL_WALL) ? 1 : 0;
			break;
		case ROBOT_ACT_SOUTH:
			sprintf_s(v4, sizeof(v4), "%s", "����");
			score = (south == MAP_CELL_WALL) ? 1 : 0;
			break;
		case ROBOT_ACT_EAST:
			sprintf_s(v4, sizeof(v4), "%s", "����");
			score = (east == MAP_CELL_WALL) ? 1 : 0;
			break;
		case ROBOT_ACT_WEST:
			sprintf_s(v4, sizeof(v4), "%s", "����");
			score = (west == MAP_CELL_WALL) ? 1 : 0;
			break;
		case ROBOT_ACT_RND:
			sprintf_s(v4, sizeof(v4), "%s", "����");
			score = (north == MAP_CELL_WALL || south == MAP_CELL_WALL || east == MAP_CELL_WALL || west == MAP_CELL_WALL) ? 1 : 0;
			break;
		case ROBOT_ACT_STAY:
			sprintf_s(v4, sizeof(v4), "%s", "����");
			score = (north == MAP_CELL_WALL && south == MAP_CELL_WALL && east == MAP_CELL_WALL && west == MAP_CELL_WALL) ? 2 : 1;
			break;
		case ROBOT_ACT_CLEAN:
			sprintf_s(v4, sizeof(v4), "%s", "ʰȡ");
			score = (center == MAP_CELL_TRUSH) ? 2 : 1;
			break;
		default:
			sprintf_s(v4, sizeof(v4), "%s", "δ֪");
			throw;
			//break;
		}

		if(score == 1){v5 = "��";} //���
		else if(score == 2){v5 = "��";} //�ܺ�
		else{v5 = NULL;} //һ��

		draw_test_table(0, rect, column, v1, v2, NULL, v3, v4, v5);
	}
	gTableListPosBottom = rect.top; //��¼����λ��
	rect.top++;
	draw_line(rect.left, rect.top, rect.right, rect.top);

	sprintf_s(v1, sizeof(v1), "%d/%d", page, pages);
	TextOut(gDrawBmpMemDC, rect.left, rect.top + DRAW_PAGE_NO_Y, v1, (int)strlen(v1));

	SelectObject(gDrawBmpMemDC, hPenOld);
	DeleteObject(hPenRed);

	draw_refresh();
}

//��ȡ����������ָ������ĵ�������
char draw_gene_get(int &page, int y, int &env)
{
	if(gTableListLineHeight == 0 || y < gTableListPosTop || y >= gTableListPosBottom)
	{
		return NULL;
	}
	int start, end;
	draw_calc_page(gConfig.robot_gene_length, DRAW_GENE_ROWS, page, start, end);
	env = start + (y - gTableListPosTop) / gTableListLineHeight;
	return gCurGene[env];
}

/////////////////////////////////////////////