// Active_xPpg.cpp : Implementation of the CActive_xPropPage property page class.

#include "stdafx.h"
#include "active_x.h"
#include "Active_xPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CActive_xPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CActive_xPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CActive_xPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CActive_xPropPage, "ACTIVEX.ActivexPropPage.1",
	0x580b12b8, 0x3085, 0x46c3, 0xbb, 0xc4, 0x10, 0x65, 0x7d, 0x18, 0xd7, 0xf7)


/////////////////////////////////////////////////////////////////////////////
// CActive_xPropPage::CActive_xPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CActive_xPropPage

BOOL CActive_xPropPage::CActive_xPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ACTIVE_X_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xPropPage::CActive_xPropPage - Constructor

CActive_xPropPage::CActive_xPropPage() :
	COlePropertyPage(IDD, IDS_ACTIVE_X_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CActive_xPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xPropPage::DoDataExchange - Moves data between page and properties

void CActive_xPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CActive_xPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CActive_xPropPage message handlers
