
// ��Ӳ˵�View.h : interface of the C��Ӳ˵�View class
//

#pragma once


class C��Ӳ˵�View : public CView
{
protected: // create from serialization only
	C��Ӳ˵�View();
	DECLARE_DYNCREATE(C��Ӳ˵�View)

// Attributes
public:
	C��Ӳ˵�Doc* GetDocument() const;

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
	virtual ~C��Ӳ˵�View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ��Ӳ˵�View.cpp
inline C��Ӳ˵�Doc* C��Ӳ˵�View::GetDocument() const
   { return reinterpret_cast<C��Ӳ˵�Doc*>(m_pDocument); }
#endif

