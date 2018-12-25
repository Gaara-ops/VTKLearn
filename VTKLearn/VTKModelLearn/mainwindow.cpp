#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Globe/GlobeInclude.h"

#include "interactortest.h"
#include "myfunc.h"
#include "MyDiocmInteractorStyleImage.h"
vtkStandardNewMacro(myVtkInteractorStyleImage);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(800,600);
	m_volumeInfo = NULL;

    //读取文件
	char *dirname = "E:/workspace/DICOM/133ceng";
    dicomReader = vtkDICOMImageReader::New();
    vtkDataArray* scalarsArr = NULL;
    double scalarRange[2];
	MyFunc::ReadDicomData(dicomReader,dirname,m_dim,m_spacing,
						  scalarRange,m_imageData,scalarsArr);
    /*对数据进行抽样
    float factors[3] = {0.5,0.5,1};
    ResampleData(m_imageData,factors);*/
    qDebug() << "dim:" << m_dim[0]<<m_dim[1]<<m_dim[2];
	qDebug() << "m_spacing:" << m_spacing[0]<<m_spacing[1]<<m_spacing[2];
	//初始化一些信息
    InitInfo();
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidget->GetRenderWindow()->Render();

	///测试用
	/*测试延中心线移动
	testReadPoint();*/
	/*测试三个切片和部分体,移动部分体
	testCreateContext();*/
	///end
}

MainWindow::~MainWindow()
{
    delete ui;
    DeleteAllThing();
    dicomReader->Delete();
    light1->Delete();
	renderer->Delete();
}

void MainWindow::testReadPoint()
{
	QFile file("./centerline.txt");
	if(!file.open(QIODevice::ReadOnly))
	{
		 return;
	}
	QTextStream in(&file);
	QString alldata = in.readAll();
	QStringList alldatalist = alldata.split("|");
	for(int i=0;i<alldatalist.size();i++){
		QStringList strlist = alldatalist.at(i).split(",");
		if(strlist.size() == 3){
			double tmpx = strlist.at(0).trimmed().toDouble();
			double tmpy = strlist.at(1).trimmed().toDouble();
			double tmpz = strlist.at(2).trimmed().toDouble();
			m_posVec.append(QVector3D(tmpx,tmpy,tmpz));
		}
	}
	file.close();
	qDebug() << m_posVec.size();
}
int pointIndex= 10;
void MainWindow::testBtnResponse1()
{
	vtkVolume* test =  (vtkVolume*)(renderer->GetVolumes()->GetLastProp());
	/*测试视角延中心线移动*/
	QVector3D beginPos = m_posVec.at(pointIndex);
	QVector3D endPos = m_posVec.at(pointIndex+1);
	double pos1[3] = {beginPos.x(),beginPos.y(),beginPos.z()};
	double pos2[3] = {endPos.x(),endPos.y(),endPos.z()};
	double tmpDir[3] = {pos1[0]-pos2[0],pos1[1]-pos2[1],pos1[2]-pos2[2]};
	vtkMath::Normalize(tmpDir);
	//方向确定
	vtkSmartPointer<vtkPlane> planeDir =
		vtkSmartPointer<vtkPlane>::New();
	planeDir->SetOrigin(0.0, 0.0, 0.0);
	planeDir->SetNormal(tmpDir);
	double projected[3];
	double pointP[3] = {1,0,0};
	planeDir->ProjectPoint(pointP, projected);
	vtkMath::Normalize(projected);
	//用面进行切割
	vtkSmartPointer<vtkPlane> plane =
	  vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(pos1);
	plane->SetNormal(-tmpDir[0],-tmpDir[1],-tmpDir[2]);
	vtkSmartPointer<vtkPlaneCollection> planes =
	  vtkSmartPointer<vtkPlaneCollection>::New();
	planes->AddItem(plane);
	vtkGPUVolumeRayCastMapper* mapper =(vtkGPUVolumeRayCastMapper*)test->GetMapper();
	mapper->SetClippingPlanes(planes);
	mapper->Modified();
	vtkCamera* camera = this->renderer->GetActiveCamera();
	qDebug() << camera->GetParallelProjection();
	camera->SetPosition(pos1[0]+tmpDir[0]*1000,pos1[1]+tmpDir[1]*1000,
			pos1[2]+tmpDir[2]*1000);
	camera->SetFocalPoint(pos1);
	camera->SetViewUp(projected);
	camera->SetClippingRange(0,100000);
	camera->Modified();
	test->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();
	pointIndex++;
	if(m_posVec.size()-1 <= pointIndex){
		pointIndex = 5;
	}
}

