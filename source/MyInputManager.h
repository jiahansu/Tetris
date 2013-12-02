// MyInputManager.h: interface for the CMyInputManager class.
//將DirectInput8包裝的函式庫
//////////////////////////////////////////////////////////////////////
#include <dinput.h>

#if !defined(AFX_MYINPUTMANAGER_H__51AF9A7A_93F2_4A09_BB6F_FD5FBB65F121__INCLUDED_)
#define AFX_MYINPUTMANAGER_H__51AF9A7A_93F2_4A09_BB6F_FD5FBB65F121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct EFFECTS_NODE//定義load from file的力回饋資料結構
{
    LPDIRECTINPUTEFFECT pDIEffect;
    DWORD               dwPlayRepeatCount;
    EFFECTS_NODE*       pNext;
};
class CMyInputManager  
{
public:
	void DisableMouse();
	void DisableKeyboard();
	bool UseMouse() const;
	void DisableJSForce();
	void DisableJoystick();
	bool UseKeyboard() const;
	bool UseJSForce() const;
	bool UseJoystick() const; 
	HRESULT LoadEffect(char str[],EFFECTS_NODE& EF);
	HRESULT GetJoystickState(UINT Size, DIJOYSTATE2 &State);
	HRESULT GetMouseState(UINT Size,DIMOUSESTATE2 &State);
	HRESULT GetKeyboardState(UINT Size,void* State);
	HRESULT OnPlayEffects(EFFECTS_NODE *EF);
	HRESULT CreateJSForce(DWORD SW, DWORD SH, DWORD Time);
	static BOOL CALLBACK EnumAndCreateEffectsCallback(LPCDIFILEEFFECT pDIFileEffect, VOID *pvRef);
	void EmptyEffectList(EFFECTS_NODE *EF);
	HRESULT SetJSForceXY(int x, int y);
	static BOOL CALLBACK EnumJSObjects(const DIDEVICEOBJECTINSTANCE* lpddoi, VOID* pvRef);
	static BOOL CALLBACK EnumJoysticks(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);
	HRESULT CreateJoystick(HWND m_hWnd,UINT dwFlags, int JP,int JN, int DZ);
	HRESULT CreateMouse(HWND m_hWnd,UINT dwFlags);
	HRESULT CreateKeyBoard(HWND m_hWnd,UINT dwFlags);
	CMyInputManager(HINSTANCE hinst);
	virtual ~CMyInputManager();

private://以下的變數是用來紀錄裝置是否被建立
	bool IsKeyboard, IsJoystick, IsJSForce,IsMouse;
};

#endif // !defined(AFX_MYINPUTMANAGER_H__51AF9A7A_93F2_4A09_BB6F_FD5FBB65F121__INCLUDED_)
