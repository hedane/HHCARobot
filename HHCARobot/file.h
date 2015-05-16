#pragma once

//extern FILE *gFile;
extern CHAR gFileName[];

//文档是否已打开
bool file_is_open(void);

//文档已改变
extern inline void file_changed(void);

//关闭已经打开的文档
void file_close(void);

//打开存档
bool file_open(HWND hWnd);

//新建存档
bool file_new(HWND hWnd);

//读取存档
bool file_load(void);

//保存存档
bool file_save(void);
