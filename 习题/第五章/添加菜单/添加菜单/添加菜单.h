
// ��Ӳ˵�.h : main header file for the ��Ӳ˵� application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// C��Ӳ˵�App:
// See ��Ӳ˵�.cpp for the implementation of this class
//

class C��Ӳ˵�App : public CWinAppEx
{
public:
	C��Ӳ˵�App();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern C��Ӳ˵�App theApp;
