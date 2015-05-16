// robot.h
// �������ַ�0~6��ɵ��ַ���������β'\0'��
//
#pragma once

//�������Ͷ���
typedef char *GENE;
typedef const char *CGENE;

//�ж��������Ͷ���
typedef char *STEPS;
typedef const char *CSTEPS;

//���������Ͷ���
typedef struct tagROBOT
{
	long id;//����
	long father_id;//����
	int generation;//����
	GENE gene;//����
	long score;//�÷�
	STEPS steps;//�ж�����
} ROBOT, *PROBOT, *ROBOTS;


//��������Ϊ����
#define ROBOT_ACT_NORTH '0' //��Y+
#define ROBOT_ACT_SOUTH '1' //����Y-
#define ROBOT_ACT_EAST	'2' //��X+
#define ROBOT_ACT_WEST	'3' //����X-
#define ROBOT_ACT_RND	'4' //����ƶ�
#define ROBOT_ACT_STAY	'5' //����
#define ROBOT_ACT_CLEAN	'6' //������
#define ROBOT_ACT_RND_NORTH 'a' //�����Y+
#define ROBOT_ACT_RND_SOUTH 'b' //�������Y-
#define ROBOT_ACT_RND_EAST	'c' //�����X+
#define ROBOT_ACT_RND_WEST	'd' //�������X-
//���忪ʼ��־
#define ROBOT_ACT_BASE	ROBOT_ACT_NORTH
#define ROBOT_ACT_RND_BASE	ROBOT_ACT_RND_NORTH

//�������ж������ж���
#define ROBOT_STEPS			2
#define ROBOT_STEPS_ACTION	0
#define ROBOT_STEPS_RESULT	1

//�����ڴ��С
extern inline int robot_gene_size(void);

//�ж������ڴ��С
extern inline int robot_steps_size(void);

//���������˻���
extern inline GENE robot_gene_create(bool make = true);
extern inline GENE robot_gene_create(CGENE tpl);

//�����������ж�����
extern inline STEPS robot_steps_create(CSTEPS tpl = NULL);

//����������
//extern inline PROBOT robot_create(void);
//����������(һ��)
extern inline ROBOTS robot_create(int count);

//���ٻ����˻���
extern inline void robot_gene_destroy(GENE gene);

//���ٻ������ж�����
extern inline void robot_steps_destroy(STEPS steps);

//���ٻ�����
//extern inline void robot_destroy(PROBOT robot);
extern inline void robot_destroy(ROBOT &robot);
extern inline void robot_destroy(ROBOTS robots, int count);


//�����˼�����ʼ��
extern inline void robot_base_init(void);

//�����˳�ʼ��
//extern inline PROBOT robot_init(PROBOT robot, long father_id = 0, int generation = 0, GENE gene = NULL, STEPS steps = NULL);
extern inline void robot_init(ROBOT &robot, long father_id = 0, int generation = 0, GENE gene = NULL, STEPS steps = NULL);


//�������ж������ʼ��
extern inline void robot_steps_init(STEPS steps);

//��ȡ�������ж�����
extern inline char robot_steps_get(CSTEPS steps, int step, int type);
//���û������ж�����
extern inline void robot_steps_set(STEPS steps, int step, char action, bool result);

//���ɻ����˻���(���)
GENE robot_gene_make(GENE gene_buf);

//�����˻��򽻲����
int robot_gene_evolve(GENE child1, GENE child2, CGENE parent1, CGENE parent2);
