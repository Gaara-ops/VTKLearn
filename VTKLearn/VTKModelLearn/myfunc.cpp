#include "myfunc.h"

MyFunc::MyFunc()
{
}

void MyFunc::VolumeThresholding(vtkImageData *data, double minV,
							double maxV, vtkImageData *resData, bool saveData)
{
	if(!saveData){
		vtkImageThreshold* imageThreshold =vtkImageThreshold::New();
		imageThreshold->SetInputData(data);
		imageThreshold->ThresholdBetween(minV, maxV);
		imageThreshold->ReplaceInOn();
		imageThreshold->SetInValue(2048);
		imageThreshold->SetOutValue(-2048);
		imageThreshold->Update();
		resData->DeepCopy(imageThreshold->GetOutput());
		imageThreshold->Delete();
	}else{
		resData->DeepCopy(data);
		vtkDataArray* dataarr =resData->GetPointData()->GetScalars();
		int nums = dataarr->GetNumberOfTuples();
		for(int i=0;i<nums;i++){
			double value = dataarr->GetComponent(i,0);
			if(value < minV || value > maxV){
				dataarr->SetComponent(i,0,-2048);
			}
		}
	}
	qDebug() << "end";
}

bool MyFunc::GetPos3DBy2D_1(vtkRenderer *render, int *pos, double pos3d[])
{
	vtkSmartPointer<vtkCellPicker> picker =
	  vtkSmartPointer<vtkCellPicker>::New();
	picker->SetTolerance(0.0005);
	bool isPicked = picker->Pick(pos[0], pos[1], 0, render);
	if(isPicked){
		double* temppos = picker->GetPickPosition();
		pos3d[0]=temppos[0];pos3d[1]=temppos[1];pos3d[2]=temppos[2];
	}else{
		return false;
	}
	return true;
}

void MyFunc::CreatePointActor(vtkPoints *points, vtkActor *pActor,
							   double color[], float pSize, double opacity)
{
	vtkSmartPointer<vtkPolyData> polydata =
	  vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(points);

	vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
		vtkSmartPointer<vtkVertexGlyphFilter>::New();
	glyphFilter->SetInputData(polydata);
	glyphFilter->Update();
	vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(glyphFilter->GetOutput());
	pActor->SetMapper(mapper);
	pActor->GetProperty()->SetPointSize(pSize);
	pActor->GetProperty()->SetColor(color);
	pActor->GetProperty()->SetOpacity(opacity);
}

int MyFunc::GetCrossVec(double vec1[], double vec2[], double &crossvalue)
{
	double temp = vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
	crossvalue = temp;
	if(temp > 0){
		return 1;
	}else if(temp < 0){
		return -1;
	}else{
		return 0;
	}
}
