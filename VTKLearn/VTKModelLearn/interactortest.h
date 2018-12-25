#ifndef INTERACTORTEST_H
#define INTERACTORTEST_H

#include "../Globe/GlobeInclude.h"
#include "GlobeFunc.h"
#include "myfunc.h"

class MouseInteractorStyleTest : public vtkInteractorStyleTrackballCamera{
public:
    bool leftBtnDown = false;//左键是否按下
    bool moving = false;//左键按下时是否在移动

    //绘制线时使用
    bool drawline = false;//是否需要绘制线
    bool initline = true;//是否第一次绘制线
    vtkPoints* clickPoints = vtkPoints::New();//鼠标点击的点和移动的点
    vtkActor* lineactor = vtkActor::New();//绘制的线

    vtkPoints* allPolyPoints = vtkPoints::New();//绘制完多边形内所有点
    vtkActor* allPointActor = vtkActor::New();//绘制完多边形内所有点
    vtkPlane* projectPlan = vtkPlane::New();//将3d点投影到指定的三维平面
    vtkPolygon* m_pNormalPlanePolygon = vtkPolygon::New();//判断点是否在多边形内
    int imagedim[3];double imagespaceing[3];double centerPos[3];//体相关信息

    static MouseInteractorStyleTest *New(){
        return new MouseInteractorStyleTest;
    }

