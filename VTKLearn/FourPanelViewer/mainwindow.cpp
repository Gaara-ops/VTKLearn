#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vtkVersion.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkImageMapper.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkPlane.h>
#include <vtkDICOMImageReader.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkImageProperty.h>
#include <vtkImageResliceMapper.h>
#include <vtkDistanceWidget.h>
#include <vtkWidgetRepresentation.h>
#include <vtkDistanceRepresentation.h>
#include <vtkImageReslice.h>
#include <vtkImageSliceMapper.h>
#include <vtkCamera.h>
#include <QDebug>
#include <QTime>

#include "interactortest.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_thickness = 5;
    //读取dicom文件
    m_dicomReader = vtkDICOMImageReader::New();
    m_dicomReader->SetDirectoryName("H:/AllData/001_000005_36362");
	m_dicomReader->Update();
	m_dicomReader->GetOutput()->GetDimensions(m_dimension);
	m_dicomReader->GetOutput()->GetSpacing(m_spacing);

    //初始化
    for(int i=0; i<4; i++){
        m_render[i] = vtkRenderer::New();
        m_resliceMapper[i] = vtkImageResliceMapper::New();
		m_resliceMapper[i]->SetInputData(m_dicomReader->GetOutput());
    }
    for(int i=0;i<3; i++){
        m_center[i] = m_dimension[i]*m_spacing[i]/2;
        m_sliceIndex[i] = m_dimension[i]/2-1;

        m_style[i] = MouseInteractorStyleTest::New();
        m_style[i]->SetDefaultRenderer(m_render[i]);
		m_dicomReader->GetOutput()->GetDimensions(m_style[i]->dimension);
		m_dicomReader->GetOutput()->GetSpacing(m_style[i]->spacing);
        m_style[i]->slicemapper = m_resliceMapper[i];
		m_style[i]->imageData = m_dicomReader->GetOutput();
		m_style[i]->reSampleData();

        m_style[i]->view4Renderer = m_render[3];
        m_style[i]->view4SliceMapper = m_resliceMapper[3];
    }

    qDebug() << "m_dimension:" << m_dimension[0] << " " << m_dimension[1] << " " << m_dimension[2];
    qDebug() << "m_spacing:" << m_spacing[0] << " " << m_spacing[1] << " " << m_spacing[2];
    qDebug() << "m_center:" << m_center[0] << " " << m_center[1] << " " << m_center[2];
    qDebug() << "m_sliceIndex:" << m_sliceIndex[0] << " " << m_sliceIndex[1] << " " << m_sliceIndex[2];
    qDebug() << "xMaxThick:" <<m_dimension[0]*m_spacing[0] << "yMaxThick:" << m_dimension[1]*m_spacing[1]<< "zMaxThick:" << m_dimension[2]*m_spacing[2];

    initView1();
    initView2();
    initView3();
    initView4Surface();
    initSlider();

	vtkImageReslice *reslicetest = vtkImageReslice::New();
    reslicetest->SetInputData(m_dicomReader->GetOutput());
    reslicetest->SetOutputExtent(0,1,0,1,0,10);
    reslicetest->SetSlabModeToMax();
    reslicetest->Update();
    reslicetest->Modified();
    vtkImageData* imagetest = reslicetest->GetOutput();
    int dimtest[3];
	imagetest->GetDimensions(dimtest);
	double spacingtest[3];
	imagetest->GetSpacing(spacingtest);
	qDebug() << "dim:" << dimtest[0] << " "<< dimtest[1] << " "<< dimtest[2] << " ";
	qDebug() << "spacingtest:" << spacingtest[0] << " "<< spacingtest[1] << " "<< spacingtest[2] << " ";


}
MainWindow::~MainWindow()
{
    m_dicomReader->Delete();

    for(int i=0; i<4; i++){
        m_render[i]->Delete();
        m_resliceMapper[i]->Delete();
    }
    delete ui;
}

