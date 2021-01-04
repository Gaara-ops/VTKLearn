#ifndef MOUSEINTERACTORSTYLE_H
#define MOUSEINTERACTORSTYLE_H
#include "../Globe/GlobeInclude.h"
#include "../Globe/myfunc.h"
#include "volumeinfo.h"
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera{
public:
	static MouseInteractorStyle *New(){
		return new MouseInteractorStyle;
	}
	bool lastClickRes = false;
	double lastPos[3] = {0};
	VolumeInfo *seedUpdate = NULL;
    vtkImageData* imagedata = NULL;
	//鼠标按下事件
	virtual void OnLeftButtonDown(){
        /*测试批量空间坐标转换*/
		int* pos = this->GetInteractor()->GetEventPosition();
		lastClickRes = MyFunc::GetPos3DBy2D_1(this->GetDefaultRenderer(),pos,lastPos);
		if(lastClickRes){
			//qDebug()<<"pos:"<<lastPos[0]<< lastPos[1]<<lastPos[2];
            //qDebug() << "true";
        }else{
            //qDebug() << "false";
        }
        if(seedUpdate != NULL){
			if(lastClickRes){
				seedUpdate->initSeedInfo(lastPos);
			}
        }
		/*QTime testtime;
		testtime.start();

		double     mat[16];
		double     view[4];
		vtkMatrix4x4::DeepCopy(mat, this->GetDefaultRenderer()->
					GetActiveCamera()->GetCompositeProjectionTransformMatrix(
						this->GetDefaultRenderer()->GetTiledAspectRatio(),0,1));

		double dx,dy;
		int sizex,sizey;
		int *size = NULL;
		size = this->GetDefaultRenderer()->GetRenderWindow()->GetSize();
		if (!size)
		{
		  return;
		}
		sizex = size[0];
		sizey = size[1];
		double *Viewport = this->GetDefaultRenderer()->GetViewport();

		double* spacing = imagedata->GetSpacing();
		int *dim = imagedata->GetDimensions();
		int imagepos[2];
		for(int z=0;z<dim[2];z++){
			for(int j=0;j<dim[1];j++){
				for(int i=0;i<dim[0];i++){
					double x = i*spacing[0];
					double y = j*spacing[1];
					double z = z*spacing[2];
					view[0] = x*mat[0] + y*mat[1] + z*mat[2] + mat[3];
					view[1] = x*mat[4] + y*mat[5] + z*mat[6] + mat[7];
					view[2] = x*mat[8] + y*mat[9] + z*mat[10] + mat[11];
					view[3] = x*mat[12] + y*mat[13] + z*mat[14] + mat[15];
					if (view[3] != 0.0)
					{
					  x = view[0]/view[3];
					  y = view[1]/view[3];
					  z = view[2]/view[3];
					}
					dx = (x + 1.0) *(sizex*(Viewport[2]-Viewport[0])) / 2.0 +
						sizex*Viewport[0];
					dy = (y + 1.0) *(sizey*(Viewport[3]-Viewport[1])) / 2.0 +
						sizey*Viewport[1];
				}
			}
		}
		qDebug() << testtime.elapsed()/1000.0;*/
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
};

#endif // MOUSEINTERACTORSTYLE_H
