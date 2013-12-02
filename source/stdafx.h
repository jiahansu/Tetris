#include <afxwin.h>
#include "DXUtil.h"
#include "ddutil.h"
#include "dsutil.h"
#include "MyInputManager.h"
#include "dmutil.h"


#define BlockSize       30

#define BlockDown       0
#define BlockLeft       1
#define BlockRight      2
#define BlockSDown      3

#define  Level2Score	250
#define  Level3Score	500
#define  Level4Score	1000
#define  Level5Score	2000
#define  Level6Score	4000
#define  Level7Score	8000
#define  Level8Score	15000
#define  Level9Score	30000
#define  Level10Score	50000
#define  MaxScore       100000
#define  BaseScore      10

#define  Level2Speed	2
#define  Level3Speed	3
#define  Level4Speed	5
#define  Level5Speed	6
#define  Level6Speed	6
#define  Level7Speed	10
#define  Level8Speed	10
#define  Level9Speed	15
#define  Level10Speed	30