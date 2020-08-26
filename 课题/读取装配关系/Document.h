// Document.h: interface for the CSwDocument class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(__DOCUMENT_H__)
#define __DOCUMENT_H__

#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSwDocument  
{
	long m_Type;

public:
	CSwDocument(long Type) : m_Type(Type) {}
	virtual ~CSwDocument() {}

	long GetType() {return m_Type;}
};
#endif // !defined(__DOCUMENT_H__)
