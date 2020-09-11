#include "stdafx.h"
#include "Assembly.h"
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>


std::vector<std::string> ConnectorList = {
	"screw","bearing","nut","pin"
};

void Assembly::addPart(CComPtr<IComponent>part)
{
	_pointer2curs[part] = _part.size();
	_part.push_back(part);


	if (isConnector(part) == true) {/*�����ǰ����������Ӽ����򽫵�ǰ�������ӵ�connector��Ա������*/
		addConnector(part);
	}
}

void Assembly::resizeRelationVector()
{
	int size = _part.size();

	_relation.resize(size);/*�ڻ�ȡ��װ����������ĸ���֮�󣬽�_relation��Ա�����е�ÿһά���ó���������Ĵ�С*/
	for (int i = 0; i < size; i++) {
		_relation[i].resize(size);
	}
}

void Assembly::addConnector(CComPtr<IComponent>connector)
{
	_connector.insert(connector);/*�����Ӽ�Connector���뵽��¼���Ӽ��ļ�����*/
}

void Assembly::addRelation(Mate mate)
{
	CComPtr<IComponent> component;
	Mate temp;
	int curs1 = 0, curs2 = 0;
	
	mate._entity1->IGetComponent(&component);/*��õ�ǰ�μ�װ���Ԫ�أ��棬�ߣ��㣩���������*/
	curs1 = _pointer2curs[component];/*ͨ�����ָ��-�����_part�е���ŵĶ��ձ��ȡ�����_part�е���ţ��Ա����뵽_relation��*/

	component = NULL;
	mate._entity2->IGetComponent(&component);/*ͬ��*/
	curs2 = _pointer2curs[component];

	if (curs1 < curs2) {
		_relation[curs1][curs2].push_back(mate);/*����ǰװ���ϵ���뵽_relation��,���е�һά���±�С�ڵڶ�ά������*/
	}
	else {
		temp._entity1 = mate._entity2;/*���뵽vector�е�Mate��ϵentity1��Ӧ���ǵ�һά��ŵ������entity2��Ӧ���ǵڶ�ά��ŵ����*/
		temp._entity2 = mate._entity1;
		temp._mateType = mate._mateType;

		_relation[curs2][curs1].push_back(temp);
	}
}

void Assembly::getDOF(std::vector<std::vector<MidDOF>> &result)
{
	int size = _part.size();

	result.resize(size);
	for (int i = 0; i < size; i++) {
		result[i].resize(size);
	}

	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			bool status = false;
			MidDOF resultDOF;

			for (Mate dof : _relation[i][j]) {
				if (status == false) {/*��һ��װ���ϵҪ��resultDOF��ֵ���ں�ߵ�װ���ϵ�Ż���װ���ϵ�Ĺ�Լ*/
					switch (dof._mateType) {
					case swMateCOINCIDENT:resultDOF = coincidentDOF(dof);break;/*���ݲ�ͬ��װ���ͷֱ���*/
					case swMateCONCENTRIC:resultDOF = concentricDOF(dof);break;
					case swMatePERPENDICULAR:resultDOF = perpendicularDOF(dof);break;
					case swMateDISTANCE:resultDOF = distanceDOF(dof);break;
					case swMateTANGENT:resultDOF = tangentDOF(dof);break;
					case swMateANGLE:resultDOF = angleDOF(dof);break;
					case swMateWIDTH:resultDOF = widthDOF(dof);break;
					case swMatePARALLEL:resultDOF = parallelDOF(dof); break;
					case swMateSYMMETRIC:resultDOF = symmetricDOF(dof); break;
					case swMateCAMFOLLOWER:resultDOF = camfollowerDOF(dof); break;
					case swMateGEAR:resultDOF = gearDOF(dof); break;
					case swMateRACKPINION:resultDOF = rackpinionDOF(dof); break;
					case swMatePATH:resultDOF = pathDOF(dof); break;
					case swMateSCREW:resultDOF = screwDOF(dof); break;
					}
					status = true;
				}
				else {
					MidDOF midDOF;

					switch (dof._mateType) {
					case swMateCOINCIDENT:midDOF = coincidentDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;/*���ݲ�ͬ��װ���ͷֱ���*/
					case swMateCONCENTRIC:midDOF = concentricDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					case swMatePERPENDICULAR:midDOF = perpendicularDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					case swMateDISTANCE:midDOF = distanceDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					case swMateTANGENT:midDOF = tangentDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					case swMateANGLE:midDOF = angleDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					case swMateWIDTH:midDOF = widthDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					case swMatePARALLEL:midDOF = parallelDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;
					}
				}
			}

			result[i][j] = resultDOF;/*���յ����ɶ�ֵ��ֵ�����������*/
		}
	}
}

