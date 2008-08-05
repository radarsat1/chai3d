; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=Cforce_shadingDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "force_shading.h"
LastPage=0

ClassCount=3
Class1=Cforce_shadingApp
Class2=Cforce_shadingDlg
Class3=COpenGL

ResourceCount=1
Resource1=IDD_force_shading_DIALOG

[CLS:Cforce_shadingApp]
Type=0
BaseClass=CWinApp
HeaderFile=force_shading.h
ImplementationFile=force_shading.cpp

[CLS:Cforce_shadingDlg]
Type=0
BaseClass=CDialog
HeaderFile=force_shadingDlg.h
ImplementationFile=force_shadingDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_CAMZOOM_SLIDER

[CLS:COpenGL]
Type=0
BaseClass=CWnd
HeaderFile=OpenGL_dlgwin.h
ImplementationFile=OpenGL_dlgwin.cpp

[DLG:IDD_force_shading_DIALOG]
Type=1
Class=Cforce_shadingDlg
ControlCount=6
Control1=IDC_GL_AREA,static,1342181383
Control2=IDC_CAMZOOM_SLIDER,msctls_trackbar32,1342242840
Control3=IDC_STATIC1,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_TOGGLEHAPTICS_BUTTON,button,1342242816
Control6=IDC_CHECK1,button,1342242819

