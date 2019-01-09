#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Globe/GlobeInclude.h"

#include "interactortest.h"
#include "../Globe/myfunc.h"

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
    qDebug() << "dim:" << m_dim[0]<<m_dim[1]<<m_dim[2];
	qDebug() << "m_spacing:" << m_spacing[0]<<m_spacing[1]<<m_spacing[2];
	//初始化一些信息
    InitInfo();
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
	ui->qvtkWidget->GetInteractor()->SetInteractorStyle(m_volumeStyle);
    ui->qvtkWidget->GetRenderWindow()->Render();
	//VE相关
	m_vecontrol = new VEControl;
	m_veactive = false;
}

MainWindow::~MainWindow()
{
    delete ui;
    DeleteAllThing();
    dicomReader->Delete();
    light1->Delete();
	renderer->Delete();
}

void MainWindow::DeleteAllThing()
{
    int numActors = renderer->GetActors()->GetNumberOfItems();
    qDebug() << "numActors: "<< numActors;
    if(numActors>0){
        for(int i=0;i<numActors; ++i){
            renderer->RemoveActor(renderer->GetActors()->GetNextProp());
		}
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
	if(m_veactive){
		m_vecontrol->UpdateViewFocus();
	}
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

void MainWindow::InitCameraPespective(int dim[3],double spacing[3])
{
	vtkSmartPointer<vtkCamera> camera =
	  vtkSmartPointer<vtkCamera>::New();
	double dataCenterPos[3]={(dim[0]-1)*spacing[0]/2,
							 (dim[1]-1)*spacing[1]/2,
							 (dim[2]-1)*spacing[2]/2};
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
	InitCamera();
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
void MainWindow::on_actionSeedGrowth_triggered()
{
	/*测试三维中种子增长*/
	int dimstart1[3] = {280,284,132};//test 1
	double tmpPos[3] = {175.438,142.456,330};
	int dimstart2[3] = {tmpPos[0]/m_spacing[0],tmpPos[1]/m_spacing[1],
					   tmpPos[2]/m_spacing[2]};//test 2_600
	MyFunc::VolumeSeedGrowth(dimstart1,m_imageData,50);
	m_imageData->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();
}
void MainWindow::on_pushButton_clicked()
{
//	renderer->RemoveAllLights();
//	renderer->AddLight(light1);
//	renderer->GetActiveCamera()->Dolly(0.5);
//	ui->qvtkWidget->GetRenderWindow()->Render();
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
	InitCamera();
	MyFunc::CreateThreeSlice(m_dim,renderer,dicomReader);

	/*提取体感兴趣的区域
	vtkSmartPointer<vtkExtractVOI> extractVOI =
		vtkSmartPointer<vtkExtractVOI>::New();
	extractVOI->SetInputConnection(dicomReader->GetOutputPort());
	extractVOI->SetVOI(m_dim[0]/4,3*m_dim[0]/4,m_dim[1]/4,3*m_dim[1]/4,
			m_dim[2]/4,3*m_dim[2]/4);
	extractVOI->Update();*/
	MyFunc::CreateVolume(m_imageData,0,1,0,renderer);

	m_mouseInter = MouseInteractorStyleTest::New();
	m_mouseInter->SetImageData(m_imageData);
	ui->qvtkWidget->GetInteractor()->SetInteractorStyle(m_mouseInter);
	m_mouseInter->SetDefaultRenderer(renderer);
	renderer->ResetCamera();
	ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionSliceShow_triggered()
{
	MyFunc::ShowSeriesDicom(dicomReader);
}

void MainWindow::on_actionInterpolateVolume_triggered()
{
	if(m_mouseInter != NULL){
		MyFunc::InterpolateImagedata(m_imageData,
									 m_mouseInter->interpolateImageData,1);
	}
}

void MainWindow::on_actionCreateThreeView_triggered()
{
	if(m_mouseInter != NULL){
		m_mouseInter->updateVierer = true;
	}
}

void MainWindow::on_actionCreateCurveLine_triggered()
{
	DeleteAllThing();
	m_veactive = true;
	m_vecontrol->m_render = renderer;
	m_vecontrol->m_oriData = m_imageData;

	renderer->RemoveAllLights();
	renderer->AddLight(light1);
	QString filepath = "./centerline.txt";
	m_vecontrol->ReadPointData(filepath);

	vtkImageData* test = vtkImageData::New();
	m_vecontrol->ResampleImageData(m_imageData,test);
	double color[3]={1,0,0};
	MyFunc::CreateSurface(test,300,0.5,color,renderer,true);
	int tmpdim[3];double tmpspacing[3];
	test->GetDimensions(tmpdim);
	test->GetSpacing(tmpspacing);
	InitCameraPespective(tmpdim,tmpspacing);
	ui->qvtkWidget->GetRenderWindow()->Render();
	m_vecontrol->CreateCurveLine();
}

void MainWindow::on_actionNextView_triggered()
{
	m_vecontrol->UpdateViewFocus();
}

void MainWindow::on_actionPlayView_triggered()
{
	if(m_timer == NULL){
		m_timer = new QTimer(this);
		connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
	}
	if(m_timer->isActive()){
		m_timer->stop();
	}else{
		m_timer->start(100);
	}
}