    //鼠标按下事件
    virtual void OnLeftButtonDown(){
        //屏幕坐标
        int* pos = this->GetInteractor()->GetEventPosition();
        double pos3d[3];
		bool bPicked = MyFunc::GetPos3DBy2D_1(this->GetDefaultRenderer(),pos,pos3d);
        if(drawline){
            leftBtnDown = true;
            if(bPicked){
                clickPoints->InsertNextPoint(pos3d[0],pos3d[1],pos3d[2]);
            }
        }
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
    //鼠标弹起事件
    virtual void OnLeftButtonUp(){
        if(drawline){
            drawline = false;
            leftBtnDown = false;
            ChangeShowPointStatus();
            qDebug() << "allPolyPoints:" << allPolyPoints->GetNumberOfPoints();
            DrawPoints();
        }
        this->DefaultRenderer->GetRenderWindow()->Render();
        vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }
    //鼠标移动事件
    virtual void OnMouseMove(){
        if(leftBtnDown && drawline){
            int* pos = this->GetInteractor()->GetEventPosition();
            double pos3d[3];
			bool bPicked = MyFunc::GetPos3DBy2D_1(this->GetDefaultRenderer(),pos,pos3d);
            if(bPicked){
                clickPoints->InsertNextPoint(pos3d[0],pos3d[1],pos3d[2]);
                DrawLine();
            }
        }else{
            vtkInteractorStyleTrackballCamera::OnMouseMove();
        }
    }
    //绘制线
    void DrawLine(){
        if(!initline){
            this->DefaultRenderer->RemoveActor(lineactor);
        }
        double color[3] = {1,0,0};
        qDebug() <<"point size:" << clickPoints->GetNumberOfPoints();
        CreateLineActor(clickPoints,lineactor,color);
        this->DefaultRenderer->AddActor(lineactor);
        initline = false;
        this->DefaultRenderer->GetRenderWindow()->Render();
    }
    //清除线
    void ClearLine(){
        clickPoints->Reset();
        allPolyPoints->Reset();
        if(m_pNormalPlanePolygon!= NULL){
            m_pNormalPlanePolygon->GetPoints()->Reset();
            m_pNormalPlanePolygon->Delete();
            m_pNormalPlanePolygon = NULL;
        }

        if(!initline){
            this->DefaultRenderer->RemoveActor(lineactor);
            this->DefaultRenderer->RemoveActor(allPointActor);
        }
        this->DefaultRenderer->GetRenderWindow()->Render();
    }
    //开始绘制线
    void BeginDrawLine(){
        ClearLine();
        drawline = true;
    }
    //点投影
    void PointProject(double point[3],double projected[3]){
        projectPlan->ProjectPoint(point, projected);
    }
    //初始化投影相关信息
    void InitSomeInfo(){
        projectPlan->SetOrigin(centerPos);
        projectPlan->SetNormal(this->DefaultRenderer->GetActiveCamera()->GetViewPlaneNormal());
        projectPlan->Modified();

        if(m_pNormalPlanePolygon == NULL){
            m_pNormalPlanePolygon = vtkPolygon::New();
        }

        double tempPos[3],tempProject[3];
        for(int i=0; i<clickPoints->GetNumberOfPoints(); i++){
            clickPoints->GetPoint(i,tempPos);
            PointProject(tempPos,tempProject);
            m_pNormalPlanePolygon->GetPoints()->InsertNextPoint(
                        tempProject[0],tempProject[1],tempProject[2]);
        }
    }
    //绘制多边形点集
    void DrawPoints(){
        double color[3] = {0,0,1};
		MyFunc::CreatePointActor(allPolyPoints,allPointActor,color);
        this->DefaultRenderer->AddActor(allPointActor);
        this->DefaultRenderer->GetRenderWindow()->Render();
    }
    //判断三维点是否可以在二维中显示
    void ChangeShowPointStatus(){
        InitSomeInfo();
        double bounds[6] = {0};
        m_pNormalPlanePolygon->GetPoints()->GetBounds(bounds);
        int numPts = m_pNormalPlanePolygon->GetPoints()->GetNumberOfPoints();
        double* pts = static_cast<double*>(m_pNormalPlanePolygon->GetPoints()->GetData()->GetVoidPointer(0));

        QTime timetest;
        timetest.start();
        double* direct = this->DefaultRenderer->GetActiveCamera()->GetViewPlaneNormal();
        double noramlXOY[3] = {0,0,1};
        double nouse;
		if(MyFunc::GetCrossVec(noramlXOY,direct,nouse)<0){
            noramlXOY[2] = -1;
        }
        double noramlXOZ[3] = {0,1,0};
		if(MyFunc::GetCrossVec(noramlXOZ,direct,nouse)<0){
            noramlXOZ[1] = -1;
        }
        double noramlYOZ[3] = {1,0,0};
		if(MyFunc::GetCrossVec(noramlYOZ,direct,nouse)<0){
            noramlYOZ[0] = -1;
        }
        bool planeIsShow[3] = {1,1,1};
        if(direct[0] > -0.1 && direct[0] < 0.1){
            planeIsShow[0] = 0;//YOZ
        }
        if(direct[1] > -0.1 && direct[1] < 0.1){
            planeIsShow[1] = 0;//XOZ
        }
        if(direct[2] > -0.1 && direct[2] < 0.1){
            planeIsShow[2] = 0;//XOY
        }
        qDebug()<< "noramlXOY:" << noramlXOY[0] << " " << noramlXOY[1] << " " << noramlXOY[2];
        qDebug()<< "noramlXOZ:" << noramlXOZ[0] << " " << noramlXOZ[1] << " " << noramlXOZ[2];
        qDebug()<< "noramlYOZ:" << noramlYOZ[0] << " " << noramlYOZ[1] << " " << noramlYOZ[2];
        qDebug()<< "direct:" << direct[0] << " " << direct[1] << " " << direct[2];

        double boxXOY[6] = {0,imagedim[0]*imagespaceing[0],0,imagedim[1]*imagespaceing[1],centerPos[2],centerPos[2]};
        double boxXOZ[6] = {0,imagedim[0]*imagespaceing[0],centerPos[1],centerPos[1],0,imagedim[2]*imagespaceing[2]};
        double boxYOZ[6] = {centerPos[0],centerPos[0],0,imagedim[1]*imagespaceing[1],0,imagedim[2]*imagespaceing[2]};
        double tempP2O[3];
        //轴位图
        if(planeIsShow[2]){
            int dimz = imagedim[2]/2;
            for(int i=0; i<imagedim[0]; i++){
                for(int j=0; j<imagedim[1]; j++){
                    double tempPos222[3] = {i*imagespaceing[0],j*imagespaceing[1],dimz*imagespaceing[2]};
                    tempP2O[0] = i*imagespaceing[0]-centerPos[0];
                    tempP2O[1] = j*imagespaceing[1]-centerPos[1];
                    tempP2O[2] = dimz*imagespaceing[2]-centerPos[2];

                    double crossvalue1,crossvalue2;
                    int tempXOZ1 = 1;
                    int tempXOZ2 = 1;
                    if(planeIsShow[1]==1){
						tempXOZ1 = MyFunc::GetCrossVec(tempP2O,noramlXOZ,crossvalue1);
						tempXOZ2 = MyFunc::GetCrossVec(direct,noramlXOZ,crossvalue2);
                    }
                    if(tempXOZ1*tempXOZ2>0){
                        bool isshow = true;
                        if(planeIsShow[0]==1){
                            isshow = PointInPlane(tempP2O,noramlYOZ,direct,tempPos222,boxYOZ,1);
                        }
                        if(isshow){
                            double tempPlacePos[3];
                            PointProject(tempPos222,tempPlacePos);
                            bool isin = m_pNormalPlanePolygon->PointInPolygon(tempPlacePos,numPts,pts,bounds,direct);
                            if(isin){
                                allPolyPoints->InsertNextPoint(i*imagespaceing[0],j*imagespaceing[1],dimz*imagespaceing[2]);
                            }

                        }
                    }else{
                        bool isshow = true;
                        double intersectPos[3];
                        double tempk = -(crossvalue1/crossvalue2);
                        GetIntersectPoint(tempPos222,direct,tempk,intersectPos);
                        isshow = IsShowPos(intersectPos,boxXOZ,2);
                        if(isshow){
                            isshow = PointInPlane(tempP2O,noramlYOZ,direct,tempPos222,boxYOZ,1);
                            if(isshow){
                                double tempPlacePos[3];
                                PointProject(tempPos222,tempPlacePos);
                                bool isin = m_pNormalPlanePolygon->PointInPolygon(tempPlacePos,numPts,pts,bounds,direct);
                                if(isin){
                                    allPolyPoints->InsertNextPoint(i*imagespaceing[0],j*imagespaceing[1],dimz*imagespaceing[2]);
                                }
    //                            clickPoints2->InsertNextPoint(i*imagespaceing[0],j*imagespaceing[1],dimz*imagespaceing[2]);
                            }
                        }
                    }
                }
            }
        }

        //冠位图
        if(planeIsShow[1]){
            int dimy = imagedim[1]/2;
            for(int i=0; i<imagedim[0]; i++){
                for(int j=0; j<imagedim[2]; j++){
                    double tempPos222[3] = {i*imagespaceing[0],dimy*imagespaceing[1],j*imagespaceing[2]};
                    tempP2O[0] = i*imagespaceing[0]-centerPos[0];
                    tempP2O[1] = dimy*imagespaceing[1]-centerPos[1];
                    tempP2O[2] = j*imagespaceing[2]-centerPos[2];
                    double crossvalue1,crossvalue2;
                    int tempXOY1 = 1;
                    int tempXOY2 = 1;
                    if(planeIsShow[2]==1){
						tempXOY1 = MyFunc::GetCrossVec(tempP2O,noramlXOY,crossvalue1);
						tempXOY2 = MyFunc::GetCrossVec(direct,noramlXOY,crossvalue2);
                    }

                    if(tempXOY1*tempXOY2>0){
                        bool isshow = true;
                        if(planeIsShow[0]==1){
                            isshow = PointInPlane(tempP2O,noramlYOZ,direct,tempPos222,boxYOZ,1);
                        }
                        if(isshow){
                            double tempPlacePos[3];
                            PointProject(tempPos222,tempPlacePos);
                            bool isin = m_pNormalPlanePolygon->PointInPolygon(tempPlacePos,numPts,pts,bounds,direct);
                            if(isin){
                                allPolyPoints->InsertNextPoint(tempPos222[0],tempPos222[1],tempPos222[2]);
                            }
    //                        clickPoints->InsertNextPoint(i*imagespaceing[0],dimy*imagespaceing[1],j*imagespaceing[2]);
                        }
                    }else{
                        bool isshow = true;
                        double intersectPos[3];
                        double tempk = -(crossvalue1/crossvalue2);
                        GetIntersectPoint(tempPos222,direct,tempk,intersectPos);
                        isshow = IsShowPos(intersectPos,boxXOY,3);
                        if(isshow){
                            isshow = PointInPlane(tempP2O,noramlYOZ,direct,tempPos222,boxYOZ,1);
                            if(isshow){
                                double tempPlacePos[3];
                                PointProject(tempPos222,tempPlacePos);
                                bool isin = m_pNormalPlanePolygon->PointInPolygon(tempPlacePos,numPts,pts,bounds,direct);
                                if(isin){
                                    allPolyPoints->InsertNextPoint(tempPos222[0],tempPos222[1],tempPos222[2]);
                                }
    //                            clickPoints2->InsertNextPoint(i*imagespaceing[0],dimy*imagespaceing[1],j*imagespaceing[2]);
                            }
                        }
                    }
                }
            }
        }

        //矢位图
        if(planeIsShow[0]){
            int dimx = imagedim[0]/2;
            for(int i=0; i<imagedim[1]; i++){
                for(int j=0; j<imagedim[2]; j++){
                    double tempPos222[3] = {dimx*imagespaceing[0],i*imagespaceing[1],j*imagespaceing[2]};
                    tempP2O[0] = dimx*imagespaceing[0]-centerPos[0];
                    tempP2O[1] = i*imagespaceing[1]-centerPos[1];
                    tempP2O[2] = j*imagespaceing[2]-centerPos[2];
                    double crossvalue1,crossvalue2;
                    int tempXOY1 = 1;
                    int tempXOY2 = 1;
                    if(planeIsShow[2]==1){
						tempXOY1 = MyFunc::GetCrossVec(tempP2O,noramlXOY,crossvalue1);
						tempXOY2 = MyFunc::GetCrossVec(direct,noramlXOY,crossvalue2);
                    }

                    if(tempXOY1*tempXOY2>0){
                        bool isshow = true;
                        if(planeIsShow[1]==1){
                            isshow = PointInPlane(tempP2O,noramlXOZ,direct,tempPos222,boxXOZ,2);
                        }
                        if(isshow){
                            double tempPlacePos[3];
                            PointProject(tempPos222,tempPlacePos);
                            bool isin = m_pNormalPlanePolygon->PointInPolygon(tempPlacePos,numPts,pts,bounds,direct);
                            if(isin){
                                allPolyPoints->InsertNextPoint(tempPos222[0],tempPos222[1],tempPos222[2]);
                            }
    //                        clickPoints->InsertNextPoint(dimx*imagespaceing[0],i*imagespaceing[1],j*imagespaceing[2]);
                        }
                    }else{
                        bool isshow = true;
                        double intersectPos[3];
                        double tempk = -(crossvalue1/crossvalue2);
                        GetIntersectPoint(tempPos222,direct,tempk,intersectPos);
                        isshow = IsShowPos(intersectPos,boxXOY,3);
                        if(isshow){
                            isshow = PointInPlane(tempP2O,noramlXOZ,direct,tempPos222,boxXOZ,2);
                            if(isshow){
                                double tempPlacePos[3];
                                PointProject(tempPos222,tempPlacePos);
                                bool isin = m_pNormalPlanePolygon->PointInPolygon(tempPlacePos,numPts,pts,bounds,direct);
                                if(isin){
                                    allPolyPoints->InsertNextPoint(tempPos222[0],tempPos222[1],tempPos222[2]);
                                }
    //                            clickPoints2->InsertNextPoint(dimx*imagespaceing[0],i*imagespaceing[1],j*imagespaceing[2]);
                            }
                        }
                    }
                }
            }
        }
        qDebug() << "timetest:" << timetest.elapsed()/1000.0;
    }
    //点是否被平面遮挡
    bool PointInPlane(double P2ODirect[3],double normal[3],double direct[3],double Point[3],double bound[6],int type){
        double crossvalue1,crossvalue2;
		int temp1 = MyFunc::GetCrossVec(P2ODirect,normal,crossvalue1);
		int temp2 = MyFunc::GetCrossVec(direct,normal,crossvalue2);
        if(temp1*temp2 > 0){
            return true;
        }else{
            bool isshow = true;
            double intersectPos[3];
            double tempk = -(crossvalue1/crossvalue2);
            GetIntersectPoint(Point,direct,tempk,intersectPos);
            isshow = IsShowPos(intersectPos,bound,type);
            if(isshow){
                return true;
            }
        }
        return false;
    }
    //射线在平面的交点
    void GetIntersectPoint(double point[3],double direction[3],double k,double intersectpos[3]){
        intersectpos[0] = point[0] + k*direction[0];
        intersectpos[1] = point[1] + k*direction[1];
        intersectpos[2] = point[2] + k*direction[2];
    }
    //判断交点是否在可见平面内
    bool IsShowPos(double point[3],double box[6],int type){
        if(type == 3){//垂直z
            if(point[0] >= box[0] && point[0] <= box[1] &&
               point[1] >= box[2] && point[1] <= box[3] ){
                return false;
            }
        }else if(type == 2){//垂直y
            if(point[0] >= box[0] && point[0] <= box[1] &&
//               point[1] >= box[2] && point[1] <= box[3] &&
               point[2] >= box[4] && point[2] <= box[5]){
                return false;
            }
        }else{//垂直x
            if(point[1] >= box[2] && point[1] <= box[3] &&
               point[2] >= box[4] && point[2] <= box[5]){
                return false;
            }
        }
        return true;
    }
};

#endif // INTERACTORTEST_H