void MainWindow::initView1()
{
    ui->view1->GetInteractor()->SetInteractorStyle(m_style[0]);
    double normal[3] = {0,0,1};
    vtkCamera* camera1 = vtkCamera::New();
    camera1->SetParallelProjection(1);
    camera1->SetPosition(m_center[0],m_center[1],(m_center[2]+100));
    camera1->SetFocalPoint(m_center);
    camera1->SetViewUp(0,1,0);
    camera1->SetParallelScale(m_center[2]);
    qDebug() << "test scale:" << m_center[1];

    initView(ui->view1,normal,m_render[0],m_resliceMapper[0],camera1,m_thickness);
    ui->view1->GetRenderWindow()->Render();
}

void MainWindow::initView2()
{
    ui->view2->GetInteractor()->SetInteractorStyle(m_style[1]);
    double normal[3] = {0,1,0};

    vtkCamera* camera2 = vtkCamera::New();
    camera2->SetParallelProjection(1);
    camera2->SetPosition(m_center[0],m_center[1]+100,m_center[2]);
    camera2->SetFocalPoint(m_center);
    camera2->SetViewUp(0,0,-1);
    camera2->SetParallelScale(m_center[2]);

    initView(ui->view2,normal,m_render[1],m_resliceMapper[1],camera2,m_thickness);

    ui->view2->GetRenderWindow()->Render();
}

void MainWindow::initView3()
{
    ui->view3->GetInteractor()->SetInteractorStyle(m_style[2]);
    double normal[3] = {1,0,0};

    vtkCamera* camera3 = vtkCamera::New();
    camera3->SetParallelProjection(1);
    camera3->SetPosition(m_center[0]+100,m_center[1],m_center[2]);
    camera3->SetFocalPoint(m_center);
    camera3->SetViewUp(0,0,-1);
    camera3->SetParallelScale(m_center[2]);

    initView(ui->view3,normal,m_render[2],m_resliceMapper[2],camera3,m_thickness);
    ui->view3->GetRenderWindow()->Render();
}

void MainWindow::initView4Surface()
{
    double normal[3] = {0,0,1};
    //initView(ui->view4,normal,m_render[3],m_resliceMapper[3],10);
    ui->view4->GetRenderWindow()->AddRenderer(m_render[3]);
    m_render[3]->ResetCamera();
    ui->view4->GetRenderWindow()->Render();
}

void MainWindow::initView4Curve()
{

}

void MainWindow::slotUpdateView1()
{
    m_resliceMapper[0]->GetSlicePlane()->SetNormal(m_render[0]->GetActiveCamera()->GetViewPlaneNormal());
    m_resliceMapper[0]->Modified();
    ui->view1->GetRenderWindow()->Render();
}

void MainWindow::initView(QVTKWidget *vtkwidget, double *normal, vtkRenderer *render, vtkImageResliceMapper *slicemapper, vtkCamera *camera, double thickness)
{
    slicemapper->SetSlabThickness(thickness);//设置层厚
//    imageResliceMapper->SetSlabTypeToMax();//最大密度投影
//    imageResliceMapper->SetSlabTypeToMean();//平均密度投影(默认)
//    imageResliceMapper->SetSlabTypeToMin();//最小密度投影
//    imageResliceMapper->SetSlabTypeToSum();//综合密度投影
    /**
      * 设置切片图的位置为相机的交点
      * 设置切片图正对相机
      * 设置后切片图跟随鼠标转动而变化*/

    slicemapper->SetSliceAtFocalPoint(1);
    slicemapper->SetSliceFacesCamera(1);

    vtkPlane* plane = vtkPlane::New();
    plane->SetOrigin(m_center);
    plane->SetNormal(normal);
    //设置切片平面
    slicemapper->SetSlicePlane(plane);

    vtkImageSlice* imageSlice = vtkImageSlice::New();
    imageSlice->SetMapper(slicemapper);
    //设置窗宽窗位
    imageSlice->GetProperty()->SetColorLevel(200);
    imageSlice->GetProperty()->SetColorWindow(600);
    imageSlice->Modified();
    imageSlice->Update();
    //test
    double* bounds = imageSlice->GetBounds();
    qDebug() << bounds[0] << " "<< bounds[1]  << " "<< bounds[2]
             << " "<< bounds[3]<< " "<< bounds[4]  << " "<< bounds[5] ;
    //end
    render->AddViewProp(imageSlice);

    render->SetActiveCamera(camera);

    vtkwidget->GetRenderWindow()->AddRenderer(render);

    plane->Delete();
    imageSlice->Delete();
}