MidDOF Assembly::setDOF(MidDOF dof1, MidDOF dof2)
{
	MidDOF dof;

	if ((0 == dof1._countT) || (0 == dof2._countT)) {/*������ɶȶ�û��ƽ�����ɶȣ����Լ��û��ƽ�����ɶȡ�*/
		dof._countT = 0;
	}
	else if ((3 == dof1._countT) || (3 == dof2._countT)) {/*����������ɶ���һ�������������ϵ�ƽ�����ɶȣ����Լ�����ɶ�Ϊ��һ����ƽ�����ɶ�*/
		if (3 == dof1._countT) {
			dof._countT = dof2._countT;
			dof._transform[0] = dof2._transform[0];/*��Ϊƽ�����ɶ����ֻ��Ҫһ���洢�ռ䣨һ��ʱ���䷽������ʱ��ƽ�淨�߷�������ʱ���ô棩��ֻ��Ҫ����һ����ֵ�Ϳ�����*/
		}
		else {
			dof._countT = dof1._countT;
			dof._transform[0] = dof1._transform[0];/*ͬ��*/
		}
	}
	else if (1 == dof1._countT && 1 == dof2._countT) {/*����������ɶȶ�ֻ��һ��ƽ�����ɶ�*/
		double angle = cosAngle(dof1._transform[0],dof2._transform[0]);/*����������ƽ�����ɶȵļнǵ�����ֵ*/

		if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*���ƽ�����ɶȵķ�����ͬ�����෴����Լ��õ�һ��ƽ�����ɶ�*/	
			dof._countT = 1;
			dof._transform[0] = dof1._transform[0];

		}
		else {/*�����ͬ�����Լ���û��ƽ�����ɶ�*/
			dof._countT = 0;
		}
	}
	else if ((1 == dof1._countT && 2 == dof2._countT) || (2 == dof1._countT && 1 == dof2._countT)) {/*����������ɶȷֱ���һ��������ƽ�����ɶ�*/
		double angle = cosAngle(dof1._transform[0], dof2._transform[0]);/*���㵥��ƽ�����ɶȷ������������ƽ�����ɶȹ���ƽ��ķ��ߵļнǵ�����*/

		if (fabs(angle) < 10E-4) {/*�����ֱ�����Լ��ʣ��һ�����ɶȣ������뵥��ƽ�����ɶȵķ�����ͬ*/
			dof._countT = 1;
			
			if (1 == dof1._countT) {/*�ж��Ǹ����ɶȵ�ƽ�����ɶ�����Ϊһ*/
				dof._transform[0] = dof1._transform[0];
			}
			else {
				dof._transform[0] = dof2._transform[0];
			}
		}
		else {
			dof._countT = 0;/*���һ��ƽ�����ɶȺ�����ƽ�����ɹ��ɵ�ƽ��ķ��߲���ֱ�����Լ���û��ƽ�����ɶ�*/
		}
	}
	else if (2 == dof1._countT && 2 == dof2._countT) {/*����������ɶȵ�ƽ�����ɶȶ�������*/
		double angle= cosAngle(dof1._transform[0], dof2._transform[0]);/*�������������ɶȵ�ƽ�����ɶ��γɵ�ƽ��ķ��ߵļнǵ�����ֵ*/

		if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*�������ƽ��ķ�����ƽ�У�ͬ����򣩣����Լ������ɶ�Ϊ����*/
			dof._countT = 2;
			dof._transform[0] = dof1._transform[0];
		}
		else {/*������߲�ƽ�У����Լ������ɶ�ֻ��һ��������Ϊ���������߲�˵���������*/
			std::vector<double> dir=multiply(dof1._transform[0]._x, dof1._transform[0]._y, dof1._transform[0]._z, /*�������߲��*/
											 dof2._transform[0]._x, dof2._transform[0]._y, dof2._transform[0]._z);

			dof._countT = 1;
			dof._transform[0]._x = dir[0];
			dof._transform[0]._y = dir[1];
			dof._transform[0]._z = dir[2];
		}
	}


	if (0 == dof1._countR || 0 == dof2._countR) {/*����������ɶȶ�û��ת�����ɶȣ���Լ���ת�����ɶȻ���Ϊ��*/
		dof._countR = 0;
	}
	else{
		dof._countR = 0;/*Ԥ���趨��ֵΪ0���±߻�����������*/

		for (int i = 0; i < dof1._countR; i++) {
			for (int j = 0; j < dof2._countR; j++) {/*��Ҫ�����������ɶ��е�����ת�����ɶ������й�Լ*/
				if (GIVEN_AXIS == dof1._rotation[i]._type&&GIVEN_AXIS == dof2._rotation[j]._type) {/*�������ת�����ɶȶ��Ǹ������ת�����ɶ�*/
					Axis axis;
					double angle = cosAngle(dof1._rotation[i]._axis, dof2._rotation[j]._axis);/*���ȼ���������ļнǵ�����ֵ*/

					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*����������߷�����ͬ���ټ����������Ƿ���*/
						axis._x = dof1._rotation[i]._point._x - dof2._rotation[j]._point._x;
						axis._y = dof1._rotation[i]._point._y - dof2._rotation[j]._point._y;/*�����������㹹���µ�����*/
						axis._z = dof1._rotation[i]._point._z - dof2._rotation[j]._point._z;

						angle = cosAngle(dof1._rotation[i]._axis, axis);/*���߼��㷽����������������㹹�ɵ�������ԭ�ȵ���ķ�����ƽ�еģ���˵�������Ṳ��*/
						if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {
							dof._rotation[dof._countR] = dof1._rotation[i];/*������������ԭ�ȵ���ƽ�У�������ת�����ɶȹ��ߣ���Լ���һ��ת�����ɶȣ���������ԭ������һ��ͬ����*/
							dof._countR++;/*ת�����ɶ�������*/
						}
					}
				}
				else if (GIVEN_DIR == dof1._rotation[i]._type&&GIVEN_DIR == dof2._rotation[j]._type) {/*�������ת�����ɶȶ��Ǹ��������*/
					double angle = cosAngle(dof1._rotation[i]._axis, dof2._rotation[j]._axis);/*������ת�����ɶȵ����߷���ļнǵ�����*/

					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*�������ת�����ɶ����ߵķ�����ͬ���෴����Լ�õ�һ������ת����������ɶȣ���ԭ������һ����ͬ����*/
						dof._rotation[dof._countR] = dof1._rotation[i];
						dof._countR++;
					}
				}
				else if (GIVEN_POINT == dof1._rotation[i]._type&&GIVEN_POINT == dof2._rotation[j]._type) {/*�������ת�����ɶȶ��Ǹ������ת�����ɶ�*/
					if (fabs(distant(dof1._rotation[i]._point._x, dof1._rotation[i]._point._y, dof1._rotation[i]._point._z,
						dof2._rotation[j]._point._x, dof2._rotation[j]._point._y, dof2._rotation[j]._point._z)) < 10E-6) {/*����������Ƿ񹲵㣨����Ϊ�㣩*/

						dof._rotation[dof._countR] = dof1._rotation[i];/*������㣬��Լ��õ�һ���������ת�����ɶ�*/
						dof._countR++;
					}
				}
				else if ((GIVEN_AXIS == dof1._rotation[i]._type&&GIVEN_DIR == dof2._rotation[j]._type) ||
					(GIVEN_DIR == dof1._rotation[i]._type&&GIVEN_AXIS == dof2._rotation[j]._type)) {/*�������ת�����ɶȷֱ�Ϊ��������͸������ת�����ɶ�*/
					double angle = cosAngle(dof1._rotation[i]._axis, dof2._rotation[j]._axis);/*������ת����ļнǵ�����ֵ*/

					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*���ͬ��������Լ��õ�һ��ת�����ɶ�*/
						if (GIVEN_AXIS == dof1._rotation[i]._type) {
							dof._rotation[dof._countR] = dof1._rotation[i];
						}
						else {
							dof._rotation[dof._countR] = dof2._rotation[j];
						}

						dof._countR++;
					}
				}
				else if ((GIVEN_AXIS == dof1._rotation[i]._type&&GIVEN_POINT == dof2._rotation[j]._type) ||/*�������ת�����ɶȷֱ��ǹ�����͹������ת�����ɶ�*/
					(GIVEN_POINT == dof1._rotation[i]._type&&GIVEN_AXIS == dof2._rotation[j]._type)) {
					Axis constructAxis, originAxis;
					double angle;

					constructAxis._x = dof1._rotation[i]._point._x - dof2._rotation[j]._point._x;/*�ö���Ͷ������㹹�������������Զ����Ƿ��ڶ�����*/
					constructAxis._y = dof1._rotation[i]._point._y - dof2._rotation[j]._point._y;
					constructAxis._z = dof1._rotation[i]._point._z - dof2._rotation[j]._point._z;

					if (GIVEN_AXIS == dof1._rotation[i]._type) {
						originAxis = dof1._rotation[i]._axis;
					}
					else {
						originAxis = dof2._rotation[j]._axis;
					}

					angle = cosAngle(constructAxis, originAxis);
					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*��������ڶ����ϣ���Լ�õ���ת�����ɶ���һ����ԭ�ȶ����ת�����ɶ���ͬ�����ɶ�*/
						dof._rotation[dof._countR]._type = GIVEN_AXIS;
						dof._rotation[dof._countR]._axis = constructAxis;
						dof._rotation[dof._countR]._point = dof1._rotation[i]._point;
						dof._countR++;
					}
				}
				else if ((GIVEN_DIR == dof1._rotation[i]._type&&GIVEN_POINT == dof2._rotation[j]._type) ||/*���ת�����ɶȷֱ��ǹ������ָ��������������ת�����ɶ�*/
					(GIVEN_POINT == dof1._rotation[i]._type&&GIVEN_DIR == dof2._rotation[j]._type)) {
					if (GIVEN_DIR == dof1._rotation[i]._type) {/*��Լ��õ�һ������Ϊԭ��ָ���������ɶȵķ������Ϊԭ�ȶ���Ĺ̶����ת�����ɶ�*/
						dof._rotation[dof._countR]._axis = dof1._rotation[i]._axis;
						dof._rotation[dof._countR]._point = dof2._rotation[j]._point;
					}
					else {
						dof._rotation[dof._countR]._axis = dof2._rotation[j]._axis;
						dof._rotation[dof._countR]._point = dof1._rotation[i]._point;
					}

					dof._rotation[dof._countR]._type = GIVEN_AXIS;
					dof._countR++;
				}
			}
		}
	}

	return dof;
}

MidDOF Assembly::concentricDOF(Mate mate)
{
	MidDOF dof;/*dof�洢����ʣ������ɶȸ���������*/
	long type1, type2;
	long mateType = -1;
	double param1[8],param2[8];

	mate._entity1->GetType(&type1);/*��ȡ����װ�������Ԫ�ص����ͣ��������ͽ������ɶ��ж�*/
	mate._entity2->GetType(&type2);

	if (type1 == swSelFACES&&type2 == swSelFACES) {
		CComPtr<IFace2> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);

		surface1->Identity(&type1);
		surface2->Identity(&type2);

		if (type1 == SPHERE_TYPE&&type2 == SPHERE_TYPE) {
			dof._countR = 1;/*��������ͬ�ģ���һ������������������ɶȣ�û��ƽ�����ɶ�*/
			dof._countT = 0;

			surface1->get_ISphereParams(param1);

			dof._rotation[0]._type = GIVEN_POINT;/*��������ͬ�ģ���һ������������������ɶ�*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._y = param1[2];
		}
		else if (type1 == CYLINDER_TYPE&&type2 == CYLINDER_TYPE) {
			mateType = 1;

			surface1->get_ICylinderParams(param1);
		}
		else if (type1 == CONE_TYPE&&type2 == CONE_TYPE) {
			mateType = 1;

			surface1->get_IConeParams(param1);
		}
		else if ((type1 == SPHERE_TYPE&&type2 == CYLINDER_TYPE) || (type1 == CYLINDER_TYPE&&type2 == SPHERE_TYPE)) {
			if (type1 == CYLINDER_TYPE) {
				surface1->get_ICylinderParams(param1);
				surface2->get_ISphereParams(param2);
			}
			else {
				surface2->get_ICylinderParams(param1);
				surface1->get_ISphereParams(param2);
			}

			dof._countR = 1;/*��������ͬ�ģ���һ�������ĵ����ɶȣ�һ�����������ߵ�ƽ�����ɶ�*/
			dof._countT = 1;

			dof._transform[0]._x = param1[3];/*ƽ�����ɶ�*/
			dof._transform[0]._y = param1[4];
			dof._transform[0]._z = param1[5];

			dof._rotation[1]._type = GIVEN_POINT;/*����������������ɶ�*/
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
		}
		else if ((type2 == CONE_TYPE&&type1 == CYLINDER_TYPE) || (type1 == CONE_TYPE&&type2 == CYLINDER_TYPE)) {
			mateType = 1;

			if (type1 == CYLINDER_TYPE) {
				surface1->get_ICylinderParams(param1);
			}
			else {
				surface2->get_ICylinderParams(param1);
			}
		}
	}
	else if (type1 == swSelEDGES&&type2 == swSelEDGES) {
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;
		long type1 = 0, type2 = 0;

		edge1 = mate._entity1;
		edge1->IGetCurve(&curve1);
		curve1->Identity(&type1);
		edge2 = mate._entity2;
		edge2->IGetCurve(&curve2);
		curve2->Identity(&type2);

		if (type1 == CIRCLE_TYPE&&type2 == CIRCLE_TYPE) {
			mateType = 1;
			curve1->get_ICircleParams(param1);
		}
	}
	else if ((type1 == swSelFACES&&type2 == swSelEDGES) || (type1 == swSelEDGES&&type2 == swSelFACES)) {
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace2> face;
		CComPtr<ISurface> surface;

		if (type1 == swSelEDGES) {
			edge = mate._entity1;
			face = mate._entity2;
		}
		else {
			edge = mate._entity2;
			face = mate._entity1;
		}
		edge->IGetCurve(&curve);
		face->IGetSurface(&surface);

		curve->Identity(&type1);
		surface->Identity(&type2);

		if (type1 == CIRCLE_TYPE && (type2 == CONE_TYPE || type2 == CYLINDER_TYPE)) {
			mateType = 1;
			curve->get_ICircleParams(param1);
		}
	}

	if (mateType == 1) {
		dof._countR = 1;/*��һ�������ߵ�ת�����ɶȣ�һ���������ߵ�ƽ�����ɶ�*/
		dof._countT = 1;

		dof._rotation[0]._type = GIVEN_AXIS;/*�����ߵ�ת�����ɶ�*/
		dof._rotation[0]._axis._x = param1[3];
		dof._rotation[0]._axis._y = param1[4];
		dof._rotation[0]._axis._z = param1[5];
		dof._rotation[0]._point._x = param1[0];
		dof._rotation[0]._point._y = param1[1];
		dof._rotation[0]._point._z = param1[2];

		dof._transform[0]._x = param1[3];/*�����ߵ�ƽ�����ɶ�*/
		dof._transform[0]._y = param1[4];
		dof._transform[0]._z = param1[5];
	}

	 return dof;	
}

