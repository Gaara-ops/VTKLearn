#ifndef MYFUNC_H
#define MYFUNC_H

#include "../Globe/GlobeInclude.h"

class MyFunc
{
public:
	MyFunc();
	//对题进行二值化处理
	static void VolumeThresholding(vtkImageData* data,double minV,double maxV,
									 vtkImageData* resData,bool saveData=true);
	//2d转3d点--到物体表面
	static bool GetPos3DBy2D_1(vtkRenderer* render,int* pos,double pos3d[3]);
	//创建点接口---点集
	static void CreatePointActor(vtkPoints* points,vtkActor* pActor,double color[3],
							float pSize=3,double opacity=0.2);
	//向量x乘
	static int GetCrossVec(double vec1[3],double vec2[3],double& crossvalue);
};

#endif // MYFUNC_H
