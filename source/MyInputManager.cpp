// MyInputManager.cpp: implementation of the CMyInputManager class.
//
//////////////////////////////////////////////////////////////////////
#include <afxwin.h>
#include "math.h"
#include "dxutil.h"
#include "MyInputManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DI_FFNOMINALMAX             10000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LPDIRECTINPUT8 pDI = NULL;
LPDIRECTINPUTDEVICE8 pDKB = NULL, pDMO = NULL, pDJS = NULL;
LPDIRECTINPUTEFFECT pEffect = NULL;
EFFECTS_NODE* EffectList;

DWORD ZWidth, ZHeight;
DWORD NumForceFeedbackAxis = 0;
int JSPX, JSNX, JSDZ;


CMyInputManager::CMyInputManager(HINSTANCE hinst)
{
	if(FAILED(DirectInput8Create( hinst, DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&pDI, NULL ) ) )
		MessageBox(NULL,TEXT("DirectInput::建立DirectInput物件失敗! "),
                    TEXT("DirectXWindowed"), MB_ICONERROR | MB_OK );
	IsKeyboard =  IsJoystick = IsJSForce = IsMouse = false;

}

CMyInputManager::~CMyInputManager()
{
	if(IsJoystick)
		pDJS->Unacquire();
	if(IsMouse)
		pDMO->Unacquire();
	if(IsKeyboard)
		pDKB->Unacquire();		
	SAFE_RELEASE(pEffect);
	SAFE_RELEASE(pDJS);
	SAFE_RELEASE(pDMO);
	SAFE_RELEASE(pDKB);			
	SAFE_RELEASE(pDI);
}

HRESULT CMyInputManager::CreateKeyBoard(HWND m_hWnd,UINT dwFlags)
{
	HRESULT result;
	
	if(IsKeyboard){
		pDKB->Unacquire();
		IsKeyboard = false;
	}
	SAFE_RELEASE(pDKB);	

	if(result = pDI->CreateDevice(GUID_SysKeyboard, &pDKB, NULL)
		!= DI_OK)
		return result;
	
	if(result = pDKB->SetDataFormat(&c_dfDIKeyboard) != DI_OK)		
		return result;
	
	if(result = pDKB->SetCooperativeLevel(m_hWnd, dwFlags) != DI_OK)	
		return result;
	
	if(result = pDKB->Acquire() != DI_OK)
		return result;
	IsKeyboard = true;
	return DI_OK;
}

HRESULT CMyInputManager::CreateMouse(HWND m_hWnd,UINT dwFlags)
{
	HRESULT result;

	if(IsMouse){
		pDMO->Unacquire();
		IsMouse = false;
	}
	SAFE_RELEASE(pDMO);

	if(result = pDI->CreateDevice(GUID_SysMouse, &pDMO, NULL)
		!= DI_OK)
		return result;
	if(result = pDMO->SetDataFormat(&c_dfDIMouse2) != DI_OK)
		return result;
	
	if(result = pDMO->SetCooperativeLevel(m_hWnd, dwFlags) != DI_OK)
		return result;	
	if(result = pDMO->Acquire() != DI_OK)
		return result;
	IsMouse = true;
	return DI_OK;
}

HRESULT CMyInputManager::CreateJoystick(HWND m_hWnd,UINT dwFlags, int JP,int JN, int DZ)//建立搖桿
{
	HRESULT result;

	if(IsJoystick){
		pDJS->Unacquire();
		IsJoystick = false;
		IsJSForce = false;
	}
    SAFE_RELEASE(pDJS);

	JSPX = JP;
	JSNX = JN;
	JSDZ = DZ;
	if(result = pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticks, NULL, 
		DIEDFL_ATTACHEDONLY|DIEDFL_FORCEFEEDBACK) != DI_OK)
		return result;
	if(pDJS == NULL)
		return S_OK;
	else{
		
		if(result = pDJS->SetDataFormat(&c_dfDIJoystick2) != DI_OK)
			return result;

		if(result = pDJS->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE|DISCL_BACKGROUND ) != 
			DI_OK)
			return result;
		DIDEVCAPS JSCap;
		JSCap.dwSize = sizeof(JSCap);
		if(result = pDJS->GetCapabilities(&JSCap) != DI_OK)
			return result;
		if(result = pDJS->EnumObjects(EnumJSObjects, (VOID*)m_hWnd,DIDFT_ALL) != DI_OK)
			return result;
		if(result = pDJS->Acquire() != DI_OK)
			return result;		
	}
	IsJoystick = true;
	return DI_OK;
}

