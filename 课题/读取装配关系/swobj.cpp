/* NOTE: You will need to add the -GX flag to your Project Options in Project->Settings->C/C++*/

// swobj.cpp : Implementation of Cswobj
#include "stdafx.h"
#include "Test.h"
#include "swobj.h"
#include <string>
#include <map>
#include "Part.h"
#include "variant.h"
#include "Assembly.h"
#include "fstream"
#include "algorithm"
/////////////////////////////////////////////////////////////////////////////
// Cswobj

bool cmp(const Facet &a1, const Facet &a2)
{
	return a1.m_area < a2.m_area;
}

Assembly assembly;
void Cswobj::AddUserInterface()
{
	AddToolbar();
	AddMenus();
}

void Cswobj::RemoveUserInterface()
{
	RemoveMenus();
	RemoveToolbar();
}

void Cswobj::AddMenus()
{
	long retval = 0;
	VARIANT_BOOL ok;
	long type;
	long position;
	CComBSTR menu;
	CComBSTR method;
	CComBSTR update;
	CComBSTR hint;

	// Add menu for main frame
	type = swDocNONE;
	position =1;
	menu.LoadString(IDS_TEST_MENU);
	m_iSldWorks->AddMenu(type, menu, position, &retval);

	position = -1;
	menu.LoadString(IDS_TEST_START_NOTEPAD_ITEM);
	method.LoadString(IDS_TEST_START_NOTEPAD_METHOD);
	hint.LoadString(IDS_TEST_START_NOTEPAD_HINT);
	m_iSldWorks->AddMenuItem2(type, m_swCookie, menu, position, method, update, hint, &ok);

	// Add menu for part frame
	type = swDocPART;
	position = 1;
	menu.LoadString(IDS_TEST_MENU);
	m_iSldWorks->AddMenu(type, menu, position, &retval);

	position = -1;
	menu.LoadString(IDS_TEST_START_NOTEPAD_ITEM);
	method.LoadString(IDS_TEST_START_NOTEPAD_METHOD);
	hint.LoadString(IDS_TEST_START_NOTEPAD_HINT);
	m_iSldWorks->AddMenuItem2(type, m_swCookie, menu, position, method, update, hint, &ok);
	position = -1;
	menu.LoadString(IDS_test_ITEM);
	method.LoadString(IDS_test_METHOD);
	hint.LoadString(IDS_test_HINT);
	m_iSldWorks->AddMenuItem2(type, m_swCookie, menu, position, method, update, hint, &ok);


	// Add menu for assembly frame
	type = swDocASSEMBLY;
	position = 5;
	menu.LoadString(IDS_TEST_MENU);
	m_iSldWorks->AddMenu(type, menu, position, &retval);

	position = -1;
	menu.LoadString(IDS_TEST_START_NOTEPAD_ITEM);
	method.LoadString(IDS_TEST_START_NOTEPAD_METHOD);
	hint.LoadString(IDS_TEST_START_NOTEPAD_HINT);
	m_iSldWorks->AddMenuItem2(type, m_swCookie, menu, position, method, update, hint, &ok);


	position = -1;
	menu.LoadString(IDS_test_ITEM);
	method.LoadString(IDS_test_METHOD);
	hint.LoadString(IDS_test_HINT);
	m_iSldWorks->AddMenuItem2(type, m_swCookie, menu, position, method, update, hint, &ok);

	// Add menu for drawing frame
	type = swDocDRAWING;
	position = 1;
	menu.LoadString(IDS_TEST_MENU);
	m_iSldWorks->AddMenu(type, menu, position, &retval);
	position = -1;
	menu.LoadString(IDS_TEST_START_NOTEPAD_ITEM);
	method.LoadString(IDS_TEST_START_NOTEPAD_METHOD);
	hint.LoadString(IDS_TEST_START_NOTEPAD_HINT);
	m_iSldWorks->AddMenuItem2(type, m_swCookie, menu, position, method, update, hint, &ok);


}

void Cswobj::RemoveMenus()
{
	VARIANT_BOOL ok;
	CComBSTR menu;

	menu.LoadString(IDS_TEST_MENU);
	m_iSldWorks->RemoveMenu(swDocNONE, menu, NULL, &ok);
	m_iSldWorks->RemoveMenu(swDocPART, menu, NULL, &ok);
	m_iSldWorks->RemoveMenu(swDocASSEMBLY, menu, NULL, &ok);
	m_iSldWorks->RemoveMenu(swDocDRAWING, menu, NULL, &ok);
}

