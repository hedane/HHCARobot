// map.h
// 地图：由字符0空、1垃圾、2墙用二维数组模式(x=2*i+0;y=2*i+1)组成的字符串（含结尾'\0'）
//

#pragma once

//地图类型定义
typedef char *MAP;
typedef const char *CMAP;


//地图格子状态定义
#define MAP_CELL_NULL	'0' //空
#define MAP_CELL_TRUSH	'1' //垃圾
#define MAP_CELL_WALL	'2' //墙


//地图内存大小
extern inline int map_size(void);

//创建地图
extern inline MAP map_create(void);

//从模板创建地图
extern inline MAP map_create(CMAP map_tpl);

//销毁地图
extern inline void map_destroy(MAP map);

//复制地图
extern inline MAP map_copy(MAP des_map, CMAP src_map);


//地图初始化(随机)
MAP map_init(MAP map);

//取地图格子
char map_cell(CMAP map, int x, int y);

//捡起垃圾
bool map_clean(MAP map, int x, int y);

//获取环境编码
int map_env_code(CMAP map, int x, int y);

//获取环境解码
void map_env_decode(int env, char &north, char &east, char &south, char &west, char &center);

//地图格子可否移动
bool map_cell_movable(CMAP map, int x, int y);