BOOL CALLBACK CMyInputManager::EnumJoysticks(const DIDEVICEINSTANCE *lpddi, VOID *pvRef)//列舉搖桿裝置
{
	if(pDI->CreateDevice(lpddi->guidInstance, &pDJS, NULL)
  		!= DI_OK)
  		return DIENUM_CONTINUE;
  	else
  		return DIENUM_STOP;
}

BOOL CALLBACK CMyInputManager::EnumJSObjects(const DIDEVICEOBJECTINSTANCE *lpddoi, VOID *pvRef)//列舉搖桿元件
{
	DIPROPDWORD du;
 	
 	du.diph.dwSize = sizeof(DIPROPDWORD);
 	du.diph.dwHeaderSize = sizeof(DIPROPHEADER);
 	du.diph.dwHow = DIPH_BYOFFSET;
 	
 	if( lpddoi->dwType & DIDFT_AXIS )
     {
         DIPROPRANGE diprg; 
         diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
         diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
         diprg.diph.dwHow        = DIPH_BYID; 
         diprg.diph.dwObj        = lpddoi->dwType; // Specify the enumerated axis
         diprg.lMin              = JSNX; 
         diprg.lMax              = JSPX; 
     
         // Set the range for the axis
         if( FAILED( pDJS->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
             return DIENUM_STOP;
          
     }
 	if( (lpddoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
         NumForceFeedbackAxis++;
 	switch(lpddoi->dwOfs){
 	case DIJOFS_X:
 			du.diph.dwObj = DIJOFS_X;
 			du.dwData = JSDZ;
 			if(pDJS->SetProperty(DIPROP_DEADZONE, &du.diph) != DI_OK)
 				AfxMessageBox("DirectInput::設定搖桿X軸無效範圍失敗! ");
 			break;
 	case DIJOFS_Y:
 			du.diph.dwObj = DIJOFS_Y;
 			du.dwData = JSDZ;
 			if(pDJS->SetProperty(DIPROP_DEADZONE, &du.diph) != DI_OK)
 				AfxMessageBox("DirectInput::設定搖桿Z軸無效範圍失敗! ");
 			break;
 	case DIJOFS_Z:
 
 		break;
 	case DIJOFS_RX:
 
 		break;
 	case DIJOFS_RY:
 
 		break;
 	case DIJOFS_RZ:
 
 		break;
 	case DIJOFS_SLIDER(0):
 
 		break;
 	case DIJOFS_SLIDER(1):
 
 		break;
 	}
 	return DIENUM_CONTINUE;
}


HRESULT CMyInputManager::SetJSForceXY(int x, int y)//利用XY值來啟動力回饋
{
	if(!IsJSForce)
		return DI_NOTATTACHED;
	int XForce , YForce;
 	LONG rglDirection[2] = { 0, 0 };
 
     DICONSTANTFORCE cf;
 
     XForce = MulDiv( x, 2 * DI_FFNOMINALMAX, ZWidth )
                  - DI_FFNOMINALMAX;
     // Keep force within bounds
     if( XForce < -DI_FFNOMINALMAX ) 
         XForce = -DI_FFNOMINALMAX;
     if( XForce > +DI_FFNOMINALMAX ) 
         XForce = +DI_FFNOMINALMAX;
 	YForce = MulDiv( y, 2 * DI_FFNOMINALMAX, ZHeight )
                  - DI_FFNOMINALMAX;
     if( YForce < -DI_FFNOMINALMAX ) 
         YForce = -DI_FFNOMINALMAX;
     if( YForce > +DI_FFNOMINALMAX ) 
         YForce = +DI_FFNOMINALMAX;
 
 	if( NumForceFeedbackAxis == 1 )
     {
         // If only one force feedback axis, then apply only one direction and 
         // keep the direction at zero
         cf.lMagnitude = XForce;
         rglDirection[0] = 0;
     }
     else
     {
         // If two force feedback axis, then apply magnitude from both directions 
         rglDirection[0] = XForce;
         rglDirection[1] = YForce;
         cf.lMagnitude = (DWORD)sqrt( (double)XForce * (double)XForce +
                                      (double)YForce * (double)YForce );
     }
 
     DIEFFECT eff;
     ZeroMemory( &eff, sizeof(eff) );
     eff.dwSize                = sizeof(DIEFFECT);
     eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
     eff.cAxes                 = NumForceFeedbackAxis;
     eff.rglDirection          = rglDirection;
     eff.lpEnvelope            = 0;
	 eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
     eff.lpvTypeSpecificParams = &cf;
     eff.dwStartDelay            = 0;
     // Now set the new parameters and start the effect immediately.
     return pEffect->SetParameters( &eff, DIEP_DIRECTION |
                                            DIEP_TYPESPECIFICPARAMS |
                                            DIEP_START );
}

void CMyInputManager::EmptyEffectList(EFFECTS_NODE *EF)//將load from file的力回饋資料從記憶體中Free
{
	EffectList = EF;
	EFFECTS_NODE* pEffectNode = EffectList->pNext;
    EFFECTS_NODE* pEffectDelete;
 
     while ( pEffectNode != EffectList )
     {
         pEffectDelete = pEffectNode;       
         pEffectNode = pEffectNode->pNext;
 
        SAFE_RELEASE( pEffectDelete->pDIEffect );
        SAFE_DELETE( pEffectDelete );
     }
 
     EffectList->pNext = EffectList;
}

BOOL CALLBACK CMyInputManager::EnumAndCreateEffectsCallback(LPCDIFILEEFFECT pDIFileEffect, 
															VOID *pvRef)
{
		LPDIRECTINPUTEFFECT pDIEffect = NULL;
 
     // Create the file effect
     if( FAILED(pDJS->CreateEffect( pDIFileEffect->GuidEffect, 
                                                 pDIFileEffect->lpDiEffect, 
                                                 &pDIEffect, NULL ) ) )
     {
         AfxMessageBox("DirectInput::無法在此裝置建立力回饋!");
         return DIENUM_CONTINUE;
     }
 
     // Create a new effect node
     EFFECTS_NODE* pEffectNode = new EFFECTS_NODE;
     if( NULL == pEffectNode )
         return DIENUM_STOP;
 
     // Fill the pEffectNode up
     ZeroMemory( pEffectNode, sizeof( EFFECTS_NODE ) );
     pEffectNode->pDIEffect         = pDIEffect;
     pEffectNode->dwPlayRepeatCount = 1;
 
     // Add pEffectNode to the circular linked list, g_EffectsList
     pEffectNode->pNext  = EffectList->pNext;
     EffectList->pNext = pEffectNode;
 
     return DIENUM_CONTINUE;
}

HRESULT CMyInputManager::OnPlayEffects(EFFECTS_NODE *EF)
{
	if(!IsJSForce)
		return DI_NOTATTACHED;
	EffectList = EF; 
	EFFECTS_NODE*       pEffectNode = EffectList->pNext;
     LPDIRECTINPUTEFFECT pDIEffect   = NULL;
     HRESULT hr;
     // Stop all previous forces
     if( FAILED( hr = pDJS->SendForceFeedbackCommand( DISFFC_STOPALL ) ) )
         return hr;
 
     while ( pEffectNode != EffectList )
     {
         // Play all of the effects enumerated in the file 
         pDIEffect = pEffectNode->pDIEffect;
 
        if( NULL != pDIEffect )
         {
             if( FAILED( hr = pDIEffect->Start( pEffectNode->dwPlayRepeatCount, 0 ) ) )
                 return hr;
         }
 
         pEffectNode = pEffectNode->pNext;
     }
 
     return S_OK;
}

HRESULT CMyInputManager::CreateJSForce(DWORD SW, DWORD SH, DWORD Time)//建立力回饋
{
	if(!IsJoystick)
		return DI_NOEFFECT;
	HRESULT result;
	
	SAFE_RELEASE(pEffect);
	IsJSForce = false;
		
	ZWidth = SW;
	ZHeight = SH;
	// This application needs only one effect: Applying raw forces.
		DWORD           rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };
		LONG            rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf              = { 0 };

		if(NumForceFeedbackAxis > 2 )
			NumForceFeedbackAxis = 2;
	
		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                  = sizeof(DIEFFECT);
		eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration              = Time;
		eff.dwSamplePeriod          = 0;
		eff.dwGain                  = DI_FFNOMINALMAX;
		eff.dwTriggerButton         = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes                   = NumForceFeedbackAxis;
		eff.rgdwAxes                = rgdwAxes;
		eff.rglDirection            = rglDirection;
		eff.lpEnvelope              = 0;
		eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams   = &cf;
		eff.dwStartDelay            = 0;

		// Create the prepared effect
		if( FAILED(result = pDJS->CreateEffect( GUID_ConstantForce, 
												  &eff, &pEffect, NULL ) ) )
			return result;

		if( NULL == pEffect )
			return S_FALSE;

		pEffect->Start(1, 0);
		IsJSForce = true;
		return DI_OK;
}

HRESULT CMyInputManager::GetKeyboardState(UINT Size, void *State)//取的鍵盤的輸入狀態
{  
	if(IsKeyboard)
		return pDKB->GetDeviceState(Size, State);
	return DI_NOTATTACHED; 

}

HRESULT CMyInputManager::GetMouseState(UINT Size, DIMOUSESTATE2 &State)//取得滑鼠的輸入狀態
{
	if(IsMouse)//在取得之前必須確定此裝置已被建立
		return pDMO->GetDeviceState(Size, (LPVOID) &State);
	return DI_NOTATTACHED; 
}

HRESULT CMyInputManager::GetJoystickState(UINT Size, DIJOYSTATE2 &State)//取得搖桿的輸入狀態
{
	if(IsJoystick){
		pDJS->Poll();
		return pDJS->GetDeviceState(Size, (LPVOID) &State);
	}
	return DI_NOTATTACHED ;
}

HRESULT CMyInputManager::LoadEffect(char str[], EFFECTS_NODE &EF)//從檔案Load力回饋資料
{
	if(IsJSForce){
		EffectList = &EF;
		ZeroMemory( EffectList, sizeof( EFFECTS_NODE ) );
		EffectList->pNext = EffectList;
 		return pDJS->EnumEffectsInFile( str, EnumAndCreateEffectsCallback, 
														  NULL, DIFEF_MODIFYIFNEEDED );
	}
	return DI_NOTATTACHED; 
}

bool CMyInputManager::UseJoystick() const//取得搖桿是否被建立的狀態
{
	return IsJoystick;
}

bool CMyInputManager::UseJSForce() const//取得力回饋是否被建立的狀態
{
	return IsJSForce;
}

bool CMyInputManager::UseKeyboard() const//取得鍵盤是否被建立的狀態
{
	return IsKeyboard;
}

void CMyInputManager::DisableJoystick() //取消搖桿裝置
{
	if(IsJoystick){
		pDJS->Unacquire();
		IsJoystick = false;
		IsJSForce = false;
	}
}

void CMyInputManager::DisableJSForce()//取消力回饋裝置
{
	IsJSForce = false;
}

bool CMyInputManager::UseMouse() const//取得滑鼠是否被建立的狀態
{
	return IsMouse;
}

void CMyInputManager::DisableKeyboard()
{
	if(IsKeyboard){
		pDKB->Unacquire();
		IsKeyboard = false;
	}
}

void CMyInputManager::DisableMouse()
{
	if(IsMouse){
		pDMO->Unacquire();
		IsMouse = false;
	}
}
