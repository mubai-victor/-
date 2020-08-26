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

	std::vector<CString> fileNames;			//����һ���ַ�����̬����
	std::string fileName;
	std::ifstream file, clear;
	char buff[SIZE];
	CComPtr<IModelDoc2> swModel;	//����ModelDoc����

	file.open("E:\\Desktop\\TempFile\\path.txt");
	if (!file) {
		AfxMessageBox("Can not open the file 'path.txt'");
		return S_OK;
	}
	clear.open("E:\\Desktop\\TempFile\\information.txt");/*ÿ�������µ�����֮��Ҫ���ϴ����ɵ����������������ͼ�ṹ�����ݺ���������֣���Ŷ��ձ�*/
	clear.close();
	clear.open("E:\\Desktop\\TempFile\\map.txt");
	clear.close();
	clear.open("E:\\Desktop\\TempFile\\map.txt");
	clear.close();

	while (file) {//���ļ������ж�ȡװ�������� 
		file.getline(buff,SIZE);		//��װ��������ת����ʽ��ĩβ���������У�װ���������а���װ�������ڵ�����·����
		fileNames.push_back(buff);
	}
	file.close();						//�ر�������

	//fileNames.resize(1);
	for (auto name : fileNames) {
		CComBSTR sAssemblyName(name);
		if (name.GetLength() == 0) {
			continue;
		}

		CComBSTR sDefaultConfiguration(L"Default");											//����Ĭ��������
		long lErrors, lWarnings;
		swModel = NULL;												 //�ͷŵ�ǰ�򿪵�װ��������ָ��
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
		TraverseChildren(RecurseLevel, pRootComponent);/*��ȡװ�������������*/

		_assembly->resizeRelationVector();/*����assembly��relation��Ա������ά�Ĵ�С��*/
		_assembly->setCurs2Pointer();

		GetEveryAsseShapeDistributions(swModel);/*��ȡװ�������������״�ֲ�*/
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
					pMate2->GetMateEntityCount(&mateCount);/*��ȡ����Ϲ�ϵԼ����entity����*/
					mate._mateType = mateType;

					pMateEntity.Release();/*��ȡ�μ���ϵ���������*/
					pMate2->MateEntity(0, &pMateEntity);
					pDispatch.Release();
					pMateEntity->get_Reference(&pDispatch);
					mate._entity1 = pDispatch;

					pMateEntity.Release();
					if (mateType == swMateWIDTH) {/*�����Ϲ�ϵ������ϵĶ������ĸ�����ȡ��һ���͵������Ϳ����ˡ�*/
						pMate2->MateEntity(2, &pMateEntity);
					}
					else {
						pMate2->MateEntity(1, &pMateEntity);
					}
					pDispatch.Release();
					pMateEntity->get_Reference(&pDispatch);
					mate._entity2 = pDispatch;

					_assembly->addRelation(mate);/*����ϵ������Ϣ��ֵ��assembly��Ա����*/
					pNextSubFeature.Release();
					pSubFeature->IGetNextSubFeature(&pNextSubFeature);/*��ȡ��һ���������*/
					pSubFeature = pNextSubFeature;

				}
			}

			pNextFeature.Release();
			pFeature->IGetNextFeature(&pNextFeature);/*��ȡ��һ��������ֱ���������е�����*/
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
	CComPtr<IComponent> *pChildren;/*���������*/
	int nChildren = 0;
	int i = 0;
	BSTR Name;
	HRESULT h_res = S_OK;
	CComPtr<IModelDoc2> m_iModelDoc;
	CComPtr<IModelDoc> m_iModelDoc1;
	VARIANT_BOOL result;

	if (RecurseLevel == 0) {
		h_res = m_iSldWorks->get_IActiveDoc2(&m_iModelDoc);/*��õ�ǰ��ĵ�����*/
		if (S_OK == h_res || m_iModelDoc != NULL) {
			h_res = m_iModelDoc->GetTitle(&Name);/*��õ�ǰ��ĵ�������*/
		}
	}

	RecurseLevel++;
	h_res = pComponent->IGetChildrenCount(&nChildren);//�������µ����������

	if (nChildren == 0) {//����ȡ�����������������µ��������Ϊ0ʱ��˵���������ǵ����������������ǣ���ݹ������
		_assembly->addPart(pComponent);
	}

	if (S_OK == h_res || nChildren > 0) {//��鵱ǰ�������û�����
		pChildren = new CComPtr<IComponent>[nChildren];
		h_res = pComponent->IGetChildren((IComponent ***)&pChildren);//��������
		if (S_OK == h_res) {
			for (int i = 0; i < nChildren; i++) {
				TraverseChildren(RecurseLevel, pChildren[i]);//�ݹ�������㲿��
				pChildren[i] = NULL;//�ͷ����������
			}
		}
		delete[] pChildren;
	}

	RecurseLevel--;
	return 0;
}