void Cswobj::AddToolbar()
{
	VARIANT_BOOL stat = VARIANT_FALSE;
	VARIANT_BOOL ok;
	HRESULT hres;

	CComBSTR title;
	CComBSTR callback;
	CComBSTR update;
	CComBSTR tip;
	CComBSTR hint;
	if (m_lToolbarID == 0)
	{
		title.LoadString(IDS_TEST_TOOLBAR_TITLE);
		callback.LoadString(IDS_TEST_TOOLBAR_CALLBACK);
		update.LoadString(IDS_TEST_TOOLBAR_UPDATE);
		tip.LoadString(IDS_TEST_TOOLBAR_TIP);
		hint.LoadString(IDS_TEST_TOOLBAR_HINT);

		hres = m_iSldWorks->AddToolbar3(m_swCookie, title, IDR_TOOLBAR_SMALL, IDR_TOOLBAR_LARGE, -1,
										swDocTemplateTypeNONE|swDocTemplateTypePART|swDocTemplateTypeASSEMBLY|swDocTemplateTypeDRAWING, &m_lToolbarID);

		m_iSldWorks->AddToolbarCommand2(m_swCookie, m_lToolbarID, 0, callback, update, tip, hint, &ok);
		m_iSldWorks->AddToolbarCommand2(m_swCookie, m_lToolbarID, 1, callback, update, tip, hint, &ok);
		m_iSldWorks->AddToolbarCommand2(m_swCookie, m_lToolbarID, 2, callback, update, tip, hint, &ok);

	}
}

void Cswobj::RemoveToolbar()
{
	if (m_lToolbarID != 0)
	{
		VARIANT_BOOL stat;

		HRESULT hres = m_iSldWorks->RemoveToolbar2( m_swCookie, m_lToolbarID, &stat);
		m_lToolbarID = 0;
	}
}

void Cswobj::AttachEventHandlers()
{
	this->m_libid = LIBID_SldWorks;
	this->m_wMajorVerNum = GetSldWorksTlbMajor();
	this->m_wMinorVerNum = 0;

	// Connect to the SldWorks event sink
	DispEventAdvise(m_iSldWorks);

	// Connect event handlers to all previously open documents
	TMapIUnknownToDocument::iterator iter;

	CComPtr<IModelDoc2> iModelDoc2;
	m_iSldWorks->IGetFirstDocument2(&iModelDoc2);
	while (iModelDoc2 != NULL)
	{
		iter = m_swDocMap.find(iModelDoc2);
		if (iter == m_swDocMap.end())
		{
			long docType = 0;
			iModelDoc2->GetType(&docType);
			switch (docType)
			{
			case swDocPART:
				{
					// Add this to the map, if it is a Part
					CComQIPtr<IPartDoc, &__uuidof(IPartDoc)> iDoc(iModelDoc2);
					if (iDoc)
					{
						CComObject<CSwPart> *pDoc;
						CComObject<CSwPart>::CreateInstance( &pDoc);
						pDoc->Init((CComObject<Cswobj>*)this, iModelDoc2);
						m_swDocMap.insert(TMapIUnknownToDocument::value_type(iModelDoc2, pDoc));
						iModelDoc2.p->AddRef();
						pDoc->AddRef();
					}
				}
				break;
			}
		}

		CComPtr<IModelDoc2> iNextModelDoc2;
		iModelDoc2->IGetNext(&iNextModelDoc2);
		iModelDoc2 = iNextModelDoc2;
	}
}

void Cswobj::DetachEventHandlers()
{
	// Disconnect from the SldWorks event sink
	DispEventUnadvise(m_iSldWorks);

	// Disconnect all event handlers
	TMapIUnknownToDocument::iterator iter;

	iter = m_swDocMap.begin();

	for (iter = m_swDocMap.begin(); iter != m_swDocMap.end(); /*iter++*/) //The iteration is performed inside the loop
	{
		TMapIUnknownToDocument::value_type obj = *iter;
		iter++;
		switch (obj.second->GetType())
		{
		case swDocPART:
			{
				CComObject<CSwPart> *pDoc = (CComObject<CSwPart>*)obj.second;
				pDoc->DetachEventHandlers();
			}
			break;
		}
	}
}

void Cswobj::DetachEventHandler(IUnknown *iUnk)
{
	TMapIUnknownToDocument::iterator iter;

	iter = m_swDocMap.find(iUnk);
	if (iter != m_swDocMap.end())
	{
		TMapIUnknownToDocument::value_type obj = *iter;
		obj.first->Release();
		switch (obj.second->GetType())
		{
		case swDocPART:
			{
				CComObject<CSwPart> *pDoc = (CComObject<CSwPart>*)obj.second;
				pDoc->Release();
			}
			break;
		}
		m_swDocMap.erase(iter);
	}
}

/////////////////////////////////////////////////////////////////////////////
// ISwAddin implementation

STDMETHODIMP Cswobj::ConnectToSW(IDispatch * ThisSW, LONG Cookie, VARIANT_BOOL * IsConnected)
{
	VARIANT_BOOL status;

	if (IsConnected == NULL)
		return E_POINTER;

	
	m_swCookie = Cookie;
	m_iSldWorks = CComQIPtr<ISldWorks, &__uuidof(ISldWorks)>(ThisSW);
	if (m_iSldWorks != NULL)
	{	
		m_iSldWorks->SetAddinCallbackInfo((long)_Module.GetModuleInstance(), static_cast<Iswobj*>(this), m_swCookie, &status);
		USES_CONVERSION;
		CComBSTR bstrNum;
		std::string strNum;
		char *buffer;

		m_iSldWorks->RevisionNumber(&bstrNum);

		strNum = W2A(bstrNum);
		m_swMajNum = strtol(strNum.c_str(), &buffer, ID_SLDWORKS_TLB_MAJOR );

		strNum = buffer;
		strNum = strNum.substr(1);
		m_swMinNum = strtol(strNum.c_str(), &buffer, ID_SLDWORKS_TLB_MINOR );

		
		AttachEventHandlers();
		AddUserInterface();
	}

	return S_OK;
}