void MainWindow::testCreateContext()
{
	/*提取体感兴趣的区域*/
	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputConnection(dicomReader->GetOutputPort());
	extractVOI->SetVOI(0,m_dim[0]/2,0,m_dim[1]/2,0,m_dim[2]/2);
	extractVOI->Update();
	MyFunc::CreateVolume(extractVOI->GetOutput(),0,1,0,renderer);

	MyFunc::CreateThreeSlice(m_dim,renderer,dicomReader);
	renderer->ResetCamera();
	ui->qvtkWidget->GetRenderWindow()->Render();
}
void MainWindow::testBtnResponse2()
{
	vtkVolume* test =  (vtkVolume*)(renderer->GetVolumes()->GetLastProp());
	/*测试移动部分切割体*/
	vtkPiecewiseFunction* opacity=test->GetProperty()->GetScalarOpacity();
	opacity->ReleaseData();
	opacity->AddPoint(-2048, 0, .49, .61 );
	opacity->AddPoint(641, .72, .5, 0.0 );
	opacity->AddPoint(3071, .71, 0.5, 0.0);
	opacity->Modified();
	double dataCenterPos[3]={(m_dim[0]-1)*m_spacing[0]/4,
							 (m_dim[1]-1)*m_spacing[1]/4,
							 (m_dim[2]-1)*m_spacing[2]/4};
	test->SetPosition(dataCenterPos);
	test->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::DeleteAllThing()
{
    int numActors = renderer->GetActors()->GetNumberOfItems();
    qDebug() << "numActors: "<< numActors;
    if(numActors>0){
        for(int i=0;i<numActors; ++i){
            renderer->RemoveActor(renderer->GetActors()->GetNextProp());
        }
        //renderer->RemoveActor(renderer->GetActors()->GetLastProp());
    }
    int numVolumes = renderer->GetVolumes()->GetNumberOfItems();
    qDebug() << "numVolumes: "<< numVolumes;
    if(numVolumes>0){
        renderer->RemoveVolume(renderer->GetVolumes()->GetLastProp());
    }

    int numProps = renderer->GetViewProps()->GetNumberOfItems();
    qDebug() << "numProps: "<< numProps;
    if(numProps>0){
        renderer->RemoveAllViewProps();
    }

    int numActor2Ds = renderer->GetActors2D()->GetNumberOfItems();
    qDebug() << "numActor2Ds: "<< numActor2Ds;
}

void MainWindow::InitInfo()
{
    //灯光
    light1 = vtkLight::New();
    light1->SetColor(0.0,0.0,0.0);
    //渲染器
	renderer = vtkRenderer::New();
	InitCamera();
}

void MainWindow::InitCamera()
{
    vtkSmartPointer<vtkCamera> camera =
      vtkSmartPointer<vtkCamera>::New();
    double dataCenterPos[3]={(m_dim[0]-1)*m_spacing[0]/2,
                             (m_dim[1]-1)*m_spacing[1]/2,
                             (m_dim[2]-1)*m_spacing[2]/2};
    double viewup[3] = {0,0,-1};
    camera->SetParallelProjection(1);
    camera->SetPosition(dataCenterPos[0],dataCenterPos[1]+1000,dataCenterPos[2]);
    camera->SetFocalPoint(dataCenterPos[0],dataCenterPos[1],dataCenterPos[2]);
    camera->SetViewUp(viewup);
    camera->SetParallelScale(dataCenterPos[1]+10);
    camera->Modified();
    renderer->SetActiveCamera(camera);
}

#include "volumeinfo.h"
void MainWindow::on_actionVoumeInfo_triggered()
{
	if(m_volumeInfo == NULL){
		m_volumeInfo = new VolumeInfo;
		m_volumeInfo->light = light1;
		m_volumeInfo->vtkwindow = ui->qvtkWidget->GetRenderWindow();
		m_volumeInfo->renderer = renderer;
		m_volumeInfo->oriImageData = dicomReader->GetOutput();
	}
	m_volumeInfo->initPlaneInfo();
	m_volumeInfo->setVisible(!m_volumeInfo->isVisible());
}

void MainWindow::on_actionVolume_triggered()
{
	DeleteAllThing();
	/*提取体感兴趣的区域
	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputConnection(dicomReader->GetOutputPort());
	extractVOI->SetVOI(0,m_dim[0]/2,0,m_dim[1]/2,0,m_dim[2]/2);
	extractVOI->Update();
	CreateVolume(extractVOI->GetOutput(),0,1,0,renderer);*/
	MyFunc::CreateVolume(m_imageData,0,1,0,renderer);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}
int ctvalue=400;
void MainWindow::on_actionSurface_triggered()
{
    DeleteAllThing();
	double color[3]={1,0,0};
	MyFunc::CreateSurface(m_imageData,ctvalue,0.5,color,renderer,true,5);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionLargestRegion_triggered()
{
    DeleteAllThing();
    double color[3]={1,0,0};
	MyFunc::CreateSurface(m_imageData,ctvalue,0.5,color,renderer,true);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionClear_triggered()
{
    DeleteAllThing();
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionMIP_triggered()
{
    DeleteAllThing();
	MyFunc::CreateVolume(m_imageData,1,1,0,renderer);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_pushButton_clicked()
{
	///测试用
	/*测试延中心线移动
	testBtnResponse1();*/
	/*测试三个切片和部分体,移动部分体
	testBtnResponse2();*/

	/*测试三维中种子增长*/
	int dimstart[3] = {280,284,132};
	MyFunc::VolumeSeedGrowth(dimstart,m_imageData);
	m_imageData->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();
	///end


}

void MainWindow::on_ClipFrustum_triggered()
{
    DeleteAllThing();
	std::string filepath = "E:/workspace/Data/ply/Armadillo.ply";
	MyFunc::CreateClipFrustum(renderer,filepath.c_str());
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionDrawLine_triggered()
{
	m_mouseInter->BeginDrawLine();
	ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionClearLine_triggered()
{
	m_mouseInter->ClearLine();
	ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionCreateThreeSlice_triggered()
{
	DeleteAllThing();
	MyFunc::CreateThreeSlice(m_dim,renderer,dicomReader);

	m_mouseInter = MouseInteractorStyleTest::New();
	m_mouseInter->centerPos[0] = m_dim[0]*m_spacing[0]/2;
	m_mouseInter->centerPos[1] = m_dim[1]*m_spacing[1]/2;
	m_mouseInter->centerPos[2] = m_dim[2]*m_spacing[2]/2;
	m_mouseInter->imagedim[0] = m_dim[0];
	m_mouseInter->imagedim[1] = m_dim[1];
	m_mouseInter->imagedim[2] = m_dim[2];
	m_mouseInter->imagespaceing[0] = m_spacing[0];
	m_mouseInter->imagespaceing[1] = m_spacing[1];
	m_mouseInter->imagespaceing[2] = m_spacing[2];

	ui->qvtkWidget->GetInteractor()->SetInteractorStyle(m_mouseInter);
	m_mouseInter->SetDefaultRenderer(renderer);
	renderer->ResetCamera();
	ui->qvtkWidget->GetRenderWindow()->Render();
}
#include<vtkImageLuminance.h>
#include<vtkImageCast.h>
#include<vtkImageGradient.h>
#include<vtkImageNonMaximumSuppression.h>
#include<vtkImageConstantPad.h>
#include<vtkImageToStructuredPoints.h>
#include<vtkLinkEdgels.h>
#include<vtkThreshold.h>
#include<vtkGeometryFilter.h>
#include<vtkSubPixelPositionEdgels.h>
#include<vtkImageMagnitude.h>
void MainWindow::on_actionCannyEdge_triggered()
{

}
#include<vtkImageMagnitude.h>
#include<vtkImageAccumulate.h>
#include<vtkBarChartActor.h>
#include<vtkProperty2D.h>
#include<vtkLegendBoxActor.h>
void MainWindow::on_actionHistogram_triggered()
{
	/*double spacing[3],ori[3];
	int extent[6];
	readtest->GetDataSpacing(spacing);
	readtest->GetDataOrigin(ori);
	readtest->GetDataExtent(extent);
	readtest->Delete();*/
}

void MainWindow::on_actionSliceShow_triggered()
{
	MyFunc::ShowSeriesDicom(dicomReader);
}