MidDOF Assembly::coincidentDOF(Mate mate)
{
	MidDOF dof;
	long type1, type2;
	double param1[8], param2[8];

	mate._entity1->GetType(&type1);
	mate._entity2->GetType(&type2);

	if (type1 == swSelFACES&&type2 == swSelFACES) {/*�������װ�������Ԫ�ض�����*/
		CComPtr<IFace2> face1, face2;
		CComPtr<ISurface> surface1, surface2;
		long type1, type2;

		face1 = mate._entity1;/*��ȡ�������surface���ͱ���*/
		face1->IGetSurface(&surface1);
		face2 = mate._entity2;
		face2->IGetSurface(&surface2);

		surface1->Identity(&type1);/*��ȡ����surface��������ͣ������ڿɽ������棬��Բ���棬Բ׶�棬ƽ�棬���棬Բ����*/
		surface2->Identity(&type2);

		if (type1 == PLANE_TYPE&&type2 == PLANE_TYPE) {
			dof._countT = 2;/*����Ӵ���ʣ������ƽ�����ɶȣ�һ����ת���ɶ�*/
			dof._countR = 1;

			surface1->get_IPlaneParams(param1);

			dof._rotation->_type = GIVEN_DIR;/*����ƽ�淨�ߵ�ת�����ɶ�*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._transform[0]._x = param1[0];/*������ƽ�����ɶȣ�����ֻ��Ҫ��¼ƽ��ķ��߼���*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
	}
	else if (type1 == swSelVERTICES&&type2 == swSelVERTICES) {/*�������װ���Ԫ�ض��ǵ�*/
		CComPtr<IVertex> vertex;
		
		vertex = mate._entity1;
		vertex->IGetPoint(param1);

		dof._countR = 1;/*����غϻ�ʣ��һ���ƹ̶����ת�����ɶȣ�û��ƽ�����ɶ�*/
		dof._countT = 0;

		dof._rotation[0]._type = GIVEN_POINT;		
		dof._rotation[0]._point._x = param1[0];
		dof._rotation[0]._point._y = param1[1];
		dof._rotation[0]._point._z = param1[2]; 
	}
	else if (type1 == swSelEDGES&&type2 == swSelEDGES) {/*���������ϵ�Ԫ�ض��Ǳ�*/
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;
		edge1->IGetCurve(&curve1);
		edge1->IGetCurve(&curve2);
		curve1->Identity(&type1);
		curve2->Identity(&type2);

		if (type1 == LINE_TYPE&&type2 == LINE_TYPE) {/*����������߶���ֱ������*/
			dof._countR = 1;/*ֱ�ߺ�ֱ�ߵ��غ�Լ����ʣ��һ��ƽ�����ɶȣ�һ��ת�����ɶ�*/
			dof._countT = 1;

			curve1->get_ILineParams(param1);

			dof._rotation[0]._type = GIVEN_AXIS;/*ת�����ɶ�����������ֱ�߹�����Ĺ̶������ɶ�*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._z = param1[4];
			dof._rotation[0]._axis._y = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._transform[0] = dof._rotation[0]._axis;/*ƽ�����ɶ�������ֱ�߷�����ƶ����ɶ�*/
		}
		else if (type1 == CIRCLE_TYPE&&type1 == CIRCLE_TYPE) {/*���������Բ�ߵ��غ�*/
			dof._countR = 1;/*����Բ�غϻ�ʣ��һ��ת�����ɶȣ�û��ƽ�����ɶ�*/
			dof._countT = 0;

			curve1->get_ICircleParams(param1);

			dof._rotation[0]._type = GIVEN_AXIS;/*����Բ�����ߵĹ̶����ת�����ɶ�*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];
		}
	}
	else if ((type1 == swSelEDGES&&type2 == swSelFACES) || (type1 == swSelFACES&&type2 == swSelEDGES)) {/*�������װ�������Ԫ�طֱ����ߺ���*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace2> face;
		CComPtr<ISurface> surface;

		if (type1 == swSelEDGES) {/*��edge��face�ֱ�����Ӧ��Ԫ��*/
			edge = mate._entity1;
			face = mate._entity2;
		}
		else {
			edge = mate._entity2;
			face = mate._entity1;
		}
		edge->IGetCurve(&curve);
		face->IGetSurface(&surface);

		curve->Identity(&type1);
		surface->Identity(&type2);
		if (type1 == LINE_TYPE) {
			curve->get_ILineParams(param1);/*�����Ԫ����ֱ�ߣ����ȡ���ֱ�ߵĲ���*/

			if (PLANE_TYPE == type2) {/*�����Ԫ����ƽ��*/
				dof._countR = 2;/*ֱ�ߣ�ƽ����غ�Լ����ʣ������ת�����ɶȣ�����ƽ�����ɶ�*/
				dof._countT = 2;
				
				surface->get_IPlaneParams(param2);

				dof._rotation[0]._type = GIVEN_AXIS;/*һ����ֱ�ߵĹ̶����ת�����ɶ�*/
				dof._rotation[0]._axis._x = param1[3];
				dof._rotation[0]._axis._y = param1[4];
				dof._rotation[0]._axis._z = param1[5];
				dof._rotation[0]._point._x = param1[0];
				dof._rotation[0]._point._y = param1[1];
				dof._rotation[0]._point._z = param1[2];

				dof._rotation[1]._type = GIVEN_DIR;/*һ����ƽ�淨�߷�����������ת�����ɶ�*/
				dof._rotation[1]._axis._x = param2[0];
				dof._rotation[1]._axis._y = param2[1];
				dof._rotation[1]._axis._z = param2[2];

				dof._transform[0]= dof._rotation[1]._axis;/*ƽ�����ɶ�Ϊ��ƽ���ڵ�ƽ�ƣ���¼��ƽ��ķ��߷���*/
			}
		}
		else if (CIRCLE_TYPE == type1) {/*�����Ԫ����Բ����Ԫ����ƽ��*/
			if (PLANE_TYPE == type2) {
				dof._countT = 2;/*���ߺ����غ��൱������Ӵ�����ʣ������ƽ�����ɶȣ�һ����ת���ɶ�*/
				dof._countR = 1;

				surface->get_IPlaneParams(param1);
				dof._rotation->_type = GIVEN_DIR;/*��ƽ��ķ��߷�����������ת�����ɶ�	*/			
				dof._rotation[0]._axis._x = param1[0];
				dof._rotation[0]._axis._y = param1[1];
				dof._rotation[0]._axis._z = param1[2];

				dof._transform[0] = dof._rotation[0]._axis;/*ƽ�Ʒ�����ƽ���У���¼����ķ��߷���*/
			}
		}
	}
	else if ((type1 == swSelVERTICES&&type2 == swSelFACES) || (type1 == swSelFACES&&type2 == swSelVERTICES)) {/*���������ϵ�Ԫ��һ���ǵ���һ������*/
		CComPtr<IFace2> face;
		CComPtr<ISurface> surface;
		CComPtr<IVertex> vertex;

		if (type1 == swSelFACES) {/*���ߣ���Ԫ�طֱ�ֵ����Ӧ��Ԫ��*/
			face = mate._entity1;
			vertex = mate._entity2;
		}
		else {
			face = mate._entity2;		
			vertex = mate._entity1;
		}

		face->IGetSurface(&surface);
		surface->Identity(&type2);
		vertex->IGetPoint(param1);

		if (PLANE_TYPE == type2) {/*�����Ԫ����ƽ��*/
			surface->get_IPlaneParams(param2);

			dof._countR = 2;/*�㣬ƽ��ĽӴ�Լ����ʣ������ƽ���ڵ�ƽ�����ɶȣ�����ת�����ɶ�*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_POINT;/*��һ��ת�����ɶ��ǹ�����������ת�����ɶ�*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_DIR;/*�ڶ����ǹ�ƽ�淨�߷����ϵ��������ת�����ɶ�*/
			dof._rotation[1]._axis._x = param2[0];
			dof._rotation[1]._axis._y = param2[1];
			dof._rotation[1]._axis._z = param2[2];

			dof._transform[0] = dof._rotation[1]._axis;/*ƽ�����ɶ���ƽ���У���¼��ƽ��ķ��߷���*/

		}
	}
	else if ((type1 == swSelVERTICES&&type2 == swSelEDGES) || (type1 == swSelEDGES&&type2 == swSelVERTICES)) {/*����Լ����Ԫ���ǵ�ͱ�*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IVertex> vertex;
		double param[8];

		if (type1 == swSelEDGES) {/*����ͱ߸�ֵ����Ӧ��ָ��*/
			edge = mate._entity1;
			vertex = mate._entity2;
		}
		else {
			edge = mate._entity2;
			vertex = mate._entity1;
		}
		edge->IGetCurve(&curve);		
		curve->Identity(&type2);
		vertex->IGetPoint(param1);

		if (LINE_TYPE == type2) {/*�����Ԫ����ƽ��*/
			curve->get_ILineParams(param2);

			dof._countR = 1;/*����Ӵ���ʣ��һ��ת�����ɶȣ�һ��ƽ�����ɶ�*/
			dof._countT = 1;

			dof._rotation[0]._type = GIVEN_POINT;/*������������ת�����ɶ�*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._transform[0]._x = param2[3];/*�����߷����ƽ�����ɶ�*/
			dof._transform[0]._y = param2[4];
			dof._transform[0]._z = param2[5];
		}
	}

	for (int i = 0; i < 3; i++) {
		dof._rotation[i]._axis._x *= 1000;
		dof._rotation[i]._axis._y *= 1000;
		dof._rotation[i]._axis._z *= 1000;

		dof._rotation[i]._point._x *= 1000;
		dof._rotation[i]._point._y *= 1000;
		dof._rotation[i]._point._z *= 1000;

		dof._transform[i]._x *= 1000;
		dof._transform[i]._y *= 1000;
		dof._transform[i]._z *= 1000;
	}

	return dof;
}

