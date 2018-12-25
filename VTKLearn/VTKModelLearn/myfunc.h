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
	//创建线接口
	static void CreateLineActor(vtkPoints* points,vtkActor* lactor,
						 double color[3],int linewidth=1);
	//读取dicom文件
	static void ReadDicomData(vtkDICOMImageReader* reader,char* path,
					   int dim[3],double spacing[3],double range[2],
						vtkImageData*& imagedata,vtkDataArray*& scalars);
	//对数据进行抽样
	static void ResampleData(vtkImageData* imagedata,float factor[3]);
	//生成体
	static void CreateVolume(vtkImageData* imagedata,int blendmode,
							bool autosample,float sampledis,vtkRenderer* render);
	//生成面
	static void CreateSurface(vtkImageData* imagedata,double value,
						double ocapity,double color[3],vtkRenderer* render,
						bool getLarge=false,int mode=4);
	//生成三个切片
	static void CreateThreeSlice(int dim[3],vtkRenderer* render,
							vtkDICOMImageReader* reader);
	//读取文件
	static vtkSmartPointer<vtkPolyData> ReadPolyData(const char *fileName);
	//使用Frustum切割
	static void CreateClipFrustum(vtkRenderer* renderer,const char *fileName);
	//显示一系列dicom图像
	static void ShowSeriesDicom(vtkDICOMImageReader* reader);
	//三维体区域生产
	static void VolumeSeedGrowth(int startDim[3],vtkImageData* imagedata);
};

#endif // MYFUNC_H
