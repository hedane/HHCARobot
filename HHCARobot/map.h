// map.h
// ��ͼ�����ַ�0�ա�1������2ǽ�ö�ά����ģʽ(x=2*i+0;y=2*i+1)��ɵ��ַ���������β'\0'��
//

#pragma once

//��ͼ���Ͷ���
typedef char *MAP;
typedef const char *CMAP;


//��ͼ����״̬����
#define MAP_CELL_NULL	'0' //��
#define MAP_CELL_TRUSH	'1' //����
#define MAP_CELL_WALL	'2' //ǽ


//��ͼ�ڴ��С
extern inline int map_size(void);

//������ͼ
extern inline MAP map_create(void);

//��ģ�崴����ͼ
extern inline MAP map_create(CMAP map_tpl);

//���ٵ�ͼ
extern inline void map_destroy(MAP map);

//���Ƶ�ͼ
extern inline MAP map_copy(MAP des_map, CMAP src_map);


//��ͼ��ʼ��(���)
MAP map_init(MAP map);

//ȡ��ͼ����
char map_cell(CMAP map, int x, int y);

//��������
bool map_clean(MAP map, int x, int y);

//��ȡ��������
int map_env_code(CMAP map, int x, int y);

//��ȡ��������
void map_env_decode(int env, char &north, char &east, char &south, char &west, char &center);

//��ͼ���ӿɷ��ƶ�
bool map_cell_movable(CMAP map, int x, int y);
