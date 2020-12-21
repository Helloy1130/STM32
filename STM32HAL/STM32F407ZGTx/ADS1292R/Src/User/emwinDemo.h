#ifndef _EMWINDEMO_H
#define _EMWINDEMO_H

#include "main.h"
#include "DIALOG.h"

extern WM_HWIN hWin_user;//对话框

//用户自定义消息
#define WM_SHOW_MSG (WM_USER+1)

void emwinDemo(void);

#endif
