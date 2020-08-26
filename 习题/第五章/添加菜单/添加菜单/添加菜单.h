
// 添加菜单.h : main header file for the 添加菜单 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// C添加菜单App:
// See 添加菜单.cpp for the implementation of this class
//

class C添加菜单App : public CWinAppEx
{
public:
	C添加菜单App();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern C添加菜单App theApp;