MidDOF Assembly::perpendicularDOF(Mate mate)
{
	MidDOF dof;
	double param1[8], param2[8];
	int flag = 0;
	long type1, type2;

	mate._entity1->GetType(&type1);//*��ȡ����װ�������Ԫ�ص����ͣ��������ͽ������ɶ��ж�
	mate._entity2->GetType(&type2);

	if (swSelEDGES == type1&&swSelEDGES == type2) {
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;
		edge1->IGetCurve(&curve1);
		edge2->IGetCurve(&curve2);
		curve1->Identity(&type1);
		curve2->Identity(&type2);

		if (LINE_TYPE == type1&&LINE_TYPE == type2) {/*����������Ԫ�ض����ߣ���Ҫ��ȡ�ߵ����ͣ�ֱ�ߣ�*/
			flag = 1;
			curve1->get_ILineParams(param1);
			curve2->get_ILineParams(param2);
		}
	}
	else if ((swSelEDGES == type1&&swSelFACES == type2) || (swSelFACES == type1&&swSelEDGES == type2)) {/*����������Ԫ�طֱ�����ͱ�*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace> face;
		CComPtr<ISurface> surface;

		if (swSelEDGES == type1) {/*�����Ԫ���б�Ԫ�ظ�ֵ��edge����Ԫ�ظ�ֵ��face*/
			edge = mate._entity1;
			face = mate._entity2;
		}
		else {
			edge = mate._entity2;
			face = mate._entity1;
		}

		edge->IGetCurve(&curve);
		face->IGetSurface(&surface);
		curve->Identity(&type1);
		surface->Identity(&type2);

		if (LINE_TYPE == type1) {
			curve->get_ILineParams(param1);/*�����Ԫ����ֱ�ߣ���ȡֱ�ߵĲ��������������������ֱ�ߵ����*/

			if (PLANE_TYPE == type2) {/*����μ���ϵ���Ԫ����ƽ�棬��ȡƽ��Ĳ���*/
				surface->get_IPlaneParams(param2);

				dof._countR = 1;/*ֱ�ߣ�ƽ��Ĵ�ֱԼ��ʣ������ɶȺ����������Ͳ�ͬ����Ҫ��������*/
				dof._countT = 3;

				dof._rotation[0]._type = GIVEN_DIR;/*ֱ�ߣ�ƽ��Ĵ�ֱԼ����ʣ������ƽ�����ɶȣ�һ����ƽ�淢�ַ�����������ת�����ɶ�*/
				dof._rotation[0]._axis._x = param2[0];
				dof._rotation[0]._axis._y = param2[1];
				dof._rotation[0]._axis._z = param2[2];
			}
			else if (CYLINDER_TYPE == type2) {/*���������ϵ���Ԫ�������棬���ȡ����Ĳ���*/
				flag = 1;
				surface->get_ICylinderParams(param2);
			}
			else if (CONE_TYPE == type2) {/*���������ϵ���Ԫ����׶�棬���ȡ׶��Ĳ���*/
				flag = 1;
				surface->get_IConeParams(param2);
			}
		}
	}
	else if (swSelFACES == type1&&swSelFACES == type2) {/*���������ϵ�����Ԫ�ض�����Ԫ��*/
		CComPtr<IFace> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		surface1->Identity(&type1);
		surface2->Identity(&type2);

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {/*���������ϵ��涼��ƽ��*/
			flag = 1;
			surface1->get_IPlaneParams(param1);
			surface2->get_IPlaneParams(param2);

			for (int i = 0; i < 3; i++) {
				param1[i + 3] = param1[i];/*ƽ��ķ��߲�����param[0-2]�У�ҪǨ�Ƶ�param[3-5]�У��Ա�ͳһ����*/
				param2[i + 3] = param2[i];
			}
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2){ /*���������ϵ��涼�����棬���ȡ����Ĳ���*/
			flag = 1;

			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);
		}
		else if (CONE_TYPE == type1&&CONE_TYPE == type2) {/*���������ϵ��涼��Բ׶�棬���ȡԲ׶��Ĳ���*/
			flag = 1;

			surface1->get_IConeParams(param1);
			surface2->get_IConeParams(param2);
		}
		else if ((CONE_TYPE == type1&&CYLINDER_TYPE == type2) || (CYLINDER_TYPE == type1&&CONE_TYPE == type2)) {/*���������ϵ����������Բ׶��*/
			flag = 1;

			if (CONE_TYPE == type1) {/*��Բ׶��Ĳ�������param1�У�����Ĳ�������param2��*/
				surface1->get_IConeParams(param1);
				surface2->get_ICylinderParams(param2);
			}
			else {
				surface1->get_ICylinderParams(param1);
				surface2->get_IConeParams(param2);
			}
		}
	}

	if (1 == flag) {/*���flag����һ��˵����Ҫͳһ����*/
		dof._countR = 2;/*���ϵ�Լ����ʣ�����������ϵ�ƽ�����ɶȣ����������ϵ�ת�����ɶ�*/
		dof._countT = 3;

		dof._rotation[0]._type = GIVEN_DIR;/*�ص�һ�����߷�����������ϵ�ת�����ɶ�*/
		dof._rotation[0]._axis._x = param1[3];
		dof._rotation[0]._axis._y = param1[4];
		dof._rotation[0]._axis._z = param1[5];

		dof._rotation[1]._type = GIVEN_DIR;/*�صڶ������߷����ϵ�ת�����ɶ�*/
		dof._rotation[1]._axis._x = param2[3];
		dof._rotation[1]._axis._y = param2[4];
		dof._rotation[1]._axis._z = param2[5];
	}
	return dof;
}

