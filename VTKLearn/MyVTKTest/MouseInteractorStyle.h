#ifndef MOUSEINTERACTORSTYLE_H
#define MOUSEINTERACTORSTYLE_H
#include "../Globe/GlobeInclude.h"
#include "../Globe/myfunc.h"
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera{
public:
	static MouseInteractorStyle *New(){
		return new MouseInteractorStyle;
	}
	bool lastClickRes = false;
    double lastPos[3] = {0};
    double nowPos[3] = {0};
	vtkImageData* imagedata = NULL;
    vtkPoints* allClickPos = vtkPoints::New();
    bool bgetpos = false;
	//鼠标按下事件
	virtual void OnLeftButtonDown(){
        //测试批量空间坐标转换
		int* pos = this->GetInteractor()->GetEventPosition();
        lastClickRes = MyFunc::GetPos3DBy2D_1(this->GetDefaultRenderer(),pos,nowPos);
        if(bgetpos){
            allClickPos->InsertNextPoint(nowPos);
            createSphere(nowPos);
        }
        /*if(lastClickRes){
            qDebug()<<"lastPos:"<<lastPos[0]<< lastPos[1]<<lastPos[2];
            qDebug()<<"nowPos:"<<nowPos[0]<< nowPos[1]<<nowPos[2];
            double dd2 = vtkMath::Distance2BetweenPoints(nowPos,lastPos);
            qDebug() << "distance:" << sqrt(dd2)<<"mm";
            for(int i=0;i<3;i++){
                lastPos[i] = nowPos[i];
            }
        }*/
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}

    void createSphere(double center[3]){
        vtkSphereSource* m_sphere =vtkSphereSource::New();
        m_sphere->SetCenter(center);
        m_sphere->SetRadius(1);
        m_sphere->SetPhiResolution(100);
        m_sphere->SetThetaResolution(100);
        vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(m_sphere->GetOutputPort());
        vtkSmartPointer<vtkActor> actor =
          vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1,0,0);
        this->GetDefaultRenderer()->AddActor(actor);
    }
};

#endif // MOUSEINTERACTORSTYLE_H
