#include "ATCmdAnalyse.h"


ErrorStatus Analysis_CSQ_Cmd(char *pdata)
{
	char *ptStrStart ;
	char *ptFindResult ;
	
	ptStrStart=gsm_global_data.frame_buf;
	
	if((strstr(ptStrStart,"99") || strstr(ptStrStart,"0,0")))
	{
		//返回的信号质量错误
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
		//返回的信号质量错误
		return ERROR;
	}else{
		return SUCCESS;
	}	
}