void MainWindow::updateSlice(vtkRenderer *render,double *center,vtkImageResliceMapper* slicemapper, double thickness)
{
    slicemapper->GetSlicePlane()->SetOrigin(center);
    slicemapper->Modified();
}

void MainWindow::updateThickness(vtkImageResliceMapper *slicemapper, double thickness)
{
    slicemapper->SetSlabThickness(thickness);
    slicemapper->Modified();
}

void MainWindow::on_actionGetCL_triggered()
{
    /*
     * 获取窗宽窗位
    vtkImageProperty* proty1 = ((vtkImageSlice*)(m_renders[0]->GetViewProps()->GetLastProp()))->GetProperty();
    qDebug() << "proty1:" <<proty1->GetColorLevel() << " " << proty1->GetColorWindow();
    vtkImageProperty* proty2 = ((vtkImageSlice*)(m_renders[1]->GetViewProps()->GetLastProp()))->GetProperty();
    qDebug() << "proty1:" <<proty2->GetColorLevel() << " " << proty2->GetColorWindow();
    vtkImageProperty* proty3 = ((vtkImageSlice*)(m_renders[2]->GetViewProps()->GetLastProp()))->GetProperty();
    qDebug() << "proty1:" <<proty3->GetColorLevel() << " " << proty3->GetColorWindow();*/
}

void MainWindow::slot_slider1_valueChanged(int position)
{
    QTime t;
    t.start();
    m_sliceIndex[2] = position-1;
    double center[3] = {m_center[0],m_center[1],m_sliceIndex[2]*m_spacing[2]};
//    qDebug() << m_sliceIndex[2];
    updateSlice(m_render[0],center,m_resliceMapper[0]);
    ui->view1->GetRenderWindow()->Render();
    qDebug() << "slider1:" << t.elapsed()/1000.0;
}

void MainWindow::slot_slider2_valueChanged(int position)
{
    QTime t;
    t.start();
    m_sliceIndex[1] = position-1;
    double center[3] = {m_center[0],m_sliceIndex[1]*m_spacing[1],m_center[2]};
//    qDebug() << m_sliceIndex[1] << " " << position;
    updateSlice(m_render[1],center,m_resliceMapper[1]);
    ui->view2->GetRenderWindow()->Render();
    qDebug() << "slider2:" << t.elapsed()/1000.0;
}

void MainWindow::slot_slider3_valueChanged(int position)
{
    QTime t;
    t.start();

    qDebug() << position ;
    m_sliceIndex[0] = position-1;
    double center[3] = {m_sliceIndex[0]*m_spacing[0],m_center[1],m_center[2]};

    updateSlice(m_render[2],center,m_resliceMapper[2]);
    ui->view3->GetRenderWindow()->Render();
    qDebug() << "slider3:" << t.elapsed()/1000.0;
}