MidDOF Assembly::tangentDOF(Mate mate)
{
	MidDOF dof;
	CComPtr<IFace2> face1, face2;
	CComPtr<ISurface> surface1, surface2;
	long type1, type2;
	double param1[8], param2[8];

	mate._entity1->GetType(&type1);/*��ȡ����װ�������Ԫ�ص����ͣ��������ͽ������ɶ��ж�*/
	mate._entity2->GetType(&type2);

	if (type1 == swSelFACES && type2 == swSelFACES) {
		CComPtr<ISurface> surface1, surface2;
		CComPtr<IDispatch> dispatch1, dispatch2;
		CComPtr<IComponent> component1, component2;
		VARIANT_BOOL retval;
		HRESULT result;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		
		surface1->Identity(&type1);/*����������*/
		surface2->Identity(&type2);

		if ((type1 == CYLINDER_TYPE&&type2 == PLANE_TYPE) || (type1 == PLANE_TYPE&&type2 == CYLINDER_TYPE)) {

			if (type1 == PLANE_TYPE) {
				surface1->get_IPlaneParams(param1);
				surface2->get_ICylinderParams(param2);
			}
			else {
				surface2->get_IPlaneParams(param1);
				surface1->get_ICylinderParams(param2);
			}

			dof._countR = 2;
			dof._countT = 2;

			dof._rotation[0]._axis._x = param1[0];/*��ƽ��ķ��ߵ�ת�����ɶȣ�Ϊ��������������ת�����ɶ�*/
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];
			dof._rotation[0]._type = GIVEN_DIR;

			dof._rotation[1]._axis._x = param2[3];/*����������ߵ�ת�����ɶȣ�Ϊ�ƹ̶����ת�����ɶ�*/
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
			dof._rotation[1]._type = GIVEN_AXIS;

			dof._transform[0]._x = param1[0];/*��������ƽ���ƽ�����ɶȣ�ֻ��Ҫ��¼���ƽ��ķ��߼���*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
		else if ((type1 == SPHERE_TYPE&&type2 == PLANE_TYPE) || (type1 == PLANE_TYPE&&type2 == SPHERE_TYPE)) {
			if (type1 == PLANE_TYPE) {
				surface1->get_IPlaneParams(param1);
				surface2->get_ISphereParams(param2);
			}
			else {
				surface2->get_IPlaneParams(param1);
				surface1->get_ISphereParams(param2);
			}

			dof._countR = 2;/*���ƽ�����л�������ƽ�Ƶ����ɶȣ�����ת�����ɶȣ�һ���������ĵ������ᣬ��һ������ƽ���ϵ�����һ�����ߣ�*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_POINT;/*һ��ת�������ɶ����ƹ����ĵ�����һ����*/
			dof._rotation[0]._point._x = param2[0];
			dof._rotation[0]._point._y = param2[1];
			dof._rotation[0]._point._z = param2[2];

			dof._rotation[1]._type = GIVEN_DIR;/*��һ��ת�����ɶ�����ƽ�������һ������*/
			dof._rotation[1]._axis._x = param1[0];
			dof._rotation[1]._axis._y = param1[1];
			dof._rotation[1]._axis._z = param1[2];

			dof._transform[0]._x = param1[0];/*��������ƽ���ƽ�����ɶȣ�ֻ��Ҫ��¼���ƽ��ķ��߼���*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
		else if (type1 == CYLINDER_TYPE&&type2 == CYLINDER_TYPE) {
			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);

			dof._countR = 2;/*������������У����У�������ת�����ɶȣ�һ�������Լ����ת������һ��������һ����ת�������ɶ�*/
			dof._countT = 1;/*��������������ߵ�ƽ�����ɶ�*/

			dof._rotation[0]._type = GIVEN_AXIS;/*��һ���̶���ת�������ɶ�*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*����һ���̶���ת�������ɶ�*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param1[3];/*ƽ�Ƶ����ɶȣ��������������*/
			dof._transform[0]._y = param1[4];
			dof._transform[0]._z = param1[5];
		}
	}

	return dof;
}

