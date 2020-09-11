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


	if (isConnector(part) == true) {/*如果当前的零件是连接件，则将当前的零件添加到connector成员变量中*/
		addConnector(part);
	}
}

void Assembly::resizeRelationVector()
{
	int size = _part.size();

	_relation.resize(size);/*在获取了装配体中零件的个数之后，将_relation成员变量中的每一维设置成零件个数的大小*/
	for (int i = 0; i < size; i++) {
		_relation[i].resize(size);
	}
}

void Assembly::addConnector(CComPtr<IComponent>connector)
{
	_connector.insert(connector);/*将连接件Connector插入到记录连接件的集合中*/
}

void Assembly::addRelation(Mate mate)
{
	CComPtr<IComponent> component;
	Mate temp;
	int curs1 = 0, curs2 = 0;
	
	mate._entity1->IGetComponent(&component);/*获得当前参加装配的元素（面，线，点）所属的零件*/
	curs1 = _pointer2curs[component];/*通过零件指针-零件在_part中的序号的对照表获取零件在_part中的序号，以备插入到_relation中*/

	component = NULL;
	mate._entity2->IGetComponent(&component);/*同上*/
	curs2 = _pointer2curs[component];

	if (curs1 < curs2) {
		_relation[curs1][curs2].push_back(mate);/*将当前装配关系插入到_relation中,其中第一维的下标小于第二维的坐标*/
	}
	else {
		temp._entity1 = mate._entity2;/*插入到vector中的Mate关系entity1对应的是第一维标号的零件，entity2对应的是第二维标号的零件*/
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
				if (status == false) {/*第一个装配关系要给resultDOF赋值，在后边的装配关系才会有装配关系的规约*/
					switch (dof._mateType) {
					case swMateCOINCIDENT:resultDOF = coincidentDOF(dof);break;/*根据不同的装类型分别处理*/
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
					case swMateCOINCIDENT:midDOF = coincidentDOF(dof); resultDOF = setDOF(midDOF, resultDOF); break;/*根据不同的装类型分别处理*/
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

			result[i][j] = resultDOF;/*最终的自由度值赋值到结果矩阵中*/
		}
	}
}

MidDOF Assembly::setDOF(MidDOF dof1, MidDOF dof2)
{
	MidDOF dof;

	if ((0 == dof1._countT) || (0 == dof2._countT)) {/*如果自由度都没有平移自由度，则规约后没有平移自由度。*/
		dof._countT = 0;
	}
	else if ((3 == dof1._countT) || (3 == dof2._countT)) {/*如果两个自由度中一个有三个方向上的平移自由度，则规约后自由度为另一个的平移自由度*/
		if (3 == dof1._countT) {
			dof._countT = dof2._countT;
			dof._transform[0] = dof2._transform[0];/*因为平移自由度最多只需要一个存储空间（一个时存其方向，两个时存平面法线方向，三个时不用存），只需要将第一个赋值就可以了*/
		}
		else {
			dof._countT = dof1._countT;
			dof._transform[0] = dof1._transform[0];/*同上*/
		}
	}
	else if (1 == dof1._countT && 1 == dof2._countT) {/*如果两个自由度都只有一个平移自由度*/
		double angle = cosAngle(dof1._transform[0],dof2._transform[0]);/*计算这两个平移自由度的夹角的余弦值*/

		if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*如果平移自由度的方向相同或者相反，规约后得到一个平移自由度*/	
			dof._countT = 1;
			dof._transform[0] = dof1._transform[0];

		}
		else {/*如果不同向则规约后的没有平移自由度*/
			dof._countT = 0;
		}
	}
	else if ((1 == dof1._countT && 2 == dof2._countT) || (2 == dof1._countT && 1 == dof2._countT)) {/*如果两个自由度分别有一个，两个平移自由度*/
		double angle = cosAngle(dof1._transform[0], dof2._transform[0]);/*计算单个平移自由度方向和另外两个平移自由度构成平面的法线的夹角的余弦*/

		if (fabs(angle) < 10E-4) {/*如果垂直，则规约后剩余一个自由度，方向与单个平移自由度的方向相同*/
			dof._countT = 1;
			
			if (1 == dof1._countT) {/*判断那个自由度的平移自由度数量为一*/
				dof._transform[0] = dof1._transform[0];
			}
			else {
				dof._transform[0] = dof2._transform[0];
			}
		}
		else {
			dof._countT = 0;/*如果一个平移自由度和两个平移自由构成的平面的法线不垂直，则规约后就没有平移自由度*/
		}
	}
	else if (2 == dof1._countT && 2 == dof2._countT) {/*如果两个自由度的平移自由度都是两个*/
		double angle= cosAngle(dof1._transform[0], dof2._transform[0]);/*计算这两个自由度的平移自由度形成的平面的法线的夹角的余弦值*/

		if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*如果两个平面的法线向平行（同向或反向），则规约后的自由度为两个*/
			dof._countT = 2;
			dof._transform[0] = dof1._transform[0];
		}
		else {/*如果法线不平行，则规约后的自由度只有一个，方向为这两个法线叉乘的向量方向*/
			std::vector<double> dir=multiply(dof1._transform[0]._x, dof1._transform[0]._y, dof1._transform[0]._z, /*两个法线叉乘*/
											 dof2._transform[0]._x, dof2._transform[0]._y, dof2._transform[0]._z);

			dof._countT = 1;
			dof._transform[0]._x = dir[0];
			dof._transform[0]._y = dir[1];
			dof._transform[0]._z = dir[2];
		}
	}


	if (0 == dof1._countR || 0 == dof2._countR) {/*如果两个自由度都没有转动自由度，规约后的转动自由度还是为零*/
		dof._countR = 0;
	}
	else{
		dof._countR = 0;/*预先设定该值为0，下边会对其进行自增*/

		for (int i = 0; i < dof1._countR; i++) {
			for (int j = 0; j < dof2._countR; j++) {/*需要遍历两个自由度中的所有转动自由度来进行规约*/
				if (GIVEN_AXIS == dof1._rotation[i]._type&&GIVEN_AXIS == dof2._rotation[j]._type) {/*如果两个转动自由度都是给定轴的转动自由度*/
					Axis axis;
					double angle = cosAngle(dof1._rotation[i]._axis, dof2._rotation[j]._axis);/*首先计算两个轴的夹角的余弦值*/

					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*如果两个轴线方向相同，再计算两个轴是否共线*/
						axis._x = dof1._rotation[i]._point._x - dof2._rotation[j]._point._x;
						axis._y = dof1._rotation[i]._point._y - dof2._rotation[j]._point._y;/*用两个轴的起点构建新的向量*/
						axis._z = dof1._rotation[i]._point._z - dof2._rotation[j]._point._z;

						angle = cosAngle(dof1._rotation[i]._axis, axis);/*共线计算方法：如果两个轴的起点构成的向量和原先的轴的方向是平行的，则说明两个轴共线*/
						if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {
							dof._rotation[dof._countR] = dof1._rotation[i];/*如果构建的轴和原先的轴平行，则两个转动自由度共线，规约后得一个转动自由度，方向起点和原来的任一相同即可*/
							dof._countR++;/*转动自由度数自增*/
						}
					}
				}
				else if (GIVEN_DIR == dof1._rotation[i]._type&&GIVEN_DIR == dof2._rotation[j]._type) {/*如果两个转动自由度都是给定方向的*/
					double angle = cosAngle(dof1._rotation[i]._axis, dof2._rotation[j]._axis);/*求两个转动自由度的轴线方向的夹角的余弦*/

					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*如果两个转动自由度轴线的方向相同或相反，规约得到一个给定转动方向的自由度，与原先任意一个相同即可*/
						dof._rotation[dof._countR] = dof1._rotation[i];
						dof._countR++;
					}
				}
				else if (GIVEN_POINT == dof1._rotation[i]._type&&GIVEN_POINT == dof2._rotation[j]._type) {/*如果两个转动自由度都是给定点的转动自由度*/
					if (fabs(distant(dof1._rotation[i]._point._x, dof1._rotation[i]._point._y, dof1._rotation[i]._point._z,
						dof2._rotation[j]._point._x, dof2._rotation[j]._point._y, dof2._rotation[j]._point._z)) < 10E-6) {/*检测两个点是否共点（距离为零）*/

						dof._rotation[dof._countR] = dof1._rotation[i];/*如果共点，规约后得到一个给定点的转动自由度*/
						dof._countR++;
					}
				}
				else if ((GIVEN_AXIS == dof1._rotation[i]._type&&GIVEN_DIR == dof2._rotation[j]._type) ||
					(GIVEN_DIR == dof1._rotation[i]._type&&GIVEN_AXIS == dof2._rotation[j]._type)) {/*如果两个转动自由度分别为给定方向和给定轴的转动自由度*/
					double angle = cosAngle(dof1._rotation[i]._axis, dof2._rotation[j]._axis);/*求两个转动轴的夹角的余弦值*/

					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*如果同向或反向，则规约后得到一个转动自由度*/
						if (GIVEN_AXIS == dof1._rotation[i]._type) {
							dof._rotation[dof._countR] = dof1._rotation[i];
						}
						else {
							dof._rotation[dof._countR] = dof2._rotation[j];
						}

						dof._countR++;
					}
				}
				else if ((GIVEN_AXIS == dof1._rotation[i]._type&&GIVEN_POINT == dof2._rotation[j]._type) ||/*如果两个转动自由度分别是过定轴和过定点的转动自由度*/
					(GIVEN_POINT == dof1._rotation[i]._type&&GIVEN_AXIS == dof2._rotation[j]._type)) {
					Axis constructAxis, originAxis;
					double angle;

					constructAxis._x = dof1._rotation[i]._point._x - dof2._rotation[j]._point._x;/*用定点和定轴的起点构建的向量来测试定点是否在定轴上*/
					constructAxis._y = dof1._rotation[i]._point._y - dof2._rotation[j]._point._y;
					constructAxis._z = dof1._rotation[i]._point._z - dof2._rotation[j]._point._z;

					if (GIVEN_AXIS == dof1._rotation[i]._type) {
						originAxis = dof1._rotation[i]._axis;
					}
					else {
						originAxis = dof2._rotation[j]._axis;
					}

					angle = cosAngle(constructAxis, originAxis);
					if (fabs(angle - 1) < 10E-4 || fabs(angle + 1) < 10E-4) {/*如果定点在定轴上，规约得到的转动自由度是一个和原先定轴的转动自由度相同的自由度*/
						dof._rotation[dof._countR]._type = GIVEN_AXIS;
						dof._rotation[dof._countR]._axis = constructAxis;
						dof._rotation[dof._countR]._point = dof1._rotation[i]._point;
						dof._countR++;
					}
				}
				else if ((GIVEN_DIR == dof1._rotation[i]._type&&GIVEN_POINT == dof2._rotation[j]._type) ||/*如果转动自由度分别是过定点和指定方向的任意轴的转动自由度*/
					(GIVEN_POINT == dof1._rotation[i]._type&&GIVEN_DIR == dof2._rotation[j]._type)) {
					if (GIVEN_DIR == dof1._rotation[i]._type) {/*规约后得到一个方向为原先指定方向自由度的方向，起点为原先定点的固定轴的转动自由度*/
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
	MidDOF dof;/*dof存储的是剩余的自由度个数，方向*/
	long type1, type2;
	long mateType = -1;
	double param1[8],param2[8];

	mate._entity1->GetType(&type1);/*获取参与装配的两个元素的类型，根据类型进行自由度判断*/
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
			dof._countR = 1;/*两个球面同心，有一个绕球心任意轴的自由度，没有平移自由度*/
			dof._countT = 0;

			surface1->get_ISphereParams(param1);

			dof._rotation[0]._type = GIVEN_POINT;/*两个球面同心，有一个绕球心任意轴的自由度*/
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

			dof._countR = 1;/*球面柱面同心，有一个过球心的自由度，一个沿柱面轴线的平移自由度*/
			dof._countT = 1;

			dof._transform[0]._x = param1[3];/*平移自由度*/
			dof._transform[0]._y = param1[4];
			dof._transform[0]._z = param1[5];

			dof._rotation[1]._type = GIVEN_POINT;/*过球心任意轴的自由度*/
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
		dof._countR = 1;/*有一个沿轴线的转动自由度，一个沿着轴线的平移自由度*/
		dof._countT = 1;

		dof._rotation[0]._type = GIVEN_AXIS;/*沿轴线的转动自由度*/
		dof._rotation[0]._axis._x = param1[3];
		dof._rotation[0]._axis._y = param1[4];
		dof._rotation[0]._axis._z = param1[5];
		dof._rotation[0]._point._x = param1[0];
		dof._rotation[0]._point._y = param1[1];
		dof._rotation[0]._point._z = param1[2];

		dof._transform[0]._x = param1[3];/*沿轴线的平移自由度*/
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

	if (type1 == swSelFACES&&type2 == swSelFACES) {/*如果参与装配的两个元素都是面*/
		CComPtr<IFace2> face1, face2;
		CComPtr<ISurface> surface1, surface2;
		long type1, type2;

		face1 = mate._entity1;/*获取两个面的surface类型变量*/
		face1->IGetSurface(&surface1);
		face2 = mate._entity2;
		face2->IGetSurface(&surface2);

		surface1->Identity(&type1);/*获取两个surface表面的类型，仅限于可解析的面，如圆柱面，圆锥面，平面，球面，圆环面*/
		surface2->Identity(&type2);

		if (type1 == PLANE_TYPE&&type2 == PLANE_TYPE) {
			dof._countT = 2;/*面面接触会剩余两个平移自由度，一个旋转自由度*/
			dof._countR = 1;

			surface1->get_IPlaneParams(param1);

			dof._rotation->_type = GIVEN_DIR;/*沿着平面法线的转动自由度*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._transform[0]._x = param1[0];/*有两个平移自由度，但是只需要记录平面的法线即可*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
	}
	else if (type1 == swSelVERTICES&&type2 == swSelVERTICES) {/*如果参与装配的元素都是点*/
		CComPtr<IVertex> vertex;
		
		vertex = mate._entity1;
		vertex->IGetPoint(param1);

		dof._countR = 1;/*点点重合会剩余一个绕固定点的转动自由度，没有平移自由度*/
		dof._countT = 0;

		dof._rotation[0]._type = GIVEN_POINT;		
		dof._rotation[0]._point._x = param1[0];
		dof._rotation[0]._point._y = param1[1];
		dof._rotation[0]._point._z = param1[2]; 
	}
	else if (type1 == swSelEDGES&&type2 == swSelEDGES) {/*如果参与配合的元素都是边*/
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;
		edge1->IGetCurve(&curve1);
		edge1->IGetCurve(&curve2);
		curve1->Identity(&type1);
		curve2->Identity(&type2);

		if (type1 == LINE_TYPE&&type2 == LINE_TYPE) {/*如果这两条边都是直线类型*/
			dof._countR = 1;/*直线和直线的重合约束会剩余一个平移自由度，一个转动自由度*/
			dof._countT = 1;

			curve1->get_ILineParams(param1);

			dof._rotation[0]._type = GIVEN_AXIS;/*转动自由度是绕这两条直线公共轴的固定轴自由度*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._z = param1[4];
			dof._rotation[0]._axis._y = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._transform[0] = dof._rotation[0]._axis;/*平移自由度是沿着直线方向的移动自由度*/
		}
		else if (type1 == CIRCLE_TYPE&&type1 == CIRCLE_TYPE) {/*如果是两个圆线的重合*/
			dof._countR = 1;/*两个圆重合会剩余一个转动自由度，没有平移自由度*/
			dof._countT = 0;

			curve1->get_ICircleParams(param1);

			dof._rotation[0]._type = GIVEN_AXIS;/*绕着圆的轴线的固定轴的转动自由度*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];
		}
	}
	else if ((type1 == swSelEDGES&&type2 == swSelFACES) || (type1 == swSelFACES&&type2 == swSelEDGES)) {/*如果参与装配的两个元素分别是线和面*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace2> face;
		CComPtr<ISurface> surface;

		if (type1 == swSelEDGES) {/*将edge和face分别存放相应的元素*/
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
			curve->get_ILineParams(param1);/*如果线元素是直线，则获取这个直线的参数*/

			if (PLANE_TYPE == type2) {/*如果面元素是平面*/
				dof._countR = 2;/*直线，平面的重合约束会剩余两个转动自由度，两个平移自由度*/
				dof._countT = 2;
				
				surface->get_IPlaneParams(param2);

				dof._rotation[0]._type = GIVEN_AXIS;/*一个绕直线的固定轴的转动自由度*/
				dof._rotation[0]._axis._x = param1[3];
				dof._rotation[0]._axis._y = param1[4];
				dof._rotation[0]._axis._z = param1[5];
				dof._rotation[0]._point._x = param1[0];
				dof._rotation[0]._point._y = param1[1];
				dof._rotation[0]._point._z = param1[2];

				dof._rotation[1]._type = GIVEN_DIR;/*一个绕平面法线方向的任意轴的转动自由度*/
				dof._rotation[1]._axis._x = param2[0];
				dof._rotation[1]._axis._y = param2[1];
				dof._rotation[1]._axis._z = param2[2];

				dof._transform[0]= dof._rotation[1]._axis;/*平移自由度为在平面内的平移，记录下平面的法线方向*/
			}
		}
		else if (CIRCLE_TYPE == type1) {/*如果线元素是圆，面元素是平面*/
			if (PLANE_TYPE == type2) {
				dof._countT = 2;/*曲线和面重合相当于面面接触，会剩余两个平移自由度，一个旋转自由度*/
				dof._countR = 1;

				surface->get_IPlaneParams(param1);
				dof._rotation->_type = GIVEN_DIR;/*绕平面的法线方向的任意轴的转动自由度	*/			
				dof._rotation[0]._axis._x = param1[0];
				dof._rotation[0]._axis._y = param1[1];
				dof._rotation[0]._axis._z = param1[2];

				dof._transform[0] = dof._rotation[0]._axis;/*平移方向在平面中，记录下面的法线方向*/
			}
		}
	}
	else if ((type1 == swSelVERTICES&&type2 == swSelFACES) || (type1 == swSelFACES&&type2 == swSelVERTICES)) {/*如果参与配合的元素一个是点另一个是面*/
		CComPtr<IFace2> face;
		CComPtr<ISurface> surface;
		CComPtr<IVertex> vertex;

		if (type1 == swSelFACES) {/*将线，面元素分别赋值给相应的元素*/
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

		if (PLANE_TYPE == type2) {/*如果面元素是平面*/
			surface->get_IPlaneParams(param2);

			dof._countR = 2;/*点，平面的接触约束会剩余两个平面内的平移自由度，两个转动自由度*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_POINT;/*第一个转动自由度是过点的任意轴的转动自由度*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_DIR;/*第二个是过平面法线方向上的任意轴的转动自由度*/
			dof._rotation[1]._axis._x = param2[0];
			dof._rotation[1]._axis._y = param2[1];
			dof._rotation[1]._axis._z = param2[2];

			dof._transform[0] = dof._rotation[1]._axis;/*平移自由度在平面中，记录下平面的法线方向*/

		}
	}
	else if ((type1 == swSelVERTICES&&type2 == swSelEDGES) || (type1 == swSelEDGES&&type2 == swSelVERTICES)) {/*参与约束的元素是点和边*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IVertex> vertex;
		double param[8];

		if (type1 == swSelEDGES) {/*将点和边赋值给相应的指针*/
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

		if (LINE_TYPE == type2) {/*如果面元素是平面*/
			curve->get_ILineParams(param2);

			dof._countR = 1;/*点面接触会剩余一个转动自由度，一个平移自由度*/
			dof._countT = 1;

			dof._rotation[0]._type = GIVEN_POINT;/*过点的任意轴的转动自由度*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._transform[0]._x = param2[3];/*沿着线方向的平移自由度*/
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

	mate._entity1->GetType(&type1);//*获取参与装配的两个元素的类型，根据类型进行自由度判断
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

		if (LINE_TYPE == type1&&LINE_TYPE == type2) {/*如果两个配合元素都是线，则要获取线的类型（直线）*/
			flag = 1;
			curve1->get_ILineParams(param1);
			curve2->get_ILineParams(param2);
		}
	}
	else if ((swSelEDGES == type1&&swSelFACES == type2) || (swSelFACES == type1&&swSelEDGES == type2)) {/*如果两个配合元素分别是面和边*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace> face;
		CComPtr<ISurface> surface;

		if (swSelEDGES == type1) {/*将配合元素中边元素赋值给edge，面元素赋值给face*/
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
			curve->get_ILineParams(param1);/*如果线元素是直线，获取直线的参数，本程序仅考虑线是直线的情况*/

			if (PLANE_TYPE == type2) {/*如果参加配合的面元素是平面，获取平面的参数*/
				surface->get_IPlaneParams(param2);

				dof._countR = 1;/*直线，平面的垂直约束剩余的自由度和其他的类型不同，需要单独处理*/
				dof._countT = 3;

				dof._rotation[0]._type = GIVEN_DIR;/*直线，平面的垂直约束会剩余三个平移自由度，一个绕平面发现方向的任意轴的转动自由度*/
				dof._rotation[0]._axis._x = param2[0];
				dof._rotation[0]._axis._y = param2[1];
				dof._rotation[0]._axis._z = param2[2];
			}
			else if (CYLINDER_TYPE == type2) {/*如果参与配合的面元素是柱面，则获取柱面的参数*/
				flag = 1;
				surface->get_ICylinderParams(param2);
			}
			else if (CONE_TYPE == type2) {/*如果参与配合的面元素是锥面，则获取锥面的参数*/
				flag = 1;
				surface->get_IConeParams(param2);
			}
		}
	}
	else if (swSelFACES == type1&&swSelFACES == type2) {/*如果参与配合的两个元素都是面元素*/
		CComPtr<IFace> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		surface1->Identity(&type1);
		surface2->Identity(&type2);

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {/*如果参与配合的面都是平面*/
			flag = 1;
			surface1->get_IPlaneParams(param1);
			surface2->get_IPlaneParams(param2);

			for (int i = 0; i < 3; i++) {
				param1[i + 3] = param1[i];/*平面的法线参数在param[0-2]中，要迁移到param[3-5]中，以便统一处理*/
				param2[i + 3] = param2[i];
			}
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2){ /*如果参与配合的面都是柱面，则获取柱面的参数*/
			flag = 1;

			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);
		}
		else if (CONE_TYPE == type1&&CONE_TYPE == type2) {/*如果参与配合的面都是圆锥面，则获取圆锥面的参数*/
			flag = 1;

			surface1->get_IConeParams(param1);
			surface2->get_IConeParams(param2);
		}
		else if ((CONE_TYPE == type1&&CYLINDER_TYPE == type2) || (CYLINDER_TYPE == type1&&CONE_TYPE == type2)) {/*如果参与配合的面是柱面和圆锥面*/
			flag = 1;

			if (CONE_TYPE == type1) {/*将圆锥面的参数放在param1中，柱面的参数放在param2中*/
				surface1->get_IConeParams(param1);
				surface2->get_ICylinderParams(param2);
			}
			else {
				surface1->get_ICylinderParams(param1);
				surface2->get_IConeParams(param2);
			}
		}
	}

	if (1 == flag) {/*如果flag被置一，说明需要统一处理*/
		dof._countR = 2;/*以上的约束会剩余三个方向上的平移自由度，两个方向上的转动自由度*/
		dof._countT = 3;

		dof._rotation[0]._type = GIVEN_DIR;/*沿第一条轴线方向的任意轴上的转动自由度*/
		dof._rotation[0]._axis._x = param1[3];
		dof._rotation[0]._axis._y = param1[4];
		dof._rotation[0]._axis._z = param1[5];

		dof._rotation[1]._type = GIVEN_DIR;/*沿第二条轴线方向上的转动自由度*/
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

	mate._entity1->GetType(&type1);/*获取参与装配的两个元素的类型，根据类型进行自由度判断*/
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
		
		surface1->Identity(&type1);/*获得面的类型*/
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

			dof._rotation[0]._axis._x = param1[0];/*绕平面的法线的转动自由度，为给定方向的任意的转动自由度*/
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];
			dof._rotation[0]._type = GIVEN_DIR;

			dof._rotation[1]._axis._x = param2[3];/*绕柱面的轴线的转动自由度，为绕固定轴的转动自由度*/
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
			dof._rotation[1]._type = GIVEN_AXIS;

			dof._transform[0]._x = param1[0];/*有两个沿平面的平移自由度，只需要记录这个平面的法线即可*/
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

			dof._countR = 2;/*球和平面相切会有两个平移的自由度，两个转动自由度（一个是绕球心的任意轴，另一个是绕平面上的任意一条法线）*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_POINT;/*一个转动的自由度是绕过球心的任意一条轴*/
			dof._rotation[0]._point._x = param2[0];
			dof._rotation[0]._point._y = param2[1];
			dof._rotation[0]._point._z = param2[2];

			dof._rotation[1]._type = GIVEN_DIR;/*另一个转动自由度是绕平面的任意一条法线*/
			dof._rotation[1]._axis._x = param1[0];
			dof._rotation[1]._axis._y = param1[1];
			dof._rotation[1]._axis._z = param1[2];

			dof._transform[0]._x = param1[0];/*有两个沿平面的平移自由度，只需要记录这个平面的法线即可*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
		else if (type1 == CYLINDER_TYPE&&type2 == CYLINDER_TYPE) {
			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);

			dof._countR = 2;/*柱面和柱面相切（线切）有两个转动自由度，一个是沿自己轴的转动，另一个是沿另一个轴转动的自由度*/
			dof._countT = 1;/*沿两个柱面的轴线的平移自由度*/

			dof._rotation[0]._type = GIVEN_AXIS;/*沿一条固定轴转动的自由度*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*沿另一条固定轴转动的自由度*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param1[3];/*平移的自由度，沿着柱面的轴线*/
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

	mate._entity1->GetType(&type1);//*获取参与装配的两个元素的类型，根据类型进行自由度判断
	mate._entity2->GetType(&type2);

	if (swSelVERTICES == type1&&swSelVERTICES == type2) {
		CComPtr<IVertex> vertex1, vertex2;

		vertex1 = mate._entity1;
		vertex2 = mate._entity2;
		vertex1->IGetPoint(param1);
		vertex2->IGetPoint(param2);

		dof._countR = 2;/*点点距离约束会剩余2个转动自由度，0个平移自由度*/
		dof._countT = 0;

		dof._rotation[0]._type = GIVEN_POINT;/*点点距离约束时会有两个绕两个固定点的转动自由度*/
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

			dof._countR = 2;/*点线距离约束有沿线轴线方向的平移自由度，一个过固定点（点）任意轴的转动自由度和一个沿轴的固定轴的自由度*/
			dof._countT = 1;

			dof._rotation[0]._type = GIVEN_POINT;/*过点的任意轴的转动自由度*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*过固定轴的转动自由度*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param2[3];/*沿线的平移自由度*/
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
		surface->Identity(&type1);/*点面距离可以施加给平面，柱面，球面*/

		if (PLANE_TYPE == type1) {
			surface->get_IPlaneParams(param2);

			dof._countR = 2;/*点平面距离约束会有两个转动自由度，沿平面法线的自由度和过点的自由度，两个平移自由度*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_DIR;/*沿着平面法线的自由度*/
			dof._rotation[0]._axis._x = param2[0];
			dof._rotation[0]._axis._y = param2[1];
			dof._rotation[0]._axis._z = param2[2];

			dof._rotation[1]._type = GIVEN_POINT;/*过顶点的旋转自由度*/
			dof._rotation[1]._point._x = param1[0];
			dof._rotation[1]._point._y = param1[1];
			dof._rotation[1]._point._z = param1[2];

			dof._transform[0]._x = param2[0];
			dof._transform[0]._y = param2[1];
			dof._transform[0]._z = param2[2];
		}
		else if (CYLINDER_TYPE == type1) {
			surface->get_ICylinderParams(param2);

			dof._countR = 2;/*点和柱面距离约束会剩余沿柱面轴线的平移自由度，过点任意方向的转动自由度，沿着柱面轴线的转动自由度*/
			dof._countT = 1;

			dof._rotation[0]._type = GIVEN_POINT;/*过固定点的转动自由度*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*沿着固定轴的转动自由度*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param2[3];/*沿着柱面轴线方向的平移自由度*/
			dof._transform[0]._y = param2[4];
			dof._transform[0]._z = param2[5];
		}
		else if (SPHERE_TYPE == type1) {
			surface->get_ISphereParams(param2);

			dof._countR = 2;/*点和球面之间的距离约束会有两个过固定点的转动自由度*/
			dof._countT = 0;

			dof._rotation[0]._type = GIVEN_POINT;/*过点的任意轴的转动自由度*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[0];
			dof._rotation[0]._point._z = param1[1];

			dof._rotation[1]._type = GIVEN_POINT;/*过球心的任意轴的转动自由度*/
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
				dof._countR = 2;/*两条边的距离约束会剩余沿着边的方向的平移和以两条边为轴线的转动*/
				dof._countT = 1;

				dof._transform[0]._x = param1[3];/*沿着边的方向的平移自由度*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
			else {
				dof._countR = 3;/*两条边的距离约束会剩余沿着边的方向的平移和以两条边为轴线的转动*/
				dof._countT = 2;

				dof._transform[1]._x = param2[3];/*沿着边的方向的平移自由度*/
				dof._transform[1]._y = param2[4];
				dof._transform[1]._z = param2[5];

				dof._rotation[2]._type = GIVEN_DIR;
				dof._rotation[2]._axis._x = param1[4] * param2[5] - param2[4] * param1[5];
				dof._rotation[2]._axis._y = -(param1[3] * param2[5] - param2[3] * param1[5]);
				dof._rotation[2]._axis._z = param1[3] * param2[4] - param2[3] * param1[4];

				dof._transform[0] = dof._rotation[2]._axis;

			}

			dof._rotation[0]._type = GIVEN_AXIS;/*第一条边为转动轴的转动自由度*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*第二条边为转动轴的转动自由度*/
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

				dof._countR = 2;/*线和平面之间的距离约束会剩余2个转动自由度，2个平移自由度*/
				dof._countT = 2;

				dof._rotation[0]._type = GIVEN_DIR;/*第一个自由度是沿着平面的法线方向的任意轴转动自由度*/
				dof._rotation[0]._axis._x = param2[0];
				dof._rotation[0]._axis._y = param2[1];
				dof._rotation[0]._axis._z = param2[2];

				dof._rotation[1]._type = GIVEN_AXIS;/*第二个自由度是沿着线的方向的固定轴的转动自由度*/
				dof._rotation[1]._axis._x = param1[3];
				dof._rotation[1]._axis._y = param1[4];
				dof._rotation[1]._axis._z = param1[5];
				dof._rotation[1]._point._x = param1[0];
				dof._rotation[1]._point._y = param1[1];
				dof._rotation[1]._point._z = param1[2];

				dof._transform[0]._x = param2[0];/*两个平移自由度，记录下这两个方向形成的平面的法线方向*/
				dof._transform[0]._y = param2[1];
				dof._transform[0]._z = param2[2];
			}
			else if (CYLINDER_TYPE == type2) {
				surface->get_ICylinderParams(param2);

				dof._countR = 2;/*线和柱面的距离约束会剩余沿着边的方向的平移和以线，柱面轴线为轴线的转动*/
				dof._countT = 1;

				dof._rotation[0]._type = GIVEN_AXIS;/*边为转动轴的转动自由度*/
				dof._rotation[0]._axis._x = param1[3];
				dof._rotation[0]._axis._y = param1[4];
				dof._rotation[0]._axis._z = param1[5];
				dof._rotation[0]._point._x = param1[0];
				dof._rotation[0]._point._y = param1[1];
				dof._rotation[0]._point._z = param1[2];

				dof._rotation[1]._type = GIVEN_AXIS;/*柱面轴线为转动轴的转动自由度*/
				dof._rotation[1]._axis._x = param2[3];
				dof._rotation[1]._axis._y = param2[4];
				dof._rotation[1]._axis._z = param2[5];
				dof._rotation[1]._point._x = param2[0];
				dof._rotation[1]._point._y = param2[1];
				dof._rotation[1]._point._z = param2[2];

				dof._transform[0]._x = param1[3];/*沿着边的方向的平移自由度*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
			else if (SPHERE_TYPE == type2) {
				surface->get_ISphereParams(param2);

				dof._countR = 2;/*球面和线距离约束有沿线轴线方向的平移自由度，一个过固定点（点）任意轴的转动自由度和一个沿线的固定轴的自由度*/
				dof._countT = 1;

				dof._rotation[0]._type = GIVEN_POINT;/*过点的任意轴的转动自由度*/
				dof._rotation[0]._point._x = param2[0];
				dof._rotation[0]._point._y = param2[1];
				dof._rotation[0]._point._z = param2[2];

				dof._rotation[1]._type = GIVEN_AXIS;/*过固定轴的转动自由度*/
				dof._rotation[1]._axis._x = param1[3];
				dof._rotation[1]._axis._y = param1[4];
				dof._rotation[1]._axis._z = param1[4];
				dof._rotation[1]._point._x = param1[0];
				dof._rotation[1]._point._y = param1[1];
				dof._rotation[1]._point._z = param1[2];

				dof._transform[0]._x = param1[3];/*沿线的平移自由度*/
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

			dof._countR = 1;/*平面和平面的距离约束会剩余一个转动自由度，两个平移自由度*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_DIR;/*一个转动自由度是沿着两个平面的法线方向*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._transform[0]._x = param1[0];/*记录下两个平移自由度形成的平面的法线方向*/
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

			dof._countR = 2;/*平面和柱面的距离约束会剩余两个转动自由度，两个平移自由度*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_DIR;/*一个剩余的转动自由度是沿着平面的法线方向的任意轴*/
			dof._rotation[0]._axis._x = param1[0];
			dof._rotation[0]._axis._y = param1[1];
			dof._rotation[0]._axis._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*另一个剩余的转动自由度是沿着柱面轴线的固定轴*/
			dof._rotation[1]._axis._x = param2[3];
			dof._rotation[1]._axis._y = param2[4];
			dof._rotation[1]._axis._z = param2[5];
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];

			dof._transform[0]._x = param1[0];/*记录下另一个平移自由度形成面的法线方向*/
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

			dof._countR = 2;/*平面与球面的距离约束会剩余两个转动自由度，两个平移自由度*/
			dof._countT = 2;

			dof._rotation[0]._type = GIVEN_POINT;/*一个转动自由度是过球心的任意轴的转动*/
			dof._rotation[0]._point._x = param2[0];
			dof._rotation[0]._point._y = param2[1];
			dof._rotation[0]._point._z = param2[2];

			dof._rotation[1]._type = GIVEN_DIR;/*另一转动自由度是沿着平面法线方向的任意轴*/
			dof._rotation[1]._point._x = param1[0];
			dof._rotation[1]._point._y = param1[1];
			dof._rotation[1]._point._z = param1[2];

			dof._transform[0]._x = param1[0];/*记录下两个平移方向形成的平面的法线方向*/
			dof._transform[0]._y = param1[1];
			dof._transform[0]._z = param1[2];
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2) {
			double angle = 0;

			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);

			angle = cosAngle(param1 + 3, param2 + 3);
			if (fabs(angle + 1) < 10e-6 || fabs(angle - 1) < 10e-6) {/*如果两个柱面轴线是平行的*/
				dof._countR = 2;/*两条边的距离约束会剩余沿着边的方向的平移和以两条边为轴线的转动*/
				dof._countT = 1;

				dof._transform[0]._x = param1[3];/*沿着边的方向的平移自由度*/
				dof._transform[0]._y = param1[4];
				dof._transform[0]._z = param1[5];
			}
			else {
				dof._countR = 3;/*两条边的距离约束会剩余沿着边的方向的平移和以两条边为轴线的转动,以两条轴线形成的平面的法线为方向的任意自由度*/
				dof._countT = 2;

				dof._transform[1]._x = param2[3];/*沿着边的方向的平移自由度*/
				dof._transform[1]._y = param2[4];
				dof._transform[1]._z = param2[5];

				dof._rotation[2]._type = GIVEN_DIR;
				dof._rotation[2]._axis._x = param1[4] * param2[5] - param2[4] * param1[5];
				dof._rotation[2]._axis._y = -(param1[3] * param2[5] - param2[3] * param1[5]);
				dof._rotation[2]._axis._z = param1[3] * param2[4] - param2[3] * param1[4];

				dof._transform[0] = dof._rotation[2]._axis;

			}

			dof._rotation[0]._type = GIVEN_AXIS;/*第一条边为转动轴的转动自由度*/
			dof._rotation[0]._axis._x = param1[3];
			dof._rotation[0]._axis._y = param1[4];
			dof._rotation[0]._axis._z = param1[5];
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_AXIS;/*第二条边为转动轴的转动自由度*/
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

			dof._countR = 2;/*球面和球面的距离约束会剩余两个转动自由度，零个平移自由度*/
			dof._countT = 0;

			dof._rotation[0]._type = GIVEN_POINT;/*第一个转动自由度是过第一个球心的任意一个轴的转动自由度*/
			dof._rotation[0]._point._x = param1[0];
			dof._rotation[0]._point._y = param1[1];
			dof._rotation[0]._point._z = param1[2];

			dof._rotation[1]._type = GIVEN_POINT;/*第二个转动自由度是过第二个球心的任意一个轴的转动自由度*/
			dof._rotation[1]._point._x = param2[0];
			dof._rotation[1]._point._y = param2[1];
			dof._rotation[1]._point._z = param2[2];
		}
	}

	return dof;
}

