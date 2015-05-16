#include "stdafx.h"
#include "map.h"

/////////////////////////////////////////////
//��ͼ�ڴ��С
inline int map_size(void)
{
	return (gConfig.map_side_num * gConfig.map_side_num + 1);
}

//������ͼ
inline MAP map_create(void)
{
	MAP ret = (MAP)malloc(map_size());
	ret[0] = '\0';
	return ret;
}

//��ģ�崴����ͼ
inline MAP map_create(CMAP map_tpl)
{
	if(map_tpl == NULL)
	{
		return map_create();
	}
	return map_copy(map_create(), map_tpl);
}

//���ٵ�ͼ
inline void map_destroy(MAP map)
{
	free(map);
}

//���Ƶ�ͼ
inline MAP map_copy(MAP des_map, CMAP src_map)
{
	return (MAP)memcpy(des_map, src_map, map_size());
}

/////////////////////////////////////////////
//����ͼ(���)
void map_fill(MAP map, char value)
{
	int i = 0;
	while(i < gConfig.map_trash_num)
	{
		int x = rand() % gConfig.map_side_num;//���X����
		int y = rand() % gConfig.map_side_num;//���Y����
		int p = gConfig.map_side_num * x + y;
		if(map[p] != value)
		{
			map[p] = value;
			i++;
		}
	}
}

//��ͼ��ʼ��(���)
MAP map_init(MAP map)
{
	int cells = map_size() - 1;

	if(gConfig.map_trash_num >= cells)//�����ȸ��ӻ���
	{
		memset(map, MAP_CELL_TRUSH, cells);//ȫ����
	}
	else if(gConfig.map_trash_num < cells / 2) //��������һ��
	{
		memset(map, MAP_CELL_NULL, cells);
		map_fill(map, MAP_CELL_TRUSH);
	}
	else //��������һ��
	{
		memset(map, MAP_CELL_TRUSH, cells);
		map_fill(map, MAP_CELL_NULL);
	}

	map[cells] = '\0';//�ַ�������λ
	return map;
}

//ȡ��ͼ����
char map_cell(CMAP map, int x, int y)
{
	if(x < 0 || x >= gConfig.map_side_num || y < 0 || y >= gConfig.map_side_num)
	{
		return MAP_CELL_WALL;//ǽ��
	}
	return map[gConfig.map_side_num * x + y];
}

//��������
bool map_clean(MAP map, int x, int y)
{
	if(map_cell(map, x, y) == MAP_CELL_TRUSH) //������
	{
		map[gConfig.map_side_num * x + y] = MAP_CELL_NULL;//�Ƴ�����
		return true;
	}
	return false;
}

//��Ԫ״̬
inline int map_cell_state(CMAP map, int x, int y)
{
	return (map_cell(map, x, y) - MAP_CELL_NULL);//convert number char to int
}
//��ȡ��������
int map_env_code(CMAP map, int x, int y)
{
	int state = 0;
	state += map_cell_state(map, x, y+1) * 3 * 3 * 3 * 3; //��(3���Ƶ�5λ)
	state += map_cell_state(map, x+1, y) * 3 * 3 * 3;//��(3���Ƶ�4λ)
	state += map_cell_state(map, x, y-1) * 3 * 3;//��(3���Ƶ�3λ)
	state += map_cell_state(map, x-1, y) * 3;//��(3���Ƶ�2λ)
	state += map_cell_state(map, x, y);//��(3���Ƶ�1λ)
	return state;
}

//��ȡ��������
void map_env_decode(int env, char &north, char &east, char &south, char &west, char &center)
{
	north = (char)(env / (3 * 3 * 3 * 3) + MAP_CELL_NULL); //��(3���Ƶ�5λ)
	env = env % (3 * 3 * 3 * 3);
	east = (char)(env / (3 * 3 * 3) + MAP_CELL_NULL);//��(3���Ƶ�4λ)
	env = env % (3 * 3 * 3);
	south = (char)(env / (3 * 3) + MAP_CELL_NULL);//��(3���Ƶ�3λ)
	env = env % (3 * 3);
	west = (char)(env / 3 + MAP_CELL_NULL);//��(3���Ƶ�2λ)
	center = (char)(env % 3 + MAP_CELL_NULL);//��(3���Ƶ�1λ)
}

//��ͼ���ӿɷ��ƶ�
bool map_cell_movable(CMAP map, int x, int y)
{
	switch(map_cell(map, x, y))
	{
		case MAP_CELL_NULL://�հ�
		case MAP_CELL_TRUSH://����
			return true;
		case MAP_CELL_WALL://�ϰ�
			return false;
		default://�쳣
			throw;
			//return false;
	}
}
