#pragma once

//extern FILE *gFile;
extern CHAR gFileName[];

//�ĵ��Ƿ��Ѵ�
bool file_is_open(void);

//�ĵ��Ѹı�
extern inline void file_changed(void);

//�ر��Ѿ��򿪵��ĵ�
void file_close(void);

//�򿪴浵
bool file_open(HWND hWnd);

//�½��浵
bool file_new(HWND hWnd);

//��ȡ�浵
bool file_load(void);

//����浵
bool file_save(void);
