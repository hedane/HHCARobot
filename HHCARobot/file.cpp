#include "stdafx.h"
#include "file.h"
#include "test.h"

FILE *gFile = NULL;
CHAR gFileName[MAX_PATH] = {0};
bool gFileChanged = false;

/////////////////////////////////////////////
//获取存档目录
/*inline char *file_folder(char *out_folder)
{
	if(gFile == NULL)
	{
		return NULL;
	}
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	_splitpath_s(gFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);
   	_makepath_s(out_folder, _MAX_PATH, drive, dir, NULL, NULL);

   return out_folder;
}*/

//文档是否已打开
inline bool file_is_open(void)
{
	return (gFile != NULL);
}

//文档已改变
inline void file_changed(void)
{
	gFileChanged = true;
}

/////////////////////////////////////////////
//打开存档
bool file_open(HWND hWnd)
{
	OPENFILENAME ofn = {};

	//char folder_buf[MAX_PATH];

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hWnd;
	ofn.lpstrFile         = gFileName;
    ofn.nMaxFile          = sizeof(gFileName);
    ofn.lpstrFilter       = "项目文件(*.xml)\0*.xml\0所有文件(*.*)\0*.*\0\0";
	//ofn.nFilterIndex      = 1;
    //ofn.lpstrDefExt       = "xml";
    ofn.lpstrTitle        = "打开项目文件";
	//ofn.nMaxFileTitle     = strlen("打开项目文件");
	//ofn.lpstrInitialDir   = file_folder(folder_buf);
    ofn.Flags             = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    //ofn.FlagsEx           = OFN_EX_NOPLACESBAR;
    if(GetOpenFileName(&ofn))
    {
		fopen_s(&gFile, gFileName, "rb+");
		return true;
    }
	return false;
}

//新建存档
bool file_new(HWND hWnd)
{
	if(file_is_open())
	{
		return false;
	}

	OPENFILENAME ofn = {};

	//char folder_buf[MAX_PATH];

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hWnd;
	ofn.lpstrFile         = gFileName;
    ofn.nMaxFile          = sizeof(gFileName);
    ofn.lpstrFilter       = "项目文件(*.xml)\0*.xml\0所有文件(*.*)\0*.*\0\0";
	//ofn.nFilterIndex      = 1;
    ofn.lpstrDefExt       = "xml";
    ofn.lpstrTitle        = "新建项目文件";
	//ofn.nMaxFileTitle     = strlen("新建项目文件");
	//ofn.lpstrInitialDir   = file_folder(folder_buf);
    ofn.Flags             = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    //ofn.FlagsEx           = OFN_EX_NOPLACESBAR;
    if(GetSaveFileName(&ofn))
    {
		fopen_s(&gFile, gFileName, "wb+");
		return true;
    }
	return false;
}