STDMETHODIMP Cswobj::DisconnectFromSW(VARIANT_BOOL * IsDisconnected)
{
	if (IsDisconnected == NULL)
		return E_POINTER;

	if (m_iSldWorks != NULL)
	{
		DetachEventHandlers();
		RemoveUserInterface();
		m_iSldWorks.Release();
	}

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// DSldWorksEvents implementation
STDMETHODIMP Cswobj::DocumentLoadNotify(BSTR docTitle, BSTR docPath)
{
	USES_CONVERSION;

	CComBSTR bstrTitle(docTitle);
	std::string strTitle = W2A(docTitle);

	TMapIUnknownToDocument::iterator iter;

	CComPtr<IModelDoc2> iModelDoc2;
	m_iSldWorks->IGetFirstDocument2(&iModelDoc2);
	while (iModelDoc2 != NULL)
	{
		iModelDoc2->GetTitle(&bstrTitle);
		if (strTitle == W2A(bstrTitle))
		{
			iter = m_swDocMap.find(iModelDoc2);
			if (iter == m_swDocMap.end())
			{
				ATLTRACE("\tCswobj::DocumentLoadNotify current part not found\n");
				long docType = 0;
				iModelDoc2->GetType(&docType);
				switch (docType)
				{
				case swDocPART:
					{
						// Add this to the map, if it is a Part
						CComQIPtr<IPartDoc, &__uuidof(IPartDoc)> iDoc(iModelDoc2);
						if (iDoc)
						{
							CComObject<CSwPart> *pDoc;
							CComObject<CSwPart>::CreateInstance( &pDoc);
							pDoc->Init((CComObject<Cswobj>*)this, iModelDoc2);
							m_swDocMap.insert(TMapIUnknownToDocument::value_type(iModelDoc2, pDoc));
							iModelDoc2.p->AddRef();
							pDoc->AddRef();
						}
					}
					break;
				}
			}
		}

		CComPtr<IModelDoc2> iNextModelDoc2;
		iModelDoc2->IGetNext(&iNextModelDoc2);
		iModelDoc2 = iNextModelDoc2;
	}

	ATLTRACE("\tCswobj::DocumentLoadNotify called\n");
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Iswobj implementation

STDMETHODIMP Cswobj::StartNotepad()
{
	// TODO: Add your implementation code here
	::WinExec("Notepad.exe", SW_SHOW);

	return S_OK;
}

STDMETHODIMP Cswobj::ToolbarUpdate(long *status)
{
	HRESULT retval = S_OK;

	*status = 1;

	return S_OK;
}

STDMETHODIMP Cswobj::Traverse()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	std::vector<CString> fileNames;			//定义一个字符串动态数组
	std::string fileName;
	std::ifstream file, clear;
	char buff[SIZE];
	CComPtr<IModelDoc2> swModel;	//定义ModelDoc对象

	file.open("E:\\Desktop\\TempFile\\path.txt");
	if (!file) {
		AfxMessageBox("Can not open the file 'path.txt'");
		return S_OK;
	}
	clear.open("E:\\Desktop\\TempFile\\information.txt");/*每次生成新的数据之后要把上次生成的数据清除掉，包括图结构的数据和零件的名字，序号对照表*/
	clear.close();
	clear.open("E:\\Desktop\\TempFile\\map.txt");
	clear.close();
	clear.open("E:\\Desktop\\TempFile\\map.txt");
	clear.close();

	while (file) {//从文件中逐行读取装配体名字 
		file.getline(buff,SIZE);		//将装配体名字转换格式在末尾插入数组中（装配体名字中包含装配体所在的完整路径）
		fileNames.push_back(buff);
	}
	file.close();						//关闭流对象

	//fileNames.resize(1);
	for (auto name : fileNames) {
		CComBSTR sAssemblyName(name);
		if (name.GetLength() == 0) {
			continue;
		}

		CComBSTR sDefaultConfiguration(L"Default");											//定义默认配置名
		long lErrors, lWarnings;
		swModel = NULL;												 //释放当前打开的装配体智能指针
		m_iSldWorks->OpenDoc6(sAssemblyName, swDocASSEMBLY, swOpenDocOptions_Silent, sDefaultConfiguration, &lErrors, &lWarnings, &swModel);

		HRESULT rs;
		CComPtr<IModelDoc2>m_iModelDoc;
		CComPtr<IConfiguration>pConfiguration;
		long RecurseLevel = 0;
		CString MyString;
		CComPtr<IComponent>pRootComponent;

		_assembly = new Assembly;

		swModel->IGetActiveConfiguration(&pConfiguration);
		pConfiguration->IGetRootComponent(&pRootComponent);
		TraverseChildren(RecurseLevel, pRootComponent);/*获取装配体中所有零件*/

		_assembly->resizeRelationVector();/*设置assembly中relation成员变量各维的大小。*/
		_assembly->setCurs2Pointer();

		GetEveryAsseShapeDistributions(swModel);/*获取装配体中零件的形状分布*/
		PrintData(m_assemblymodel, _assembly->getPointer2PartMap(), "E:\\Desktop\\TempFile\\ShapeDistribution.txt");
		m_assemblymodel.AsseRelease();

		CComPtr<IFeature> pFeature;
		CComPtr<IFeature> pNextFeature;
		CComPtr<IFeature> pNextSubFeature;
		BSTR featureTypeName;
		CString featureType;
		CComPtr<IDispatch> pDispatch;
		CComPtr<IFeature> pSubFeature;
		CComQIPtr<IMate2> pMate2;
		CComQIPtr<IMate> pMate;
		CComPtr<IEntity> pEntity;
		CComPtr<IMateEntity2> pMateEntity;
		VARIANT_BOOL reval;
		Mate mate;
		long mateCount = 0;
		long mateType = -1;

		pFeature.Release();
		swModel->IFirstFeature(&pFeature);
		while (pFeature != NULL) {
			pFeature->GetTypeName(&featureTypeName);
			featureType = featureTypeName;

			if (featureType == "MateGroup") {
				pSubFeature.Release();
				pFeature->IGetFirstSubFeature(&pSubFeature);

				while (pSubFeature != NULL) {
					pDispatch.Release();
					pSubFeature->GetSpecificFeature2(&pDispatch);
					pMate2 = pDispatch;
					pMate2->get_Type(&mateType);
					pMate2->GetMateEntityCount(&mateCount);/*获取该配合关系约束的entity个数*/
					mate._mateType = mateType;

					pMateEntity.Release();/*获取参加配合的两个对象*/
					pMate2->MateEntity(0, &pMateEntity);
					pDispatch.Release();
					pMateEntity->get_Reference(&pDispatch);
					mate._entity1 = pDispatch;

					pMateEntity.Release();
					if (mateType == swMateWIDTH) {/*宽度配合关系参与配合的对象有四个，获取第一个和第三个就可以了。*/
						pMate2->MateEntity(2, &pMateEntity);
					}
					else {
						pMate2->MateEntity(1, &pMateEntity);
					}
					pDispatch.Release();
					pMateEntity->get_Reference(&pDispatch);
					mate._entity2 = pDispatch;

					_assembly->addRelation(mate);/*将配合的相关信息赋值给assembly成员变量*/
					pNextSubFeature.Release();
					pSubFeature->IGetNextSubFeature(&pNextSubFeature);/*获取下一个配合特征*/
					pSubFeature = pNextSubFeature;

				}
			}

			pNextFeature.Release();
			pFeature->IGetNextFeature(&pNextFeature);/*获取下一个特征，直到遍历所有的特征*/
			pFeature = pNextFeature;
		}

		std::vector<std::vector<MidDOF>> result;
		_assembly->getDOF(result);
		_assembly->output(result);

		delete _assembly;

		m_iSldWorks->CloseDoc(sAssemblyName);
	}

	AfxMessageBox("Finished.");
	return S_OK;
}

int Cswobj::TraverseChildren(long RecurseLevel, CComPtr<IComponent> pComponent)
{
	CComPtr<IComponent> *pChildren;/*定义子零件*/
	int nChildren = 0;
	int i = 0;
	BSTR Name;
	HRESULT h_res = S_OK;
	CComPtr<IModelDoc2> m_iModelDoc;
	CComPtr<IModelDoc> m_iModelDoc1;
	VARIANT_BOOL result;

	if (RecurseLevel == 0) {
		h_res = m_iSldWorks->get_IActiveDoc2(&m_iModelDoc);/*获得当前活动文档对象*/
		if (S_OK == h_res || m_iModelDoc != NULL) {
			h_res = m_iModelDoc->GetTitle(&Name);/*获得当前活动文档的名称*/
		}
	}

	RecurseLevel++;
	h_res = pComponent->IGetChildrenCount(&nChildren);//获得组件下的子零件个数

	if (nChildren == 0) {//仅提取单独的零件，当组件下的子零件数为0时，说明这个组件是单独的零件，如果不是，则递归遍历。
		_assembly->addPart(pComponent);
	}

	if (S_OK == h_res || nChildren > 0) {//检查当前组件下有没有零件
		pChildren = new CComPtr<IComponent>[nChildren];
		h_res = pComponent->IGetChildren((IComponent ***)&pChildren);//获得子零件
		if (S_OK == h_res) {
			for (int i = 0; i < nChildren; i++) {
				TraverseChildren(RecurseLevel, pChildren[i]);//递归遍历子零部件
				pChildren[i] = NULL;//释放子零件对象
			}
		}
		delete[] pChildren;
	}

	RecurseLevel--;
	return 0;
}

//2.提取每个装配体的形状分布
void Cswobj::GetEveryAsseShapeDistributions(const CComPtr<IModelDoc2> & swAssemDoc)
{
	HRESULT rs;			//返回结果标志
	long docType = -1;	//定义文档类型
	swAssemDoc->GetType(&docType);	//获取文档类型
	if (docType != swDocASSEMBLY) {
		AfxMessageBox(_T("打开的不是装配体"), MB_OK | MB_ICONINFORMATION);
		return;						//返回
	}
	//遍历每一个装配体，得到属性值
	TraverseEveryAssembly(swAssemDoc);
	return;
}


void Cswobj::TraverseEveryAssembly(const CComPtr<IModelDoc2>& swAssemDoc)
{
	long RecurseLevel = 0;							//定义递归级别
	CComPtr<IConfiguration> pConfiguration;			//定义配置对象			
	CComPtr<IComponent>	pRootComponent = NULL;		//定义根组件对象

	swAssemDoc->IGetActiveConfiguration(&pConfiguration);	//获得配置对象
	pConfiguration->IGetRootComponent(&pRootComponent);		//获得根组件对象

															//设置单位体系为MMGS
	CComPtr<IModelDocExtension> docExtension;
	swAssemDoc->get_Extension(&docExtension);
	VARIANT_BOOL flag;
	docExtension->SetUserPreferenceInteger(swUnitSystem, swDetailingNoOptionSpecified, swUnitSystem_MMGS, &flag);
	//遍历装配体下面的子零件
	TraverseChildrenComponent(swAssemDoc, RecurseLevel, pRootComponent);
	return;
}

int Cswobj::TraverseChildrenComponent(const CComPtr<IModelDoc2> &swAssemDoc, long RecurseLevel, CComPtr<IComponent> pComponent)
{
	srand(time(NULL));
	std::default_random_engine ran(time(NULL));						//产生随机种子
	CComPtr<IComponent> *pChildren = NULL;		//定义子零件
	int nChildren;								//定义子零件个数
	CComBSTR Name;								//定义组件名称
	HRESULT hres = S_OK;						//调用结果的标志											
	pComponent->IGetChildrenCount(&nChildren);	//获取孩子的个数
	if (RecurseLevel == 0) {					//判断RecurseLevel的值是否为0 ，递归级别为0，代表最开始的装配体  Recurse:递归
												//为总装配体本身
	}
	else if (RecurseLevel >= 1 && nChildren > 0)
	{
		//为子装配体，不作任何处理
	}
	else {
		//递归级别不为0，表示是总装配体下面的零件或子装配体中的零件
		hres = pComponent->get_Name(&Name);
		//存储当前零件名称,预分配空间
		m_partmodel.m_PartName = (_bstr_t)Name;
		m_partmodel.component = pComponent;
		m_partmodel.shapeDistributions.resize(512);

		VARIANT value;
		VariantInit(&value);
		value.vt = VT_ARRAY;
		//获取零件的包围盒角点坐标
		pComponent->GetBox(false, false, &value);
		SafeDoubleArray box(&value);
		//box[0],box[1],box[2]分别存储包围盒角点坐标的最小值，也就是左下角的坐标
		m_partmodel.box[0] = min(box[0], box[3]);
		m_partmodel.box[1] = min(box[1], box[4]);
		m_partmodel.box[2] = min(box[2], box[5]);
		//box[3],box[4],box[5]分别存储包围盒角点坐标的最大值，也即右上角的坐标
		m_partmodel.box[3] = max(box[0], box[3]);
		m_partmodel.box[4] = max(box[1], box[4]);
		m_partmodel.box[5] = max(box[2], box[5]);



		//对当前零件模型进行镶嵌操作，获取表面的三角面片的顶点
		TessellationPartModel(pComponent);

		//按照面积从小到大排序
		sort(m_multiFacets.begin(), m_multiFacets.end(), cmp);

		distance.resize(1048576);
		//定义柱状图数组，为一个桶，值全部初始化为0
		int histogram[512];
		memset(histogram, 0, sizeof(histogram));

		//获取累计面积的比例(用累计面积除以总面积之和）
		int iSize = m_multiFacets.size();
		double AccuAreaSum = 0;
		for (int i = 0; i < iSize; i++)
		{
			AccuAreaSum += m_multiFacets[i].m_area;
			accumuArea.push_back(AccuAreaSum);
		}

		//设置最小距离和最大距离，最小距离赋大值，最大距离赋小值
		double mindist = 1000000, maxdist = -1;
		double randomArea = 0.0;
		//随机采样1024*1024次，并计算欧氏距离
		for (int i = 0; i < 1048576; i++)
		{
			myPoint newPoint[2];				//两个随机点
												//每次采样随机取两个点的坐标
			for (int j = 0; j < 2; j++)
			{
				std::uniform_real_distribution<double> dis1(0.0, AccuAreaSum);
				randomArea = dis1(ran);		//在0和最大面积之间产生一个随机数
				
				/*randomArea = (double)rand() / RAND_MAX*AccuAreaSum;*/
				int index = 0;
				auto it = upper_bound(accumuArea.begin(), accumuArea.end(), randomArea);
				if (it == accumuArea.end())
				{
					index = iSize - 1;
				}
				else
				{
					index = it - accumuArea.begin();
				}

				//获得随机的三角面片的三个顶点
				myPoint &A = m_multiFacets[index].m_point[0];
				myPoint &B = m_multiFacets[index].m_point[1];
				myPoint &C = m_multiFacets[index].m_point[2];
				std::uniform_real_distribution<double> dis2(0.0, 1.0);
				double r1 = dis2(ran);						//产生0-1之间的随机数
				double r2 = dis2(ran);
				//double r1 = (double)rand() / RAND_MAX;
				//double r2 = (double)rand() / RAND_MAX;
				//计算随机点的坐标
				newPoint[j].x = (1 - sqrt(r1))*A.x + sqrt(r1)*(1 - r2)*B.x + sqrt(r1)*r2*C.x;
				newPoint[j].y = (1 - sqrt(r1))*A.y + sqrt(r1)*(1 - r2)*B.y + sqrt(r1)*r2*C.y;
				newPoint[j].z = (1 - sqrt(r1))*A.z + sqrt(r1)*(1 - r2)*B.z + sqrt(r1)*r2*C.z;
			}
			//计算两个随机点之间的距离
			distance[i] = ComputeTwoPointDistance(newPoint[0], newPoint[1]);

			if (distance[i] < mindist)
			{//比最小值小，则替换
				mindist = distance[i];
			}
			if (distance[i] > maxdist)
			{//比最大值大，则替换
				maxdist = distance[i];
			}

		}
		//分组，构造形状分布,计算柱状图每个bin的宽度
		double gap = (maxdist - mindist) / 512;

		//统计落入每个桶的个数
		int id = 0;
		for (int i = 0; i < 1048576; i++)
		{		//histogram为左闭右开，最后一个值特殊处理
			id = (distance[i] - mindist) / gap;
			if (id >= 512)
			{
				histogram[511]++;
				continue;
			}
			histogram[id]++;
		}
		for (int i = 0; i < 512; i++)
		{
			m_partmodel.shapeDistributions[i] = (double)histogram[i];
		}

		m_assemblymodel.multiPartModel.push_back(m_partmodel);	//数据传入

																//关闭当前零件模型
																//CComBSTR title;
																//swCurPartModel->GetTitle(&title);
																//m_iSldWorks->QuitDoc(title);

																//释放掉当前零件对象
		m_multiFacets.clear();		//释放存储三角面片的数组
		accumuArea.clear();			//释放存储累积面积数组
		distance.clear();
		m_partmodel.PartRelease();					//释放当前零件模型
	}
	ASSERT(S_OK == hres);

	RecurseLevel++;

	hres = pComponent->IGetChildrenCount(&nChildren);	//获得组件下子零件个数

	if (S_OK == hres || nChildren > 0) {	//判断上一步操作是否成功，子零件的个数
		pChildren = new CComPtr<IComponent>[nChildren];			//初始化字符串数组

		hres = pComponent->IGetChildren((IComponent ***)&pChildren);	//获得子零件
		if (S_OK == hres) {
			for (int i = 0; i < nChildren; i++) {
				TraverseChildrenComponent(swAssemDoc, RecurseLevel, pChildren[i]);	//递归遍历子零部件
				pChildren[i] = NULL;									//释放子零件对象
			}
		}
		delete[] pChildren;
	}
	RecurseLevel--;
	return nChildren;			//返回子零件个数
}

//3.存储得到的信息
void Cswobj::PrintData(const AssemblyModel & asse, std::map<CComPtr<IComponent>, int>pointer2curs, const std::string & path)
{
	//定义文件输出路径
	//std::string fileOutputPath = SameDirection + asse.m_AssemblyName + ".txt";

	std::ofstream fout(path, std::ios_base::out|std::ios_base::app);
	//fout << asse.pathName << std::endl;					//输出装配体路径名称
	fout << "#" << asse.multiPartModel.size() << std::endl;		//输出零件模型的数量

	for (int i = 0; i < asse.multiPartModel.size(); i++)
	{
		//输出零件模型的序号
		fout << pointer2curs[asse.multiPartModel[i].component] << std::endl;//因为对零件按照体积的大小进行了排序，顺序不再是最初的顺序了
		for (int j = 0; j < 512; j++)
		{//输出形状分布的数据，每行16个，共64行
			fout << asse.multiPartModel[i].shapeDistributions[j] << " ";
			if ((j + 1) % 32 == 0)
				fout << std::endl;
		}
		fout << "box:" << asse.multiPartModel[i].box[0] << "," << asse.multiPartModel[i].box[1] << "," << asse.multiPartModel[i].box[2] << "," 
			<< asse.multiPartModel[i].box[3] << "," << asse.multiPartModel[i].box[4] << "," << asse.multiPartModel[i].box[5] << std::endl;
	}
	
	fout.close();		//关闭文件
}

//对零件模型进行镶嵌处理
void Cswobj::TessellationPartModel(const CComPtr<IComponent> &pComponent)
{
	HRESULT res;
	//获取组件对应的激活文档
	CComPtr<IModelDoc> swModel;
	CComPtr<IPartDoc> swPart;
	pComponent->IGetModelDoc(&swModel);
	//将文档指针传递给零件
	swPart = swModel;

	//得到所有实体
	VARIANT vBodies;
	swPart->GetBodies2(swAllBodies, VARIANT_FALSE, &vBodies);
	//循环遍历数组
	ASSERT(NULL != vBodies.pparray);
	SAFEARRAY	*psaBody = V_ARRAY(&vBodies);
	LPDISPATCH	*pBodyDispArray = NULL;
	long nBodyHighIndex = -1;
	long nBodyCount = -1;

	res = SafeArrayAccessData(psaBody, (void**)&pBodyDispArray);		//获取实体
	ASSERT(res == S_OK);
	ASSERT(NULL != pBodyDispArray);
	res = SafeArrayGetUBound(psaBody, 1, &nBodyHighIndex);				//获取实体的数量
	ASSERT(S_OK == res);
	nBodyCount = nBodyHighIndex + 1;
	CComQIPtr<IBody2> swBody;
	for (int i = 0; i < nBodyCount; i++)			//遍历实体，得到每一个实体
	{
		//得到一个实体
		swBody = pBodyDispArray[i];
		ASSERT(swBody);
		//现在调用在实体上的一个方法
		long nBodyType = 0;
		swBody->GetType(&nBodyType);
		if (nBodyType == swSolidBody)
		{
			CComPtr<ITessellation> swTessellation;
			long faceNum = 0;
			swBody->GetFaceCount(&faceNum);
			IFace2 **ptrFace = new IFace2*[faceNum];
			swBody->IGetFaces(faceNum, ptrFace);
			//传入空，因此整个实体将会被镶嵌
			swBody->IGetTessellation(faceNum, ptrFace, &swTessellation);
			//设置镶嵌对象
			swTessellation->put_NeedFaceFacetMap(VARIANT_TRUE);
			swTessellation->put_NeedVertexParams(VARIANT_TRUE);
			swTessellation->put_NeedVertexNormal(VARIANT_TRUE);
			//设置选项来改进返回数据的质量
			swTessellation->put_ImprovedQuality(VARIANT_TRUE);
			//如何处理匹配的普通交叉边
			swTessellation->put_MatchType(swTesselationMatchFacetTopology);
			//执行镶嵌
			VARIANT_BOOL var_flag;
			swTessellation->Tessellate(&var_flag);
			//得到顶点和面片的数量
			long vertexNum = 0;
			swTessellation->GetVertexCount(&vertexNum);
			long facetsNum = 0;
			swTessellation->GetFacetCount(&facetsNum);
			//加速
			swModel->SetAddToDB(VARIANT_TRUE);
			swModel->SetDisplayWhenAdded(VARIANT_FALSE);
			//插入草图
			swModel->Insert3DSketch2(VARIANT_FALSE);
			//现在得到每个面的面片数据
			CComPtr<IFace2> swFace;
			swBody->IGetFirstFace(&swFace);
			while (swFace)
			{
				long* pFacetArray = NULL;
				long nFacetIndex = -1;
				long nFacetCount = -1;

				swTessellation->IGetFaceFacetsCount2(swFace, &nFacetCount);

				if (nFacetCount > 0) {
					VARIANT aFacetIds;
					swTessellation->GetFaceFacets(swFace, &aFacetIds);
					SAFEARRAY* pFacetIds = V_ARRAY(&aFacetIds);

					res = SafeArrayAccessData(pFacetIds, (void**)&pFacetArray);		//获取实体
					res = SafeArrayGetUBound(pFacetIds, 1, &nFacetIndex);			//获取实体的数量
					nFacetCount = nFacetIndex + 1;
					for (int j = 0; j < nFacetCount; j++)
					{
						VARIANT aFinIds;
						swTessellation->GetFacetFins(pFacetArray[j], &aFinIds);
						SAFEARRAY* pFinsIds = V_ARRAY(&aFinIds);
						long* pFinsArray = NULL;
						res = SafeArrayAccessData(pFinsIds, (void**)&pFinsArray);		//获取实体
						Facet tmpFacet;
						myPoint pt1, pt2;
						//每个面片应该总会有三个边
						for (int k = 0; k < 3; k++)
						{
							VARIANT aVertexIds;
							swTessellation->GetFinVertices(pFinsArray[k], &aVertexIds);
							SAFEARRAY* pVertexIds = V_ARRAY(&aVertexIds);
							long* pVertexArray = NULL;
							res = SafeArrayAccessData(pVertexIds, (void**)&pVertexArray);		//获取实体

																								//每条边应该有两个顶点
							VARIANT aVertexCoords1;
							swTessellation->GetVertexPoint(pVertexArray[0], &aVertexCoords1);
							SAFEARRAY* pVertexCoords1Ids = V_ARRAY(&aVertexCoords1);
							double* pVertexCoords1Array = NULL;
							res = SafeArrayAccessData(pVertexCoords1Ids, (void**)&pVertexCoords1Array);

							VARIANT aVertexCoords2;
							swTessellation->GetVertexPoint(pVertexArray[1], &aVertexCoords2);
							SAFEARRAY* pVertexCoords2Ids = V_ARRAY(&aVertexCoords2);
							double* pVertexCoords2Array = NULL;
							res = SafeArrayAccessData(pVertexCoords2Ids, (void**)&pVertexCoords2Array);

							pt1.x = pVertexCoords1Array[0];
							pt1.y = pVertexCoords1Array[1];
							pt1.z = pVertexCoords1Array[2];

							pt2.x = pVertexCoords2Array[0];
							pt2.y = pVertexCoords2Array[1];
							pt2.z = pVertexCoords2Array[2];

							if (k == 0)
							{
								tmpFacet.m_point[0] = pt1;
								tmpFacet.m_point[1] = pt2;
							}
							else if (k == 1)
							{
								if (pt1 != tmpFacet.m_point[0] && pt1 != tmpFacet.m_point[1])
								{
									tmpFacet.m_point[2] = pt1;
								}
								else if (pt2 != tmpFacet.m_point[0] && pt2 != tmpFacet.m_point[1])
								{
									tmpFacet.m_point[2] = pt2;
								}
							}
							SafeArrayUnaccessData(pVertexIds);
							SafeArrayUnaccessData(pVertexCoords1Ids);
							SafeArrayUnaccessData(pVertexCoords2Ids);
							//释放指针
							SafeArrayDestroy(pVertexIds);
							SafeArrayDestroy(pVertexCoords1Ids);
							SafeArrayDestroy(pVertexCoords2Ids);
							//delete[] pVertexArray;
							pVertexArray = NULL;
							//delete[] pVertexCoords2Array;
							pVertexCoords2Array = NULL;
							//delete[] pVertexCoords1Array;
							pVertexCoords1Array = NULL;
						}

						//将坐标单位由m->mm,，扩大1000倍
						for (int k = 0; k < 3; k++)
						{
							tmpFacet.m_point[k].x = 1000 * tmpFacet.m_point[k].x;
							tmpFacet.m_point[k].y = 1000 * tmpFacet.m_point[k].y;
							tmpFacet.m_point[k].z = 1000 * tmpFacet.m_point[k].z;
						}

						//利用海伦公式求面积，a、b、c代表三边长，p代表周长的一半
						double a = 0, b = 0, c = 0, p = 0, ans = 0;
						a = ComputeTwoPointDistance(tmpFacet.m_point[0], tmpFacet.m_point[1]);
						b = ComputeTwoPointDistance(tmpFacet.m_point[0], tmpFacet.m_point[2]);
						c = ComputeTwoPointDistance(tmpFacet.m_point[1], tmpFacet.m_point[2]);

						p = (a + b + c) / 2;

						ans = p*(p - a)*(p - b)*(p - c);
						if (ans >= 0) {
							tmpFacet.m_area = sqrt(ans);
						}
						else {
							tmpFacet.m_area = 0;
						}

						//存储到面片数组之中
						m_multiFacets.push_back(tmpFacet);

						SafeArrayUnaccessData(pFinsIds);
						SafeArrayDestroy(pFinsIds);
						//delete[] pFinsArray;
						pFinsArray = NULL;
					}
					SafeArrayUnaccessData(pFacetIds);
					SafeArrayDestroy(pFacetIds);
					//delete[] pFacetArray;
					pFacetArray = NULL;
				}

				CComPtr<IFace2> swNextFace;
				swFace->IGetNextFace(&swNextFace);
				swFace.Release();
				swFace = swNextFace;
			}
			//关闭草图
			swModel->Insert3DSketch2(VARIANT_TRUE);
			//清除选择便于下一次传递
			swModel->ClearSelection();
			//恢复设置
			swModel->SetAddToDB(VARIANT_FALSE);
			swModel->SetDisplayWhenAdded(VARIANT_TRUE);
			//delete[] ptrFace;
			ptrFace = NULL;
		}
	}
	SafeArrayUnaccessData(psaBody);
	SafeArrayDestroy(psaBody);
	//delete[] pBodyDispArray;
	pBodyDispArray = NULL;
}


//返回两点间的欧式距离
double Cswobj::ComputeTwoPointDistance(const myPoint &p1, const myPoint &p2)
{
	double dis1 = 0, dis2 = 0, dis3 = 0;
	dis1 = p1.x - p2.x;
	dis2 = p1.y - p2.y;
	dis3 = p1.z - p2.z;
	return sqrt(dis1*dis1 + dis2*dis2 + dis3*dis3);
}
