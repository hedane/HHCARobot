#pragma once

#include "map.h"
#include "robot.h"
#include "history.h"

//extern HWND hDrawWnd;
//extern RECT gDrawClientRect;
//extern HBITMAP gDrawBitmap;
//extern HDC gDrawBmpMemDC;


#define DRAW_MAP_WIDTH	600
#define DRAW_MAP_HEIGHT	450
#define DRAW_MAP_MARGIN_X	20
#define DRAW_MAP_MARGIN_Y	15
#define DRAW_MAP_CELL_LEN_X	24
#define DRAW_MAP_CELL_LEN_Y	18
#define DRAW_MAP_TRUSH_MARGIN	1
#define DRAW_MAP_SCORE_Y	10
#define DRAW_MAP_CELL_HALF_X	(DRAW_MAP_CELL_LEN_X / 2)
#define DRAW_MAP_CELL_HALF_Y	(DRAW_MAP_CELL_LEN_Y / 2)
#define DRAW_MAP_CIRCLE_X	8
#define DRAW_MAP_CIRCLE_Y	6
#define DRAW_MAP_CROSS_OFFSET	3


//Y����ӳ�䣨���½�Ϊ(0,0)��
#define DRAW_MAP_COORD_X(X) (X)
#define DRAW_MAP_COORD_Y(Y) (DRAW_MAP_HEIGHT + DRAW_MAP_MARGIN_Y*2 + 1 - (Y))
//�������궨λ
#define DRAW_MAP_CELL_X(X)	(DRAW_MAP_MARGIN_X + DRAW_MAP_CELL_LEN_X * (X))
#define DRAW_MAP_CELL_Y(Y)	(DRAW_MAP_MARGIN_Y + DRAW_MAP_CELL_LEN_Y * (Y))

//����ģ���ʼ��
void draw_init(HWND hWnd);

//����ģ��OnPaint�¼�
void draw_paint(HDC &hdc);

//���ƽ���ˢ��
//void draw_refresh(void);

//����ģ���ͷ���Դ
void draw_free(void);

//����
//void draw_line(int x1, int y1, int x2, int y2);
//����
void draw_clear(bool refresh = true);

//�����ı�
void draw_text(const char* fmt, ...);

//����ͼ
void draw_map(CMAP map);

//����ʵ����
void draw_steps(CSTEPS steps, long score, int count, int max = INT_MAX, int min = INT_MIN);

#define DRAW_PAGE_NO_Y	5

//�����б��и���̬
void draw_table_row_hover(int y);

//����ʵ������
#define DRAW_TEST_ROBOTS_ROWS	20
#define DRAW_TEST_MARGIN	10
void draw_test(int &page);

//��ȡʵ��������ָ������Ļ�����
PROBOT draw_test_robot_get(int &page, int y);

//����ʵ��ͳ������
#define DRAW_STAT_MARGIN	80
#define DRAW_STAT_TAG_NUM	10
#define DRAW_STAT_TAG_LEN	5
#define DRAW_STAT_LIST_MARGIN	10
#define DRAW_STAT_LIST_ROWS	25
void draw_stat(PHISTORY history);
void draw_stat_list(PHISTORY history, int &page);
//void draw_stat_runtime(int round, PHISTORY history);

//���ƻ���
#define DRAW_GENE_MARGIN	10
#define DRAW_GENE_ROWS	24
void draw_gene(CGENE gene, int &page);

//��ȡָ������Ļ���
char draw_gene_get(int &page, int y, int &env);

//��ȡ���ڻ��ƵĻ�����������
extern inline const char *draw_env_get(char code);