//2.��ȡÿ��װ�������״�ֲ�
void Cswobj::GetEveryAsseShapeDistributions(const CComPtr<IModelDoc2> & swAssemDoc)
{
	HRESULT rs;			//���ؽ����־
	long docType = -1;	//�����ĵ�����
	swAssemDoc->GetType(&docType);	//��ȡ�ĵ�����
	if (docType != swDocASSEMBLY) {
		AfxMessageBox(_T("�򿪵Ĳ���װ����"), MB_OK | MB_ICONINFORMATION);
		return;						//����
	}
	//����ÿһ��װ���壬�õ�����ֵ
	TraverseEveryAssembly(swAssemDoc);
	return;
}


void Cswobj::TraverseEveryAssembly(const CComPtr<IModelDoc2>& swAssemDoc)
{
	long RecurseLevel = 0;							//����ݹ鼶��
	CComPtr<IConfiguration> pConfiguration;			//�������ö���			
	CComPtr<IComponent>	pRootComponent = NULL;		//������������

	swAssemDoc->IGetActiveConfiguration(&pConfiguration);	//������ö���
	pConfiguration->IGetRootComponent(&pRootComponent);		//��ø��������

															//���õ�λ��ϵΪMMGS
	CComPtr<IModelDocExtension> docExtension;
	swAssemDoc->get_Extension(&docExtension);
	VARIANT_BOOL flag;
	docExtension->SetUserPreferenceInteger(swUnitSystem, swDetailingNoOptionSpecified, swUnitSystem_MMGS, &flag);
	//����װ��������������
	TraverseChildrenComponent(swAssemDoc, RecurseLevel, pRootComponent);
	return;
}

