// swobj.h : Declaration of the Cswobj

#ifndef __SWOBJ_H_
#define __SWOBJ_H_
#define ID_SLDWORKS_TLB_MINOR	0
#define ID_SLDWORKS_TLB_MAJOR	10
#define ID_SLDWORKS_EVENTS		114
#define ID_PARTDOC_EVENTS		115

#include "resource.h"       // main symbols
#include <map>
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <map>
#include <afxdlgs.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <comutil.h>
#include <comdef.h>
#include <cstdlib>
#include <ctime>
#include <random>

#import "D:\\SW\\SOLIDWORKS\\sldworks.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#import "D:\\SW\\SOLIDWORKS\\swpublished.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids 
#import "D:\\SW\\SOLIDWORKS\\swconst.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids 

class CSwDocument;
typedef std::map<IUnknown*, CSwDocument*> TMapIUnknownToDocument;


/////////////////////////////////////////////////////////////////////////////
// 自定义数据类型

//定义零件模型类型
class PartModel
{
public:
	double box[6];
	CComPtr<IComponent> component;
	std::string m_PartName;
	std::vector<double> shapeDistributions;
	PartModel()
	{
		m_PartName = "";
	}
	~PartModel()
	{
		//析构函数，清空内存
		PartRelease();
	}
	void PartRelease()
	{
		std::string().swap(m_PartName);
		std::vector<double>().swap(shapeDistributions);
	}
};

//定义装配体类型
class AssemblyModel
{
public:
	std::string pathName;
	std::string m_AssemblyName;
	std::vector<PartModel> multiPartModel;
	std::vector<CComPtr<IComponent>> partComponent;
	AssemblyModel()
	{
		pathName = "";
		m_AssemblyName = "";
	}
	~AssemblyModel()
	{
		AsseRelease();
	}
	void AsseRelease()
	{
		std::string().swap(pathName);
		std::string().swap(m_AssemblyName);
		multiPartModel.clear();
	}
};

//定义点类型
class myPoint
{
public:
	double x;
	double y;
	double z;
public:
	myPoint()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	myPoint(double a, double b, double c)
	{
		this->x = a;
		this->y = b;
		this->z = c;
	}
	myPoint& operator=(const myPoint& p)
	{
		this->x = p.x;
		this->y = p.y;
		this->z = p.z;
		return *this;
	}
	bool operator==(const myPoint &p)
	{
		if (this->x != p.x)
			return false;
		if (this->y != p.y)
			return false;
		if (this->z != p.z)
			return false;

		return true;
	}
	bool operator!=(const myPoint &p)
	{
		return !(*this == p);
	}
};

