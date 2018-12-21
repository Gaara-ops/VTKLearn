#ifndef INTERACTORTEST_H
#define INTERACTORTEST_H

#include<vtkInteractorStyleTrackballCamera.h>
#include<vtkPoints.h>
#include<vtkLineSource.h>
#include<vtkCellPicker.h>
#include<vtkPolyDataMapper.h>
#include<vtkProperty.h>
#include<vector>
#include<vtkRenderer.h>
#include<vtkCamera.h>
#include<vtkPlane.h>
#include<vtkImageData.h>
#include<vtkMath.h>

#include<vtkCoordinate.h>
#include<vtkMatrix4x4.h>
#include<math.h>
#include<QTime>
#include<QDebug>
#include<QWaitCondition>
#include<synchapi.h>
#include<vtkPolygon.h>
#include<QVector>
#include<QMap>
#include<QList>
#include<QVector3D>
#include<vtkAssemblyPath.h>
#include<vtkLODProp3D.h>
#include<vtkImageSlice.h>
#include<vtkAbstractMapper3D.h>
#include<vtkBox.h>
#include<vtkProp3DCollection.h>
#include <vtkCoordinate.h>
#include<vtkRenderWindow.h>
#include<vtkMath.h>
#include<QObject>
#include<vtkImageResliceMapper.h>
#include<vtkImageResample.h>
#include<vtkVector.h>
#include<vtkTransform.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include<vtkSpline.h>
#include<QTime>
#include<vtkSphereSource.h>
#include<vtkProbeFilter.h>
#include<vtkWindowLevelLookupTable.h>
#include<vtkDataSetMapper.h>
#include<vtkColorTransferFunction.h>
#include<vtkPointData.h>
#include<vtkDoubleArray.h>
#include<vtkWorldPointPicker.h>

class MouseInteractorStyleTest : public vtkInteractorStyleTrackballCamera{
public:
    int dimension[3];//维数
    double spacing[3];//间隔
    vtkImageResliceMapper* slicemapper = NULL;//控制轴冠矢图
    vtkImageData* imageData = NULL;//原始数据
    vtkImageResample* reSample = NULL;//抽样数据

    bool lbdown = false;//鼠标是否按下
    bool mousemove = false;//鼠标是否按下并移动
    bool surfaceRect = false;//是否进行面重建
    bool rotateslice = false;//切片是否跟随鼠标移动旋转

    vtkLineSource* linesource = NULL;//绘制直线
    vtkPoints* points = vtkPoints::New();//点击的点集合

    vtkParametricSpline* spline = NULL;//绘制曲线
    int splinenum = 200;
    //获取绘制曲线的点集合
    vtkSmartPointer<vtkParametricFunctionSource> functionSource =
      vtkSmartPointer<vtkParametricFunctionSource>::New();

    vtkImageResliceMapper* view4SliceMapper = NULL;//用于显示重建的面
    vtkRenderer* view4Renderer = NULL;//用于显示重建的面的渲染对象

    //投影面
    vtkPlane* planeproject = NULL;
    double dcolor[3] = {1.0,0,0};
    double projcolor[3] = {1.0,1.0,0};

