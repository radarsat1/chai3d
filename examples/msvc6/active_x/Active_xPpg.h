#if !defined(AFX_ACTIVE_XPPG_H__E0B68595_8D37_4725_9FB8_A6E27E6D0EE2__INCLUDED_)
#define AFX_ACTIVE_XPPG_H__E0B68595_8D37_4725_9FB8_A6E27E6D0EE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Active_xPpg.h : Declaration of the CActive_xPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CActive_xPropPage : See Active_xPpg.cpp.cpp for implementation.

class CActive_xPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CActive_xPropPage)
	DECLARE_OLECREATE_EX(CActive_xPropPage)

// Constructor
public:
	CActive_xPropPage();

// Dialog Data
	//{{AFX_DATA(CActive_xPropPage)
	enum { IDD = IDD_PROPPAGE_ACTIVE_X };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CActive_xPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIVE_XPPG_H__E0B68595_8D37_4725_9FB8_A6E27E6D0EE2__INCLUDED)
