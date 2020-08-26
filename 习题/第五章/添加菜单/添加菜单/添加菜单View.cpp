
// 添加菜单View.cpp : implementation of the C添加菜单View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "添加菜单.h"
#endif

#include "添加菜单Doc.h"
#include "添加菜单View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// C添加菜单View

IMPLEMENT_DYNCREATE(C添加菜单View, CView)

BEGIN_MESSAGE_MAP(C添加菜单View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// C添加菜单View construction/destruction

C添加菜单View::C添加菜单View()
{
	// TODO: add construction code here

}

C添加菜单View::~C添加菜单View()
{
}

BOOL C添加菜单View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// C添加菜单View drawing

void C添加菜单View::OnDraw(CDC* /*pDC*/)
{
	C添加菜单Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// C添加菜单View printing

BOOL C添加菜单View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void C添加菜单View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void C添加菜单View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// C添加菜单View diagnostics

#ifdef _DEBUG
void C添加菜单View::AssertValid() const
{
	CView::AssertValid();
}

void C添加菜单View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C添加菜单Doc* C添加菜单View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C添加菜单Doc)));
	return (C添加菜单Doc*)m_pDocument;
}
#endif //_DEBUG


// C添加菜单View message handlers
