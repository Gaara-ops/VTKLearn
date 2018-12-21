#include "myfunc.h"

MyFunc::MyFunc()
{
}

void MyFunc::VolumeThresholding(vtkImageData *data, double minV,
								double maxV, vtkImageData *resData)
{
	/*
	vtkImageThreshold* imageThreshold =vtkImageThreshold::New();
	imageThreshold->SetInputData(data);
	imageThreshold->ThresholdBetween(minV, maxV);
	imageThreshold->ReplaceInOn();
	imageThreshold->SetInValue(2048);
	imageThreshold->SetOutValue(-2048);
	imageThreshold->Update();
	resData->DeepCopy(imageThreshold->GetOutput());
	imageThreshold->Delete();*/

	resData->DeepCopy(data);
	vtkDataArray* dataarr =resData->GetPointData()->GetScalars();
	int nums = dataarr->GetNumberOfTuples();
	for(int i=0;i<nums;i++){
		double value = dataarr->GetComponent(i,0);
		if(value < minV || value > maxV){
			dataarr->SetComponent(i,0,-2048);
		}
	}
	qDebug() << "end";
}
