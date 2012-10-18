# Microsoft Developer Studio Project File - Name="sample8" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sample8 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sample8.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sample8.mak" CFG="sample8 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sample8 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sample8 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sample8 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../"
# PROP Intermediate_Dir "tmp/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "../../include" /I "../../include/math" /I "../../common/include" /I "../../common/lua/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"tmp/Release/sample8.pch" /YX /Fo"tmp/Release/" /Fd"tmp/Release/" /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "../../include" /I "../../include/math" /I "../../common/include" /I "../../common/lua/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"tmp/sample8.bsc"
# ADD BSC32 /nologo /o"tmp/release/sample8.bsc"
LINK32=link.exe
# ADD BASE LINK32 tokamak.lib d3dengine.lib demoframework.lib ne_base.lib ne_pattern.lib lua.lib lualib.lib tolua_lib.lib d3dx8.lib d3d8.lib winmm.lib d3dxof.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:"../sample8.pdb" /machine:I386 /out:"../sample8.exe" /libpath:"../../lib" /libpath:"../../common/lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 tokamak.lib d3dengine.lib demoframework.lib ne_base.lib ne_pattern.lib lua.lib lualib.lib tolua_lib.lib d3dx8.lib d3d8.lib winmm.lib d3dxof.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /libpath:"../../lib" /libpath:"../../common/lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "sample8 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../"
# PROP Intermediate_Dir "tmp/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "../../include" /I "../../include/math" /I "../../common/include" /I "../../common/lua/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"tmp/Debug/sample8.pch" /YX /Fo"tmp/Debug/" /Fd"tmp/Debug/" /FD /GZ /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "../../include" /I "../../include/math" /I "../../common/include" /I "../../common/lua/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"tmp/sample8_d.bsc"
# ADD BSC32 /nologo /o"tmp/debug/sample8.bsc"
LINK32=link.exe
# ADD BASE LINK32 tokamak_d.lib d3dengine_d.lib demoframework_d.lib ne_base_d.lib ne_pattern_d.lib lua.lib lualib.lib tolua_lib.lib d3dx8.lib d3d8.lib winmm.lib d3dxof.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:"../sample8_d.pdb" /debug /machine:I386 /out:"../sample8_d.exe" /pdbtype:sept /libpath:"../../lib" /libpath:"../../common/lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ne_base_d.lib tokamak_d.lib d3dengine_d.lib demoframework_d.lib ne_pattern_d.lib lua.lib lualib.lib tolua_lib.lib d3dx8.lib d3d8.lib winmm.lib d3dxof.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../sample8_d.exe" /pdbtype:sept /libpath:"../../lib" /libpath:"../../common/lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "sample8 - Win32 Release"
# Name "sample8 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sample8.cpp
# End Source File
# Begin Source File

SOURCE=.\sample8.h
# End Source File
# Begin Source File

SOURCE=..\samplebase.cpp
# End Source File
# Begin Source File

SOURCE=..\samplebase.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