MidDOF Assembly::distanceDOF(Mate mate)
{
	MidDOF dof;
	long type1, type2;
	double param1[8], param2[8];

	mate._entity1->GetType(&type1);//*��ȡ����װ�������Ԫ�ص����ͣ��������ͽ������ɶ��ж�
	mate._entity2->GetType(&type2);

	if (swSelVERTICES == type1&&swSelVERTICES == type2) {
		CComPtr<IVertex> vertex1, vertex2;

		vertex1 = mate._entity1;
		vertex2 = mate._entity2;
		vertex1->IGetPoint(param1);
		vertex2->IGetPoint(param2);

		dof._countR = 2;/*������Լ����ʣ��2��ת�����ɶȣ�0��ƽ�����ɶ�*/
		dof._countT = 0;

		dof._rotation[0]._type = GIVEN_POINT;/*������Լ��ʱ���������������̶����ת�����ɶ�*/
		dof._rotation[0]._point._x = param1[0];
		dof._rotation[0]._point._y = param1[1];
		dof._rotation[0]._point._z = param1[2];

		dof._rotation[1]._type = GIVEN_POINT;
		dof._rotation[1]._point._x = param2[0];
		dof._rotation[1]._point._y = param2[1];
		dof._rotation[1]._point._z = param2[2];
	}
	else if ((swSelEDGES == type1&&swSelVERTICES == type2) || (swSelVERTICES == type1&&swSelEDGES == type2)) {
		CComPtr<IVertex> vertex;
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;

		if (swSelVERTICES == type1) {
			vertex = mate._entity1;
			edge = mate._entity2;

		}
		else {
			vertex = mate._entity2;
			edge = mate._entity1;
		}

		vertex->IGetPoint(param1);
		edge->IGetCurve(&curve);

		curve->Identity(&type1);
		if (type1 == LINE_TYPE) {
			curve->get_ILineParams(param2);

			dof._countR = 2;/*���߾���Լ�����������߷����ƽ�����ɶȣ�һ�����̶��㣨�㣩�������ת�����ɶȺ�һ������Ĺ̶�������ɶ�*/
			dof._countT = 1;

			dof._rotation[0]._type = GIVEN_POINT;/*������������ת�����ɶ�*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*���̶����ת�����ɶ�*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param2[3];/*���ߵ�ƽ�����ɶ�*/
			dof._transform[0]._y = param2[4];
			dof._transform[0]._z = param2[5];
		}
	}
	else if ((swSelVERTICES == type1&&swSelFACES == type2) || (swSelFACES == type1&&swSelVERTICES == type2)) {
		CComPtr<IVertex> vertex;
		CComPtr<IFace2> face;
		CComPtr<ISurface> surface;

		if (swSelVERTICES == type1) {
			vertex = mate._entity1;
			face = mate._entity2;
		}
		else {
			vertex = mate._entity2;
			face = mate._entity1;
		}

		face->IGetSurface(&surface);
		vertex->IGetPoint(param1);
		surface->Identity(&type1);/*����������ʩ�Ӹ�ƽ�棬���棬����*/

		if (PLANE_TYPE == type1) {
			surface->get_IPlaneParams(param2);

			dof._countR = 2;/*��ƽ�����Լ����������ת�����ɶȣ���ƽ�淨�ߵ����ɶȺ͹�������ɶȣ�����ƽ�����ɶ�*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_DIR;/*����ƽ�淨�ߵ����ɶ�*/
			dof._rotation[0]._axis._x = param2[0];
			dof._rotation[0]._axis._y = param2[1];
			dof._rotation[0]._axis._z = param2[2];

			dof._rotation[1]._type = GIVEN_POINT;/*���������ת���ɶ�*/
			dof._rotation[1]._point._x = param1[0];
			dof._rotation[1]._point._y = param1[1];
			dof._rotation[1]._point._z = param1[2];

			dof._transform[0]._x = param2[0];
			dof._transform[0]._y = param2[1];
			dof._transform[0]._z = param2[2];
		}
		else if (CYLINDER_TYPE == type1) {
			surface->get_ICylinderParams(param2);

			dof._countR = 2;/*����������Լ����ʣ�����������ߵ�ƽ�����ɶȣ��������ⷽ���ת�����ɶȣ������������ߵ�ת�����ɶ�*/
			dof._countT = 1;

			dof._rotation[0]._type = GIVEN_POINT;/*���̶����ת�����ɶ�*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*���Ź̶����ת�����ɶ�*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param2[3];/*�����������߷����ƽ�����ɶ�*/
			dof._transform[0]._y = param2[4];
			dof._transform[0]._z = param2[5];
		}
		else if (SPHERE_TYPE == type1) {
			surface->get_ISphereParams(param2);

			dof._countR = 2;/*�������֮��ľ���Լ�������������̶����ת�����ɶ�*/
			dof._countT = 0;

			dof._rotation[0]._type = GIVEN_POINT;/*������������ת�����ɶ�*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[0];
			dof._rotation[0]._point._z = param1[1];

			dof._rotation[1]._type = GIVEN_POINT;/*�����ĵ��������ת�����ɶ�*/
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
		}

	}
	else if (swSelEDGES == type1&&swSelEDGES == type2) {
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;
		double angle;

		edge1 = mate._entity1;
		edge2 = mate._entity2;
		edge1->IGetCurve(&curve1);
		edge2->IGetCurve(&curve2);

		curve1->Identity(&type1);
		curve2->Identity(&type2);

		if (LINE_TYPE == type1&&LINE_TYPE == type2) {
			curve1->get_ILineParams(param1);
			curve2->get_ILineParams(param2);

			angle = cosAngle(param1 + 3, param2 + 3);
			if (fabs(angle + 1) < 10e-6 || fabs(angle - 1) < 10e-6) {
				dof._countR = 2;/*�����ߵľ���Լ����ʣ�����űߵķ����ƽ�ƺ���������Ϊ���ߵ�ת��*/
				dof._countT = 1;

				dof._transform[0]._x = param1[3];/*���űߵķ����ƽ�����ɶ�*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
			else {
				dof._countR = 3;/*�����ߵľ���Լ����ʣ�����űߵķ����ƽ�ƺ���������Ϊ���ߵ�ת��*/
				dof._countT = 2;

				dof._transform[1]._x = param2[3];/*���űߵķ����ƽ�����ɶ�*/
				dof._transform[1]._y = param2[4];
				dof._transform[1]._z = param2[5];

				dof._rotation[2]._type = GIVEN_DIR;
				dof._rotation[2]._axis._x = param1[4] * param2[5] - param2[4] * param1[5];
				dof._rotation[2]._axis._y = -(param1[3] * param2[5] - param2[3] * param1[5]);
				dof._rotation[2]._axis._z = param1[3] * param2[4] - param2[3] * param1[4];

				dof._transform[0] = dof._rotation[2]._axis;

			}

			dof._rotation[0]._type = GIVEN_AXIS;/*��һ����Ϊת�����ת�����ɶ�*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*�ڶ�����Ϊת�����ת�����ɶ�*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
		}
	}
	else if ((swSelEDGES == type1&&swSelFACES == type2) || (swSelFACES == type1&&swSelEDGES == type2)) {
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace> face;
		CComPtr<ISurface> surface;

		if (swSelEDGES == type1) {
			edge = mate._entity1;
			face = mate._entity2;
		}
		else {
			edge = mate._entity2;
			face = mate._entity1;
		}

		edge->IGetCurve(&curve);
		face->IGetSurface(&surface);
		curve->Identity(&type1);
		surface->Identity(&type2);

		if (LINE_TYPE == type1) {
			curve->get_ILineParams(param1);

			if (PLANE_TYPE == type2) {
				surface->get_IPlaneParams(param2);

				dof._countR = 2;/*�ߺ�ƽ��֮��ľ���Լ����ʣ��2��ת�����ɶȣ�2��ƽ�����ɶ�*/
				dof._countT = 2;

				dof._rotation[0]._type = GIVEN_DIR;/*��һ�����ɶ�������ƽ��ķ��߷����������ת�����ɶ�*/
				dof._rotation[0]._axis._x = param2[0];
				dof._rotation[0]._axis._y = param2[1];
				dof._rotation[0]._axis._z = param2[2];

				dof._rotation[1]._type = GIVEN_AXIS;/*�ڶ������ɶ��������ߵķ���Ĺ̶����ת�����ɶ�*/
				dof._rotation[1]._axis._x = param1[3];
				dof._rotation[1]._axis._y = param1[4];
				dof._rotation[1]._axis._z = param1[5];
				dof._rotation[1]._point._x = param1[0];
				dof._rotation[1]._point._y = param1[1];
				dof._rotation[1]._point._z = param1[2];

				dof._transform[0]._x = param2[0];/*����ƽ�����ɶȣ���¼�������������γɵ�ƽ��ķ��߷���*/
				dof._transform[0]._y = param2[1];
				dof._transform[0]._z = param2[2];
			}
			else if (CYLINDER_TYPE == type2) {
				surface->get_ICylinderParams(param2);

				dof._countR = 2;/*�ߺ�����ľ���Լ����ʣ�����űߵķ����ƽ�ƺ����ߣ���������Ϊ���ߵ�ת��*/
				dof._countT = 1;

				dof._rotation[0]._type = GIVEN_AXIS;/*��Ϊת�����ת�����ɶ�*/
				dof._rotation[0]._axis._x = param1[3];
				dof._rotation[0]._axis._y = param1[4];
				dof._rotation[0]._axis._z = param1[5];
				dof._rotation[0]._point._x = param1[0];
				dof._rotation[0]._point._y = param1[1];
				dof._rotation[0]._point._z = param1[2];

				dof._rotation[1]._type = GIVEN_AXIS;/*��������Ϊת�����ת�����ɶ�*/
				dof._rotation[1]._axis._x = param2[3];
				dof._rotation[1]._axis._y = param2[4];
				dof._rotation[1]._axis._z = param2[5];
				dof._rotation[1]._point._x = param2[0];
				dof._rotation[1]._point._y = param2[1];
				dof._rotation[1]._point._z = param2[2];

				dof._transform[0]._x = param1[3];/*���űߵķ����ƽ�����ɶ�*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
			else if (SPHERE_TYPE == type2) {
				surface->get_ISphereParams(param2);

				dof._countR = 2;/*������߾���Լ�����������߷����ƽ�����ɶȣ�һ�����̶��㣨�㣩�������ת�����ɶȺ�һ�����ߵĹ̶�������ɶ�*/
				dof._countT = 1;

				dof._rotation[0]._type = GIVEN_POINT;/*������������ת�����ɶ�*/
				dof._rotation[0]._point._x = param2[0];
				dof._rotation[0]._point._y = param2[1];
				dof._rotation[0]._point._z = param2[2];

				dof._rotation[1]._type = GIVEN_AXIS;/*���̶����ת�����ɶ�*/
				dof._rotation[1]._axis._x = param1[3];
				dof._rotation[1]._axis._y = param1[4];
				dof._rotation[1]._axis._z = param1[4];
				dof._rotation[1]._point._x = param1[0];
				dof._rotation[1]._point._y = param1[1];
				dof._rotation[1]._point._z = param1[2];

				dof._transform[0]._x = param1[3];/*���ߵ�ƽ�����ɶ�*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
		}

	}
	else if (swSelFACES == type1&&swSelFACES == type2) {
		CComPtr<IFace> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		surface1->Identity(&type1);
		surface2->Identity(&type2);

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {
			surface1->get_IPlaneParams(param1);

			dof._countR = 1;/*ƽ���ƽ��ľ���Լ����ʣ��һ��ת�����ɶȣ�����ƽ�����ɶ�*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_DIR;/*һ��ת�����ɶ�����������ƽ��ķ��߷���*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._transform[0]._x = param1[0];/*��¼������ƽ�����ɶ��γɵ�ƽ��ķ��߷���*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];

		}
		else if ((PLANE_TYPE == type1&&CYLINDER_TYPE == type2) || (CYLINDER_TYPE == type1&&PLANE_TYPE == type2)) {
			if (PLANE_TYPE == type1) {
				surface1->get_IPlaneParams(param1);
				surface2->get_ICylinderParams(param2);
			}
			else {
				surface1->get_ICylinderParams(param2);
				surface2->get_IPlaneParams(param1);
			}

			dof._countR = 2;/*ƽ�������ľ���Լ����ʣ������ת�����ɶȣ�����ƽ�����ɶ�*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_DIR;/*һ��ʣ���ת�����ɶ�������ƽ��ķ��߷����������*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*��һ��ʣ���ת�����ɶ��������������ߵĹ̶���*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param1[0];/*��¼����һ��ƽ�����ɶ��γ���ķ��߷���*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
		else if ((PLANE_TYPE == type1&&SPHERE_TYPE == type2) || (SPHERE_TYPE == type1&&PLANE_TYPE == type2)) {
			if (PLANE_TYPE == type1) {
				surface1->get_IPlaneParams(param1);
				surface2->get_ISphereParams(param2);
			}
			else {
				surface1->get_ISphereParams(param2);
				surface2->get_IPlaneParams(param1);
			}

			dof._countR = 2;/*ƽ��������ľ���Լ����ʣ������ת�����ɶȣ�����ƽ�����ɶ�*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_POINT;/*һ��ת�����ɶ��ǹ����ĵ��������ת��*/
			dof._rotation[0]._point._x = param2[0];
			dof._rotation[0]._point._y = param2[1];
			dof._rotation[0]._point._z = param2[2];

			dof._rotation[1]._type = GIVEN_DIR;/*��һת�����ɶ�������ƽ�淨�߷����������*/
			dof._rotation[1]._point._x = param1[0];
			dof._rotation[1]._point._y = param1[1];
			dof._rotation[1]._point._z = param1[2];

			dof._transform[0]._x = param1[0];/*��¼������ƽ�Ʒ����γɵ�ƽ��ķ��߷���*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2) {
			double angle = 0;

			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);

			angle = cosAngle(param1 + 3, param2 + 3);
			if (fabs(angle + 1) < 10e-6 || fabs(angle - 1) < 10e-6) {/*�����������������ƽ�е�*/
				dof._countR = 2;/*�����ߵľ���Լ����ʣ�����űߵķ����ƽ�ƺ���������Ϊ���ߵ�ת��*/
				dof._countT = 1;

				dof._transform[0]._x = param1[3];/*���űߵķ����ƽ�����ɶ�*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
			else {
				dof._countR = 3;/*�����ߵľ���Լ����ʣ�����űߵķ����ƽ�ƺ���������Ϊ���ߵ�ת��,�����������γɵ�ƽ��ķ���Ϊ������������ɶ�*/
				dof._countT = 2;

				dof._transform[1]._x = param2[3];/*���űߵķ����ƽ�����ɶ�*/
				dof._transform[1]._y = param2[4];
				dof._transform[1]._z = param2[5];

				dof._rotation[2]._type = GIVEN_DIR;
				dof._rotation[2]._axis._x = param1[4] * param2[5] - param2[4] * param1[5];
				dof._rotation[2]._axis._y = -(param1[3] * param2[5] - param2[3] * param1[5]);
				dof._rotation[2]._axis._z = param1[3] * param2[4] - param2[3] * param1[4];

				dof._transform[0] = dof._rotation[2]._axis;

			}

			dof._rotation[0]._type = GIVEN_AXIS;/*��һ����Ϊת�����ת�����ɶ�*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*�ڶ�����Ϊת�����ת�����ɶ�*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
		}
		else if (SPHERE_TYPE == type1&&SPHERE_TYPE == type2) {
			surface1->get_ISphereParams(param1);
			surface2->get_ISphereParams(param2);

			dof._countR = 2;/*���������ľ���Լ����ʣ������ת�����ɶȣ����ƽ�����ɶ�*/
			dof._countT = 0;

			dof._rotation[0]._type = GIVEN_POINT;/*��һ��ת�����ɶ��ǹ���һ�����ĵ�����һ�����ת�����ɶ�*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_POINT;/*�ڶ���ת�����ɶ��ǹ��ڶ������ĵ�����һ�����ת�����ɶ�*/
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
		}
	}

	return dof;
}

