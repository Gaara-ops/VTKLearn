#include "mainwindow.h"
#include <QApplication>
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
#include "globeinclude.h"
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkNamedColors.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#define M_PI 3.1415926
float toRadian(float degree){
    return degree*M_PI/180.0;
}

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow m;
//    m.show();
//    return a.exec();
/*
    vtkSmartPointer<vtkSphereSource> sphereg =
      vtkSmartPointer<vtkSphereSource>::New();
    sphereg->SetRadius(1);
    sphereg->SetCenter(0.0,0.0,0.0);
    sphereg->SetPhiResolution(5);
    sphereg->SetThetaResolution(10);
    sphereg->Update();

    vtkPolyData* polydata = sphereg->GetOutput();
    qDebug() << "points:" << polydata->GetNumberOfPoints();
    qDebug() << "cells:" << polydata->GetNumberOfCells();

    vtkPoints* allpoints = polydata->GetPoints();
    polydata->SetStrips();//vtkCellArray
    vtkCellArray* cellarr = vtkCellArray::New();
    cellarr->InsertNextCell()

    vtkIdList* idlist = vtkIdList::New();
    polydata->GetCellPoints(0,idlist);
    qDebug() << "NumberOfIds:"<< idlist->GetNumberOfIds();
    for(int i=0; i< idlist->GetNumberOfIds();i++){
        vtkIdType vertexid =  idlist->GetId(i);
        qDebug() << "vertexid:" << vertexid;
        qDebug() << allpoints->GetData()->GetComponent(vertexid,0)
                 << allpoints->GetData()->GetComponent(vertexid,1)
                 << allpoints->GetData()->GetComponent(vertexid,2);
    }
    return 0;
*/


    // Setup four points
    vtkSmartPointer<vtkPoints> points =
      vtkSmartPointer<vtkPoints>::New();
    // Add the polygon to a list of polygons
    vtkSmartPointer<vtkCellArray> polygons =
      vtkSmartPointer<vtkCellArray>::New();
    //sphere
    int col = 10;
    int row = 5;
    float radius = 3;
    points->InsertNextPoint(0, 0, radius);
    float rowAngle = 180.0/(row+1);
    float colAngle = 360.0/col;
    for(int j = 1;j <row+1;j++){
        float tmpRowAngle = rowAngle*j;
        float tmpRadius = radius*sin(toRadian(tmpRowAngle));
        float tmpz = radius*cos(toRadian(tmpRowAngle));
        for(int i= 0; i<=col;i++){
            float tmpColAngle = colAngle*i;
            float tmpx = tmpRadius*cos(toRadian(tmpColAngle));
            float tmpy = tmpRadius*sin(toRadian(tmpColAngle));
            points->InsertNextPoint(tmpx, tmpy, tmpz);
        }
        int posnum = points->GetNumberOfPoints();
        if(j == 1){
            int startindex = posnum-(col+1);
            for(int i=startindex;i<posnum-1;i++){
                vtkIdList* tmpidlist = vtkIdList::New();
                tmpidlist->InsertNextId(0);
                tmpidlist->InsertNextId(i);
                tmpidlist->InsertNextId(i+1);
                polygons->InsertNextCell(tmpidlist);
            }
        }else{
            int startindex = posnum-(col+1)*2;
            int lastindex = posnum-(col+1);
            for(int i=startindex;i<lastindex-1;i++){
                vtkIdList* tmpidlist1 = vtkIdList::New();
                tmpidlist1->InsertNextId(i);
                tmpidlist1->InsertNextId(i+col+1);
                tmpidlist1->InsertNextId(i+col+1+1);
                polygons->InsertNextCell(tmpidlist1);
                vtkIdList* tmpidlist2 = vtkIdList::New();
                tmpidlist2->InsertNextId(i);
                tmpidlist2->InsertNextId(i+col+1+1);
                tmpidlist2->InsertNextId(i+1);
                polygons->InsertNextCell(tmpidlist2);
            }
        }
    }
    int posnum = points->GetNumberOfPoints();
    int startindex = posnum-(col+1);
    int lastindex = posnum;
    for(int i=startindex;i<posnum-1;i++){
        vtkIdList* tmpidlist = vtkIdList::New();
        tmpidlist->InsertNextId(i);
        tmpidlist->InsertNextId(lastindex);
        tmpidlist->InsertNextId(i+1);
        polygons->InsertNextCell(tmpidlist);
    }
    points->InsertNextPoint(0, 0, -radius);
    //sphere end



    // Create a PolyData
    vtkSmartPointer<vtkPolyData> polygonPolyData =
      vtkSmartPointer<vtkPolyData>::New();
    polygonPolyData->SetPoints(points);
    polygonPolyData->SetStrips(polygons);
    polygonPolyData->Modified();


    //test
    vtkIdList* idlist = vtkIdList::New();
    polygonPolyData->GetCellPoints(1,idlist);
    qDebug() << "NumberOfIds:"<< idlist->GetNumberOfIds();
    for(int i=0; i< idlist->GetNumberOfIds();i++){
        vtkIdType vertexid =  idlist->GetId(i);
        qDebug() << "vertexid:" << vertexid;
        qDebug() << points->GetData()->GetComponent(vertexid,0)
                 << points->GetData()->GetComponent(vertexid,1)
                 << points->GetData()->GetComponent(vertexid,2);
    }
    //end


    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polygonPolyData);

    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1,0,1);

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetWindowName("Polygon");
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);
    renderer->SetBackground(0.1,0.2,0.3);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}
/*
    int col = 10;
    int row = 5;
    float radius = 3;
    float rowAngle = 180.0/(row+1);
    float colAngle = 360.0/col;
//    for(int j = 0;j <=row+1;j++){
//        float tmpRowAngle = rowAngle*j;
//        float tmpRadius = radius*sin(toRadian(tmpRowAngle));
//        float tmpz = radius*cos(toRadian(tmpRowAngle));
//        for(int i= 0; i<=col;i++){
//            float tmpColAngle = colAngle*i;
//            float tmpx = tmpRadius*cos(toRadian(tmpColAngle));
//            float tmpy = tmpRadius*sin(toRadian(tmpColAngle));
//            points->InsertNextPoint(tmpx, tmpy, tmpz);
//        }
//    }

    for(int i= 0; i<=col;i++){
        float tmpColAngle = colAngle*i;
        for(int j = 0;j <=row+1;j++){
            float tmpRowAngle = rowAngle*j;
            float tmpRadius = radius*sin(toRadian(tmpRowAngle));
            float tmpz = radius*cos(toRadian(tmpRowAngle));
            float tmpx = tmpRadius*cos(toRadian(tmpColAngle));
            float tmpy = tmpRadius*sin(toRadian(tmpColAngle));
            points->InsertNextPoint(tmpx, tmpy, tmpz);
        }
    }
    qDebug() << "points num:" << points->GetNumberOfPoints();
*/
