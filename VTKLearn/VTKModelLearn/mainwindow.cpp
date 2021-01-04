#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Globe/GlobeInclude.h"

#include "interactortest.h"
#include "../Globe/myfunc.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "heartseg.h"



class vtkMyCameraCallback : public vtkCommand
{
public:
    static vtkMyCameraCallback *New()
    {
        return new vtkMyCameraCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkCamera *camera = static_cast<vtkCamera*>(caller);
        double camPos[3];
        double focalPos[3];
        double upVector[3];
        camera->GetPosition(camPos);
        camera->GetFocalPoint(focalPos);
        camera->GetViewUp(upVector);

        //遍历所有renderer，都设置一遍最新的相机
        m_renderer1->SetActiveCamera(camera);
        m_renderer2->SetActiveCamera(camera);
        m_renderer1->GetRenderWindow()->Render();
        m_renderer2->GetRenderWindow()->Render();
    }
    vtkRenderer* m_renderer1;
    vtkRenderer* m_renderer2;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	m_timer = NULL;
    this->resize(800,600);
	m_volumeInfo = NULL;

    //读取文件"H:/niu/20190301/YANGYUEMING";
    char *dirname = "H:/AllData/133ceng";
//    char *dirname = "E:/segdicom";

    dicomReader = vtkDICOMImageReader::New();
    vtkDataArray* scalarsArr = NULL;
    double scalarRange[2];
	MyFunc::ReadDicomData(dicomReader,dirname,m_dim,m_spacing,
						  scalarRange,m_imageData,scalarsArr);
    qDebug() << "dim:" << m_dim[0]<<m_dim[1]<<m_dim[2];
	qDebug() << "m_spacing:" << m_spacing[0]<<m_spacing[1]<<m_spacing[2];

    //initImagePartData();
	//初始化一些信息
    InitInfo();
    vtkSmartPointer<vtkCamera> camera =vtkSmartPointer<vtkCamera>::New();
    renderer->SetActiveCamera(camera);
    renderer2->SetActiveCamera(camera);

//    vtkSmartPointer<vtkMyCameraCallback> cameraCallback1 = vtkSmartPointer<vtkMyCameraCallback>::New();
//    cameraCallback1->m_renderer1 = renderer;
//    cameraCallback1->m_renderer2 = renderer2;
//    camera->AddObserver(vtkCommand::ModifiedEvent, cameraCallback1);

    ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
//    ui->qvtkWidget->GetInteractor()->SetInteractorStyle(m_volumeStyle);
    ui->qvtkWidget->GetRenderWindow()->Render();

    ui->qvtkWidget_2->GetRenderWindow()->AddRenderer(renderer2);
    ui->qvtkWidget_2->GetRenderWindow()->Render();
	//VE相关
	m_vecontrol = new VEControl;
	m_veactive = false;
    m_polyDataOpe = new PolyDataOpe;
}