//定义面片类型，包含三个点和一个面积
class Facet
{
public:
	myPoint m_point[3];
	double m_area;
	Facet()
	{
		m_area = 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
// Cswobj
class Assembly;
class ATL_NO_VTABLE Cswobj : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Cswobj, &CLSID_swobj>,
	public IDispatchImpl<ISwAddin, &IID_ISwAddin, &LIBID_SWPublished>,
	public IDispatchImpl<Iswobj, &IID_Iswobj, &LIBID_TESTLib>,
	public IDispEventImpl<ID_SLDWORKS_EVENTS, Cswobj, &DIID_DSldWorksEvents>
{

private:
	CComPtr<ISldWorks> m_iSldWorks;
	DWORD m_dwSldWorksEvents;
	long m_swCookie;
	long m_lToolbarID;
	long m_swMajNum;
	long m_swMinNum;


	TMapIUnknownToDocument m_swDocMap;

protected:
	void AddUserInterface();
	void RemoveUserInterface();
	void AddToolbar();
	void RemoveToolbar();
	void AddMenus();
	void RemoveMenus();

	void AttachEventHandlers();
	void DetachEventHandlers();

public:
	void DetachEventHandler(IUnknown *iUnk);

public:
	Cswobj()  :  m_swCookie(0)
	{
		m_lToolbarID = 0;
		m_dwSldWorksEvents = 0;
	}
	
	int GetSldWorksTlbMajor() {return (m_swMajNum >= ID_SLDWORKS_TLB_MAJOR ) ? m_swMajNum : 0;}
	int GetSldWorksTlbMinor() {return (m_swMinNum >= ID_SLDWORKS_TLB_MINOR ) ? m_swMinNum : 0;}
	

DECLARE_REGISTRY_RESOURCEID(IDR_SWOBJ)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(Cswobj)
	COM_INTERFACE_ENTRY(Iswobj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY2(IDispatch, Iswobj)
	COM_INTERFACE_ENTRY(ISwAddin)
END_COM_MAP()

// Iswobj
public:
	//STDMETHOD(firstmethod)();
	STDMETHOD(ToolbarUpdate)(/*[out,retval]*/ long *status);
	STDMETHOD(StartNotepad)();

// ISwAddin
	STDMETHOD(ConnectToSW)(IDispatch * ThisSW, LONG Identifier, VARIANT_BOOL * IsConnected);
	STDMETHOD(DisconnectFromSW)(VARIANT_BOOL * IsDisconnected);

// DSldWorksEvents
BEGIN_SINK_MAP(Cswobj)
	SINK_ENTRY_EX(ID_SLDWORKS_EVENTS, DIID_DSldWorksEvents, swAppDocumentLoadNotify, DocumentLoadNotify)
	SINK_ENTRY_EX(ID_SLDWORKS_EVENTS, DIID_DSldWorksEvents, swAppFileNewNotify, FileNewNotify)
	SINK_ENTRY_EX(ID_SLDWORKS_EVENTS, DIID_DSldWorksEvents, swAppFileNewNotify2, FileNewNotify2)
	SINK_ENTRY_EX(ID_SLDWORKS_EVENTS, DIID_DSldWorksEvents, swAppActiveDocChangeNotify, ActiveDocChangeNotify)
	SINK_ENTRY_EX(ID_SLDWORKS_EVENTS, DIID_DSldWorksEvents, swAppActiveModelDocChangeNotify, ActiveModelDocChangeNotify)
END_SINK_MAP()

	STDMETHOD(DocumentLoadNotify)(BSTR docTitle, BSTR docPath);
	STDMETHOD(FileNewNotify)(IDispatch * newDoc, LONG DocType)
	{
		ATLTRACE("\tCswobj::FileNewNotify called\n");
		return 0;
	}
	STDMETHOD(ActiveDocChangeNotify)()
	{
		ATLTRACE("\tCswobj::ActiveDocChangeNotify called\n");
		return 0;
	}
	STDMETHOD(ActiveModelDocChangeNotify)()
	{
		ATLTRACE("\tCswobj::ActiveModelDocChangeNotify called\n");
		return 0;
	}
	STDMETHOD(FileNewNotify2)(IDispatch * newDoc, LONG DocType, BSTR templateName)
	{
		ATLTRACE("\tCswobj::FileNewNotify2 called\n");
		return 0;
	}
	
	STDMETHOD(Traverse)();
	int TraverseChildren(long RecurseLevel, CComPtr<IComponent> pComponent);

private:
	Assembly *_assembly;
public:
	AssemblyModel m_assemblymodel;			//存储单个装配体模型
	PartModel m_partmodel;					//存储单个零件模型
	std::vector<Facet> m_multiFacets;			//存储一个零件中的面片数据
	std::vector<double> accumuArea;				//存储三角面片的累积面积
	CComPtr<IModelDoc2> swCurAsseModel;		//定义当前打开的装配体文档对象	
	std::vector<double> distance;				//定义两点间距离数组
public:
	void GetEveryAsseShapeDistributions(const CComPtr<IModelDoc2> & swAssemDoc);
	void TraverseEveryAssembly(const CComPtr<IModelDoc2> &swAssemDoc);
	int TraverseChildrenComponent(const CComPtr<IModelDoc2> &swAssemDoc,
		long RecurseLevel, CComPtr<IComponent> pComponent);
	void PrintData(const AssemblyModel & asse, std::map<CComPtr<IComponent>, int>_pointer2curs, const std::string & SameDirection);
	void TessellationPartModel(const CComPtr<IComponent> &pCurComponent);
	double ComputeTwoPointDistance(const myPoint &p1, const myPoint &p2);
	void OutputShape();
};


#endif //__SWOBJ_H_
