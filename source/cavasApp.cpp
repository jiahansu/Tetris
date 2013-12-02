// cavasApp.cpp: implementation of the cavasApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "canvasr.h"
#include "cavasApp.h"
#include "canvasFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
cavasApp canvas;

cavasApp::cavasApp()
{
	
}

cavasApp::~cavasApp()
{

}

BOOL cavasApp::InitInstance()
{
	m_pMainWnd = new canvasFrame;
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return true;
}