MainWindow::~MainWindow()
{
    delete ui;
    DeleteAllThing();
    dicomReader->Delete();
    light1->Delete();
	renderer->Delete();
	m_imagePartData->Delete();
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

void MainWindow::initImagePartData()
{
	m_imagePartData = vtkImageData::New();
	m_imagePartData->SetDimensions(m_dim);
	m_imagePartData->SetSpacing(m_spacing);
	m_imagePartData->AllocateScalars(VTK_SHORT,1);
	SetImagePartDataValue();
}

void MainWindow::SetImagePartDataValue(short ctvalue)
{
	short *ptr = static_cast<short*>(m_imagePartData->GetScalarPointer());
	for(int k=0;k<m_dim[2];k++){
		for(int j=0;j<m_dim[1];j++){
			for(int i=0;i<m_dim[0];i++){
				ptr[k*m_dim[1]*m_dim[0]+j*m_dim[0]+i] = ctvalue;
			}
		}
	}
    m_imagePartData->Modified();
}

void MainWindow::AddTwoVolume()
{
    vtkMetaImageReader* read1 = vtkMetaImageReader::New();
    read1->SetFileName("E:/mhddata/47830/gg1_1_47830.mhd");
    read1->Update();
    vtkImageData* image1 = read1->GetOutput();
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> mapper1 =
        vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    mapper1->SetInputData(image1);
    mapper1->SetBlendMode(1);
    mapper1->SetAutoAdjustSampleDistances(1);
    vtkSmartPointer<vtkColorTransferFunction> colorFun1 =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    vtkSmartPointer<vtkPiecewiseFunction> opacityFun1 =
            vtkSmartPointer<vtkPiecewiseFunction>::New();
    vtkSmartPointer<vtkVolumeProperty> property1 =
            vtkSmartPointer<vtkVolumeProperty>::New();
    colorFun1->AddRGBPoint( -1024, 0, 0, 0);
    colorFun1->AddRGBPoint( 1600, 1, 1, 1 );
    opacityFun1->AddPoint(-1024, 0 );
    opacityFun1->AddPoint(1600, 0.8 );
    property1->SetColor( colorFun1 );
    property1->SetScalarOpacity( opacityFun1 );
    property1->SetInterpolationTypeToLinear();
    vtkSmartPointer<vtkVolume> volume1 = vtkSmartPointer<vtkVolume>::New();
    volume1->SetProperty( property1 );
    volume1->SetMapper( mapper1 );
    renderer->AddActor( volume1 );

    vtkMetaImageReader* read2 = vtkMetaImageReader::New();
    read2->SetFileName("E:/mhddata/47830/gg1_2_47830.mhd");
    read2->Update();
    vtkImageData* image2 = read2->GetOutput();
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> mapper2 =
        vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    mapper2->SetInputData(image2);
    mapper2->SetBlendMode(1);
    mapper2->SetAutoAdjustSampleDistances(1);
    vtkSmartPointer<vtkColorTransferFunction> colorFun2 =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    vtkSmartPointer<vtkPiecewiseFunction> opacityFun2 =
            vtkSmartPointer<vtkPiecewiseFunction>::New();
    vtkSmartPointer<vtkVolumeProperty> property2 =
            vtkSmartPointer<vtkVolumeProperty>::New();
    colorFun2->AddRGBPoint( -1024, 0, 0, 0);
    colorFun2->AddRGBPoint( 0, 0.6, 0.6, 0.6);
    colorFun2->AddRGBPoint( 1600, 1, 1, 1 );
    opacityFun2->AddPoint(-1024, 0 );
    opacityFun2->AddPoint(0, 0.6 );
    opacityFun2->AddPoint(1600, 1 );
    property2->SetColor( colorFun2 );
    property2->SetScalarOpacity( opacityFun2 );
    property2->SetInterpolationTypeToLinear();
    vtkSmartPointer<vtkVolume> volume2 = vtkSmartPointer<vtkVolume>::New();
    volume2->SetProperty( property2 );
    volume2->SetMapper( mapper2 );
    renderer->AddActor( volume2 );
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
	m_volumeStyle->imagedata = m_imageData;

    renderer2 = vtkRenderer::New();
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
	camera->SetPosition(dataCenterPos[0],dataCenterPos[1]+200,dataCenterPos[2]);
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
int ctvalue=-600;
void MainWindow::on_actionSurface_triggered()
{
    DeleteAllThing();
	double color[3]={1,0,0};
	MyFunc::CreateSurface(m_imagePartData,ctvalue,1,color,renderer,false,5);
//	double color2[3]={1,1,0};
//	MyFunc::CreateSurface(m_imageData,300,1,color2,renderer,false,5);
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
void MainWindow::on_actionSeedGrowth_triggered()
{
	/*测试三维中种子增长*/
	int dimstart1[3] = {280,284,132};//test 1
	double tmpPos[3] = {163.77 ,207.436,17.5};//
	int dimstart2[3] = {tmpPos[0]/m_spacing[0],tmpPos[1]/m_spacing[1],
					   tmpPos[2]/m_spacing[2]};//test 2_600
	MyFunc::VolumeSeedGrowth(dimstart2,m_imageData,m_imagePartData,30);
	m_imageData->Modified();
	ui->qvtkWidget->GetRenderWindow()->Render();
}
void MainWindow::TestCreateLine(QString allposstr, double color[3])
{
    vtkSmartPointer<vtkActor> lineactor =
      vtkSmartPointer<vtkActor>::New();

    vtkSmartPointer<vtkPoints> Points =
      vtkSmartPointer<vtkPoints>::New();

    QStringList poslist = allposstr.split(",");
    for(int i=0;i<poslist.size();i++){
        QStringList tmpPos = poslist.at(i).split("|");
        if(tmpPos.size() != 3){
            continue;
        }
        Points->InsertNextPoint(tmpPos[0].toFloat(),tmpPos[1].toFloat(),tmpPos[2].toFloat());
    }

    qDebug() <<"point size:" << Points->GetNumberOfPoints();
    MyFunc::CreateLineActor(Points,lineactor,color);
    renderer->AddActor(lineactor);
}

void MainWindow::TestCreatePoint(double center[], double color[])
{
    float radius = 0.4;
    vtkSmartPointer<vtkSphereSource> sphere =
      vtkSmartPointer<vtkSphereSource>::New();

    sphere->SetCenter(center);
    sphere->SetRadius(radius);
    sphere->SetPhiResolution(100);
    sphere->SetThetaResolution(100);
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphere->GetOutputPort());
    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    renderer->AddActor(actor);
}

void MainWindow::TestDrawAdjustCenterLine()
{
/** 绘制调整中心线*/
    vtkSmartPointer<vtkNamedColors> colors =
      vtkSmartPointer<vtkNamedColors>::New();
    QVector<QString> colorname;
    colorname<<"Banana"<<"Tomato"<<"Wheat"<<"Lavender"<<"Flesh"
               <<"Raspberry"<<"Salmon"<<"Mint"<<"Peacock";
    vtkStringArray *colorarr = vtkStringArray::New();
    colors->GetColorNames(colorarr);

    //绘制点集
    for(int i=1;i<10;i++){
        QString filepath = "E:/test/allpos"+QString::number(i)+".txt";

        QFile file(filepath);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "not have file:" << filepath;
             continue;
        }
        QTextStream in(&file);
        QString allposinfo = in.readAll();
        QByteArray bytearr = allposinfo.toUtf8();//QString转QByteArray
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(bytearr,&jsonError);//QByteArray转QJsonDocument
        if (jsonError.error != QJsonParseError::NoError){
            QString errdata = QString(jsonError.errorString().toUtf8().constData());
            qDebug() << errdata;
        }
        QJsonObject obj = doc.object();//QJsonDocument转QJsonObject

        QJsonArray stroldposarr = obj["oldpos"].toArray();
        QString stroldallpos = obj["oldposstr"].toString();
        QJsonArray strnewposarr = obj["newpos"].toArray();
        QString strnewallpos = obj["newposstr"].toString();

        double oldcolor[3] = {1,0,0};
        colors->GetColor(colorarr->GetValue(i),oldcolor[0],oldcolor[1],oldcolor[2]);
        qDebug() << "oldcolor:" << oldcolor[0]<<oldcolor[1]<<oldcolor[2];
        double oldcenter[3] = {stroldposarr.at(0).toDouble(),
                              stroldposarr.at(1).toDouble(),
                              stroldposarr.at(2).toDouble()};
        TestCreateLine(stroldallpos,oldcolor);
        TestCreatePoint(oldcenter,oldcolor);

        double newcolor[3] = {0,1,0};
        colors->GetColor(colorarr->GetValue(i*20+1),newcolor[0],newcolor[1],newcolor[2]);
        qDebug() << "newcolor:" << newcolor[0]<<newcolor[1]<<newcolor[2];
        double newcenter[3] = {strnewposarr.at(0).toDouble(),
                               strnewposarr.at(1).toDouble(),
                               strnewposarr.at(2).toDouble()};
        TestCreateLine(strnewallpos,newcolor);
        TestCreatePoint(newcenter,newcolor);
        file.close();
    }

}
#include <vtkPropAssembly.h>
#include <vtkCullerCollection.h>
#include <vtkFrustumCoverageCuller.h>
#include <vtkCuller.h>
void MainWindow::on_pushButton_clicked()
{

    m_heartseg = new HeartSeg;

    m_render1 = vtkRenderer::New();
    m_render2 = vtkRenderer::New();
    m_render3 = vtkRenderer::New();
    m_render4 = vtkRenderer::New();
    m_render5 = vtkRenderer::New();
    m_render6 = vtkRenderer::New();

    m_volume1 = vtkVolume::New();
    m_heartseg->ReadmhdData(m_volume1,1);
//    renderer->AddVolume(m_volume1);

    m_volume2 = vtkVolume::New();
    m_heartseg->ReadmhdData(m_volume2,2);
//    renderer->AddVolume(m_volume2);

    m_volume3 = vtkVolume::New();
    m_heartseg->ReadmhdData(m_volume3,3);
//    renderer->AddVolume(m_volume3);

    m_volume4 = vtkVolume::New();
    m_heartseg->ReadmhdData(m_volume4,4);
//    renderer->AddVolume(m_volume4);

    m_volume5 = vtkVolume::New();
    m_heartseg->ReadmhdData(m_volume5,5);
//    renderer->AddVolume(m_volume5);

    m_volume6 = vtkVolume::New();
    m_heartseg->ReadmhdData(m_volume6,6);
//    renderer->AddVolume(m_volume6);
    m_render1->AddVolume(m_volume1);
    m_render1->AddVolume(m_volume2);
    m_render1->AddVolume(m_volume3);
    m_render1->AddVolume(m_volume4);
    m_render1->AddVolume(m_volume5);
    m_render1->AddVolume(m_volume6);

//    vtkFrustumCoverageCuller* realculler = (vtkFrustumCoverageCuller*)(m_render1->GetCullers()->GetItemAsObject(0));
//    vtkCuller* culler = m_render1->GetCullers()->GetLastItem();
//    vtkFrustumCoverageCuller* realculler = static_cast<vtkFrustumCoverageCuller*>(culler);
//    realculler->SetSortingStyleToBackToFront();


    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render1);
//    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render2);
//    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render3);
//    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render4);
//    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render5);
//    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render6);
/*
    vtkPropAssembly* props = vtkPropAssembly::New();
    props->AddPart(m_volume1);
    props->AddPart(m_volume2);
    props->AddPart(m_volume3);
    props->AddPart(m_volume4);
    props->AddPart(m_volume5);
    props->AddPart(m_volume6);
    renderer->AddVolume(props);
*/
    m_render1->ResetCamera();
//    m_render2->ResetCamera();
//    m_render3->ResetCamera();
//    m_render4->ResetCamera();
//    m_render5->ResetCamera();
//    m_render6->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();


//    m_volume2 = vtkVolume::New();
//    m_heartseg->ReadmhdData2(m_volume2);
//    renderer2->AddVolume(m_volume2);
//    renderer2->ResetCamera();
//    ui->qvtkWidget_2->GetRenderWindow()->Render();
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
//	MyFunc::CreateVolume(m_imageData,0,1,0,renderer);

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

void MainWindow::on_actionPolyDataWidget_triggered()
{
    m_polyDataOpe->show();
}

void MainWindow::on_SwitchBtn_clicked()
{
    static int num = 1;
    ++num;
    if(num>6){
        num = 1;
    }

    vtkGPUVolumeRayCastMapper* gmapper =
            (vtkGPUVolumeRayCastMapper*)(m_volume2->GetMapper());
    gmapper->RemoveAllInputs();
    vtkImageData* tmpData = vtkImageData::New();
    m_heartseg->GetSectionImageData(tmpData,num);
    gmapper->SetInputData(tmpData);
    gmapper->Modified();

    vtkPiecewiseFunction* opacity=m_volume2->GetProperty()->GetScalarOpacity();
    opacity->ReleaseData();
    m_heartseg->GetOpacityFun(opacity,num);
    opacity->Modified();

    m_volume2->Modified();
    renderer2->ResetCamera();
    ui->qvtkWidget_2->GetRenderWindow()->Render();
    tmpData->Delete();

    //-------------old
    vtkGPUVolumeRayCastMapper* gmapper1 =
            (vtkGPUVolumeRayCastMapper*)(m_volume1->GetMapper());
    gmapper1->RemoveAllInputs();
    vtkImageData* tmpData1 = vtkImageData::New();
    m_heartseg->GetSectionImageData(tmpData1,num,1);
    gmapper1->SetInputData(tmpData1);
    gmapper1->Modified();

//    vtkPiecewiseFunction* opacity1=m_volume1->GetProperty()->GetScalarOpacity();
//    opacity1->ReleaseData();
//    m_heartseg->GetOpacityFun(opacity1,num);
//    opacity1->Modified();
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
    tmpData->Delete();
}
