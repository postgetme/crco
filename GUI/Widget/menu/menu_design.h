//**************** �����õĲ��� ************************
#define MENU_MAX_CNT    		50    //�˵��������
#define MENU_MAX_CNT_IN_LAYER 	20    //�˵�ÿ���������
#define MENU_MAX_LAYER 			10    //�˵�������
//******************************************************

typedef struct
{
    INT8U* node;
    INT8U* ch_name;
    INT8U* en_name;
    void (*func) (void* pdata);
    INT8U flag;     //�Ƿ���ʾ 
}MENU_INFO;

void test (void* pdata)
{
	widget_enter_sub ("time_set", 0);
}

void test2 (void* pdata)
{
	widget_enter_sub ("list", 0);
}

const MENU_INFO menu_info[] =
{
	{"1", "��������", "System Setting", test2, 1},
	{"2", "ʱ������", "Time Setting", test, 1},
};
