
// RadioServiceApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRadioServiceAppApp:
// See RadioServiceApp.cpp for the implementation of this class
//

class CRadioServiceAppApp : public CWinApp
{
public:
	CRadioServiceAppApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRadioServiceAppApp theApp;