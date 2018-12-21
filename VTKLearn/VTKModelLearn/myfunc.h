#ifndef MYFUNC_H
#define MYFUNC_H

#include "GlobeInclude.h"

class MyFunc
{
public:
	MyFunc();
	//对题进行二值化处理
	static void VolumeThresholding(vtkImageData* data,double minV,double maxV,
									 vtkImageData* resData);
};

#endif // MYFUNC_H
