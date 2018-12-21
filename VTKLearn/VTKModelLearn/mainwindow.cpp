#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GlobeInclude.h"

#include "interactortest.h"
#include "GlobeFunc.h"
#include "myfunc.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(800,600);
	m_volumeInfo = NULL;

    //读取文件
	char *dirname = "E:/workspace/DICOM/133ceng";
//    char *dirname = "G:/TestClient/VesselAnalysis_client/He Genqiang_";
    dicomReader = vtkDICOMImageReader::New();
    vtkDataArray* scalarsArr = NULL;
    double scalarRange[2];
    ReadDicomData(dicomReader,dirname,m_dim,m_spacing,scalarRange,m_imageData,scalarsArr);
    /*对数据进行抽样
    float factors[3] = {0.5,0.5,1};
    ResampleData(m_imageData,factors);*/
    qDebug() << "dim:" << m_dim[0]<<m_dim[1]<<m_dim[2];
	qDebug() << "m_spacing:" << m_spacing[0]<<m_spacing[1]<<m_spacing[2];
	//初始化一些信息
    InitInfo();
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidget->GetRenderWindow()->Render();
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
	m_volumeInfo->setVisible(!m_volumeInfo->isVisible());
}

void MainWindow::on_actionVolume_triggered()
{
	DeleteAllThing();
	CreateVolume(m_imageData,0,1,0,renderer);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}
int ctvalue=400;
void MainWindow::on_actionSurface_triggered()
{
    DeleteAllThing();
	double color[3]={1,0,0};
	CreateSurface(m_imageData,ctvalue,0.5,color,renderer,true,5);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_actionLargestRegion_triggered()
{
    DeleteAllThing();
    double color[3]={1,0,0};
    CreateSurface(m_imageData,ctvalue,0.5,color,renderer,true);
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
    CreateVolume(m_imageData,1,1,0,renderer);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_pushButton_clicked()
{
	vtkVolume* test =  (vtkVolume*)(renderer->GetVolumes()->GetLastProp());

	vtkPiecewiseFunction* opacity=test->GetProperty()->GetScalarOpacity();
	opacity->ReleaseData();
	opacity->AddPoint(200, 0, .49, .61 );
	opacity->AddPoint(641, .72, .5, 0.0 );
	opacity->AddPoint(3071, .71, 0.5, 0.0);
	opacity->Modified();
	test->Modified();

	ui->qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::on_ClipFrustum_triggered()
{
    DeleteAllThing();
    std::string filepath = "../../Data/ply/Armadillo.ply";
    CreateClipFrustum(renderer,filepath.c_str());
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
	CreateThreeSlice(m_dim,renderer,dicomReader);

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
