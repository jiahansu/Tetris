#if !defined(AFX_CANVASFRAME_H__D3BE92EB_1F56_44D3_8525_6746383A5C7A__INCLUDED_)
#define AFX_CANVASFRAME_H__D3BE92EB_1F56_44D3_8525_6746383A5C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// canvasFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// canvasFrame frame 
class canvasFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(canvasFrame)
public:
	canvasFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void SetTempo(float i);
	void ResetGame();
	void DrawOutside();
	void BltWall();
	void FreeHW();
	HRESULT InitHW();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(canvasFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~canvasFrame();

	// Generated message map functions
	//{{AFX_MSG(canvasFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#define StyleLine		0
#define StyleT			1
#define StyleCube		2
#define StyleLN			3
#define StyleRN			4
#define StyleLL			5
#define StyleRL			6

#define Upper			BlockSize
#define Lower			BlockSize*20
#define Lefter          BlockSize
#define Righter         BlockSize*10
#define StyleSize       4

class CRBlock  
{
	struct RBlock
	{
		CPoint point;
		bool Style[StyleSize][StyleSize];
	};
public:
	int HSpeed, DSpeed;
	void Reset();
	void SetSpeed(int i);
	bool End;
	void ShowLose();
	bool AllDead;
	bool DIndex[20];
	int TCount;
	bool LineFull;
	void DeleteDead();
	int CheckDead();
	void KillBlock();
	void DrawDead(CDisplay *pD, CSurface *Block);
	int AddNR(int NR);
	RBlock Blocks[7][4];
	bool DetectHit(CPoint point, int NR,bool Merge = false);
	void SetStatus(int i);
	int BSpeed, MoveCount;
	bool MoveBlock(CDisplay *pD, CSurface *Block);
	int NowStatus;
	bool Rotate();
	int NowRotate;
	HRESULT DrawBlock(CDisplay* pD, CSurface* Block, CPoint point,bool Now = true);
	bool NewBlock(int i, int j);
	int NowStyle, NowColor, NextStyle, NextColor;
	CPoint BPoint;
	int BArray[10][20];
	CRBlock();
	virtual ~CRBlock();

};

/////////////////////////////////////////////////////////////////////////////
// CWelcome dialog

class CWelcome : public CDialog
{
// Construction
public:
	CButton* CJS, *CFE, *CMusic;
	CWelcome(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWelcome)
	enum { IDD = IDD_Welcome };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWelcome)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWelcome)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	virtual void OnOK();
	afx_msg void OnJs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANVASFRAME_H__D3BE92EB_1F56_44D3_8525_6746383A5C7A__INCLUDED_)
