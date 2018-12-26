#ifndef MOUSEINTERACTORSTYLE_H
#define MOUSEINTERACTORSTYLE_H
#include "../Globe/GlobeInclude.h"
#include "myfunc.h"
#include "volumeinfo.h"
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera{
public:
	static MouseInteractorStyle *New(){
		return new MouseInteractorStyle;
	}
	bool lastClickRes = false;
	double lastPos[3] = {0};
	VolumeInfo *seedUpdate = NULL;
	//鼠标按下事件
	virtual void OnLeftButtonDown(){
		//屏幕坐标
		int* pos = this->GetInteractor()->GetEventPosition();
		lastClickRes = MyFunc::GetPos3DBy2D_1(this->GetDefaultRenderer(),pos,lastPos);
		if(lastClickRes){
			//qDebug()<<"pos:"<<lastPos[0]<< lastPos[1]<<lastPos[2];
		}
		if(seedUpdate != NULL){
			if(lastClickRes){
				seedUpdate->initSeedInfo(lastPos);
			}
		}
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
};

#endif // MOUSEINTERACTORSTYLE_H