    static MouseInteractorStyleTest *New(){
        return new MouseInteractorStyleTest;
    }
    void Delete(){
        if(reSample != NULL){
            reSample->Delete();
        }
        if(linesource != NULL){
            linesource->Delete();
        }
        if(spline != NULL){
            spline->Delete();
        }
        points->Delete();
    }
    //获取鼠标当前点击坐标
    void getButtonPos(double pos3d[3]){
        int* pos = this->GetInteractor()->GetEventPosition();

        vtkCamera* cametratest = this->DefaultRenderer->GetActiveCamera();

        /*使用vtkWorldPointPicker进行坐标转换
        vtkSmartPointer<vtkWorldPointPicker> testpicker =
          vtkSmartPointer<vtkWorldPointPicker>::New();
        testpicker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());
        double* testpos = testpicker->GetPickPosition();
        qDebug() << "testpos-------:" << testpos[0] <<
                    " " << testpos[1] << " " << testpos[2];*/


        qDebug() << "test scale click:" << this->GetDefaultRenderer()->GetActiveCamera()->GetParallelScale();

        /*屏幕坐标*/
        vtkSmartPointer<vtkCellPicker> picker =
          vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.0005);
        int reslut = picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());
        if(reslut){
            double* worldPosition = picker->GetPickPosition();
            qDebug() << "worldPosition-------:" << worldPosition[0] <<
                        " " << worldPosition[1] << " " << worldPosition[2];
            pos3d[0] = worldPosition[0];
            pos3d[1] = worldPosition[1];
            pos3d[2] = worldPosition[2];
        }


        float* pixtest = this->GetDefaultRenderer()->GetRenderWindow()->GetRGBAPixelData(pos[0],pos[1],pos[0],pos[1],1);
        qDebug()<< "test:" << (*pixtest) << " " << (*(pixtest+1))
                         << " " << (*(pixtest+2))
                         << " " << (*(pixtest+3));
    }

    //鼠标按下事件
    virtual void OnLeftButtonDown(){
        double temppos2[3];
        getButtonPos(temppos2);
        lbdown = true;
        if(surfaceRect){
            createPoint(temppos2,dcolor);
            double projpos[3];
            projectPoint(temppos2,projpos);
            qDebug() << "projpos-------:" << projpos[0] <<
                        " " << projpos[1] << " " << projpos[2];
            createPoint(projpos,projcolor);
            points->InsertNextPoint(projpos[0], projpos[1], projpos[2]);
        }
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
    //鼠标弹起事件
    virtual void OnLeftButtonUp(){
        lbdown = false;
        mousemove = false;
        if(surfaceRect){
            if(points->GetNumberOfPoints() > 1){
                createSpline();//绘制曲线
                QTime m;
                m.start();
                curveReconstruction();
                qDebug() << "spend time:" << m.elapsed()/1000.0;
                /*createLine();
                surfaceReconstruction();
                surfaceRect = false;*/
            }
        }

        if(rotateslice){
            slicemapper->SetInputData(imageData);
            slicemapper->GetSlicePlane()->SetNormal(this->GetDefaultRenderer()->GetActiveCamera()->GetViewPlaneNormal());
            slicemapper->Modified();
        }
        vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }
    //鼠标移动事件
    virtual void OnMouseMove(){
        if(slicemapper == NULL){
            return;
        }
        if(!lbdown)
            return;

        //以下代码只针对切片跟随鼠标旋转
        if(!mousemove){
            mousemove = true;
            slicemapper->SetInputData(reSample->GetOutput());
        }
        if(rotateslice){
            slicemapper->GetSlicePlane()->SetNormal(this->GetDefaultRenderer()->GetActiveCamera()->GetViewPlaneNormal());
            slicemapper->Modified();
        }
        vtkInteractorStyleTrackballCamera::OnMouseMove();
    }
    //图像抽样
    void reSampleData(){
        if(reSample == NULL){
            reSample = vtkImageResample::New();
            reSample->SetInputData(imageData);
            reSample->SetAxisMagnificationFactor(0,0.5);//x轴抽样0.5
            reSample->SetAxisMagnificationFactor(1,0.5);//y轴抽样0.5
            reSample->SetAxisMagnificationFactor(2,1);//z轴抽样1
            reSample->Update();
        }
    }
    //绘制直线
    void createLine(){
        if(linesource == NULL){
            linesource = vtkLineSource::New();
            linesource->SetPoints(points);
            // Visualize
            vtkSmartPointer<vtkPolyDataMapper> mapper =
              vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(linesource->GetOutputPort());
            vtkSmartPointer<vtkActor> actor =
              vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetLineWidth(2);
            actor->GetProperty()->SetColor(1.0,0,0);
            this->DefaultRenderer->AddActor(actor);
        }else{
            linesource->SetPoints(points);
            linesource->Modified();
        }
        this->DefaultRenderer->Render();
    }
    //绘制曲线
    void createSpline(){
        if(spline == NULL){
            spline = vtkParametricSpline::New();
            spline->SetPoints(points);

            functionSource->SetUResolution(splinenum);
            functionSource->SetVResolution(splinenum);
            functionSource->SetWResolution(splinenum);
            functionSource->SetParametricFunction(spline);
            functionSource->Update();

            vtkSmartPointer<vtkPolyDataMapper> mapper =
              vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(functionSource->GetOutputPort());

            vtkSmartPointer<vtkActor> actor =
              vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);
            actor->GetProperty()->SetColor(0,1.0,0);
            actor->GetProperty()->SetLineWidth(2.0);
            this->DefaultRenderer->AddActor(actor);
        }else{
            spline->SetPoints(points);
            spline->Modified();
        }
        this->DefaultRenderer->Render();
    }

    //平面重建
    void surfaceReconstruction(){
        qDebug() << "surfaceReconstruction:";
        double temp0[3],temp1[3];
        points->GetPoint(0,temp0);
        points->GetPoint(1,temp1);
        vtkVector3d v(temp1[0]-temp0[0],temp1[1]-temp0[1],temp1[2]-temp0[2]);
        vtkVector3d vn = v.Normalized();
        double *nor = this->GetDefaultRenderer()->GetActiveCamera()->GetViewPlaneNormal();
        vtkVector3d normal(nor[0],nor[1],nor[2]);
        vtkVector3d rl = vn.Cross(normal);
        view4SliceMapper->GetSlicePlane()->SetOrigin((temp1[0]+temp0[0])/2,(temp1[1]+temp0[1])/2,(temp1[2]+temp0[2])/2);
        view4SliceMapper->GetSlicePlane()->SetNormal(rl.GetX(),rl.GetY(),rl.GetZ());
        view4SliceMapper->Modified();
        view4Renderer->Render();
        double xangel = acos(rl.GetX())*180.0 / vtkMath::Pi();
        double yangel = acos(rl.GetY())*180.0 / vtkMath::Pi();
        double zangel = acos(rl.GetZ())*180.0 / vtkMath::Pi();
        qDebug() << "angel:" << xangel << " "<< yangel << " "<< zangel << " ";
        view4Renderer->GetRenderWindow()->Render();
    }
    //创建点
    void createPoint(double* pos,double* color){
        vtkSmartPointer<vtkSphereSource> sphereSource =
          vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetCenter(pos[0], pos[1], pos[2]);
        sphereSource->SetRadius(8.0);

        vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sphereSource->GetOutputPort());

        vtkSmartPointer<vtkActor> actor =
          vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(color);
        this->DefaultRenderer->AddActor(actor);
    }

    vtkSmartPointer<vtkPolyData> SweepLine (vtkPolyData *line,
                                            double direction[3],
                                            double distance,
                                            unsigned int cols)
    {
      unsigned int rows = line->GetNumberOfPoints();
      double spacing = 1.0;//distance / cols;
      vtkSmartPointer<vtkPolyData> surface =
        vtkSmartPointer<vtkPolyData>::New();

      // Generate the points
      cols++;
      unsigned int numberOfPoints = rows * cols;
      unsigned int numberOfPolys = (rows - 1) * (cols - 1);
      vtkSmartPointer<vtkPoints> points =
        vtkSmartPointer<vtkPoints>::New();
      points->Allocate(numberOfPoints);
      vtkSmartPointer<vtkCellArray> polys =
        vtkSmartPointer<vtkCellArray>::New();
      polys->Allocate(numberOfPolys*4);

      double x[3];
      unsigned int cnt = 0;
      for (unsigned int row = 0; row < rows; row++)
      {
          for (unsigned int col = 0; col < cols; col++)
          {
              double p[3];
              line->GetPoint(row, p);
              x[0] = p[0] + direction[0] * col * spacing;
              x[1] = p[1] + direction[1] * col * spacing;
              x[2] = p[2] + direction[2] * col * spacing;
              points->InsertPoint(cnt++, x);
          }
      }
      // Generate the quads
      vtkIdType pts[4];
      for (unsigned int row = 0; row < rows - 1; row++)
      {
          for (unsigned int col = 0; col < cols - 1; col++)
          {
              pts[0] = col + row * (cols);
              pts[1] = pts[0] + 1;
              pts[2] = pts[0] + cols + 1;
              pts[3] = pts[0] + cols;
              polys->InsertNextCell(4,pts);
          }
      }
      surface->SetPoints(points);
      surface->SetPolys(polys);

      return surface;
    }

    //设置曲线拉直面上的点
    void setPolyDataValue(vtkDataArray* polydataarr,int len,double* direction){
        /*
         * 遍历绘制曲面上的点
         */
        int pointsnum = functionSource->GetOutput()->GetNumberOfPoints();
        qDebug() << "spline points:" << pointsnum;
        double tempposS[3];
        int polydataIndex = 0;
        for(int i=0; i<pointsnum; i++){
            functionSource->GetOutput()->GetPoint(i,tempposS);
            pointExtend(tempposS,polydataarr,polydataIndex,len,direction);
        }
    }
    //点拉伸
    void pointExtend(double posS[3],vtkDataArray* arr,int& dataindex,int len,double* direction){
        double temppos[3];
        for(int i=0; i<len+1; i++ ){
            temppos[0] = posS[0]+direction[0]*i;
            temppos[1] = posS[1]+direction[1]*i;
            temppos[2] = posS[2]+direction[2]*i;
            setpointData(temppos,arr,dataindex);
            ++dataindex;
        }
    }
    //点赋值
    void setpointData(double posS[3],vtkDataArray* arr,int& dataindex){
        int tempposD[3];
        tempposD[0] = int(posS[0]/spacing[0]);
        tempposD[1] = int(posS[1]/spacing[1]);
        tempposD[2] = int(posS[2]/spacing[2]);
        if(tempposD[0] >=0 && tempposD[0] <dimension[0] &&
                tempposD[1] >=0 && tempposD[1] <dimension[1] &&
                tempposD[2] >=0 && tempposD[2] <dimension[2]){
            short* pixelData =  (short *) (imageData->GetScalarPointer(tempposD[0],tempposD[1],tempposD[2]));
            arr->InsertComponent(dataindex,0,*pixelData);
        }else{
            arr->InsertComponent(dataindex,0,-2048);
        }

    }

    //曲面重建
    void curveReconstruction(){
        QTime mm;
        mm.start();
        double* direction =
                this->GetDefaultRenderer()->GetActiveCamera()->GetDirectionOfProjection();

        double tempxyz[3] = {dimension[0]*spacing[0],dimension[1]*spacing[1],dimension[2]*spacing[2]};
        double radius2 = sqrt(tempxyz[0]*tempxyz[0]+tempxyz[1]*tempxyz[1]+tempxyz[2]*tempxyz[2]);
        qDebug() << "radius2:" << radius2;
        double distance = radius2;
        unsigned int resolution = radius2;

        qDebug() << "surface before" << mm.elapsed()/1000.0;
        vtkSmartPointer<vtkPolyData> surface =
          SweepLine(functionSource->GetOutput(),
                    direction,
                    distance,
                    resolution);
        qDebug() << "surface after" << mm.elapsed()/1000.0;
        /*vtkSmartPointer<vtkProbeFilter> sampleVolume =
          vtkSmartPointer<vtkProbeFilter>::New();
        sampleVolume->SetInputData(surface);
        sampleVolume->SetSourceData(imageData);
        sampleVolume->Update();*/

        vtkDoubleArray * dataarr = vtkDoubleArray ::New();
        setPolyDataValue(dataarr,(int)radius2,direction);
        qDebug() << "sampleVolume:" << mm.elapsed()/1000.0;

        double sunlen = getLength(points);
        double fstep = sunlen/splinenum;
        int temprows = functionSource->GetOutput()->GetNumberOfPoints();
        int tempcols = (int)radius2;
        qDebug() << "surface2 before" << mm.elapsed()/1000.0;
        vtkSmartPointer<vtkPolyData> surface2 = createPolyData(temprows,tempcols,fstep);
        qDebug() << "surface2 after" << mm.elapsed()/1000.0;

        /*
         *遍历dataset每个点数据
        vtkDataSet* tempset = sampleVolume->GetOutput();
        int numtyples = tempset->GetPointData()->GetNumberOfTuples();
        qDebug() << "numtyples:" << numtyples;
        for(int i=0; i<2000;i+=10){//numtyples/100
            double temp = tempset->GetPointData()->GetScalars()->GetComponent(i,0);
            qDebug() << i << "=" << temp;
        }*/

//        surface2->GetPointData()->SetScalars(sampleVolume->GetOutput()->GetPointData()->GetScalars());
        surface2->GetPointData()->SetScalars(dataarr);
        surface2->Modified();
        qDebug() << "surface2 Modified after" << mm.elapsed()/1000.0;

        vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
        colorFun->AddRGBPoint( -1400, 0.0, 0.0, 0.0 );
        colorFun->AddRGBPoint( 200, 1.0, 1.0, 1.0);
        colorFun->SetColorSpaceToRGB();

        // Create a mapper and actor.
        vtkSmartPointer<vtkDataSetMapper> mapper =
          vtkSmartPointer<vtkDataSetMapper>::New();
        mapper->SetInputData(surface2);//sampleVolume->GetOutputPort()
        mapper->SetLookupTable(colorFun);

        vtkSmartPointer<vtkActor> actor =
          vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        qDebug() << "view4Renderer before" << mm.elapsed()/1000.0;
        view4Renderer->RemoveAllViewProps();
        qDebug() << "RemoveAllViewProps" << mm.elapsed()/1000.0;
        view4Renderer->Render();
        qDebug() << "Render" << mm.elapsed()/1000.0;
        view4Renderer->AddActor(actor);
        qDebug() << "AddActor" << mm.elapsed()/1000.0;
//        view4Renderer->GetActiveCamera()->SetViewUp(direction[0],direction[1],direction[2]);
        view4Renderer->ResetCamera();
        qDebug() << "ResetCamera" << mm.elapsed()/1000.0;
        view4Renderer->Render();
        qDebug() << "Render" << mm.elapsed()/1000.0;
        view4Renderer->GetRenderWindow()->Render();
        qDebug() << "end" << mm.elapsed()/1000.0;
    }

    //获取点击和总长度
    double getLength(vtkPoints* tpoints){
        int npts = tpoints->GetNumberOfPoints();

        if ( npts < 2 ) { return 0.0; }

        double a[3];
        double b[3];
        double sum = 0.0;
        int i = 0;
        tpoints->GetPoint(i, a);
        int imax = (npts%2 == 0) ? npts-2 : npts-1;

        while ( i < imax )
          {
          tpoints->GetPoint(i+1, b);
          sum += sqrt(vtkMath::Distance2BetweenPoints(a, b));
          i = i + 2;
          tpoints->GetPoint(i, a);
          sum = sum + sqrt(vtkMath::Distance2BetweenPoints(a, b));
          }

        if ( npts%2 == 0 )
          {
          tpoints->GetPoint(i+1, b);
          sum += sqrt(vtkMath::Distance2BetweenPoints(a, b));
          }
        return sum;
    }

    //创建曲面图拉直平面
    vtkSmartPointer<vtkPolyData> createPolyData(int rows,int cols,double fstep){
        vtkSmartPointer<vtkPolyData> surface =
          vtkSmartPointer<vtkPolyData>::New();

        cols++;
        // Generate the points
        unsigned int numberOfPoints = rows * cols;
        unsigned int numberOfPolys = (rows - 1) * (cols - 1);
        vtkSmartPointer<vtkPoints> points =
          vtkSmartPointer<vtkPoints>::New();
        points->Allocate(numberOfPoints);
        vtkSmartPointer<vtkCellArray> polys =
          vtkSmartPointer<vtkCellArray>::New();
        polys->Allocate(numberOfPolys*4);

        double x[3];
        unsigned int cnt = 0;
        for (unsigned int row = 0; row < rows; row++)
        {
            for (unsigned int col = 0; col < cols; col++)
            {
                x[0] = row*fstep;
                x[1] = col;
                x[2] = 0;
                points->InsertPoint(cnt++, x);
            }
        }
        // Generate the quads
        vtkIdType pts[4];
        for (unsigned int row = 0; row < rows - 1; row++)
        {
            for (unsigned int col = 0; col < cols - 1; col++)
            {
                pts[0] = col + row * (cols);
                pts[1] = pts[0] + 1;
                pts[2] = pts[0] + cols + 1;
                pts[3] = pts[0] + cols;
                polys->InsertNextCell(4,pts);
            }
        }
        surface->SetPoints(points);
        surface->SetPolys(polys);
        return surface;
    }

    //点投影
    void projectPoint(double* x,double* xproj){
        if(planeproject == NULL ){
            planeproject = vtkPlane::New();
            double center[3] = {dimension[0]*spacing[0]/2,dimension[1]*spacing[1]/2,dimension[2]*spacing[2]/2};
            double radius = sqrt(center[0]*center[0]+center[1]*center[1]+center[2]*center[2]);
            double* outnormal = this->GetDefaultRenderer()->GetActiveCamera()->GetViewPlaneNormal();
            double outcenter[3] = {center[0]+radius*outnormal[0],center[1]+radius*outnormal[1],center[2]+radius*outnormal[2]};
            planeproject->SetNormal(outnormal);
            planeproject->SetOrigin(outcenter);
        }
        planeproject->ProjectPoint(x,xproj);
    }
};

#endif // INTERACTORTEST_H
