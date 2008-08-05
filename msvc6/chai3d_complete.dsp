# Microsoft Developer Studio Project File - Name="chai3d_complete" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=chai3d_complete - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chai3d_complete.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chai3d_complete.mak" CFG="chai3d_complete - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chai3d_complete - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "chai3d_complete - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chai3d_complete - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /Ob2 /I "../src/graphics" /I "../src/collisions" /I "../src/files" /I "../src/tools" /I "../src/devices" /I "../src/display" /I "../src/forces" /I "../src/math" /I "../src/timers" /I "../src/scenegraph" /I "../src/widgets" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_MSVC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/msvc6/chai3d_complete.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=../scripts/copyHeaderFiles.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "chai3d_complete - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "chai3d_complete___Win32_Debug"
# PROP BASE Intermediate_Dir "chai3d_complete___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "chai3d_complete___Win32_Debug"
# PROP Intermediate_Dir "chai3d_complete___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../src/graphics" /I "../src/collisions" /I "../src/files" /I "../src/tools" /I "../src/devices" /I "../src/display" /I "../src/forces" /I "../src/math" /I "../src/timers" /I "../src/scenegraph" /I "../src/widgets" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_MSVC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/msvc6/chai3d_complete.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=../scripts/copyHeaderFiles.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "chai3d_complete - Win32 Release"
# Name "chai3d_complete - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\widgets\CBitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionAABB.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionAABBBox.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionAABBTree.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionBrute.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionSpheres.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionSpheresGeometry.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CColor.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\tools\CDelta3dofPointer.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CDeltaDevices.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CDraw3D.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CDriverSensoray626.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CDriverServotogo.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoader3DS.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoaderBMP.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoaderOBJ.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoaderTGA.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CFont.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tools\CFreedom6S3dofPointer.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CFreedom6SDevice.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\tools\CGeneric3dofPointer.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CGenericCollision.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CGenericDevice.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CGenericObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\forces\CGenericPointForceAlgo.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CGenericPotentialField.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tools\CGenericTool.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\files\CImageLoader.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CLabelPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CLight.cpp
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CMacrosGL.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CMaterial.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\math\CMaths.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\src\files\CMeshLoader.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\tools\CMeta3dofPointer.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tools\CPhantom3dofPointer.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CPhantomDevices.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\forces\CPotentialFieldForceAlgo.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\timers\CPrecisionClock.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\timers\CPrecisionTimer.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\forces\CProxyPointForceAlgo.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CShaders.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CShapeSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CShapeTorus.cpp
# End Source File
# Begin Source File

SOURCE=..\src\math\CString.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CTexture2D.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CTriangle.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CVBOMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CVertex.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\display\CViewport.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\devices\CVirtualDevice.cpp
# ADD CPP /I "../include/external"
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CWorld.cpp
# End Source File
# Begin Source File

SOURCE=..\src\devices\WIN626.C
# ADD CPP /I "../include/external"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\devices\App626.h
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CBitmap.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CCamera.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionAABB.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionAABBBox.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionAABBTree.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionBrute.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionSpheres.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CCollisionSpheresGeometry.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CColor.h
# End Source File
# Begin Source File

SOURCE=..\src\math\CConstants.h
# End Source File
# Begin Source File

SOURCE=..\src\tools\CDelta3dofPointer.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CDeltaDevices.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CDraw3D.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CDriverSensoray626.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CDriverServotogo.h
# End Source File
# Begin Source File

SOURCE=..\src\timers\celapsed.h
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoader3DS.h
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoaderBMP.h
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoaderOBJ.h
# End Source File
# Begin Source File

SOURCE=..\src\files\CFileLoaderTGA.h
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CFont.h
# End Source File
# Begin Source File

SOURCE=..\src\tools\CFreedom6S3dofPointer.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CFreedom6SDevice.h
# End Source File
# Begin Source File

SOURCE=..\src\tools\CGeneric3dofPointer.h
# End Source File
# Begin Source File

SOURCE=..\src\collisions\CGenericCollision.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CGenericDevice.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CGenericObject.h
# End Source File
# Begin Source File

SOURCE=..\src\forces\CGenericPointForceAlgo.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CGenericPotentialField.h
# End Source File
# Begin Source File

SOURCE=..\src\tools\CGenericTool.h
# End Source File
# Begin Source File

SOURCE=..\src\files\CImageLoader.h
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CLabelPanel.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CLight.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CMacrosGL.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CMaterial.h
# End Source File
# Begin Source File

SOURCE=..\src\math\CMaths.h
# End Source File
# Begin Source File

SOURCE=..\src\math\CMatrix3d.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CMesh.h
# End Source File
# Begin Source File

SOURCE=..\src\files\CMeshLoader.h
# End Source File
# Begin Source File

SOURCE=..\src\tools\CMeta3dofPointer.h
# End Source File
# Begin Source File

SOURCE=..\src\widgets\CPanel.h
# End Source File
# Begin Source File

SOURCE=..\src\tools\CPhantom3dofPointer.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CPhantomDevices.h
# End Source File
# Begin Source File

SOURCE=..\src\forces\CPotentialFieldForceAlgo.h
# End Source File
# Begin Source File

SOURCE=..\src\timers\CPrecisionClock.h
# End Source File
# Begin Source File

SOURCE=..\src\timers\CPrecisionTimer.h
# End Source File
# Begin Source File

SOURCE=..\src\forces\CProxyPointForceAlgo.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CShaders.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CShapeSphere.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CShapeTorus.h
# End Source File
# Begin Source File

SOURCE=..\src\math\CString.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CTexture2D.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CTriangle.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CVBOMesh.h
# End Source File
# Begin Source File

SOURCE=..\src\math\CVector3d.h
# End Source File
# Begin Source File

SOURCE=..\src\graphics\CVertex.h
# End Source File
# Begin Source File

SOURCE=..\src\display\CViewport.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\CVirtualDevice.h
# End Source File
# Begin Source File

SOURCE=..\src\scenegraph\CWorld.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\dhdc.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\DLPORTIO.H
# End Source File
# Begin Source File

SOURCE=..\src\devices\phantomDLL.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\PhantomDriver.h
# End Source File
# Begin Source File

SOURCE=..\src\devices\Win626.h
# End Source File
# End Group
# End Target
# End Project