//TODO：支持线面角，线线角
MidDOF Assembly::angleDOF(Mate mate)
{
	MidDOF dof;
	long type1, type2;
	double param1[8], param2[8];

	mate._entity1->GetType(&type1);//*获取参与装配的两个元素的类型，根据类型进行自由度判断
	mate._entity2->GetType(&type2);

	if (type1 == swSelFACES&&type2 == swSelFACES) {/*如果两个元素都是面*/
		double param[6];
		CComPtr<IFace2> face1, face2;
		CComPtr<ISurface> surface1, surface2;

		face1 = mate._entity1;
		face2 = mate._entity2;
		face1->IGetSurface(&surface1);
		face2->IGetSurface(&surface2);
		surface1->Identity(&type1);/*获取曲线和面的类型*/
		surface2->Identity(&type2);

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {/*如果两个元素都是平面类型*/
			surface1->get_IPlaneParams(param1);/*获取两个平面的参数*/
			surface2->get_IPlaneParams(param2);

			for (int i = 0; i < 3; i++) {/*因为平面的法线方向在param[0-2]中，而最后赋值的时候所有的轴线都是在param[3-5]中的，所以数据迁移*/
				param1[i + 3] = param1[i];
				param2[i + 3] = param2[i];
			}
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2) {/*如果两个面都是柱面，则获取柱面的参数*/
			surface1->get_ICylinderParams(param1);
			surface2->get_ICylinderParams(param2);
		}
		else if ((CYLINDER_TYPE == type1&&CONE_TYPE == type2) || (CONE_TYPE == type1&&CYLINDER_TYPE == type2)) {/*如果两个面是柱面和圆锥面，则分别获取参数*/
			if (CYLINDER_TYPE == type1) {
				surface1->get_ICylinderParams(param1);
				surface2->get_IConeParams(param2);
			}
			else {
				surface2->get_ICylinderParams(param1);
				surface1->get_IConeParams(param2);
			}
		}
		else if (CONE_TYPE == type1&&CONE_TYPE == type2) {/*如果两个面都是圆锥面，获取圆锥面的参数*/
			surface1->get_IConeParams(param1);
			surface2->get_IConeParams(param2);
		}
	}
	else if (type1 == swSelEDGES&&type2 == swSelEDGES) {/*如果两个元素都是线，获取线的相应的参数*/
		CComPtr<ICurve> curve1, curve2;
		CComPtr<IEdge> edge1, edge2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;

		edge1->IGetCurve(&curve1);
		edge2->IGetCurve(&curve2);
		curve1->Identity(&type1);/*获取曲线和面的类型*/
		curve2->Identity(&type2);

		if (type1 == LINE_TYPE&&type2 == LINE_TYPE) {
			curve1->get_ILineParams(param1);
			curve2->get_ILineParams(param2);
		}
	}
	else if ((type1 == swSelEDGES&&type2 == swSelFACES) || (type1 == swSelFACES&&type2 == swSelEDGES)) {/*如果两个元素是边和面，分别获取其参数*/
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
		curve->Identity(&type1);/*获取曲线和面的类型*/
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

	dof._countR = 2;/*所有的角度约束都会剩余两个给定方向的转动自由度，三个平移自由度*/
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

	mate._entity1->GetType(&type1);//*获取参与装配的两个元素的类型，根据类型进行自由度判断
	mate._entity2->GetType(&type2);

	if (swSelEDGES == type1&&swSelEDGES == type2) {/*如果参与装配的两个元素都是线*/
		CComPtr<IEdge> edge1, edge2;
		CComPtr<ICurve> curve1, curve2;

		edge1 = mate._entity1;
		edge2 = mate._entity2;
		edge1->IGetCurve(&curve1);
		edge2->IGetCurve(&curve2);
		curve1->Identity(&type1);
		curve2->Identity(&type2);

		if (LINE_TYPE == type1&&LINE_TYPE == type2) {/*如果参与装配的两个元素都是直线，获取直线的参数*/
			flag = 1;
			curve1->get_ILineParams(param1);
		}
	}
	else if ((swSelEDGES == type1&&swSelFACES == type2) || (swSelFACES == type1&&swSelEDGES == type2)) {/*如果参与装配的元素分别为线和面*/
		CComPtr<IEdge> edge;
		CComPtr<ICurve> curve;
		CComPtr<IFace> face;
		CComPtr<ISurface> surface;

		if (swSelEDGES == type1) {/*面赋值给面指针，线赋值给面指针*/
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
			curve->get_ILineParams(param1);/*如果线的类型是直线，则获取直线的参数*/

			if (PLANE_TYPE == type2) {/*如果参与装配的面的类型是平面，这种配合的自由度与其他的不同，需要单独处理*/
				surface->get_IPlaneParams(param2);

				dof._countT = 3;/*直线-平面的平行配合剩余三个平移自由度，两个转动自由度*/
				dof._countR = 2;

				dof._rotation[0]._type = GIVEN_DIR;/*绕与直线平行的任意轴转动*/
				dof._rotation[0]._axis._x = param1[3];
				dof._rotation[0]._axis._y = param1[4];
				dof._rotation[0]._axis._z = param1[5];

				dof._rotation[1]._type = GIVEN_DIR;/*绕平面法线方向的任意轴的转动*/
				dof._rotation[1]._axis._x = param2[0];
				dof._rotation[1]._axis._y = param2[1];
				dof._rotation[1]._axis._z = param2[2];
			}
			else if (CYLINDER_TYPE == type2 || CONE_TYPE == type2) {/*其他元素配合方式限制的自由度都是一样的，所以到最后统一处理*/
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

		if (PLANE_TYPE == type1&&PLANE_TYPE == type2) {/*如果参与装配的两个元素都是面，获取面的参数*/
			flag = 1;
			surface1->get_IPlaneParams(param1);

			param1[3] = param1[0];/*因为面的法线在param1数组中是第0-2个元素，但是其他的元素的法线（轴）都是在3-5，所以数据迁移，方便统一处理*/
			param1[4] = param1[1];
			param1[5] = param1[2];
		}
		else if (CYLINDER_TYPE == type1&&CYLINDER_TYPE == type2) {/*如果参与装配的元素都是柱面，获取柱面的参数*/
			flag = 1;
			surface1->get_ICylinderParams(param1);
		}
		else if ((CYLINDER_TYPE == type1&&CONE_TYPE == type2) || (CONE_TYPE == type1&&CYLINDER_TYPE == type2)) {/*如果参与装配的元素分别是柱面和锥面，获取其参数（仅获取其中一个就可）*/
			flag = 1;
			if (CYLINDER_TYPE == type1) {/*获取柱面的参数*/
				surface1->get_ICylinderParams(param1);
			}
			else {
				surface2->get_ICylinderParams(param1);
			}
		}
		else if (CONE_TYPE == type1&&CONE_TYPE == type2) {/*如果参与装配的元素是锥面，获取其中一个锥面的参数*/
			flag = 1;
			surface1->get_IConeParams(param1);
		}
	}

	if (1 == flag) {/*统一处理，最后都会剩余三个平移自由度，以及一个转动自由度*/
		dof._countR = 1;
		dof._countT = 3;

		dof._rotation[0]._type = GIVEN_DIR;/*绕平面法线（柱面，锥面，轴线）方向的任意轴的转动自由度*/
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

	part->GetPathName(&name0);/*获取零部件的名字*/
	tempName = name0;
	name = tempName;

	for (std::string keyword : ConnectorList) {/*遍历ConnectorList中的每一项，如果是零部件名字的子串，则为连接件*/
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
	//输出装配体图结构的子图类型以及数量
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
			//向文件中输出装配体中存在配合关系的两个零件的相对运动关系
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
			//计算每一种装配关系的数量,装配关系种类由CNT_RELATIONS定义
			int limitation[CNT_RELATIONS]; memset(limitation, 0, sizeof(limitation));
			for (Mate mate : _relation[i][j]) {
				limitation[mate._mateType]++;
			}
			//向文件中输出每一种装配关系的数量
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
/*获取当前边的值，12-15位表示平移自由度的个数，7-11位表示转动自由度的个数，低8位表示是否有相应的高级配合,如果没有配合关系，返回0*/
int getArcValue(MidDOF dof)
{
	int arcValue = 0;

	if (dof._countR == 3 && dof._countT == 3) {
		return -1;
	}
	else {
		arcValue = dof._countT << 12;
		arcValue |= dof._countR << 8;

		if (dof._constrain.find('S') != -1) {/*如果有对称约束*/
			arcValue |= SwMateSYMMETRIC;
		}
		if (dof._constrain.find('C') != -1) {/*如果有凸轮约束*/
			arcValue |= SwMateCAMFOLLOWER;
		}
		if (dof._constrain.find('G') != -1) {/*如果有齿轮约束*/
			arcValue |= SwMateGEAR;
		}
		if (dof._constrain.find('R') != -1) {/*如果有齿轮齿条约束*/
			arcValue |= SwMateRACKPINION;
		}
		if (dof._constrain.find('P') != -1) {/*如果有路径约束*/
			arcValue |= SwMatePATH;
		}
		if (dof._constrain.find('L') != -1) {/*如果有锁定约束*/
			arcValue |= swMateLOCK;
		}
		if (dof._constrain.find('W') != -1) {/*如果有螺纹约束*/
			arcValue |= SwMateSCREM;
		}
		if (dof._constrain.find('K') != -1) {/*如果有宽度约束*/
			arcValue |= SwMateWIDTH;
		}
	}

	return arcValue;
}

