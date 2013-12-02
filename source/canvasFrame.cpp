// canvasFrame.cpp : implementation file
//
#include "stdafx.h"
#include "canvasr.h"
#include "canvasFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// canvasFrame
DWORD SWidth = 30*12, SHeight = 30*22, SFRate = 85, SBit = 32;

CDisplay* pDD = NULL;
CSurface* BKSurface, *TextLevel,*TextUp, *BGSurface, *SDSurface, *NUSurface, *EDSurface
,*PicSpeedBar, *TextFps;
HFONT FoUp, FoLevel;

CSoundManager* pDS = NULL;
CSound* WavRotate = NULL, *WavCant = NULL, *WavKill = NULL, *WavDelete = NULL,
*WavUp = NULL, *WavWin = NULL, *WavDanger = NULL;
LPDIRECTSOUND3DBUFFER pS3DWavRotate = NULL, pS3DCant = NULL, pS3DKill = NULL, pS3DDelete = NULL
, pS3DUp = NULL, pS3DWin = NULL, pS3DDanger;
LPDIRECTSOUND3DLISTENER pSListener = NULL;
DS3DBUFFER              DS3DBufParams;               // 3D buffer properties
DS3DLISTENER            DSListenerParams;             // Listener properties

CMusicManager*     pDM          = NULL;
CMusicSegment*     MuBG          = NULL;
HANDLE             pDMEvent    = NULL;

CMyInputManager* pDI;

HRESULT result;


CPoint SPoint(0,0);
char KB[256];
DIJOYSTATE2 JState;
int Score = 0, Level = 1, OldScore = 1, TCount = 0, OldLevel = 1, Key[10], RCount = 0,
WCount = 0;
bool R = false,Music = true, Ro = false;

CRBlock* pBlock;

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

canvasFrame::canvasFrame()
{	
RECT rect;
	Create(NULL,"俄羅斯方塊Beta",WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_THICKFRAME);

}

