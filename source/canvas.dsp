# Microsoft Developer Studio Project File - Name="canvas" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=canvas - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "canvas.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "canvas.mak" CFG="canvas - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "canvas - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "canvas - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "canvas - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib dxguid.lib dxerr8.lib ddraw.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib Dsound.lib Dinput8.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "canvas - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib dxerr8.lib ddraw.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib Dsound.lib Dinput8.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "canvas - Win32 Release"
# Name "canvas - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\canvasFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\canvasr.rc
# End Source File
# Begin Source File

SOURCE=.\cavasApp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\canvasFrame.h
# End Source File
# Begin Source File

SOURCE=.\cavasApp.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Pic\bground.BMP
# End Source File
# Begin Source File

SOURCE=.\Pic\Block.bmp
# End Source File
# Begin Source File

SOURCE=.\Sound\CANSEL.WAV
# End Source File
# Begin Source File

SOURCE=.\Sound\Cant.wav
# End Source File
# Begin Source File

SOURCE=.\Sound\Danger.wav
# End Source File
# Begin Source File

SOURCE=.\Sound\Delete.wav
# End Source File
# Begin Source File

SOURCE=.\Pic\directx.ico
# End Source File
# Begin Source File

SOURCE=.\Pic\edge.bmp
# End Source File
# Begin Source File

SOURCE=.\Pic\ICON.ico
# End Source File
# Begin Source File

SOURCE=.\Music\music.mid
# End Source File
# Begin Source File

SOURCE=.\Pic\Number1.bmp
# End Source File
# Begin Source File

SOURCE=.\Pic\OK.WAV
# End Source File
# Begin Source File

SOURCE=.\Sound\OK.WAV
# End Source File
# Begin Source File

SOURCE=".\Pic\Resize of 2.bmp"
# End Source File
# Begin Source File

SOURCE=.\Pic\SpeedBars.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\Sound\Up.wav
# End Source File
# Begin Source File

SOURCE=.\Sound\Win.wav
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\ddutil.h
# End Source File
# Begin Source File

SOURCE=.\dmutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dmutil.h
# End Source File
# Begin Source File

SOURCE=.\dsutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dsutil.h
# End Source File
# Begin Source File

SOURCE=.\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxutil.h
# End Source File
# Begin Source File

SOURCE=.\MyInputManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MyInputManager.h
# End Source File
# End Group
# End Target
# End Project
