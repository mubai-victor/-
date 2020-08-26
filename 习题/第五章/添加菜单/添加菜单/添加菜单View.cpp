
// ��Ӳ˵�View.cpp : implementation of the C��Ӳ˵�View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "��Ӳ˵�.h"
#endif

#include "��Ӳ˵�Doc.h"
#include "��Ӳ˵�View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// C��Ӳ˵�View

IMPLEMENT_DYNCREATE(C��Ӳ˵�View, CView)

BEGIN_MESSAGE_MAP(C��Ӳ˵�View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// C��Ӳ˵�View construction/destruction

C��Ӳ˵�View::C��Ӳ˵�View()
{
	// TODO: add construction code here

}

C��Ӳ˵�View::~C��Ӳ˵�View()
{
}

BOOL C��Ӳ˵�View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// C��Ӳ˵�View drawing

void C��Ӳ˵�View::OnDraw(CDC* /*pDC*/)
{
	C��Ӳ˵�Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// C��Ӳ˵�View printing

BOOL C��Ӳ˵�View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void C��Ӳ˵�View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void C��Ӳ˵�View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// C��Ӳ˵�View diagnostics

#ifdef _DEBUG
void C��Ӳ˵�View::AssertValid() const
{
	CView::AssertValid();
}

void C��Ӳ˵�View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C��Ӳ˵�Doc* C��Ӳ˵�View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C��Ӳ˵�Doc)));
	return (C��Ӳ˵�Doc*)m_pDocument;
}
#endif //_DEBUG


// C��Ӳ˵�View message handlers
