//**************** 可配置的参数 ************************
#define MENU_MAX_CNT    		50    //菜单最大项数
#define MENU_MAX_CNT_IN_LAYER 	20    //菜单每层最大项数
#define MENU_MAX_LAYER 			10    //菜单最大层数
//******************************************************

typedef struct
{
    INT8U* node;
    INT8U* ch_name;
    INT8U* en_name;
    void (*func) (void* pdata);
    INT8U flag;     //是否显示 
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
	{"1", "参数设置", "System Setting", test2, 1},
	{"2", "时间设置", "Time Setting", test, 1},
};
