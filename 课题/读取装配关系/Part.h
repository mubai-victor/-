// Part.h: Definition of the CSwPart class
//
//////////////////////////////////////////////////////////////////////
#if !defined(__PART_H_)
#define __PART_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"       // main symbols
#include "Document.h"
#include <map>
#include "swobj.h"
#import "D:\\SW\\SOLIDWORKS\\sldworks.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids 
class Cswobj;
/////////////////////////////////////////////////////////////////////////////
// CSwPart
class CSwPart : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSwPart,&CLSID_Part>,
	public CSwDocument,
	public IDispatchImpl<IPart, &IID_IPart, &LIBID_TESTLib>, 
	public IDispEventImpl<ID_PARTDOC_EVENTS, CSwPart, &DIID_DPartDocEvents>
{
private:
	CComPtr<IModelDoc2> m_iModelDoc2;
	CComObject<Cswobj> *m_App;

protected:

public:
	CSwPart() : CSwDocument(swDocPART) {}
	void Init(CComObject<Cswobj> *theApp, IModelDoc2 *iModelDoc2);

	void AttachEventHandlers();
	void DetachEventHandlers();

BEGIN_COM_MAP(CSwPart)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPart)
END_COM_MAP()

//DECLARE_NOT_AGGREGATABLE(CSwPart) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_Part)

// DPartDocEvents
BEGIN_SINK_MAP(CSwPart)
	SINK_ENTRY_EX(ID_PARTDOC_EVENTS, DIID_DPartDocEvents, swPartDestroyNotify, DestroyNotify)
	SINK_ENTRY_EX(ID_PARTDOC_EVENTS, DIID_DPartDocEvents, swPartNewSelectionNotify, NewSelectionNotify)
END_SINK_MAP()

	STDMETHOD(DestroyNotify)()
	{
		DetachEventHandlers();
		ATLTRACE("\tCSwPart::DestroyNotify called\n");
		return 0;
	}

	STDMETHOD(NewSelectionNotify)()
	{
		ATLTRACE("\tCSwPart::NewSelectionNotify called\n");
		return 0;
	}

// IPart
public:
};

#endif // !defined(__PART_H_)
