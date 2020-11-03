#ifndef  __ATCMDANALYSE_H__
#define  __ATCMDANALYSE_H__
#include "stdlib.h"
#include "stdint.h"
#include "sim900a.h"
typedef enum {NOTFOUND = 0, FOUND = !NOTFOUND} FundState;

ErrorStatus Analysis_CSQ_Cmd(char *pdata);
ErrorStatus Analysis_CGATT_Cmd(char *pdata);

#endif