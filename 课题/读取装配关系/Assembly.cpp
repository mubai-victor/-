#include "stdafx.h"
#include "Assembly.h"
#include <assert.h>

#define TOLERANCE 0.0005

Assembly::Assembly()
{
	count = 0;
	vRelations.clear();
	vParts.clear();
}
Assembly::~Assembly()
{
	count = 0;
	for (int i = 0; i < count; i++) {
		vParts[i].vfaces.clear();
	}
	vParts.clear();
}
void Assembly::AddPart(CString name,long num, CComPtr<IComponent> component)
{
	part.count = num;
	part.name = name;
	part.vfaces.clear();
	part.part = component;
}
void Assembly::AddPartFace(CComPtr<IFace2> newFace)
{
	part.vfaces.push_back(newFace);
}
void Assembly::CheckRelation()
{
	long result;
	AssemRelation relation;
	CComPtr<IEntity> entity1,entity2;
	CComPtr<IModelDoc2> m_iModelDoc;
	double point1[3], point2[3];
	double retval;
	VARIANT_BOOL val;

	for (int i = 0; i < count; i++) {
		for (int j = i+1; j < count; j++) {
			if (i != j) {
				m_iModelDoc->IClosestDistance(vParts[i].part, vParts[j].part, point1, point2, &retval);
				if (retval < TOLERANCE) {
					relation.part1=vParts[i].part;
					relation.part2=vParts[j].part;
					for(int m=0;m<vParts[i].count;m++){
						for(int n=0;n<vParts[j].count;m++){
							m_iModelDoc->IClosestDistance(vParts[i].vfaces[m], vParts[j].vfaces[n], point1, point2, &retval);
							if(retval<TOLERANCE){
								entity1 = entity2 = NULL;
								entity1 = vParts[i].vfaces[m];
								entity2 = vParts[j].vfaces[n];
								entity1->Select(VARIANT_TRUE,&val);
								entity2->Select(VARIANT_TRUE,&val);
							}
						}
					}
				}
			}
		}
	}

	//for (int i = 0; i < count; i++) {
	//	for (int j = 0; j < count; j++) {
	//		if (i != j) {
	//			for (int k = 0; k < vParts[i].count; k++) {
	//				for (int l = 0; l < vParts[j].count; l++) {
	//					m_iModelDoc->IClosestDistance(vParts[i].vfaces[k], vParts[j].vfaces[l], point1, point2, &retval);
	//					if (retval < TOLERANCE) {
	//						vParts[i].relations++;
	//						goto here;
	//					}
	//				}
	//			}
	//		}
	//	here:;
	//	}
	//}

}

void Assembly::Add()
{
	vParts.push_back(part);
	count++;
}
int Assembly::TraverseChildren(long RecurseLevel, CString* MyString, CComPtr<IComponent> pComponent)
{
	CComPtr<IComponent> *pChildren;/*定义子零件*/
	int nChildren = 0;
	int i = 0;
	BSTR Name;
	HRESULT h_res = S_OK;
	CComPtr<IModelDoc2> m_iModelDoc;
	CComPtr<IModelDoc> m_iModelDoc1;

	VARIANT vMass;
	VariantInit(&vMass);
	vMass.vt = VT_ARRAY;
	CString zhongliang[3];

	CString tempstr;

	if (RecurseLevel == 0) {
		h_res = m_iSldWorks->get_IActiveDoc2(&m_iModelDoc);/*获得当前活动文档对象*/
		if (S_OK == h_res || m_iModelDoc != NULL) {
			h_res = m_iModelDoc->GetTitle(&Name);/*获得当前活动文档的名称*/
		}
	}
	else {
		h_res = pComponent->get_Name(&Name);//获得当前组件的名称
		pComponent->IGetModelDoc(&m_iModelDoc1);//获得当前零件的ModelDoc对象
												/*  ****************************************************/
		CComPtr<IBody> Body;
		CComPtr<IDispatch>dispatch;
		CComPtr<IFace2>subfaces;
		CComPtr<IEntity>entity;
		VARIANT_BOOL bRet;
		long faceCount = 0;
		pComponent->IGetBody(&Body);
		Body->GetFirstFace(&dispatch);
		Body->GetFaceCount(&faceCount);

		entity = dispatch;
		subfaces = dispatch;
		CString c1 = "grip-3", c2 = "Enlace2-4";
		AddPart(Name, faceCount,pComponent);
		AddPartFace(subfaces);

		//		entity->Select(VARIANT_TRUE, &bRet);
		while (faceCount > 1) {
			dispatch = NULL;
			subfaces->GetNextFace(&dispatch);
			entity = dispatch;
			subfaces = dispatch;
			//			entity->Select(VARIANT_TRUE, &bRet);
			AddPartFace(subfaces);
			faceCount--;
		}

		Add();
		/* ****************************************************/
		m_iModelDoc1->GetMassProperties(&vMass);//获得组件的质量属性
		SafeDoubleArray safeMass(&vMass);
		for (int i = 0; i < 3; i++) {
			double weight = safeMass[i];
			weight *= 1000;
			zhongliang[i].Format("%f", weight);
		}
	}
	if (S_OK == h_res&&Name != NULL) {

		for (int i = 1; i <= RecurseLevel; i++) {
			tempstr += " ";
		}
		CString Tmp(Name);
		double weight;
		tempstr = tempstr + Tmp + "  (";
		for (int i = 0; i < 3; i++) {
			tempstr = tempstr + zhongliang[i] + ',';
		}
		tempstr += ')';
		tempstr += "\r\n";
		*MyString = *MyString + tempstr;
	}
	RecurseLevel++;
	h_res = pComponent->IGetChildrenCount(&nChildren);//获得组件下的子零件个数
	if (S_OK == h_res || nChildren > 0) {//检查当前组件下有没有零件
		pChildren = new CComPtr<IComponent>[nChildren];
		h_res = pComponent->IGetChildren((IComponent ***)&pChildren);//获得子零件
		if (S_OK == h_res) {
			for (int i = 0; i < nChildren; i++) {
				TraverseChildren(RecurseLevel, MyString, pChildren[i]);//递归遍历子零部件
				pChildren[i] = NULL;//释放子零件对象
			}
		}
		delete[] pChildren;
	}
	RecurseLevel--;
	return nChildren;
}

void Assembly::Traverse()
{
	HRESULT rs;
	CComPtr<IModelDoc2>m_iModelDoc;
	CComPtr<IConfiguration>pConfiguration;
	long RecurseLevel = 0;
	CString MyString;
	CComPtr<IComponent>pRootComponent;
	rs = m_iSldWorks->get_IActiveDoc2(&m_iModelDoc);
	m_iModelDoc->IGetActiveConfiguration(&pConfiguration);
	pConfiguration->IGetRootComponent(&pRootComponent);
	TraverseChildren(RecurseLevel, &MyString, pRootComponent);
	pRootComponent = NULL;
	AfxMessageBox(MyString);
	pConfiguration = NULL;

	CheckRelation();

	MyString.Empty();
	CString temp;
	for (int i = 0; i < count; i++) {
		MyString += vParts[i].name;
		MyString += ":";
		temp.Empty();
		temp.Format("%d", vParts[i].relations);
		MyString += temp;
		MyString += "\n";
	}

	AfxMessageBox(MyString);

	MyString.Empty();
	vParts.clear();
	m_iModelDoc = NULL;
	count = 0;
}

void Assembly::Set(CComPtr<ISldWorks>m_iSldWorks)
{
	this->m_iSldWorks = m_iSldWorks;
}