int Cswobj::TraverseChildrenComponent(const CComPtr<IModelDoc2> &swAssemDoc, long RecurseLevel, CComPtr<IComponent> pComponent)
{
	srand(time(NULL));
	std::default_random_engine ran(time(NULL));						//�����������
	CComPtr<IComponent> *pChildren = NULL;		//���������
	int nChildren;								//�������������
	CComBSTR Name;								//�����������
	HRESULT hres = S_OK;						//���ý���ı�־											
	pComponent->IGetChildrenCount(&nChildren);	//��ȡ���ӵĸ���
	if (RecurseLevel == 0) {					//�ж�RecurseLevel��ֵ�Ƿ�Ϊ0 ���ݹ鼶��Ϊ0�������ʼ��װ����  Recurse:�ݹ�
												//Ϊ��װ���屾��
	}
	else if (RecurseLevel >= 1 && nChildren > 0)
	{
		//Ϊ��װ���壬�����κδ���
	}
	else {
		//�ݹ鼶��Ϊ0����ʾ����װ����������������װ�����е����
		hres = pComponent->get_Name(&Name);
		//�洢��ǰ�������,Ԥ����ռ�
		m_partmodel.m_PartName = (_bstr_t)Name;
		m_partmodel.component = pComponent;
		m_partmodel.shapeDistributions.resize(512);

		VARIANT value;
		VariantInit(&value);
		value.vt = VT_ARRAY;
		//��ȡ����İ�Χ�нǵ�����
		pComponent->GetBox(false, false, &value);
		SafeDoubleArray box(&value);
		//box[0],box[1],box[2]�ֱ�洢��Χ�нǵ��������Сֵ��Ҳ�������½ǵ�����
		m_partmodel.box[0] = min(box[0], box[3]);
		m_partmodel.box[1] = min(box[1], box[4]);
		m_partmodel.box[2] = min(box[2], box[5]);
		//box[3],box[4],box[5]�ֱ�洢��Χ�нǵ���������ֵ��Ҳ�����Ͻǵ�����
		m_partmodel.box[3] = max(box[0], box[3]);
		m_partmodel.box[4] = max(box[1], box[4]);
		m_partmodel.box[5] = max(box[2], box[5]);



		//�Ե�ǰ���ģ�ͽ�����Ƕ��������ȡ�����������Ƭ�Ķ���
		TessellationPartModel(pComponent);

		//���������С��������
		sort(m_multiFacets.begin(), m_multiFacets.end(), cmp);

		distance.resize(1048576);
		//������״ͼ���飬Ϊһ��Ͱ��ֵȫ����ʼ��Ϊ0
		int histogram[512];
		memset(histogram, 0, sizeof(histogram));

		//��ȡ�ۼ�����ı���(���ۼ�������������֮�ͣ�
		int iSize = m_multiFacets.size();
		double AccuAreaSum = 0;
		for (int i = 0; i < iSize; i++)
		{
			AccuAreaSum += m_multiFacets[i].m_area;
			accumuArea.push_back(AccuAreaSum);
		}

		//������С����������룬��С���븳��ֵ�������븳Сֵ
		double mindist = 1000000, maxdist = -1;
		double randomArea = 0.0;
		//�������1024*1024�Σ�������ŷ�Ͼ���
		for (int i = 0; i < 1048576; i++)
		{
			myPoint newPoint[2];				//���������
												//ÿ�β������ȡ�����������
			for (int j = 0; j < 2; j++)
			{
				std::uniform_real_distribution<double> dis1(0.0, AccuAreaSum);
				randomArea = dis1(ran);		//��0��������֮�����һ�������
				
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

				//��������������Ƭ����������
				myPoint &A = m_multiFacets[index].m_point[0];
				myPoint &B = m_multiFacets[index].m_point[1];
				myPoint &C = m_multiFacets[index].m_point[2];
				std::uniform_real_distribution<double> dis2(0.0, 1.0);
				double r1 = dis2(ran);						//����0-1֮��������
				double r2 = dis2(ran);
				//double r1 = (double)rand() / RAND_MAX;
				//double r2 = (double)rand() / RAND_MAX;
				//��������������
				newPoint[j].x = (1 - sqrt(r1))*A.x + sqrt(r1)*(1 - r2)*B.x + sqrt(r1)*r2*C.x;
				newPoint[j].y = (1 - sqrt(r1))*A.y + sqrt(r1)*(1 - r2)*B.y + sqrt(r1)*r2*C.y;
				newPoint[j].z = (1 - sqrt(r1))*A.z + sqrt(r1)*(1 - r2)*B.z + sqrt(r1)*r2*C.z;
			}
			//�������������֮��ľ���
			distance[i] = ComputeTwoPointDistance(newPoint[0], newPoint[1]);

			if (distance[i] < mindist)
			{//����СֵС�����滻
				mindist = distance[i];
			}
			if (distance[i] > maxdist)
			{//�����ֵ�����滻
				maxdist = distance[i];
			}

		}
		//���飬������״�ֲ�,������״ͼÿ��bin�Ŀ��
		double gap = (maxdist - mindist) / 512;

		//ͳ������ÿ��Ͱ�ĸ���
		int id = 0;
		for (int i = 0; i < 1048576; i++)
		{		//histogramΪ����ҿ������һ��ֵ���⴦��
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

		m_assemblymodel.multiPartModel.push_back(m_partmodel);	//���ݴ���

																//�رյ�ǰ���ģ��
																//CComBSTR title;
																//swCurPartModel->GetTitle(&title);
																//m_iSldWorks->QuitDoc(title);

																//�ͷŵ���ǰ�������
		m_multiFacets.clear();		//�ͷŴ洢������Ƭ������
		accumuArea.clear();			//�ͷŴ洢�ۻ��������
		distance.clear();
		m_partmodel.PartRelease();					//�ͷŵ�ǰ���ģ��
	}
	ASSERT(S_OK == hres);

	RecurseLevel++;

	hres = pComponent->IGetChildrenCount(&nChildren);	//�����������������

	if (S_OK == hres || nChildren > 0) {	//�ж���һ�������Ƿ�ɹ���������ĸ���
		pChildren = new CComPtr<IComponent>[nChildren];			//��ʼ���ַ�������

		hres = pComponent->IGetChildren((IComponent ***)&pChildren);	//��������
		if (S_OK == hres) {
			for (int i = 0; i < nChildren; i++) {
				TraverseChildrenComponent(swAssemDoc, RecurseLevel, pChildren[i]);	//�ݹ�������㲿��
				pChildren[i] = NULL;									//�ͷ����������
			}
		}
		delete[] pChildren;
	}
	RecurseLevel--;
	return nChildren;			//�������������
}

//3.�洢�õ�����Ϣ
void Cswobj::PrintData(const AssemblyModel & asse, std::map<CComPtr<IComponent>, int>pointer2curs, const std::string & path)
{
	//�����ļ����·��
	//std::string fileOutputPath = SameDirection + asse.m_AssemblyName + ".txt";

	std::ofstream fout(path, std::ios_base::out|std::ios_base::app);
	//fout << asse.pathName << std::endl;					//���װ����·������
	fout << "#" << asse.multiPartModel.size() << std::endl;		//������ģ�͵�����

	for (int i = 0; i < asse.multiPartModel.size(); i++)
	{
		//������ģ�͵����
		fout << pointer2curs[asse.multiPartModel[i].component] << std::endl;//��Ϊ�������������Ĵ�С����������˳�����������˳����
		for (int j = 0; j < 512; j++)
		{//�����״�ֲ������ݣ�ÿ��16������64��
			fout << asse.multiPartModel[i].shapeDistributions[j] << " ";
			if ((j + 1) % 32 == 0)
				fout << std::endl;
		}
		fout << "box:" << asse.multiPartModel[i].box[0] << "," << asse.multiPartModel[i].box[1] << "," << asse.multiPartModel[i].box[2] << "," 
			<< asse.multiPartModel[i].box[3] << "," << asse.multiPartModel[i].box[4] << "," << asse.multiPartModel[i].box[5] << std::endl;
	}
	
	fout.close();		//�ر��ļ�
}

//�����ģ�ͽ�����Ƕ����
void Cswobj::TessellationPartModel(const CComPtr<IComponent> &pComponent)
{
	HRESULT res;
	//��ȡ�����Ӧ�ļ����ĵ�
	CComPtr<IModelDoc> swModel;
	CComPtr<IPartDoc> swPart;
	pComponent->IGetModelDoc(&swModel);
	//���ĵ�ָ�봫�ݸ����
	swPart = swModel;

	//�õ�����ʵ��
	VARIANT vBodies;
	swPart->GetBodies2(swAllBodies, VARIANT_FALSE, &vBodies);
	//ѭ����������
	ASSERT(NULL != vBodies.pparray);
	SAFEARRAY	*psaBody = V_ARRAY(&vBodies);
	LPDISPATCH	*pBodyDispArray = NULL;
	long nBodyHighIndex = -1;
	long nBodyCount = -1;

	res = SafeArrayAccessData(psaBody, (void**)&pBodyDispArray);		//��ȡʵ��
	ASSERT(res == S_OK);
	ASSERT(NULL != pBodyDispArray);
	res = SafeArrayGetUBound(psaBody, 1, &nBodyHighIndex);				//��ȡʵ�������
	ASSERT(S_OK == res);
	nBodyCount = nBodyHighIndex + 1;
	CComQIPtr<IBody2> swBody;
	for (int i = 0; i < nBodyCount; i++)			//����ʵ�壬�õ�ÿһ��ʵ��
	{
		//�õ�һ��ʵ��
		swBody = pBodyDispArray[i];
		ASSERT(swBody);
		//���ڵ�����ʵ���ϵ�һ������
		long nBodyType = 0;
		swBody->GetType(&nBodyType);
		if (nBodyType == swSolidBody)
		{
			CComPtr<ITessellation> swTessellation;
			long faceNum = 0;
			swBody->GetFaceCount(&faceNum);
			IFace2 **ptrFace = new IFace2*[faceNum];
			swBody->IGetFaces(faceNum, ptrFace);
			//����գ��������ʵ�彫�ᱻ��Ƕ
			swBody->IGetTessellation(faceNum, ptrFace, &swTessellation);
			//������Ƕ����
			swTessellation->put_NeedFaceFacetMap(VARIANT_TRUE);
			swTessellation->put_NeedVertexParams(VARIANT_TRUE);
			swTessellation->put_NeedVertexNormal(VARIANT_TRUE);
			//����ѡ�����Ľ��������ݵ�����
			swTessellation->put_ImprovedQuality(VARIANT_TRUE);
			//��δ���ƥ�����ͨ�����
			swTessellation->put_MatchType(swTesselationMatchFacetTopology);
			//ִ����Ƕ
			VARIANT_BOOL var_flag;
			swTessellation->Tessellate(&var_flag);
			//�õ��������Ƭ������
			long vertexNum = 0;
			swTessellation->GetVertexCount(&vertexNum);
			long facetsNum = 0;
			swTessellation->GetFacetCount(&facetsNum);
			//����
			swModel->SetAddToDB(VARIANT_TRUE);
			swModel->SetDisplayWhenAdded(VARIANT_FALSE);
			//�����ͼ
			swModel->Insert3DSketch2(VARIANT_FALSE);
			//���ڵõ�ÿ�������Ƭ����
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

					res = SafeArrayAccessData(pFacetIds, (void**)&pFacetArray);		//��ȡʵ��
					res = SafeArrayGetUBound(pFacetIds, 1, &nFacetIndex);			//��ȡʵ�������
					nFacetCount = nFacetIndex + 1;
					for (int j = 0; j < nFacetCount; j++)
					{
						VARIANT aFinIds;
						swTessellation->GetFacetFins(pFacetArray[j], &aFinIds);
						SAFEARRAY* pFinsIds = V_ARRAY(&aFinIds);
						long* pFinsArray = NULL;
						res = SafeArrayAccessData(pFinsIds, (void**)&pFinsArray);		//��ȡʵ��
						Facet tmpFacet;
						myPoint pt1, pt2;
						//ÿ����ƬӦ���ܻ���������
						for (int k = 0; k < 3; k++)
						{
							VARIANT aVertexIds;
							swTessellation->GetFinVertices(pFinsArray[k], &aVertexIds);
							SAFEARRAY* pVertexIds = V_ARRAY(&aVertexIds);
							long* pVertexArray = NULL;
							res = SafeArrayAccessData(pVertexIds, (void**)&pVertexArray);		//��ȡʵ��

																								//ÿ����Ӧ������������
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
							//�ͷ�ָ��
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

						//�����굥λ��m->mm,������1000��
						for (int k = 0; k < 3; k++)
						{
							tmpFacet.m_point[k].x = 1000 * tmpFacet.m_point[k].x;
							tmpFacet.m_point[k].y = 1000 * tmpFacet.m_point[k].y;
							tmpFacet.m_point[k].z = 1000 * tmpFacet.m_point[k].z;
						}

						//���ú��׹�ʽ�������a��b��c�������߳���p�����ܳ���һ��
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

						//�洢����Ƭ����֮��
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
			//�رղ�ͼ
			swModel->Insert3DSketch2(VARIANT_TRUE);
			//���ѡ�������һ�δ���
			swModel->ClearSelection();
			//�ָ�����
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


//����������ŷʽ����
double Cswobj::ComputeTwoPointDistance(const myPoint &p1, const myPoint &p2)
{
	double dis1 = 0, dis2 = 0, dis3 = 0;
	dis1 = p1.x - p2.x;
	dis2 = p1.y - p2.y;
	dis3 = p1.z - p2.z;
	return sqrt(dis1*dis1 + dis2*dis2 + dis3*dis3);
}
