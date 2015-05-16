#include "stdafx.h"
#include "map.h"

/////////////////////////////////////////////
//地图内存大小
inline int map_size(void)
{
	return (gConfig.map_side_num * gConfig.map_side_num + 1);
}

//创建地图
inline MAP map_create(void)
{
	MAP ret = (MAP)malloc(map_size());
	ret[0] = '\0';
	return ret;
}

//从模板创建地图
inline MAP map_create(CMAP map_tpl)
{
	if(map_tpl == NULL)
	{
		return map_create();
	}
	return map_copy(map_create(), map_tpl);
}

//销毁地图
inline void map_destroy(MAP map)
{
	free(map);
}

//复制地图
inline MAP map_copy(MAP des_map, CMAP src_map)
{
	return (MAP)memcpy(des_map, src_map, map_size());
}

/////////////////////////////////////////////
//填充地图(随机)
void map_fill(MAP map, char value)
{
	int i = 0;
	while(i < gConfig.map_trash_num)
	{
		int x = rand() % gConfig.map_side_num;//随机X坐标
		int y = rand() % gConfig.map_side_num;//随机Y坐标
		int p = gConfig.map_side_num * x + y;
		if(map[p] != value)
		{
			map[p] = value;
			i++;
		}
	}
}

//地图初始化(随机)
MAP map_init(MAP map)
{
	int cells = map_size() - 1;

	if(gConfig.map_trash_num >= cells)//垃圾比格子还多
	{
		memset(map, MAP_CELL_TRUSH, cells);//全垃圾
	}
	else if(gConfig.map_trash_num < cells / 2) //垃圾不到一半
	{
		memset(map, MAP_CELL_NULL, cells);
		map_fill(map, MAP_CELL_TRUSH);
	}
	else //垃圾超过一半
	{
		memset(map, MAP_CELL_TRUSH, cells);
		map_fill(map, MAP_CELL_NULL);
	}

	map[cells] = '\0';//字符串结束位
	return map;
}

//取地图格子
char map_cell(CMAP map, int x, int y)
{
	if(x < 0 || x >= gConfig.map_side_num || y < 0 || y >= gConfig.map_side_num)
	{
		return MAP_CELL_WALL;//墙壁
	}
	return map[gConfig.map_side_num * x + y];
}

//捡起垃圾
bool map_clean(MAP map, int x, int y)
{
	if(map_cell(map, x, y) == MAP_CELL_TRUSH) //有垃圾
	{
		map[gConfig.map_side_num * x + y] = MAP_CELL_NULL;//移除垃圾
		return true;
	}
	return false;
}

//单元状态
inline int map_cell_state(CMAP map, int x, int y)
{
	return (map_cell(map, x, y) - MAP_CELL_NULL);//convert number char to int
}
//获取环境编码
int map_env_code(CMAP map, int x, int y)
{
	int state = 0;
	state += map_cell_state(map, x, y+1) * 3 * 3 * 3 * 3; //北(3进制第5位)
	state += map_cell_state(map, x+1, y) * 3 * 3 * 3;//东(3进制第4位)
	state += map_cell_state(map, x, y-1) * 3 * 3;//南(3进制第3位)
	state += map_cell_state(map, x-1, y) * 3;//西(3进制第2位)
	state += map_cell_state(map, x, y);//中(3进制第1位)
	return state;
}

//获取环境解码
void map_env_decode(int env, char &north, char &east, char &south, char &west, char &center)
{
	north = (char)(env / (3 * 3 * 3 * 3) + MAP_CELL_NULL); //北(3进制第5位)
	env = env % (3 * 3 * 3 * 3);
	east = (char)(env / (3 * 3 * 3) + MAP_CELL_NULL);//东(3进制第4位)
	env = env % (3 * 3 * 3);
	south = (char)(env / (3 * 3) + MAP_CELL_NULL);//南(3进制第3位)
	env = env % (3 * 3);
	west = (char)(env / 3 + MAP_CELL_NULL);//西(3进制第2位)
	center = (char)(env % 3 + MAP_CELL_NULL);//中(3进制第1位)
}

//地图格子可否移动
bool map_cell_movable(CMAP map, int x, int y)
{
	switch(map_cell(map, x, y))
	{
		case MAP_CELL_NULL://空白
		case MAP_CELL_TRUSH://垃圾
			return true;
		case MAP_CELL_WALL://障碍
			return false;
		default://异常
			throw;
			//return false;
	}
}
