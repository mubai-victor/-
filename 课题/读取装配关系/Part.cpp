// Part.cpp : Implementation of CRootApp and DLL registration.

#include "stdafx.h"
#include "Test.h"
#include "Part.h"
#include "swobj.h"

/////////////////////////////////////////////////////////////////////////////
// CSwPart implementation

void CSwPart::Init(CComObject<Cswobj> *theApp, IModelDoc2 *iModelDoc2)
{
	m_App = theApp;
	m_iModelDoc2 = iModelDoc2;
	AttachEventHandlers();
}
void CSwPart::AttachEventHandlers()
{

	m_libid = LIBID_SldWorks;
	m_wMajorVerNum = m_App->GetSldWorksTlbMajor();
	m_wMinorVerNum = 0;

	// Connect to the PartDoc event sink
	DispEventAdvise(m_iModelDoc2);
}
void CSwPart::DetachEventHandlers()
{

	// Disconnect from the PartDoc event sink
	DispEventUnadvise(m_iModelDoc2);
	m_App->DetachEventHandler(m_iModelDoc2);

}


