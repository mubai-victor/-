
// 添加菜单View.h : interface of the C添加菜单View class
//

#pragma once


class C添加菜单View : public CView
{
protected: // create from serialization only
	C添加菜单View();
	DECLARE_DYNCREATE(C添加菜单View)

// Attributes
public:
	C添加菜单Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~C添加菜单View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 添加菜单View.cpp
inline C添加菜单Doc* C添加菜单View::GetDocument() const
   { return reinterpret_cast<C添加菜单Doc*>(m_pDocument); }
#endif

