# Microsoft Developer Studio Project File - Name="implicit_functions" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=implicit_functions - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "implicit_functions.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "implicit_functions.mak" CFG="implicit_functions - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "implicit_functions - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "implicit_functions - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "implicit_functions - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../include" /I "../../../external/OpenGL/msvc6" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MSVC" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib opengl32.lib chai3d_complete.lib glu32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"msvcprt.lib libc.lib msvcp60.lib" /out:"../../../bin/implicit_functions_msvc6.exe" /libpath:"../../../lib/msvc6" /libpath:"../../../external/OpenGL/msvc6"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "implicit_functions - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../include" /I "../../../external/OpenGL/msvc6" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_MSVC" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib opengl32.lib chai3d_complete.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../bin/implicit_functions_msvc6.exe" /pdbtype:sept /libpath:"../../../lib/msvc6" /libpath:"../../../external/OpenGL/msvc6"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "implicit_functions - Win32 Release"
# Name "implicit_functions - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CImplicitCone.cpp
# End Source File
# Begin Source File

SOURCE=.\CImplicitCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\CImplicitShape.cpp
# End Source File
# Begin Source File

SOURCE=.\CImplicitSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\CImplicitTorus.cpp
# End Source File
# Begin Source File

SOURCE=.\implicit_functions.cpp
# End Source File
# Begin Source File

SOURCE=.\implicit_functions.rc
# End Source File
# Begin Source File

SOURCE=.\implicit_functionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CImplicitCone.h
# End Source File
# Begin Source File

SOURCE=.\CImplicitCylinder.h
# End Source File
# Begin Source File

SOURCE=.\CImplicitShape.h
# End Source File
# Begin Source File

SOURCE=.\CImplicitSphere.h
# End Source File
# Begin Source File

SOURCE=.\CImplicitTorus.h
# End Source File
# Begin Source File

SOURCE=.\implicit_functions.h
# End Source File
# Begin Source File

SOURCE=.\implicit_functions_globals.h
# End Source File
# Begin Source File

SOURCE=.\implicit_functionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\implicit_functions.ico
# End Source File
# Begin Source File

SOURCE=.\res\implicit_functions.rc2
# End Source File
# End Group
# End Target
# End Project