//TODO��֧������ǣ����߽�
MidDOF Assembly::angleDOF(Mate mate)
{
	MidDOF dof;
	long type1, type2;
	double param1[8], param2[8];

	mate._entity1->GetType(&type1);//*��ȡ����װ�������Ԫ�ص����ͣ��������ͽ������ɶ��ж�
	mate._entity2->GetType(&type2);

	if (type1 == swSelFACES&&type2 == swSelFACES) {/*�������Ԫ�ض�����*/
		double param[6];
		CComPtr<IFace2> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		surface1->Identity(&type1);/*��ȡ���ߺ��������*/
		surface2->Identity(&type2);

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {/*�������Ԫ�ض���ƽ������*/
			surface1->get_IPlaneParams(param1);/*��ȡ����ƽ��Ĳ���*/
			surface2->get_IPlaneParams(param2);

			for (int i = 0; i < 3; i++) {/*��Ϊƽ��ķ��߷�����param[0-2]�У������ֵ��ʱ�����е����߶�����param[3-5]�еģ���������Ǩ��*/
				param1[i + 3] = param1[i];
				param2[i + 3] = param2[i];
			}
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2) {/*��������涼�����棬���ȡ����Ĳ���*/
			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);
		}
		else if ((CYLINDER_TYPE == type1&&CONE_TYPE == type2) || (CONE_TYPE == type1&&CYLINDER_TYPE == type2)) {/*����������������Բ׶�棬��ֱ��ȡ����*/
			if (CYLINDER_TYPE == type1) {
				surface1->get_ICylinderParams(param1);
				surface2->get_IConeParams(param2);
			}
			else {
				surface2->get_ICylinderParams(param1);
				surface1->get_IConeParams(param2);
			}
		}
		else if (CONE_TYPE == type1&&CONE_TYPE == type2) {/*��������涼��Բ׶�棬��ȡԲ׶��Ĳ���*/
			surface1->get_IConeParams(param1);
			surface2->get_IConeParams(param2);
		}
	}
	else if (type1 == swSelEDGES&&type2 == swSelEDGES) {/*�������Ԫ�ض����ߣ���ȡ�ߵ���Ӧ�Ĳ���*/
		CComPtr<ICurve> curve1, curve2;
		CComPtr<IEdge> edge1, edge2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;

		edge1->IGetCurve(&curve1);
		edge2->IGetCurve(&curve2);
		curve1->Identity(&type1);/*��ȡ���ߺ��������*/
		curve2->Identity(&type2);

		if (type1 == LINE_TYPE&&type2 == LINE_TYPE) {
			curve1->get_ILineParams(param1);
			curve2->get_ILineParams(param2);
		}
	}
	else if ((type1 == swSelEDGES&&type2 == swSelFACES) || (type1 == swSelFACES&&type2 == swSelEDGES)) {/*�������Ԫ���Ǳߺ��棬�ֱ��ȡ�����*/
		CComPtr<ICurve> curve;
		CComPtr<IEdge> edge;
		CComPtr<IFace> face;
		CComPtr<ISurface> surface;

		if (swSelEDGES == type1) {
			edge = mate._entity1;
			face = mate._entity2;
		}
		else {
			edge = mate._entity2;
			face = mate._entity1;
		}

		face->IGetSurface(&surface);
		edge->IGetCurve(&curve);
		curve->Identity(&type1);/*��ȡ���ߺ��������*/
		surface->Identity(&type2);

		if (LINE_TYPE == type1) {
			curve->get_ILineParams(param1);

			if (CYLINDER_TYPE == type2) {
				surface->get_ICylinderParams(param2);
			}
			else if (CONE_TYPE == type2) {
				surface->get_IConeParams(param2);
			}
		}
	}

	dof._countR = 2;/*���еĽǶ�Լ������ʣ���������������ת�����ɶȣ�����ƽ�����ɶ�*/
	dof._countT = 3;

	dof._rotation[0]._type = GIVEN_DIR;
	dof._rotation[0]._axis._x = param1[3];
	dof._rotation[0]._axis._y = param1[4];
	dof._rotation[0]._axis._z = param1[5];

	dof._rotation[1]._type = GIVEN_DIR;
	dof._rotation[1]._axis._x = param2[3];
	dof._rotation[1]._axis._y = param2[4];
	dof._rotation[1]._axis._z = param2[5];
	
	return dof;
}

MidDOF Assembly::parallelDOF(Mate mate)
{
	double param1[8], param2[8];
	int flag = 0;
	long type1, type2;
	MidDOF dof;

	mate._entity1->GetType(&type1);//*��ȡ����װ�������Ԫ�ص����ͣ��������ͽ������ɶ��ж�
	mate._entity2->GetType(&type2);

	if (swSelEDGES == type1&&swSelEDGES == type2) {/*�������װ�������Ԫ�ض�����*/
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;
		edge1->IGetCurve(&curve1);
		edge2->IGetCurve(&curve2);
		curve1->Identity(&type1);
		curve2->Identity(&type2);

		if (LINE_TYPE == type1&&LINE_TYPE == type2) {/*�������װ�������Ԫ�ض���ֱ�ߣ���ȡֱ�ߵĲ���*/
			flag = 1;
			curve1->get_ILineParams(param1);
		}
	}
	else if ((swSelEDGES == type1&&swSelFACES == type2) || (swSelFACES == type1&&swSelEDGES == type2)) {/*�������װ���Ԫ�طֱ�Ϊ�ߺ���*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace> face;
		CComPtr<ISurface> surface;

		if (swSelEDGES == type1) {/*�渳ֵ����ָ�룬�߸�ֵ����ָ��*/
			edge = mate._entity1;
			face = mate._entity2;
		}
		else {
			edge = mate._entity2;
			face = mate._entity1;
		}

		edge->IGetCurve(&curve);
		curve->Identity(&type1);
		face->IGetSurface(&surface);
		surface->Identity(&type2);

		if (LINE_TYPE == type1) {
			curve->get_ILineParams(param1);/*����ߵ�������ֱ�ߣ����ȡֱ�ߵĲ���*/

			if (PLANE_TYPE == type2) {/*�������װ������������ƽ�棬������ϵ����ɶ��������Ĳ�ͬ����Ҫ��������*/
				surface->get_IPlaneParams(param2);

				dof._countT = 3;/*ֱ��-ƽ���ƽ�����ʣ������ƽ�����ɶȣ�����ת�����ɶ�*/
				dof._countR = 2;

				dof._rotation[0]._type = GIVEN_DIR;/*����ֱ��ƽ�е�������ת��*/
				dof._rotation[0]._axis._x = param1[3];
				dof._rotation[0]._axis._y = param1[4];
				dof._rotation[0]._axis._z = param1[5];

				dof._rotation[1]._type = GIVEN_DIR;/*��ƽ�淨�߷�����������ת��*/
				dof._rotation[1]._axis._x = param2[0];
				dof._rotation[1]._axis._y = param2[1];
				dof._rotation[1]._axis._z = param2[2];
			}
			else if (CYLINDER_TYPE == type2 || CONE_TYPE == type2) {/*����Ԫ����Ϸ�ʽ���Ƶ����ɶȶ���һ���ģ����Ե����ͳһ����*/
				flag = 1;
			}
		}
	}
	else if (swSelFACES == type1&&swSelFACES == type2) {
		CComPtr<IFace> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		surface1->Identity(&type1);
		surface2->Identity(&type2);

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {/*�������װ�������Ԫ�ض����棬��ȡ��Ĳ���*/
			flag = 1;
			surface1->get_IPlaneParams(param1);

			param1[3] = param1[0];/*��Ϊ��ķ�����param1�������ǵ�0-2��Ԫ�أ�����������Ԫ�صķ��ߣ��ᣩ������3-5����������Ǩ�ƣ�����ͳһ����*/
			param1[4] = param1[1];
			param1[5] = param1[2];
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2) {/*�������װ���Ԫ�ض������棬��ȡ����Ĳ���*/
			flag = 1;
			surface1->get_ICylinderParams(param1);
		}
		else if ((CYLINDER_TYPE == type1&&CONE_TYPE == type2) || (CONE_TYPE == type1&&CYLINDER_TYPE == type2)) {/*�������װ���Ԫ�طֱ��������׶�棬��ȡ�����������ȡ����һ���Ϳɣ�*/
			flag = 1;
			if (CYLINDER_TYPE == type1) {/*��ȡ����Ĳ���*/
				surface1->get_ICylinderParams(param1);
			}
			else {
				surface2->get_ICylinderParams(param1);
			}
		}
		else if (CONE_TYPE == type1&&CONE_TYPE == type2) {/*�������װ���Ԫ����׶�棬��ȡ����һ��׶��Ĳ���*/
			flag = 1;
			surface1->get_IConeParams(param1);
		}
	}

	if (1 == flag) {/*ͳһ������󶼻�ʣ������ƽ�����ɶȣ��Լ�һ��ת�����ɶ�*/
		dof._countR = 1;
		dof._countT = 3;

		dof._rotation[0]._type = GIVEN_DIR;/*��ƽ�淨�ߣ����棬׶�棬���ߣ�������������ת�����ɶ�*/
		dof._rotation[0]._axis._x = param1[3];
		dof._rotation[0]._axis._y = param1[4];
		dof._rotation[0]._axis._z = param1[5];
	}
	return dof;
}

MidDOF Assembly::widthDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "I";

	return dof;
}

MidDOF Assembly::symmetricDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "S";

	return dof;
}

MidDOF Assembly::camfollowerDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "C";

	return dof;
}

