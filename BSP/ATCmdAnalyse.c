#include "ATCmdAnalyse.h"


ErrorStatus Analysis_CSQ_Cmd(char *pdata)
{
	char *ptStrStart ;
	char *ptFindResult ;
	
	ptStrStart=gsm_global_data.frame_buf;
	
	if((strstr(ptStrStart,"99") || strstr(ptStrStart,"0,0")))
	{
		//���ص��ź���������
		return ERROR;
	}else{
		return SUCCESS;
	}
}

ErrorStatus Analysis_CGATT_Cmd(char *pdata)
{
	char *ptStrStart ;
	char *ptFindResult ;
	
	ptStrStart=gsm_global_data.frame_buf;
	
	if(!strstr(ptStrStart,"1"))
	{
		//���ص��ź���������
		return ERROR;
	}else{
		return SUCCESS;
	}	
}
