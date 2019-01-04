#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Globe/GlobeInclude.h"

#include "interactortest.h"
#include "../Globe/myfunc.h"
#include "MyDiocmInteractorStyleImage.h"
vtkStandardNewMacro(myVtkInteractorStyleImage);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_timer = NULL;
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
	ui->qvtkWidget->GetInteractor()->SetInteractorStyle(m_volumeStyle);
    ui->qvtkWidget->GetRenderWindow()->Render();

	///测试用
	/*测试延中心线移动*/
	m_testSphere = NULL;
	testReadPoint();
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
	QVector<QVector3D> m_posVec2;
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
			m_posVec2.append(QVector3D(tmpx,tmpy,tmpz));
		}
	}
	file.close();
	qDebug() << m_posVec2.size();

	m_posVec.push_back(m_posVec2.at(0));
	double disThshold = 10;
	//方向确定
	vtkSmartPointer<vtkPlane> planeDir =
		vtkSmartPointer<vtkPlane>::New();
	double projected[3];
	int startIndex = 0;
	for(int i=2;i<m_posVec2.size();i++){
		double posori[3] = {m_posVec2.at(i).x(),m_posVec2.at(i).y(),
							m_posVec2.at(i).z()};
		double tmpDir[3] = {posori[0]-m_posVec2.at(startIndex).x(),
						   posori[1]-m_posVec2.at(startIndex).y(),
						   posori[2]-m_posVec2.at(startIndex).z()};
		vtkMath::Normalize(tmpDir);
		planeDir->SetOrigin(posori);
		planeDir->SetNormal(tmpDir);
		for(int j=startIndex;j<i;j++){
			double tmppos3d[3] = {m_posVec2.at(j).x(),m_posVec2.at(j).y(),
								  m_posVec2.at(j).z()};
			planeDir->ProjectPoint(tmppos3d, projected);
			double dis2 = vtkMath::Distance2BetweenPoints(posori,projected);
			if(dis2>disThshold){
				m_posVec.push_back(m_posVec2.at(i-1));
				startIndex = i;
				break;
			}
		}
		if(i==m_posVec2.size()-1){
			m_posVec.push_back(m_posVec2.at(i));
		}
	}
	qDebug() << m_posVec.size();
}

