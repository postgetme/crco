typedef struct
{
    INT8U id;				//参数ID	
	char* ch_name;         //名称
    char* en_name;
    
    FP32* var_p;        //变量指针
    INT8U var_type;     //变量类型，浮点，整型。。。
    
    INT8U var_attr;     //变量属性，只读，读写
    
    FP32 step_val;      //步进值
    
    char* ch_range_expr;         //范围表达示，"[0-5000]"/"(是|否)"
    char* en_range_expr;    
    
    char* ch_format_expr;  //显示格式表达式，"%04d毫安"/"%s"
    char* en_format_expr;  
    
    INT8U disp_flag;         //是否显示
}LIST_INFO;

#define  LIST_TYPE_FLOAT 11

#define  LIST_RW		  21
#define	 LIST_RO		  22

#define	 LIST_DISP_ON	  31
#define	 LIST_DISP_OFF	  32

static FP32 work_mode = 1;
static FP32 para_mode = 3;
static FP32 search_type = 0;

static FP32 limit_point = 0;
static FP32 sense = 0;
static FP32 balance = 0;
static FP32 voice_strength = 0;
static FP32 voice_seq = 0;

static FP32 voice_alert_point = 0;
static FP32 alert_type = 0;
static FP32 alert_dev = 0;

static FP32 bl_time = 0;
static FP32 language = 0;
static FP32 bat = 80;

LIST_INFO list_info[] =
{
    //可读可写项
    {1, "工作模式", "", &work_mode, LIST_TYPE_FLOAT, LIST_RW, 1, "(地平衡|识别)", "", "%s", "%s", LIST_DISP_ON}, 
    {2, "参数模式", "", &para_mode, LIST_TYPE_FLOAT, LIST_RW, 1, "(稳定型|标准型|灵敏型|自定义)", "", "%s", "%s", LIST_DISP_ON},
    {3, "搜索方式", "", &search_type, LIST_TYPE_FLOAT, LIST_RW, 1, "(普通|深度)", "", "%s", "%s", LIST_DISP_ON},
    
    {4, "临界点", "", &limit_point, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {5, "灵敏度", "", &sense, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {6, "地平衡", "", &balance, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {7, "音量", "", &voice_strength, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {8, "音频", "", &voice_seq, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    
    {9, "金属发声点", "", &voice_alert_point, LIST_TYPE_FLOAT, LIST_RW, 1, "(有色金属|黄金|银)", "", "%s", "%s", LIST_DISP_ON},
    {10, "报警方式", "", &alert_type, LIST_TYPE_FLOAT, LIST_RW, 1, "(渐强|突发)", "", "%s", "%s", LIST_DISP_ON},        
    {11, "报警设备", "", &alert_dev, LIST_TYPE_FLOAT, LIST_RW, 1, "(无线|内置)", "", "%s", "%s", LIST_DISP_ON},

    {12, "背光时间", "", &bl_time, LIST_TYPE_FLOAT, LIST_RW, 1, "(15秒|30秒|60秒)", "(15S|30S|60S)", "%s", "%s", LIST_DISP_ON},
    {13, "语言", "", &language, LIST_TYPE_FLOAT, LIST_RW, 1, "(中文|English)", "(中文|English)", "%s", "%s", LIST_DISP_ON},
                            
    //只读项 
    {14, "剩余电量", "", &bat, LIST_TYPE_FLOAT, LIST_RO, 1, "[0-100]", "[0-100]", "%1.0f%%", "%1.0f%%", LIST_DISP_ON},   
};

void cat5113_set (INT8U sel, INT8U val);

void list_on_change (INT8U id)
{
	switch (id)
	{
		case 1:
			if (work_mode == 0)
			{
				voice_alert_point = 0;
				list_info[8].ch_range_expr = "(---)";
				list_info[8].en_range_expr = "(---)";
				list_info[8].var_attr = LIST_RO;
			}
			else if (work_mode == 1)				  
			{
				voice_alert_point = 0;
				list_info[8].ch_range_expr = "(有色金属|黄金|银)";
				list_info[8].var_attr = LIST_RW;
			}
			break;
		case 2:
			//稳定型
			if (para_mode == 0)
			{
				balance = 30;
				limit_point = 20;
				sense = 0;
				voice_strength = 80;
				voice_alert_point = 1;
				work_mode = 1;

				list_info[8].ch_range_expr = "(有色金属|黄金|银)";
				list_info[3].var_attr = LIST_RO;
				list_info[4].var_attr = LIST_RO;
				list_info[5].var_attr = LIST_RO;
				list_info[6].var_attr = LIST_RO;
				list_info[8].var_attr = LIST_RO;
				list_info[0].var_attr = LIST_RO;
			}
			//标准型
			else if (para_mode == 1)
			{
				balance = 50;
				limit_point = 30;
				sense = 50;
				voice_strength = 80;
				voice_alert_point = 1;
				work_mode = 1;

				list_info[8].ch_range_expr = "(有色金属|黄金|银)";
				list_info[3].var_attr = LIST_RO;
				list_info[4].var_attr = LIST_RO;
				list_info[5].var_attr = LIST_RO;
				list_info[6].var_attr = LIST_RO;
				list_info[8].var_attr = LIST_RO;
				list_info[0].var_attr = LIST_RO;
			}
			//灵敏型
			else if (para_mode == 2)
			{
				balance = 85;
				limit_point = 50;
				sense = 100;
				voice_strength = 100;
				voice_alert_point = 1;
				work_mode = 1;

				list_info[8].ch_range_expr = "(有色金属|黄金|银)";
				list_info[3].var_attr = LIST_RO;
				list_info[4].var_attr = LIST_RO;
				list_info[5].var_attr = LIST_RO;
				list_info[6].var_attr = LIST_RO;
				list_info[8].var_attr = LIST_RO;
				list_info[0].var_attr = LIST_RO;
			}
			//自定义
			else if (para_mode == 3)
			{
				voice_alert_point = 0;
				list_info[3].var_attr = LIST_RW;
				list_info[4].var_attr = LIST_RW;
				list_info[5].var_attr = LIST_RW;
				list_info[6].var_attr = LIST_RW;
				list_info[0].var_attr = LIST_RW;
				list_info[8].var_attr = LIST_RW;
			}
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;

		default:
			break;
	}	
}