canvasFrame::~canvasFrame()
{	
	if(Music)
		MuBG->Stop();
	delete pBlock;
	FreeHW();
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MOVING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rect;
	HINSTANCE hinst = AfxGetInstanceHandle();
	InitHW();
	CWelcome WC;
	if(WC.DoModal() != IDOK)//Show出Dialog給User設定選項
		PostMessage(WM_CLOSE);

	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = (width - (rect.right - rect.left ))/2;
	height = (height - (rect.bottom - rect.top ))/2;
	MoveWindow( width , height , dc.GetDeviceCaps(HORZRES)/2-50, dc.GetDeviceCaps(VERTRES)-100 ,
		true);

	CPoint *p = new CPoint(SWidth/2, SHeight/2);
	ClientToScreen(p);
	::SetCursorPos(p->x, p->y);
	delete p;
	SPoint = CPoint(SWidth/2, SHeight/2);
	if( FAILED( pDD->CreateSurfaceFromBitmap(&BGSurface, MAKEINTRESOURCE(IDB_BGROUND), BlockSize*10
		,BlockSize*20 ) ) )//載入圖形
			MessageBox(TEXT("CDisplay::建立背景暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//方塊的背景圖案
	if( FAILED( pDD->CreateSurfaceFromBitmap(&BKSurface, MAKEINTRESOURCE(IDB_BLOCK), BlockSize*7
		,BlockSize ) ) )//方塊的圖案
			MessageBox(TEXT("CDisplay::建立方塊暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	if( FAILED( pDD->CreateSurfaceFromBitmap(&EDSurface, MAKEINTRESOURCE(IDB_EDGE), BlockSize
		,BlockSize ) ) )
			MessageBox(TEXT("CDisplay::建立牆暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//磚塊的圖案
	if( FAILED( pDD->CreateSurfaceFromBitmap(&NUSurface, MAKEINTRESOURCE(IDB_NUMBER), 300
		,30 ) ) )
			MessageBox(TEXT("CDisplay::建立分數暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//分數的圖案
	if( FAILED( pDD->CreateSurfaceFromBitmap(&PicSpeedBar, MAKEINTRESOURCE(IDB_SBARS), 880
		,100 ) ) )
			MessageBox(TEXT("CDisplay::建立SpeedBar暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	PicSpeedBar->SetColorKey(0x00000000);//表示速度的圖案
	FoLevel = pDD->CreateHFONT("Comic Sans MS", 18, FW_NORMAL, false, false, false);
	if( FAILED( pDD->CreateSurfaceFromText(&TextLevel, FoLevel,"Level 01", RGB(0, 0, 0) 
		, RGB(0, 255, 0) )) )//建立文字
			MessageBox(TEXT("CDisplay::建立文字暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//表示Level的文字
	FoUp = pDD->CreateHFONT("Comic Sans MS", 16, FW_NORMAL, true, true, false);
	if( FAILED( pDD->CreateSurfaceFromText(&TextUp, FoUp,"Level Up!", RGB(0, 0, 0) 
		, RGB(255, 0, 0) )) )//表示Level Up的文字
			MessageBox(TEXT("CDisplay::建立文字暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	TextUp->SetColorKey(0x00000000);
	if( FAILED( pDD->CreateSurfaceFromText(&TextFps, NULL,"1000.0 FPS", RGB(0, 0, 0) 
		, RGB(255, 255, 0) )) )//表示FPS的文字
			MessageBox(TEXT("CDisplay::建立文字暫存區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	TextFps->SetColorKey(0x00000000);
 
	if(FAILED(pDS->Create(&WavRotate, MAKEINTRESOURCE(IDW_ROTATE), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立轉向次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入方塊轉向的音效
	if(FAILED(WavRotate->Get3DBufferInterface(0, &pS3DWavRotate)))
		MessageBox(TEXT("CSound::建立轉向3D次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	DS3DBufParams.dwSize = sizeof(DS3DBUFFER);
	pS3DWavRotate->GetAllParameters(&DS3DBufParams);
	DS3DBufParams.dwMode = DS3DMODE_HEADRELATIVE;
	pS3DWavRotate->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);

	if(FAILED(pDS->Create(&WavCant, MAKEINTRESOURCE(IDW_CANT), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立卡住次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入方塊轉向失敗的音效
	if(FAILED(WavCant->Get3DBufferInterface(0, &pS3DCant)))
		MessageBox(TEXT("CSound::建立3D卡住次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	pS3DCant->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);

	if(FAILED(pDS->Create(&WavKill, MAKEINTRESOURCE(IDW_KILL), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立死亡次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入方塊被定住的音效
	if(FAILED(WavKill->Get3DBufferInterface(0, &pS3DKill)))
		MessageBox(TEXT("CSound::建立3D死亡次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	pS3DKill->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);

	if(FAILED(pDS->Create(&WavDelete, MAKEINTRESOURCE(IDW_DELETE), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立刪除次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入方塊被消除的音效
	if(FAILED(WavDelete->Get3DBufferInterface(0, &pS3DDelete)))
		MessageBox(TEXT("CSound::建立3D刪除次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	pS3DDelete->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);
	
	if(FAILED(pDS->Create(&WavUp, MAKEINTRESOURCE(IDW_UP), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立升級次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入升級的音效
	if(FAILED(WavUp->Get3DBufferInterface(0, &pS3DUp)))
		MessageBox(TEXT("CSound::建立3D升級次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	pS3DUp->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);

	if(FAILED(pDS->Create(&WavWin, MAKEINTRESOURCE(IDW_WIN), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立勝利次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入勝利的音效
	if(FAILED(WavWin->Get3DBufferInterface(0, &pS3DWin)))
		MessageBox(TEXT("CSound::建立3D勝利次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	pS3DWin->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);

	if(FAILED(pDS->Create(&WavDanger, MAKEINTRESOURCE(IDW_DANGER), DSBCAPS_CTRL3D, DS3DALG_HRTF_FULL)))
		MessageBox(TEXT("CSoundManager::建立危險次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);//載入危險的音效
	if(FAILED(WavDanger->Get3DBufferInterface(0, &pS3DDanger)))
		MessageBox(TEXT("CSound::建立3D危險次緩衝區失敗!"), TEXT("DirectX Windowed"),
			MB_ICONERROR | MB_OK);
	pS3DDanger->SetAllParameters(&DS3DBufParams, DS3D_IMMEDIATE);
    
	if(Music){//載入音樂
		if( FAILED( pDM->CreateSegmentFromResource( &MuBG, MAKEINTRESOURCE(IDM_MUSIC),"MIDI", 
															TRUE, true ) ) )
			MessageBox(TEXT("CMusicManager::建立背景音樂失敗! "),
						TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
		IDirectMusicPerformance* pPerf = NULL;
		long nVolume = -1000;
		if( pDM )
			pPerf = pDM->GetPerformance();
		pPerf->SetGlobalParam( GUID_PerfMasterVolume, 
									(void*)&nVolume, sizeof(long) );
		MuBG->SetRepeats( DMUS_SEG_REPEAT_INFINITE );
		SetTempo(0.5);//設定音樂一開始的tempo
		MuBG->Play( DMUS_SEGF_BEAT );
	}


	pBlock = new CRBlock;//此物件控制所有方塊的狀態
	memset(Key, 0, sizeof(Key));
	pDD->Clear(0);
	SetTimer(1, 15,NULL);//設定接收輸入狀態的Timer
	SetTimer(2, 30,NULL);//設定畫面更新的Timer
	return 0;
}

void canvasFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	pDD->UpdateBounds();
	// Do not call CFrameWnd::OnPaint() for painting messages
}

void canvasFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 1){	
		pDI->GetKeyboardState(sizeof(KB),(LPVOID)&KB);
		
		pDI->GetJoystickState(sizeof(JState), JState);

		if(KB[DIK_P] & 0x80 || JState.rgbButtons[5] & 0x80){
			KillTimer(1);
			KillTimer(2);
			if(Music)
				MuBG->Stop( DMUS_SEGF_BEAT );
			MessageBox(TEXT("遊戲暫停中!"), TEXT("暫停"),
			MB_ICONEXCLAMATION | MB_OK);
			SetTimer(1, 15, NULL);
			SetTimer(2, 30, NULL);
			if(Music)
				MuBG->Play(DMUS_SEGF_BEAT);
		}
		if(KB[DIK_RIGHT] & 0x80  ||KB[DIK_NUMPAD6]|| JState.lX >= 1)
			pBlock->SetStatus(BlockRight);
		if(KB[DIK_LEFT] & 0x80 ||KB[DIK_NUMPAD4] & 0x80|| JState.lX <= -1)
			pBlock->SetStatus(BlockLeft);
		if(KB[DIK_DOWN] & 0x80||KB[DIK_NUMPAD2] & 0x80|| JState.lY >= 1)
			pBlock->SetStatus(BlockSDown);
		if((KB[DIK_SPACE] & 0x80 || KB[DIK_UP] & 0x80 ||KB[DIK_NUMPAD8]|| 
			JState.rgbButtons[0] & 0x80)){
			if(!R){//必須使得user按下的狀態才執行
				R = true;
				Ro = true;
			}
		}
		else
			R = false;
	}
	if(nIDEvent == 2){		
		int i; 
		DWORD FpsCount = GetTickCount();
		CString str;
		if(pBlock->End){
			KillTimer(1);
			KillTimer(2);
			pDI->SetJSForceXY(SWidth, SHeight);
			if(Music)
				MuBG->Stop( DMUS_SEGF_BEAT );
			if(MessageBox(TEXT("你要再挑戰一次ㄇ？"), TEXT("認輸吧"),
				MB_ICONQUESTION  | MB_YESNO ) == 6){
				ResetGame();
				SetTimer(1, 15, NULL);
				SetTimer(2, 30, NULL);
			}
			else
				PostMessage(WM_CLOSE);
		}
		pDD->Blt(BlockSize, BlockSize, BGSurface, NULL);
		if(Ro){	
			Ro = false;
			if(pBlock->Rotate())
						WavRotate->Play(0, 0);
			else{
				WavCant->Play(0, 0);
				pDI->SetJSForceXY(SWidth/10, 0);
			}
		}
		if(WCount == 0){	
			if(!pBlock->MoveBlock(pDD, BKSurface)){		
				pDI->SetJSForceXY(0, SHeight/20);
				if((i = pBlock->CheckDead()) == 0){
						if(!pBlock->NewBlock(rand()%7,rand()%6))
							WavDanger->Stop();
						else
							WavDanger->Play(0,1);								
				}
				else{
					Score += BaseScore*Level*(i*i);
					WavDelete->Play(0, 0);
					Key[Level - 1] += i;
				}
				if(i == 0){
					WavKill->Play(0, 0);
					WCount = 20;
				}
			}
		}
		else
			WCount--;
		if(TCount >0){
			TCount--;
			pDD->ColorKeyBlt(SWidth/2 - 50, SHeight/2 - 100 , TextUp->GetDDrawSurface(),
		     NULL);
		}
		DrawOutside();
		BltWall();
		pDD->ColorKeyBlt(BlockSize, BlockSize , TextFps->GetDDrawSurface(), NULL);
		pDD->Present();
		FpsCount = GetTickCount() - FpsCount;
		if(FpsCount == 0)
			FpsCount = 1;
		str.Format("%0.1f FPS      ", (float)1000/FpsCount);
		TextFps->DrawText(NULL, str, 0, 0, RGB(0, 0, 0),RGB(255, 255, 0) );
	}
			
	CFrameWnd::OnTimer(nIDEvent);
}

HRESULT canvasFrame::InitHW()//初始化硬體設定
{	
	pDD = new CDisplay;//初始化DirectDraw
	if( FAILED( result = pDD->CreateWindowedDisplay(m_hWnd, SWidth+180, SHeight)
		) ){ 
        MessageBox(TEXT("CDisplay::設定顯示模式失敗! "),
                    TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
        return result;
    }
    pDS = new CSoundManager;//初始化DirectSound
	if(FAILED(result = pDS->Initialize(m_hWnd, DSSCL_PRIORITY, 2, 44100, 16))){
		MessageBox(TEXT("CSound::初始化DirectSound失敗! "),
                    TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
        return result;
	}
	if(FAILED(result = pDS->Get3DListenerInterface(&pSListener))){
		MessageBox(TEXT("CSound::建立傾聽者失敗! "),
                    TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
        return result;
	}
    DSListenerParams.dwSize = sizeof(DS3DLISTENER);
	pSListener->GetAllParameters(&DSListenerParams);

	HINSTANCE hinst = AfxGetInstanceHandle();//初始化DirectInput
	pDI = new CMyInputManager(hinst);
	if(pDI->CreateKeyBoard(m_hWnd, DISCL_BACKGROUND| DISCL_NONEXCLUSIVE) != DI_OK)
		MessageBox(TEXT("CMyInputManager::建立鍵盤輸入失敗! "),
                    TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
	if(FAILED(pDI->CreateJoystick(m_hWnd, DISCL_EXCLUSIVE|DISCL_BACKGROUND, 10, -10, 500)))
			MessageBox(TEXT("CMyInputManager::建立搖桿輸入失敗! "),
                    TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
 
	pDI->CreateJSForce(SWidth, SHeight, 150000) != DI_OK;//150000就是1.5秒(microseconds)

	pDM = new CMusicManager;//初始化DirectMusic
	if( FAILED( pDM->Initialize( m_hWnd ) ) )
		MessageBox(TEXT("CMusicManager::初始化失敗! "),
                    TEXT("DirectDrawWindowed"), MB_ICONERROR | MB_OK );
	IDirectMusicPerformance* pPerf = pDM->GetPerformance();
    GUID guid = GUID_NOTIFICATION_SEGMENT;
    pPerf->AddNotificationType( guid );
    pPerf->SetNotificationHandle( pDMEvent, 0 ); 
	return S_OK;
}

void canvasFrame::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnMoving(fwSide, pRect);
	// TODO: Add your message handler code here
	pDD->UpdateBounds();
}

void canvasFrame::FreeHW()//釋放硬體
{
 	CloseHandle( pDMEvent );
	SAFE_DELETE(MuBG);
	SAFE_DELETE(pDM);	
	SAFE_DELETE(pDI);
	SAFE_RELEASE(pS3DDanger);
	SAFE_DELETE(WavDanger);
	SAFE_RELEASE(pSListener);
	SAFE_RELEASE(pS3DWavRotate);
	SAFE_RELEASE(pS3DKill);
	SAFE_DELETE(WavKill);
	SAFE_DELETE(WavRotate);
	SAFE_RELEASE(pS3DCant);
	SAFE_DELETE(WavCant);
	SAFE_RELEASE(pS3DDelete);
	SAFE_DELETE(WavDelete);
	SAFE_RELEASE(pS3DUp);
	SAFE_DELETE(WavUp);
	SAFE_RELEASE(pS3DWin);
	SAFE_DELETE(WavWin);
	SAFE_DELETE(pDS);
	SAFE_DELETE(BGSurface);
	SAFE_DELETE(TextLevel);
	SAFE_DELETE(TextUp);
	SAFE_DELETE(TextFps);
	SAFE_DELETE(EDSurface);
	SAFE_DELETE(BKSurface);
	SAFE_DELETE(SDSurface);
	SAFE_DELETE(NUSurface);
	SAFE_DELETE(PicSpeedBar);
	SAFE_DELETE(pDD);

}




void canvasFrame::BltWall()//畫出磚塊
{
  for(int y = 0; y < SHeight; y += SHeight - BlockSize)
	for(int x = 0; x < SWidth; x +=BlockSize)
		pDD->Blt(x, y, EDSurface, NULL);
  for(int x = 0; x < SWidth; x +=SWidth - BlockSize)
	  for(y = BlockSize; y < SHeight - BlockSize; y += BlockSize)
		pDD->Blt(x, y, EDSurface, NULL);
}

//////////////////////////////////////////////////////////////////////
// CRBlock Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRBlock::CRBlock()
{
	Reset();
	
	memset(Blocks[StyleLine][0].Style, 0, sizeof(Blocks[StyleLine][0].Style));//先將所有方向磚塊的格式定義好
	Blocks[StyleLine][0].point = CPoint(1,0);
	Blocks[StyleLine][0].Style[1][0] = true;
	Blocks[StyleLine][0].Style[0][0] = true;
	Blocks[StyleLine][0].Style[2][0] = true;
	Blocks[StyleLine][0].Style[3][0] = true;	
	memset(Blocks[StyleLine][1].Style, 0, sizeof(Blocks[StyleLine][1].Style));
	Blocks[StyleLine][1].point = CPoint(0,1);
	Blocks[StyleLine][1].Style[0][0] = true;
	Blocks[StyleLine][1].Style[0][1] = true;
	Blocks[StyleLine][1].Style[0][2] = true;
	Blocks[StyleLine][1].Style[0][3] = true;
	
	memset(Blocks[StyleT][0].Style, 0, sizeof(Blocks[StyleT][0].Style));
	Blocks[StyleT][0].point = CPoint(1,0);
	Blocks[StyleT][0].Style[1][0] = true;
	Blocks[StyleT][0].Style[0][0] = true;
	Blocks[StyleT][0].Style[2][0] = true;
	Blocks[StyleT][0].Style[1][1] = true;
	memset(Blocks[StyleT][1].Style, 0, sizeof(Blocks[StyleT][1].Style));
	Blocks[StyleT][1].point = CPoint(1,1);
	Blocks[StyleT][1].Style[1][1] = true;
	Blocks[StyleT][1].Style[1][0] = true;
	Blocks[StyleT][1].Style[0][1] = true;
	Blocks[StyleT][1].Style[1][2] = true;
	memset(Blocks[StyleT][2].Style, 0, sizeof(Blocks[StyleT][2].Style));
	Blocks[StyleT][2].point = CPoint(1,1);
	Blocks[StyleT][2].Style[1][1] = true;
	Blocks[StyleT][2].Style[1][0] = true;
	Blocks[StyleT][2].Style[0][1] = true;
	Blocks[StyleT][2].Style[2][1] = true;
	memset(Blocks[StyleT][3].Style, 0, sizeof(Blocks[StyleT][3].Style));
	Blocks[StyleT][3].point = CPoint(0,1);
	Blocks[StyleT][3].Style[0][1] = true;
	Blocks[StyleT][3].Style[0][0] = true;
	Blocks[StyleT][3].Style[1][1] = true;
	Blocks[StyleT][3].Style[0][2] = true;
	
	memset(Blocks[StyleCube][0].Style, 0, sizeof(Blocks[StyleCube][0].Style));
	Blocks[StyleCube][0].point = CPoint(0,0);
	Blocks[StyleCube][0].Style[0][0] = true;
	Blocks[StyleCube][0].Style[1][0] = true;
	Blocks[StyleCube][0].Style[0][1] = true;
	Blocks[StyleCube][0].Style[1][1] = true;

	memset(Blocks[StyleLN][0].Style, 0, sizeof(Blocks[StyleLN][0].Style));
	Blocks[StyleLN][0].point = CPoint(1,0);
	Blocks[StyleLN][0].Style[1][0] = true;
	Blocks[StyleLN][0].Style[0][0] = true;
	Blocks[StyleLN][0].Style[1][1] = true;
	Blocks[StyleLN][0].Style[2][1] = true;
	memset(Blocks[StyleLN][1].Style, 0, sizeof(Blocks[StyleLN][1].Style));
	Blocks[StyleLN][1].point = CPoint(1,1);
	Blocks[StyleLN][1].Style[1][1] = true;
	Blocks[StyleLN][1].Style[1][0] = true;
	Blocks[StyleLN][1].Style[0][1] = true;
	Blocks[StyleLN][1].Style[0][2] = true;

	memset(Blocks[StyleRN][0].Style, 0, sizeof(Blocks[StyleRN][0].Style));
	Blocks[StyleRN][0].point = CPoint(1,0);
	Blocks[StyleRN][0].Style[1][0] = true;
	Blocks[StyleRN][0].Style[2][0] = true;
	Blocks[StyleRN][0].Style[1][1] = true;
	Blocks[StyleRN][0].Style[0][1] = true;
	memset(Blocks[StyleRN][1].Style, 0, sizeof(Blocks[StyleRN][1].Style));
	Blocks[StyleRN][1].point = CPoint(0,1);
	Blocks[StyleRN][1].Style[0][1] = true;
	Blocks[StyleRN][1].Style[0][0] = true;
	Blocks[StyleRN][1].Style[1][1] = true;
	Blocks[StyleRN][1].Style[1][2] = true;

	memset(Blocks[StyleLL][0].Style, 0, sizeof(Blocks[StyleLL][0].Style));
	Blocks[StyleLL][0].point = CPoint(1,1);
	Blocks[StyleLL][0].Style[0][1] = true;
	Blocks[StyleLL][0].Style[0][0] = true;
	Blocks[StyleLL][0].Style[1][1] = true;
	Blocks[StyleLL][0].Style[2][1] = true;
	memset(Blocks[StyleLL][1].Style, 0, sizeof(Blocks[StyleLL][1].Style));
	Blocks[StyleLL][1].point = CPoint(0,1);
	Blocks[StyleLL][1].Style[0][0] = true;
	Blocks[StyleLL][1].Style[1][0] = true;
	Blocks[StyleLL][1].Style[0][1] = true;
	Blocks[StyleLL][1].Style[0][2] = true;
	memset(Blocks[StyleLL][2].Style, 0, sizeof(Blocks[StyleLL][2].Style));
	Blocks[StyleLL][2].point = CPoint(1,0);
	Blocks[StyleLL][2].Style[2][0] = true;
	Blocks[StyleLL][2].Style[2][1] = true;
	Blocks[StyleLL][2].Style[0][0] = true;
	Blocks[StyleLL][2].Style[1][0] = true;
	memset(Blocks[StyleLL][3].Style, 0, sizeof(Blocks[StyleLL][3].Style));
	Blocks[StyleLL][3].point = CPoint(1,1);
	Blocks[StyleLL][3].Style[1][2] = true;
	Blocks[StyleLL][3].Style[0][2] = true;
	Blocks[StyleLL][3].Style[1][1] = true;
	Blocks[StyleLL][3].Style[1][0] = true;

	memset(Blocks[StyleRL][0].Style, 0, sizeof(Blocks[StyleRL][0].Style));
	Blocks[StyleRL][0].point = CPoint(1,1);
	Blocks[StyleRL][0].Style[2][1] = true;
	Blocks[StyleRL][0].Style[2][0] = true;
	Blocks[StyleRL][0].Style[1][1] = true;
	Blocks[StyleRL][0].Style[0][1] = true;
	memset(Blocks[StyleRL][1].Style, 0, sizeof(Blocks[StyleRL][1].Style));
	Blocks[StyleRL][1].point = CPoint(0,1);
	Blocks[StyleRL][1].Style[0][2] = true;
	Blocks[StyleRL][1].Style[1][2] = true;
	Blocks[StyleRL][1].Style[0][1] = true;
	Blocks[StyleRL][1].Style[0][0] = true;
	memset(Blocks[StyleRL][2].Style, 0, sizeof(Blocks[StyleRL][2].Style));
	Blocks[StyleRL][2].point = CPoint(1,0);
	Blocks[StyleRL][2].Style[0][0] = true;
	Blocks[StyleRL][2].Style[0][1] = true;
	Blocks[StyleRL][2].Style[1][0] = true;
	Blocks[StyleRL][2].Style[2][0] = true;
	memset(Blocks[StyleRL][3].Style, 0, sizeof(Blocks[StyleRL][3].Style));
	Blocks[StyleRL][3].point = CPoint(1,1);
	Blocks[StyleRL][3].Style[1][0] = true;
	Blocks[StyleRL][3].Style[0][0] = true;
	Blocks[StyleRL][3].Style[1][1] = true;
	Blocks[StyleRL][3].Style[1][2] = true;
}

CRBlock::~CRBlock()
{

}

bool CRBlock::NewBlock(int i, int j)
{
	BPoint = CPoint(0,0);
	NowStyle = NextStyle;
	NowColor = NextColor;
	NextStyle = i;
	NextColor = j;
	NowRotate = 0;
	NowStatus = BlockDown;
	BPoint = CPoint(BlockSize*5, Upper + BlockSize);
	MoveCount = 0;
	for( i = 4; i < 7; i++){
		for( j = 1; j < 2; j++)
			if(BArray[i][j] != 0){
               AllDead = true;
			   return true;
			}		
	}
	for( i = 0; i < 10; i++){
		for( j = 4; j > 0; j--)
			if(BArray[i][j] != 0){
               return true;
			}
	}
	return false;

				
}

HRESULT CRBlock::DrawBlock(CDisplay *pD, CSurface *Block, CPoint point,bool Now)//畫出現在移動中的方塊
{
	CRect rect;
	int NS, NR, Count = 0;
	
	if(Now){//必須確定畫出的方塊是使用中的,還是待命中的
		NS = NowStyle;
		NR = NowRotate;
		rect = CRect( BlockSize*NowColor, 0, BlockSize*(NowColor + 1), 30);
	}
	else{
		NS = NextStyle;
		NR = 0;
		rect = CRect( BlockSize*NextColor, 0, BlockSize*(NextColor + 1), 30);
	}

	for(int i = 0; i < StyleSize; i++){
		for(int j = 0; j < StyleSize; j++)
			if(Blocks[NS][NR].Style[i][j]){
				pD->Blt(point.x+(i - Blocks[NS][NR].point.x)*BlockSize, 
					point.y+(j - Blocks[NS][NR].point.y)*BlockSize, Block, rect);
				Count++;
			}
		if(Count >= 4)
			break;
	}
    DrawDead(pD, Block);
	return DD_OK;
}

bool CRBlock::Rotate()
{	
	if(NowStyle == StyleCube)
		return false;
	if(DetectHit(BPoint,AddNR(NowRotate)))
		NowRotate = AddNR(NowRotate);
	else
		return false;

	return true;
}

bool CRBlock::MoveBlock(CDisplay *pD, CSurface *Pic)//移動方塊
{
	if(BSpeed > HSpeed)
		DSpeed = HSpeed = BSpeed;
	if(AllDead){
		ShowLose();
		DrawDead(pD, Pic);
		return true;
	}
	if(LineFull){
		DeleteDead();
		DrawDead(pD, Pic);
		return true;
	}
	switch(NowStatus){//先確認方塊移動的方向
	 case BlockLeft:
		 if(MoveCount%BlockSize == 0){//只需在會發生碰撞的情況下預測
			 if(DetectHit(CPoint(BPoint.x - BlockSize, BPoint.y), NowRotate)){//在移動前都要先預測碰撞
				BPoint.x -= HSpeed;
				MoveCount += HSpeed;
			 }
		 }
		 else{
			 BPoint.x -= HSpeed;
			MoveCount += HSpeed;
		 }
		 break;
	 case BlockRight:
		 if(MoveCount%BlockSize == 0){
			 if(DetectHit(CPoint(BPoint.x + BlockSize, BPoint.y), NowRotate)){
				BPoint.x += HSpeed;
				MoveCount += HSpeed;
			 }
		 }
		 else{
			BPoint.x += HSpeed;
			MoveCount += HSpeed;
		 }
		 break;
	 case BlockSDown:
		 if(MoveCount%BlockSize == 0){
			if(DetectHit(CPoint(BPoint.x, BPoint.y + BlockSize), NowRotate, true)){
				BPoint.y += DSpeed;
				MoveCount += DSpeed;
			}
			else
				return false;
		 }
		 else{
			BPoint.y += DSpeed;
			MoveCount += DSpeed;
		 }
		 break;
	}
	if(MoveCount%BlockSize == 0){
		MoveCount = NowStatus = BlockDown;
		if(BPoint.y%BlockSize == 0 && NowStatus != BlockSDown){
			if(DetectHit(CPoint(BPoint.x, BPoint.y + BlockSize), NowRotate, true))
				BPoint.y += BSpeed;
			else
				return false;
		}
		else
			BPoint.y += BSpeed;
	}	
	DrawBlock(pD, Pic,BPoint);

	return true;
}

void CRBlock::SetStatus(int i)//設定方塊的移動方向
{
 if(NowStatus == BlockDown)//必須是再方塊向下移的狀態時才能改變方向
	 NowStatus = i;
	 
}

bool CRBlock::DetectHit(CPoint point, int NR, bool Merge)//偵測碰撞
{
	int Count = 0, LF = 0, RH = 0, LW = 0;
	if(point.x%BlockSize != 0)
		switch(NowStatus){
		case BlockRight:
			point.x += 30 -(point.x%BlockSize);
			break;
		case BlockLeft:
			point.x -= point.x%BlockSize;
	}
	if(point.y%BlockSize != 0)
		point.y += 30 -(point.y%BlockSize);
	for(int i = 0; i < StyleSize; i++){
		for(int j = 0; j < StyleSize; j++)
			if(Blocks[NowStyle][NR].Style[i][j]){
				Count++;
				if(point.x+(i - Blocks[NowStyle][NR].point.x)*BlockSize < Lefter ||
					point.x+(i - Blocks[NowStyle][NR].point.x)*BlockSize > Righter ||
                    point.y+(j - Blocks[NowStyle][NR].point.y)*BlockSize < Upper ||
					point.y+(j - Blocks[NowStyle][NR].point.y + LW)*BlockSize > Lower){
					
					if(Merge)
						KillBlock();	
					return false;
				}
			}
		if(Count >= 4)
			break;
	}
	Count = 0;	
	for( i = 0; i < StyleSize; i++){
		for(int j = 0; j < StyleSize; j++)
			if(Blocks[NowStyle][NR].Style[i][j]){
				Count++;
				if(BArray[(point.x - Lefter)/BlockSize + 
					(i - Blocks[NowStyle][NR].point.x)]
					[(point.y - Upper)/BlockSize + 
					(j - Blocks[NowStyle][NR].point.y)] 
					!= 0 
					){
					if(Merge)
						KillBlock();
					return false;
				}
			}                                                             			
		if(Count >= 4)
			break;
	}
	return true;
}

int CRBlock::AddNR(int NR)//將方塊轉向
{
  switch(NowStyle){//以方塊的類型決定其增加的值
		case StyleLine:
			return (NR +1)%2;
			break;
		case StyleCube:
			return 0;
			break;
		case StyleLN:
			return (NR +1)%2;
			break;
		case StyleRN:
			return (NR +1)%2;
			break;
		default:
			return (NR +1)%4;
	}
}

void CRBlock::DrawDead(CDisplay *pD, CSurface *Block)//畫出疊起來的方塊
{
	CRect rect;
    int Color;

	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 20; j++){
			if((Color = BArray[i][j]) > 0 ){
				rect = CRect( BlockSize*(Color-1), 0, BlockSize*Color, 30);
				pD->Blt(i*BlockSize + Lefter, j*BlockSize + Upper, Block,rect);
			}
		}
}

void CRBlock::KillBlock()//將使用中的方塊,使其定在其所在的位置
{
	int Count =0;
	if(BPoint.x%BlockSize != 0)
		BPoint.x += 30 -(BPoint.x%BlockSize);
	if(BPoint.y%BlockSize != 0)
		BPoint.y += 30 -(BPoint.y%BlockSize);
	for(int i = 0; i < StyleSize; i++){
		for(int j = 0; j < StyleSize; j++)
			if(Blocks[NowStyle][NowRotate].Style[i][j]){
				Count++;
				BArray[(BPoint.x - Lefter)/BlockSize + 
					(i - Blocks[NowStyle][NowRotate].point.x)]
					[(BPoint.y - Upper)/BlockSize + 
					(j - Blocks[NowStyle][NowRotate].point.y)] 
					 = NowColor + 1;
			}
		if(Count >= 4)
			break;
	}
}

int CRBlock::CheckDead()//檢查是否有方塊可以消去
{
	bool Full = false;
	int count  = 0;
	
	for(int j = 0; j< 20; j++){
		Full = true;
		for(int i = 0; i <10; i++){
			if(BArray[i][j] == 0){
				Full = false;
				break;
			}
		}
		if(Full){
			LineFull = DIndex[j] = true;
			count++;
		}
	}
	return count;
}
/*
	int **b;
	b = new int *[20];
	for(int i = 0; i < 20; i++)
		*b[i] = new int[10];

	for(i = 0; i < 20;i++)
		delete [] b[i];
	delete [] b;
  */
void CRBlock::DeleteDead(){//將滿的方塊消去
	if(TCount == 0)
		for(int j = 0; j < 20; j++)
			if(DIndex[j])
				for(int i = 0; i < 10; i++)
					BArray[i][j] = 0;
	TCount++;
	if(TCount > 15){
		LineFull = false;
		TCount = 0;
		for(int j = 19; j > 0; j--)
			if(DIndex[j])
				for(int i = j -1;i > 0;i--)
					if(!DIndex[i]){
						DIndex[i] = true;
						for(int k = 0 ; k<10; k++)
							BArray[k][j] = BArray[k][i];
						break;
					}
	NewBlock(rand()%7,rand()%6);
	memset(DIndex, 0, sizeof(DIndex));
	}
		
}

void canvasFrame::DrawOutside()//畫出方塊以外的圖形
{
	pDD->Blt(SWidth, BlockSize*8, BGSurface, CRect(0,0, BlockSize *4, BlockSize*3));
	pBlock->DrawBlock(pDD, BKSurface, CPoint(SWidth + 30, BlockSize*9), false);
	if(OldScore == Score)
		return;
	CRect rect;
	
   if(Score >= MaxScore){
			KillTimer(1);
			KillTimer(2);
			bool Cheat = false;
			for(int j = 0; j < 10; j++)
				if(Key[j] < 3){
					AfxMessageBox("作弊的肉腳，憑實力ㄅ!");
					Cheat = true;
					break;
				}
			if(!Cheat){
				WavWin->Play(0,1);
			    AfxMessageBox("你太~利害了!我認輸，現在休息一下ㄅ");
			}
			PostMessage(WM_CLOSE);
	}
	else if(Score >= Level10Score){
		Level = 10;
		pBlock->SetSpeed(Level10Speed);
	}
	else if(Score >= Level9Score){
		Level = 9;
		pBlock->SetSpeed(Level9Speed);
	}
	else if(Score >= Level8Score){
		Level = 8;
		pBlock->SetSpeed(Level8Speed);
	}
	else if(Score >= Level7Score){
		Level = 7;
		pBlock->SetSpeed(Level7Speed);
	}
	else if(Score >= Level6Score){
		Level = 6;
		pBlock->SetSpeed(Level6Speed);
	}
	else if(Score >= Level5Score){
		Level = 5;
		pBlock->SetSpeed(Level5Speed);
	}
	else if(Score >= Level4Score){
		Level = 4;
		pBlock->SetSpeed(Level4Speed);
	}
	else if(Score >= Level3Score){
		Level = 3;
		pBlock->SetSpeed(Level3Speed);
	}
	else if(Score >= Level2Score){
		Level = 2;
		pBlock->SetSpeed(Level2Speed);
	}
	
	int n1, n2, n3, n4, n5, n6;
	n6 = (Score-(Score%100000));
	n5 = (Score-(Score%10000))-n6  ;
	n4 = (Score-(Score%1000))-(n6 + n5) ;
	n3 = (Score-(Score%100))-(n6+n5+n4);
	n2 = (Score-(Score%10))-(n6+n5+n4+n3);
	n1 = Score-(n6+n5+n4+n3+n2)  ;

	if(n6 == 0)
		n6 = 1000000;
	if(n5 == 0)
		n5 = 100000;
	if(n4 == 0)
		n4 = 10000;
	if(n3 == 0)
		n3 = 1000;
	if(n2 == 0)
		n2 = 100;
	if(n1 == 0)
		n1 = 10;
	rect = CRect((n6/100000-1)*30, 0, n6/100000*30, 30);
	pDD->Blt(SWidth, 0, NUSurface, rect);
	rect = CRect((n5/10000-1)*30, 0, n5/10000*30, 30);
	pDD->Blt(SWidth+30, 0, NUSurface, rect);
	rect = CRect((n4/1000-1)*30, 0, n4/1000*30, 30);
	pDD->Blt(SWidth+30*2, 0, NUSurface, rect);
	rect = CRect((n3/100-1)*30, 0, n3/100*30, 30);
	pDD->Blt(SWidth+30*3, 0, NUSurface, rect);
	rect = CRect((n2/10-1)*30, 0, n2/10*30, 30);
	pDD->Blt(SWidth+30*4, 0, NUSurface, rect);
	rect = CRect((n1-1)*30, 0, n1*30, 30);
	pDD->Blt(SWidth+30*5, 0, NUSurface, rect);
    if(OldLevel != Level){
		TCount = 45;
		SetTempo((float)Level/10 + 0.4);
		WavUp->Play(0, 0);
	}
	char str[10];
	COLORREF Color;

	sprintf(str, "Level %00d  ", Level);
	if(Level <= 3)
		Color =  RGB(0, 255, 0);
	else if(Level <= 6)
		Color =  RGB(255, 255, 0);
	else
		Color =  RGB(255, 0, 0);
	TextLevel->DrawText(FoLevel,str, 0, 0, RGB(0, 0, 0), Color);
	pDD->Blt(SWidth+2, 535, TextLevel, NULL);
	pDD->ColorKeyBlt(SWidth, SHeight- 100 , PicSpeedBar->GetDDrawSurface(),
		CRect(Level*80, 0, (Level+1)*80, 100));
	OldLevel = Level;
	OldScore = Score;
}

void CRBlock::ShowLose()//失敗時,將方塊所有的顏色轉成灰色
{
	if(TCount > 19){
		TCount == 0;
		End = true;
		return;
	}
	for(int i = 0; i < 10;i++)
		if(BArray[i][TCount] != 0)
			BArray[i][TCount] = 7;
	TCount++;
}

void CRBlock::SetSpeed(int i)//設定方塊落下的速度(必須是30得因數)
{
	BSpeed = i;
}

void canvasFrame::ResetGame()//重設遊戲設定
{   if(Music)
		MuBG->Play( DMUS_SEGF_BEAT );
    SetTempo(0.5);
	Score = 0; 
	Level = 1; 
	OldScore = 1; 
	TCount = 0; 
	OldLevel = 1; 
	memset(Key, 0, sizeof(Key));
	WavDanger->Stop();
	pBlock->Reset();
}

void CRBlock::Reset()
{	
	HSpeed = BlockSize/3;
	DSpeed = 15;
	memset(BArray, 0, sizeof(BArray));
	NowStyle = rand()%7;
	NowColor = rand()%6;
	NextStyle = rand()%7;
	NextColor = rand()%6;
	NowRotate = 0;
	NowStatus = BlockDown;
	BPoint = CPoint(BlockSize*5, Upper + BlockSize);
	BSpeed = 1;
	MoveCount = 0;
	LineFull = false;
	TCount = 0;
	AllDead = false;
	End = false;
	memset(DIndex, 0, sizeof(DIndex));
}
/////////////////////////////////////////////////////////////////////////////
// CWelcome dialog


CWelcome::CWelcome(CWnd* pParent /*=NULL*/)
	: CDialog(CWelcome::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWelcome)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWelcome::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcome)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWelcome, CDialog)
	//{{AFX_MSG_MAP(CWelcome)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_JS, OnJs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcome message handlers



BOOL CWelcome::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CJS = (CButton*)GetDlgItem(IDC_JS);
	CFE = (CButton*)GetDlgItem(IDC_FE);
	CMusic = (CButton*)GetDlgItem(IDC_Music);
	
	CMusic->SetCheck(1);
	if(pDI->UseJoystick())
		CJS->SetCheck(1);
	else
		CJS->EnableWindow(false);

	if(pDI->UseJSForce())
		CFE->SetCheck(1);
	else
		CFE->EnableWindow(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWelcome::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CWelcome::OnOK() 
{
	// TODO: Add extra validation here
	if(CJS->GetCheck() == 0)
		pDI->DisableJoystick();
	if(CFE->GetCheck() == 0)
		pDI->DisableJSForce();
	if(CMusic->GetCheck() == 0)
		Music = false;
	
	CDialog::OnOK();
}

void CWelcome::OnJs() 
{
	// TODO: Add your control notification handler code here
	if(CJS->GetCheck() == 0){
		CFE->EnableWindow(false);
        CFE->SetCheck(0);
	}
	else
		if(pDI->UseJSForce()){
			CFE->EnableWindow(true);
			CFE->SetCheck(1);
		}
	
}

void canvasFrame::SetTempo(float i)//設定音樂Tempo
{
	if(!Music)
		return;
	IDirectMusicPerformance* pPerf = NULL;
	if( pDM )
        pPerf = pDM->GetPerformance();
	pPerf->SetGlobalParam( GUID_PerfMasterTempo, 
                                (void*)&i, sizeof(float) ); 
}