void MainWindow::testCreateSphere(double center[])
{
	if(m_testSphere == NULL){
		m_testSphere =vtkSphereSource::New();
		m_testSphere->SetCenter(center);
		m_testSphere->SetRadius(0.1);
		m_testSphere->SetPhiResolution(100);
		m_testSphere->SetThetaResolution(100);
		vtkSmartPointer<vtkPolyDataMapper> mapper =
		  vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(m_testSphere->GetOutputPort());
		vtkSmartPointer<vtkActor> actor =
		  vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(1,0,0);
		renderer->AddActor(actor);
	}else{
		m_testSphere->SetCenter(center);
		m_testSphere->Modified();
	}

}
int pointIndex= 1;
void MainWindow::testBtnResponse1()
{
	/*测试视角延中心线移动*/
	int sumpoint = m_testSource->GetOutput()->GetNumberOfPoints();
//	QVector3D beginPos = m_posVec.at(pointIndex);
//	QVector3D endPos = m_posVec.at(pointIndex+1);
//	double pos1[3] = {beginPos.x(),beginPos.y(),beginPos.z()};
//	testCreateSphere(pos1);
//	double pos2[3] = {endPos.x(),endPos.y(),endPos.z()};
	double pos1[3],pos2[3];
	m_testSource->GetOutput()->GetPoint(pointIndex,pos1);
	m_testSource->GetOutput()->GetPoint(pointIndex+1,pos2);
	testCreateSphere(pos1);

	double tmpDir[3] = {pos1[0]-pos2[0],pos1[1]-pos2[1],pos1[2]-pos2[2]};
	vtkMath::Normalize(tmpDir);
	//方向确定
	vtkSmartPointer<vtkPlane> planeDir =
		vtkSmartPointer<vtkPlane>::New();
	planeDir->SetOrigin(0,0,0);
	planeDir->SetNormal(tmpDir);
	double projected[3];
	double tmppos3d[3] = {1,0,0};
	planeDir->ProjectPoint(tmppos3d, projected);
	vtkMath::Normalize(projected);

	vtkCamera* camera = this->renderer->GetActiveCamera();
	double camradis = 8;//1000
	camera->SetWindowCenter(0,0);
	camera->SetPosition(pos1[0]+tmpDir[0]*camradis,pos1[1]+tmpDir[1]*camradis,
			pos1[2]+tmpDir[2]*camradis);
	camera->SetFocalPoint(pos1);
	camera->SetViewUp(projected);
	camera->Modified();
	this->renderer->Render();

//	if(! light1->GetSwitch()){
//		light1->SwitchOn();
//	}
//	light1->SetFocalPoint(pos2);
//	double camradis2 = 10;//1000
//	light1->SetPosition(pos1[0]+tmpDir[0]*camradis2,pos1[1]+tmpDir[1]*camradis2,
//			pos1[2]+tmpDir[2]*camradis2);
//	light1->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();
	pointIndex++;
	if(sumpoint-1 <= pointIndex){
		pointIndex = 0;
	}
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

void MainWindow::slotTimeOut()
{
	testBtnResponse1();
}

void MainWindow::InitInfo()
{
    //灯光
    light1 = vtkLight::New();
	light1->SetColor(1,1,1);
	light1->SetIntensity(0.8);
	light1->SetPositional(1);
	light1->SetLightTypeToHeadlight();
    //渲染器
	renderer = vtkRenderer::New();
	m_volumeStyle = MouseInteractorStyle::New();
	m_volumeStyle->SetDefaultRenderer(renderer);
	//InitCamera();

	renderer->AddLight(light1);
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

void MainWindow::InitCameraPespective()
{
	vtkSmartPointer<vtkCamera> camera =
	  vtkSmartPointer<vtkCamera>::New();
	double dataCenterPos[3]={(m_dim[0]-1)*m_spacing[0]/2,
							 (m_dim[1]-1)*m_spacing[1]/2,
							 (m_dim[2]-1)*m_spacing[2]/2};
	double viewup[3] = {0,0,-1};
	camera->SetPosition(dataCenterPos[0],dataCenterPos[1]+500,dataCenterPos[2]);
	camera->SetFocalPoint(dataCenterPos[0],dataCenterPos[1],dataCenterPos[2]);
	camera->SetViewUp(viewup);
	camera->SetClippingRange(0.01,10000);
	camera->Modified();
	renderer->SetActiveCamera(camera);
	renderer->Render();
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
		m_volumeStyle->seedUpdate = m_volumeInfo;
	}
	m_volumeInfo->initPlaneInfo();
	if(m_volumeStyle->lastClickRes){
		int dimx = m_volumeStyle->lastPos[0]/m_spacing[0];
		int dimy = m_volumeStyle->lastPos[1]/m_spacing[1];
		int dimz = m_volumeStyle->lastPos[2]/m_spacing[2];
		QString seedpos = QString::number(dimx)+","+
				QString::number(dimy)+","+QString::number(dimz);

		short *tmpct = static_cast<short*>(m_imageData->GetScalarPointer(
										dimx,dimy,dimz));
		m_volumeInfo->initSeedInfo(seedpos,*tmpct);
	}
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
int ctvalue=300;
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
	InitCameraPespective();
    ui->qvtkWidget->GetRenderWindow()->Render();


	vtkPoints* points = vtkPoints::New();
	for(int i=0;i<m_posVec.size();i++){
		points->InsertNextPoint(m_posVec.at(i).x(),
								m_posVec.at(i).y(),
								m_posVec.at(i).z());
	}
	vtkActor* allPointActor = vtkActor::New();
	double pointcolor[3] = {0,0,1};
	MyFunc::CreatePointActor(points,allPointActor,pointcolor,5,1);
	renderer->AddActor(allPointActor);

	vtkParametricSpline* spline = vtkParametricSpline::New();
	spline->SetPoints(points);
	m_testSource =vtkParametricFunctionSource::New();
	int extendsize = 20;
	if(m_posVec.size() <=10){
		extendsize = 30;
	}
	int splinenum = m_posVec.size()*extendsize;

	m_testSource->SetUResolution(splinenum);
	m_testSource->SetVResolution(splinenum);
	m_testSource->SetWResolution(splinenum);
	m_testSource->SetParametricFunction(spline);
	m_testSource->Update();
	qDebug() << m_testSource->GetOutput()->GetNumberOfPoints();

	vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(m_testSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor =
	  vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(0,1.0,0);
	actor->GetProperty()->SetLineWidth(2.0);
	renderer->AddActor(actor);
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
	m_mouseInter->updateVierer = true;

	///测试用
	/*测试延中心线移动
	if(m_timer == NULL){
		m_timer = new QTimer(this);
		connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
	}
	if(m_timer->isActive()){
		m_timer->stop();
	}else{
		m_timer->start(100);
	}*/

//	testBtnResponse1();
	/*测试三个切片和部分体,移动部分体
	testBtnResponse2();*/

	/*测试三维中种子增长
	int dimstart1[3] = {280,284,132};//test 1
	double tmpPos[3] = {175.438,142.456,330};
	int dimstart2[3] = {tmpPos[0]/m_spacing[0],tmpPos[1]/m_spacing[1],
					   tmpPos[2]/m_spacing[2]};//test 2_600
	MyFunc::VolumeSeedGrowth(dimstart1,m_imageData);
	m_imageData->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();*/
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

	m_mouseInter->oriImageData = m_imageData;
	MyFunc::InterpolateImagedata(m_imageData,m_mouseInter->interpolateImageData,1);
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