/////////////////////////////////////////////
//读取存档
const char *file_xml_get_text(TiXmlNode *node, const char *name, const char *def = NULL)
{
	TiXmlElement *element = node->FirstChildElement(name);
	if(element == NULL)
	{
		return def;
	}
	return element->GetText();
}
int file_xml_get_value(TiXmlNode *node, const char *name, int def = 0, int min = 0)
{
	const char *text = file_xml_get_text(node, name, NULL);
	if(text == NULL)
	{
		return def;
	}
	int value = atoi(text);
	return value < min ? def : value;
}
bool file_load(void)
{
	TiXmlDocument doc;
	if(!doc.LoadFile(gFile))
	{
		return false;
	}
	TiXmlElement *root = doc.RootElement();
	if(root == NULL)
	{
		return false;
	}
	TiXmlNode *global = root->FirstChild("global");
	if(global == NULL)
	{
		return false;
	}
	TiXmlNode *map = global->FirstChild("map");
	if(map == NULL)
	{
		return false;
	}
	TiXmlNode *robot = global->FirstChild("robot");
	if(robot == NULL)
	{
		return false;
	}
	TiXmlNode *lab = global->FirstChild("lab");
	if(lab == NULL)
	{
		return false;
	}
	TiXmlNode *test = root->FirstChild("test");
	if(test == NULL)
	{
		return false;
	}
	TiXmlNode *items = test->FirstChild("robots");
	if(items == NULL)
	{
		return false;
	}

	//global
	gConfig.map_side_num = file_xml_get_value(map, "sideNum", MAP_SIDE_NUM, 1);
	gConfig.map_trash_num = file_xml_get_value(map, "trashNum", MAP_TRASH_NUM, 1);

	gConfig.robot_gene_length = file_xml_get_value(robot, "geneLength", ROBOT_GENE_LENGTH, 1);
	gConfig.robot_step_count = file_xml_get_value(robot, "stepCount", ROBOT_STEP_COUNT, 1);
	gConfig.robot_id_base = file_xml_get_value(robot, "idBase", 0, 0);
	gConfig.robot_father_id_base = file_xml_get_value(robot, "fatherIdBase", 0, 0);
	gConfig.robot_evo_var_rate = file_xml_get_value(robot, "evoVarRate", ROBOT_EVO_VAR_RATE, 1);
	gConfig.robot_evo_var_piece = file_xml_get_value(robot, "evoVarPiece", ROBOT_EVO_VAR_PIECE, 1);

	int value;
	gConfig.lab_action_trash_ok = file_xml_get_value(lab, "actionTrashOK", LAB_ACTION_TRASH_OK, 1);
	value = file_xml_get_value(lab, "actionTrashFail", LAB_ACTION_TRASH_FAIL);
	gConfig.lab_action_trash_fail = value < 0 ? value : LAB_ACTION_TRASH_FAIL;
	value = file_xml_get_value(lab, "actionKonckWall", LAB_ACTION_KONCK_WALL);
	gConfig.lab_action_konck_wall = value < 0 ? value : LAB_ACTION_KONCK_WALL;
	gConfig.lab_more_count = file_xml_get_value(lab, "moreCount", LAB_MORE_COUNT, 1);
	gConfig.lab_more_robot_count = file_xml_get_value(lab, "moreRobotCount", LAB_MORE_ROBOT_COUNT, 1);
	gConfig.lab_more_robot_top_count = file_xml_get_value(lab, "moreRobotTopCount", LAB_MORE_ROBOT_TOP_COUNT, 1);
	gConfig.lab_more_robot_top_racial_limit = file_xml_get_value(lab, "moreRobotTopRacialLimit", LAB_MORE_ROBOT_TOP_RACIAL_LIMIT, 1);

	//test
	gTestParam->round = file_xml_get_value(test, "round", 0, 1);
	gTestParam->ts_start = file_xml_get_value(test, "tsStart", 0, 1);
	gTestParam->ts_stop = file_xml_get_value(test, "tsStop", 0, 1);
	gTestParam->ts_total = file_xml_get_value(test, "tsTotal", 0, 1);

	ROBOTS robots = NULL;
	if(!items->NoChildren())
	{
		robots = robot_create(gConfig.lab_more_robot_count);
		int n = 0;
		for(TiXmlNode *item = items->FirstChild("robot"); item != NULL; item = item->NextSibling("robot"))
		{
			if(n >= gConfig.lab_more_robot_count)
			{
				throw;//数量超过全局变量了
				//return false;
			}
			ROBOT &robot = robots[n];

			robot.id = file_xml_get_value(item, "id");
			robot.father_id = file_xml_get_value(item, "fatherId");
			robot.generation = file_xml_get_value(item, "generation");
			robot.gene = robot_gene_create((CGENE)file_xml_get_text(item, "gene"));
			robot.score = file_xml_get_value(item, "score", 0, INT_MIN);
			robot.steps = robot_steps_create(); //robot_steps_create((CSTEPS)file_xml_get_text(item, "steps"));

			n++;
			if(n % 1000 == 1){DoEvents();}
		}
		for(int i=n;i<gConfig.lab_more_robot_count;i++)//数量不足全局变量的
		{
			robot_init(robots[i]);
		}
	}

	//history
	PHISTORY history = history_create();
	TiXmlNode *rounds = test->FirstChild("history");
	if(rounds != NULL && !rounds->NoChildren())
	{
		int n = 0;
		for(TiXmlElement *item = rounds->FirstChildElement("round"); item != NULL; item = item->NextSiblingElement("round"))
		{
			if(n >= gTestParam->round)
			{
				throw;//数量超过世代了
				//return false;
			}
			value = atoi(item->FirstAttribute()->Value());
			if(value <= 0 || value > gTestParam->round)
			{
				throw;//编号不正确
				//return false;
			}
			PHISTORYITEM history_item = history_append(history, value);

			int m = 0;
			for(TiXmlNode *itm = item->FirstChildElement("robot"); itm != NULL; itm = item->NextSibling("robot"))
			{
				if(m >= history_item->top_len)
				{
					throw;//数量超过缓存了
					//return false;
				}

				ROBOT &robot = history_item->top[m];

				robot.id = file_xml_get_value(itm, "id");
				robot.father_id = file_xml_get_value(itm, "fatherId");
				robot.generation = file_xml_get_value(itm, "generation");
				robot.gene = NULL; //robot_gene_create((CGENE)file_xml_get_text(itm, "gene"));
				robot.score = file_xml_get_value(itm, "score", 0, INT_MIN);
				robot.steps = NULL; //robot_steps_create((CSTEPS)file_xml_get_text(itm, "steps"));

				m++;
			}

			n++;
			if(n % 1000 == 1){DoEvents();}
		}
	}

	srand((UINT)gTestParam->ts_stop); //设置随机种子
	int thread_num = test_thread_num(); //获取实验线程数

	//创建1代试验参数
	PLAB_THREAD_PARAM_GROUP tpg = NULL;
	if(robots != NULL)
	{
		/*TiXmlElement *test_map = test->FirstChildElement("map");
		CMAP map_tpl = test_map != NULL ? (CMAP)test_map->GetText() : NULL;*/
		tpg = lab_thread_params_create(robots, gTestParam->ts_stop, thread_num/*, map_tpl*/);
	}

	gTestParam->robots = robots;
	gTestParam->tpg = tpg;
	gTestParam->history = history;
	//gTestParam->running = false;

	gFileChanged = false;
	return true;
}

