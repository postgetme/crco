typedef struct
{
    INT8U id;				//����ID	
	char* ch_name;         //����
    char* en_name;
    
    FP32* var_p;        //����ָ��
    INT8U var_type;     //�������ͣ����㣬���͡�����
    
    INT8U var_attr;     //�������ԣ�ֻ������д
    
    FP32 step_val;      //����ֵ
    
    char* ch_range_expr;         //��Χ���ʾ��"[0-5000]"/"(��|��)"
    char* en_range_expr;    
    
    char* ch_format_expr;  //��ʾ��ʽ���ʽ��"%04d����"/"%s"
    char* en_format_expr;  
    
    INT8U disp_flag;         //�Ƿ���ʾ
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
    //�ɶ���д��
    {1, "����ģʽ", "", &work_mode, LIST_TYPE_FLOAT, LIST_RW, 1, "(��ƽ��|ʶ��)", "", "%s", "%s", LIST_DISP_ON}, 
    {2, "����ģʽ", "", &para_mode, LIST_TYPE_FLOAT, LIST_RW, 1, "(�ȶ���|��׼��|������|�Զ���)", "", "%s", "%s", LIST_DISP_ON},
    {3, "������ʽ", "", &search_type, LIST_TYPE_FLOAT, LIST_RW, 1, "(��ͨ|���)", "", "%s", "%s", LIST_DISP_ON},
    
    {4, "�ٽ��", "", &limit_point, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {5, "������", "", &sense, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {6, "��ƽ��", "", &balance, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {7, "����", "", &voice_strength, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    {8, "��Ƶ", "", &voice_seq, LIST_TYPE_FLOAT, LIST_RW, 1, "[0-100]", "[0-100]", "%1.0f", "%1.0f", LIST_DISP_ON},
    
    {9, "����������", "", &voice_alert_point, LIST_TYPE_FLOAT, LIST_RW, 1, "(��ɫ����|�ƽ�|��)", "", "%s", "%s", LIST_DISP_ON},
    {10, "������ʽ", "", &alert_type, LIST_TYPE_FLOAT, LIST_RW, 1, "(��ǿ|ͻ��)", "", "%s", "%s", LIST_DISP_ON},        
    {11, "�����豸", "", &alert_dev, LIST_TYPE_FLOAT, LIST_RW, 1, "(����|����)", "", "%s", "%s", LIST_DISP_ON},

    {12, "����ʱ��", "", &bl_time, LIST_TYPE_FLOAT, LIST_RW, 1, "(15��|30��|60��)", "(15S|30S|60S)", "%s", "%s", LIST_DISP_ON},
    {13, "����", "", &language, LIST_TYPE_FLOAT, LIST_RW, 1, "(����|English)", "(����|English)", "%s", "%s", LIST_DISP_ON},
                            
    //ֻ���� 
    {14, "ʣ�����", "", &bat, LIST_TYPE_FLOAT, LIST_RO, 1, "[0-100]", "[0-100]", "%1.0f%%", "%1.0f%%", LIST_DISP_ON},   
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
				list_info[8].ch_range_expr = "(��ɫ����|�ƽ�|��)";
				list_info[8].var_attr = LIST_RW;
			}
			break;
		case 2:
			//�ȶ���
			if (para_mode == 0)
			{
				balance = 30;
				limit_point = 20;
				sense = 0;
				voice_strength = 80;
				voice_alert_point = 1;
				work_mode = 1;

				list_info[8].ch_range_expr = "(��ɫ����|�ƽ�|��)";
				list_info[3].var_attr = LIST_RO;
				list_info[4].var_attr = LIST_RO;
				list_info[5].var_attr = LIST_RO;
				list_info[6].var_attr = LIST_RO;
				list_info[8].var_attr = LIST_RO;
				list_info[0].var_attr = LIST_RO;
			}
			//��׼��
			else if (para_mode == 1)
			{
				balance = 50;
				limit_point = 30;
				sense = 50;
				voice_strength = 80;
				voice_alert_point = 1;
				work_mode = 1;

				list_info[8].ch_range_expr = "(��ɫ����|�ƽ�|��)";
				list_info[3].var_attr = LIST_RO;
				list_info[4].var_attr = LIST_RO;
				list_info[5].var_attr = LIST_RO;
				list_info[6].var_attr = LIST_RO;
				list_info[8].var_attr = LIST_RO;
				list_info[0].var_attr = LIST_RO;
			}
			//������
			else if (para_mode == 2)
			{
				balance = 85;
				limit_point = 50;
				sense = 100;
				voice_strength = 100;
				voice_alert_point = 1;
				work_mode = 1;

				list_info[8].ch_range_expr = "(��ɫ����|�ƽ�|��)";
				list_info[3].var_attr = LIST_RO;
				list_info[4].var_attr = LIST_RO;
				list_info[5].var_attr = LIST_RO;
				list_info[6].var_attr = LIST_RO;
				list_info[8].var_attr = LIST_RO;
				list_info[0].var_attr = LIST_RO;
			}
			//�Զ���
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