MidDOF Assembly::gearDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "G";

	return dof;
}

MidDOF Assembly::rackpinionDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "R";

	return dof;
}

MidDOF Assembly::pathDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "P";

	return dof;
}

MidDOF Assembly::screwDOF(Mate mate)
{
	MidDOF dof;

	dof._countR = 0;
	dof._countT = 0;
	dof._constrain += "W";

	return dof;
}

bool Assembly::isConnector(CComPtr<IComponent> part)
{
	BSTR name0;
	CString tempName;
	std::string name;

	part->GetPathName(&name0);/*��ȡ�㲿��������*/
	tempName = name0;
	name = tempName;

	for (std::string keyword : ConnectorList) {/*����ConnectorList�е�ÿһ�������㲿�����ֵ��Ӵ�����Ϊ���Ӽ�*/
		if (name.find(name) != -1) {
			return true;
		}
	}
	return false;
}

void Assembly::display()
{
	VARIANT_BOOL retval;
	int size = _part.size();

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (Mate mate : _relation[i][j]) {
				mate._entity1->Select(VARIANT_TRUE, &retval);
				mate._entity2->Select(VARIANT_TRUE, &retval);

				//AfxMessageBox(MateType[mate._mateType]);

				mate._entity1->Select(VARIANT_FALSE, &retval);
				mate._entity2->Select(VARIANT_FALSE, &retval);
			}
		}		
	}
}

void Assembly::setCurs2Pointer()
{
	double mass[20];
	VARIANT_BOOL res;
	CComPtr<IModelDoc> modelDoc;
	std::list<Sort> work;
	Sort sort;
	int cur = 0;

	for (int i = 0; i < _part.size(); i++) {
		modelDoc = NULL;
		_part[i]->IGetModelDoc(&modelDoc);
		modelDoc->IGetMassProperties(mass, &res);

		sort._curs = i;
		sort._volume = mass[5];
		work.push_back(sort);
	}

	work.sort();
	for (Sort sort : work) {
		_pointer2curs[_part[sort._curs]] = cur;
		cur++;
	}
}

void Assembly::output(std::vector<std::vector<MidDOF>> &result)
{
	std::ofstream file, information, map;
	int size = _part.size();
	std::string name;
	CString nameC;
	BSTR nameBSTR;
	std::map<unsigned long long, int> subGraph;
	std::map<const unsigned long long, int>::iterator iter;

	file.open("E:\\Desktop\\TempFile\\info.txt");
	if (!file) {
		AfxMessageBox("Can not open the file 'info.txt'.");
		return;
	}
	information.open("E:\\Desktop\\TempFile\\information.txt",std::ios::app);
	if (!information) {
		AfxMessageBox("Can not open the file 'information.txt'.");
		return;
	}
	map.open("E:\\Desktop\\TempFile\\map.txt", std::ios::app);
	if (!map) {
		AfxMessageBox("Can not open the file 'map.txt'.");
		return;
	}

	map << "#" << std::endl;
	for (CComPtr<IComponent> component : _part) {
		component->get_Name2(&nameBSTR);
		nameC = nameBSTR;
		name = nameC;
		map << name << ":" << _pointer2curs[component] << std::endl;
	}
	map.close();

	nameC.Format("%d", _part.size());
	//���װ����ͼ�ṹ����ͼ�����Լ�����
	name = nameC;
	information << name << std::endl;

	subGraph = getSubGraph3Elems(result);
	for (iter = subGraph.begin(); iter != subGraph.end();iter++) {
		information << iter->first << "(" << iter->second << ")" << " ";
	}
	information << std::endl;

	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (result[i][j]._countR == 3 && result[i][j]._countT == 3) {
				continue;
			}

			_part[i]->get_Name2(&nameBSTR);
			nameC = nameBSTR;
			name = nameC;
			file << name << " and ";
			//���ļ������װ�����д�����Ϲ�ϵ���������������˶���ϵ
			nameC.Format("%d", i);
			name = nameC;
			information << "(" << name << ",";

			_part[j]->get_Name2(&nameBSTR);
			nameC = nameBSTR;
			name = nameC;
			file << name << ":";

			nameC.Format("%d", j);
			name = nameC;
			information << name << ",";

			nameC.Format("%d", result[i][j]._countT);
			name = nameC;
			file << name << "T";
			information << name << "T";
			nameC.Format("%d", result[i][j]._countR);
			name = nameC;
			file << name << "R( ";
			information << name << "R";

			std::string trans[3] = { "GIVEN_AXIS","GIVEN_DIR","GIVEN_POINT" };
			for (int k = 0; k < result[i][j]._countR; k++) {
				file << trans[result[i][j]._rotation[k]._type] << " ";
			}

			file << ")" << result[i][j]._constrain << std::endl;

			information << result[i][j]._constrain << ",";
			//����ÿһ��װ���ϵ������,װ���ϵ������CNT_RELATIONS����
			int limitation[CNT_RELATIONS]; memset(limitation, 0, sizeof(limitation));
			for (Mate mate : _relation[i][j]) {
				limitation[mate._mateType]++;
			}
			//���ļ������ÿһ��װ���ϵ������
			information << limitation[0];
			for (int i = 1; i < CNT_RELATIONS; i++) {
				information << ',' <<limitation[i];
			}
			information << ')' << std::endl;
		}
	}
}

std::map<unsigned long long, int> Assembly::getSubGraph3Elems(std::vector<std::vector<MidDOF>>& graph)
{
	int count = graph.size();
	unsigned long long entry = 0;
	std::map<unsigned long long,int> result;
	
	for (int i = 0; i < count; i++) {
		for (int j = i + 1; j < count; j++) {
			for (int k = j + 1; k < count; k++) {
				int arcValue[3] = { 0,0,0 };

				arcValue[0] = getArcValue(graph[i][j]);
				arcValue[1] = getArcValue(graph[i][k]);
				arcValue[2] = getArcValue(graph[j][k]);

				std::sort(arcValue, arcValue + 3);

				if (arcValue[1] != -1) {
					entry = 0;
					entry = arcValue[0];
					entry = entry << 16;
					entry += arcValue[1];
					entry = entry << 16;
					entry += arcValue[2];

					if (result.count(entry) == 0) {
						result[entry] = 1;
					}
					else {
						result[entry] = result[entry] + 1;
					}
				}
			}
		}
	}

	return result;
}

double cosAngle(double * vect1, double * vect2)
{
	return (vect1[0]*vect2[0]+vect1[1]*vect2[1]+vect1[2]*vect2[2])/
		(sqrtf((vect1[0]*vect1[0]+vect1[1]*vect1[1]+vect1[2]*vect1[2])*(vect2[0]*vect2[0]+vect2[1]*vect2[1]+vect2[2]*vect2[2])));
}

double cosAngle(Axis & axis1, Axis & axis2)
{
	double vect1[3] = { axis1._x,axis1._y,axis1._z };
	double vect2[3] = { axis2._x,axis2._y,axis2._z };

	return cosAngle(vect1, vect2);
}

double cosAngle(Normal &norm1, Normal &norm2)
{
	double vect1[3] = { norm1._x,norm1._y,norm1._z };
	double vect2[3] = { norm2._x,norm2._y,norm2._z };

	return cosAngle(vect1, vect2);
}

std::vector<double> multiply(double x1, double y1, double z1, double x2, double y2, double z2)
{
	std::vector<double> result;
	result.resize(3);

	result[0] = y1*z2 - y2*z1;
	result[1] = -(x1*z2 - x2*z1);
	result[2] = x1*y2 + x2*y1;

	return result;
}

double distant(double x1, double y1, double z1, double x2, double y2, double z2)
{
	return sqrtf((x1 - x2)*(x1*x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2));
}
/*��ȡ��ǰ�ߵ�ֵ��12-15λ��ʾƽ�����ɶȵĸ�����7-11λ��ʾת�����ɶȵĸ�������8λ��ʾ�Ƿ�����Ӧ�ĸ߼����,���û����Ϲ�ϵ������0*/
int getArcValue(MidDOF dof)
{
	int arcValue = 0;

	if (dof._countR == 3 && dof._countT == 3) {
		return -1;
	}
	else {
		arcValue = dof._countT << 12;
		arcValue |= dof._countR << 8;

		if (dof._constrain.find('S') != -1) {/*����жԳ�Լ��*/
			arcValue |= SwMateSYMMETRIC;
		}
		if (dof._constrain.find('C') != -1) {/*�����͹��Լ��*/
			arcValue |= SwMateCAMFOLLOWER;
		}
		if (dof._constrain.find('G') != -1) {/*����г���Լ��*/
			arcValue |= SwMateGEAR;
		}
		if (dof._constrain.find('R') != -1) {/*����г��ֳ���Լ��*/
			arcValue |= SwMateRACKPINION;
		}
		if (dof._constrain.find('P') != -1) {/*�����·��Լ��*/
			arcValue |= SwMatePATH;
		}
		if (dof._constrain.find('L') != -1) {/*���������Լ��*/
			arcValue |= swMateLOCK;
		}
		if (dof._constrain.find('W') != -1) {/*���������Լ��*/
			arcValue |= SwMateSCREM;
		}
		if (dof._constrain.find('K') != -1) {/*����п��Լ��*/
			arcValue |= SwMateWIDTH;
		}
	}

	return arcValue;
}