//保存存档
inline TiXmlNode *file_xml_node_ensure(TiXmlNode *parent, const char *name)
{
	TiXmlNode *node = parent->FirstChild(name);
	if(node != NULL)
	{
		return node;
	}
	return parent->LinkEndChild(new TiXmlElement(name));
}
inline void file_xml_set_value(TiXmlNode *parent, const char *name, const char *value)
{
	TiXmlNode *node = file_xml_node_ensure(parent, name);
	TiXmlNode *child = node->FirstChild();
	if(child == NULL)
	{
		node->LinkEndChild(new TiXmlText(value));
	}
	else
	{
		if(strcmp(child->Value(), value) != 0)
		{
			child->SetValue(value);
		}
	}
}
bool file_save(void)
{
	if(!gFileChanged)
	{
		return false;
	}

	TiXmlDocument doc;
	if(!doc.LoadFile(gFile))
	{
		doc.SetUseMSBOM(true);//utf8
		doc.LinkEndChild(new TiXmlDeclaration("1.0", "UTF-8", ""));
	}
	TiXmlNode *root = file_xml_node_ensure(&doc, "root");

	//global
	TiXmlNode *global = file_xml_node_ensure(root, "global");
	TiXmlNode *map = file_xml_node_ensure(global, "map");
	TiXmlNode *robot = file_xml_node_ensure(global, "robot");
	TiXmlNode *lab = file_xml_node_ensure(global, "lab");

	char buf[MAX_SAVESTRING];
	sprintf_s(buf, sizeof(buf), "%d", gConfig.map_side_num);
	file_xml_set_value(map, "sideNum", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.map_trash_num);
	file_xml_set_value(map, "trashNum", buf);

	sprintf_s(buf, sizeof(buf), "%d", gConfig.robot_gene_length);
	file_xml_set_value(robot, "geneLength", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.robot_step_count);
	file_xml_set_value(robot, "stepCount", buf);
	sprintf_s(buf, sizeof(buf), "%ld", gConfig.robot_id_base);
	file_xml_set_value(robot, "idBase", buf);
	sprintf_s(buf, sizeof(buf), "%ld", gConfig.robot_father_id_base);
	file_xml_set_value(robot, "fatherIdBase", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.robot_evo_var_rate);
	file_xml_set_value(robot, "evoVarRate", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.robot_evo_var_piece);
	file_xml_set_value(robot, "evoVarPiece", buf);

	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_action_trash_ok);
	file_xml_set_value(lab, "actionTrashOK", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_action_trash_fail);
	file_xml_set_value(lab, "actionTrashFail", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_action_konck_wall);
	file_xml_set_value(lab, "actionKonckWall", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_more_count);
	file_xml_set_value(lab, "moreCount", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_more_robot_count);
	file_xml_set_value(lab, "moreRobotCount", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_more_robot_top_count);
	file_xml_set_value(lab, "moreRobotTopCount", buf);
	sprintf_s(buf, sizeof(buf), "%d", gConfig.lab_more_robot_top_racial_limit);
	file_xml_set_value(lab, "moreRobotTopRacialLimit", buf);

	//test
	TiXmlNode *test = file_xml_node_ensure(root, "test");
	sprintf_s(buf, sizeof(buf), "%d", gTestParam->round);
	file_xml_set_value(test, "round", buf);
	sprintf_s(buf, sizeof(buf), "%u", gTestParam->ts_start);
	file_xml_set_value(test, "tsStart", buf);
	sprintf_s(buf, sizeof(buf), "%u", gTestParam->ts_stop);
	file_xml_set_value(test, "tsStop", buf);
	sprintf_s(buf, sizeof(buf), "%u", gTestParam->ts_total);
	file_xml_set_value(test, "tsTotal", buf);
	/*if(gTestParam->tpg != NULL)
	{
		file_xml_set_value(test, "map", gTestParam->tpg->map);
	}*/

	TiXmlNode *items = file_xml_node_ensure(test, "robots");
	items->Clear();
	if(gTestParam->robots != NULL)
	{
		for(int i=0;i<gConfig.lab_more_robot_count;i++)
		{
			TiXmlNode *item = new TiXmlElement("robot");
			ROBOT &robot = gTestParam->robots[i];

			sprintf_s(buf, sizeof(buf), "%ld", robot.id);
			file_xml_set_value(item, "id", buf);
			sprintf_s(buf, sizeof(buf), "%ld", robot.father_id);
			file_xml_set_value(item, "fatherId", buf);
			sprintf_s(buf, sizeof(buf), "%d", robot.generation);
			file_xml_set_value(item, "generation", buf);
			file_xml_set_value(item, "gene", robot.gene);
			sprintf_s(buf, sizeof(buf), "%ld", robot.score);
			file_xml_set_value(item, "score", buf);
			//file_xml_set_value(item, "steps", robot.steps);

			items->LinkEndChild(item);
			if(i % 1000 == 0){DoEvents();}
		}
	}
	
	TiXmlNode *history = file_xml_node_ensure(test, "history");
	int last_gen = 0;
	TiXmlNode *last = history->LastChild("round");
	if(last != NULL)
	{
		int value = atoi(last->ToElement()->FirstAttribute()->Value());
		if(value > last_gen){last_gen = value;}
	}
	PHISTORYITEM cur = gTestParam->history->head;
	PHISTORYITEM prev = NULL;
	int n = 0;
	while(cur != NULL)
	{
		if(cur->round <= last_gen) //已经有历史记录的，直接跳过
		{
			goto lblNext;
		}
		if(prev != NULL/* && prev->top_len > 0 && cur->top_len > 0*/) //过滤相同记录
		{
			if(prev->top[0].generation == cur->top[0].generation) //子代没变，不存档
			{
				goto lblNext;
			}
		}

		TiXmlElement *item = new TiXmlElement("round");

		item->SetAttribute("id", cur->round);
		for(int i=0;i<cur->top_len;i++)
		{
			TiXmlNode *itm = new TiXmlElement("robot");
			ROBOT &robot = cur->top[i];

			sprintf_s(buf, sizeof(buf), "%ld", robot.id);
			file_xml_set_value(itm, "id", buf);
			sprintf_s(buf, sizeof(buf), "%ld", robot.father_id);
			file_xml_set_value(itm, "fatherId", buf);
			sprintf_s(buf, sizeof(buf), "%d", robot.generation);
			file_xml_set_value(itm, "generation", buf);
			//file_xml_set_value(itm, "gene", robot.gene);
			sprintf_s(buf, sizeof(buf), "%ld", robot.score);
			file_xml_set_value(itm, "score", buf);
			//file_xml_set_value(itm, "steps", robot.steps);

			item->LinkEndChild(itm);
		}

		history->LinkEndChild(item);
lblNext:
		prev = cur;
		cur = cur->next;
		n++;
		if(n % 1000 == 1){DoEvents();}
	}

	//save
	rewind(gFile);
	if(!doc.SaveFile(gFile))
	{
		return false;
	}

	//clear
	fputc(EOF, gFile);
	/*int curpos = ftell(gFile);
	fseek(gFile, 0, SEEK_END);
	int length = ftell(gFile);
	for(int i=curpos;i<length;i++)
	{
		fputc(EOF, gFile);
	}
	fseek(gFile, curpos, SEEK_SET);*/

	gFileChanged = false;
	return true;
}

//关闭已经打开的文档
void file_close(void)
{
	if(file_is_open())
	{
		fclose(gFile);
		gFile = NULL;
		gFileChanged = false;
	}
}