void MainWindow::initSlider()
{
    ui->slider1->setRange(1,m_dimension[2]);
    ui->slider1->setPageStep(1);
    ui->slider1->setValue(m_sliceIndex[2]);

    ui->slider2->setRange(1,m_dimension[1]);
    ui->slider2->setPageStep(1);
    ui->slider2->setValue(m_sliceIndex[1]);

    ui->slider3->setRange(1,m_dimension[0]);
    ui->slider3->setPageStep(1);
    ui->slider3->setValue(m_sliceIndex[0]);

    connect(ui->slider1,SIGNAL(valueChanged(int)),this,SLOT(slot_slider1_valueChanged(int)));
    connect(ui->slider2,SIGNAL(valueChanged(int)),this,SLOT(slot_slider2_valueChanged(int)));
    connect(ui->slider3,SIGNAL(valueChanged(int)),this,SLOT(slot_slider3_valueChanged(int)));


    ui->doubleSpinBox1->setValue(m_thickness);
    ui->doubleSpinBox2->setValue(m_thickness);
    ui->doubleSpinBox3->setValue(m_thickness);
}

void MainWindow::on_actionRoll_triggered()
{
    m_render[0]->GetActiveCamera()->Roll(90);
    ui->view1->GetRenderWindow()->Render();
    vtkCamera* temp = m_render[0]->GetActiveCamera();
    int a = 1;
}

void MainWindow::on_actionAzimuth_triggered()
{
    m_render[0]->GetActiveCamera()->Azimuth(90);
    ui->view1->GetRenderWindow()->Render();
    vtkCamera* temp = m_render[0]->GetActiveCamera();
    int a = 1;
}

void MainWindow::on_actionElevation_triggered()
{
    m_render[0]->GetActiveCamera()->Elevation(90);
    ui->view1->GetRenderWindow()->Render();
    vtkCamera* temp = m_render[0]->GetActiveCamera();
    int a = 1;
}

void MainWindow::setInterStyle()
{
//    ui->view1->GetInteractor()->SetInteractorStyle(m_styles2[0]);
//    ui->view2->GetInteractor()->SetInteractorStyle(m_styles2[1]);
//    ui->view3->GetInteractor()->SetInteractorStyle(m_styles2[2]);
}

void MainWindow::on_actionSetThinkness_triggered()
{
    updateThickness(m_resliceMapper[0],ui->doubleSpinBox1->value());
    updateThickness(m_resliceMapper[1],ui->doubleSpinBox2->value());
    updateThickness(m_resliceMapper[2],ui->doubleSpinBox3->value());
    ui->view1->GetRenderWindow()->Render();
    ui->view2->GetRenderWindow()->Render();
    ui->view3->GetRenderWindow()->Render();
}

void MainWindow::on_doubleSpinBox1_editingFinished()
{
    QTime t;
    t.start();
    updateThickness(m_resliceMapper[0],ui->doubleSpinBox1->value());
    ui->view1->GetRenderWindow()->Render();
    qDebug() << "SpinBox1:" << t.elapsed()/1000.0;
}

void MainWindow::on_doubleSpinBox2_editingFinished()
{
    QTime t;
    t.start();
    updateThickness(m_resliceMapper[1],ui->doubleSpinBox2->value());
    ui->view2->GetRenderWindow()->Render();
    qDebug() << "SpinBox2:" << t.elapsed()/1000.0;
}

void MainWindow::on_doubleSpinBox3_editingFinished()
{
    QTime t;
    t.start();
    updateThickness(m_resliceMapper[2],ui->doubleSpinBox3->value());
    ui->view3->GetRenderWindow()->Render();
    qDebug() << "SpinBox3:" << t.elapsed()/1000.0;
}

void MainWindow::on_actionSurface_triggered()
{
    m_style[0]->surfaceRect = !(m_style[0]->surfaceRect);
    m_style[0]->points->Reset();
}

void MainWindow::on_actionDiswidget_triggered()
{
    vtkDistanceWidget* distanceWidget =vtkDistanceWidget::New();
    distanceWidget->SetInteractor(ui->view1->GetInteractor());
    distanceWidget->CreateDefaultRepresentation();
    static_cast<vtkDistanceRepresentation *>(distanceWidget->GetRepresentation())->SetLabelFormat("%-#6.3g mm");
    ui->view1->GetRenderWindow()->Render();
    distanceWidget->On();
    ui->view1->GetRenderWindow()->Render();
}
