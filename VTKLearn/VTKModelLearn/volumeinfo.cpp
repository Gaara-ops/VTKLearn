#include "volumeinfo.h"
#include "ui_volumeinfo.h"
#include "../Globe/GlobeInclude.h"
#include "myfunc.h"

VolumeInfo::VolumeInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VolumeInfo)
{
    ui->setupUi(this);
	thresholdImage = vtkImageData::New();
    {
    ///初始化灯光面板信息
    ui->lineEdit_LC->setText("1,1,1");
    ui->lineEdit_LAC->setText("1,0,0");
    ui->lineEdit_LDC->setText("0,1,0");
    ui->lineEdit_LSC->setText("0,0,1");
    ui->lineEdit_LI->setText("50");
    ui->lineEdit_LS->setText("1");
    ///初始化阴影面板信息
    ui->lineEdit_SA->setText("0.4");
    ui->lineEdit_SD->setText("0.6");
    ui->lineEdit_SS->setText("0.5");
    ui->lineEdit_SSP->setText("50");
    ui->lineEdit_SSD->setText("0.5");
	///阈值筛选
	ui->lineEdit_minV->setText("200");
	ui->lineEdit_maxV->setText("400");
    }

	ui->lineEdit_SeedThreshold->setText("50");
}

VolumeInfo::~VolumeInfo()
{
	delete ui;
}

void VolumeInfo::initPlaneInfo()
{
	///平面设置
	int dim[3];double spacing[3];
	oriImageData->GetDimensions(dim);
	oriImageData->GetSpacing(spacing);
	double center[3] = {(dim[0]-1)*spacing[0]/2,(dim[1]-1)*spacing[1]/2,
					   (dim[2]-1)*spacing[2]/2};
	double* direction=renderer->GetActiveCamera()->GetDirectionOfProjection();
	QString oristr = QString::number(center[0])+ "," ;
			oristr += QString::number(center[1])+"," ;
			oristr += QString::number(center[2]);
	QString dirstr = QString::number(direction[0])+ "," ;
			dirstr += QString::number(direction[1])+"," ;
			dirstr += QString::number(direction[2]);
	ui->lineEdit_Origin->setText(oristr);
	ui->lineEdit_Direction->setText(dirstr);
}

void VolumeInfo::initSeedInfo(QString seedpos, short tmpct)
{
	ui->lineEdit_SeedPos->setText(seedpos);
	ui->lineEdit_SeedCT->setText(QString::number(tmpct));
}

void VolumeInfo::initSeedInfo(double seedpos[])
{
	double spacing[3];
	oriImageData->GetSpacing(spacing);
	int dimx = seedpos[0]/spacing[0];
	int dimy = seedpos[1]/spacing[1];
	int dimz = seedpos[2]/spacing[2];
	QString seedposstr = QString::number(dimx)+","+
			QString::number(dimy)+","+QString::number(dimz);

	short *tmpct = static_cast<short*>(oriImageData->GetScalarPointer(
									dimx,dimy,dimz));
	initSeedInfo(seedposstr,*tmpct);
}

void VolumeInfo::on_LightUpdateBtn_clicked()
{
    //灯光颜色解析
    QString lstr = ui->lineEdit_LC->text();
    QStringList lstrlist = lstr.split(',');
    float lr = lstrlist.at(0).toFloat();
    float lg = lstrlist.at(1).toFloat();
    float lb = lstrlist.at(2).toFloat();
    //环境光颜色解析
    QString astr = ui->lineEdit_LAC->text();
    QStringList astrlist = astr.split(',');
    float ar = astrlist.at(0).toFloat();
    float ag = astrlist.at(1).toFloat();
    float ab = astrlist.at(2).toFloat();
    //漫反射光颜色解析
    QString dstr = ui->lineEdit_LDC->text();
    QStringList dstrlist = dstr.split(',');
    float dr = dstrlist.at(0).toFloat();
    float dg = dstrlist.at(1).toFloat();
    float db = dstrlist.at(2).toFloat();
    //镜面反射光颜色解析
    QString sstr = ui->lineEdit_LSC->text();
    QStringList sstrlist = sstr.split(',');
    float sr = sstrlist.at(0).toFloat();
    float sg = sstrlist.at(1).toFloat();
    float sb = sstrlist.at(2).toFloat();

    light->SetColor(lr,lg,lb);
    light->SetAmbientColor(ar,ag,ab);
    light->SetDiffuseColor(dr,dg,db);
    light->SetSpecularColor(sr,sg,sb);
    //亮度
    double brightness = ui->lineEdit_LI->text().toDouble();
    //是否取消灯光
    int isoff = ui->lineEdit_LS->text().toInt();
    light->SetIntensity(brightness);
    light->SetSwitch(isoff);
    light->Modified();
    vtkwindow->Render();
}

void VolumeInfo::on_closeBtn_clicked()
{
    this->close();
}

void VolumeInfo::on_ShadeUpdateBtn_clicked()
{
    QString ambient = ui->lineEdit_SA->text();
    QString diffuse = ui->lineEdit_SD->text();
    QString specular = ui->lineEdit_SS->text();
    QString specularpower = ui->lineEdit_SSP->text();

    vtkVolume* volume = (vtkVolume*)(renderer->GetVolumes()->GetLastProp());
    volume->GetProperty()->ShadeOn();
    volume->GetProperty()->SetAmbient(ambient.toDouble());
    volume->GetProperty()->SetDiffuse(diffuse.toDouble());
    volume->GetProperty()->SetSpecular(specular.toDouble());
    volume->GetProperty()->SetSpecularPower(specularpower.toDouble());

    QString sampledistance = ui->lineEdit_SSD->text();
	((vtkGPUVolumeRayCastMapper*)(volume->GetMapper()))->SetSampleDistance(
				sampledistance.toDouble());
    volume->Modified();
    vtkwindow->Render();
}

void VolumeInfo::on_ThresholdBtn_clicked()
{
	double minValue = ui->lineEdit_minV->text().toDouble();
	double maxValue = ui->lineEdit_maxV->text().toDouble();
	vtkVolume* test =  (vtkVolume*)(renderer->GetVolumes()->GetLastProp());
	vtkGPUVolumeRayCastMapper* gmapper =
			(vtkGPUVolumeRayCastMapper*)(test->GetMapper());
	gmapper->RemoveAllInputs();
	thresholdImage->ReleaseData();
	MyFunc::VolumeThresholding(oriImageData,minValue,maxValue,thresholdImage);
	gmapper->SetInputData(thresholdImage);
	gmapper->Modified();

	//更新传递函数
	vtkPiecewiseFunction* opacity=test->GetProperty()->GetScalarOpacity();
	opacity->ReleaseData();
	opacity->AddPoint(minValue, 0, .49, .61 );
	opacity->AddPoint(maxValue, .9, 0.5, 0.0);
	opacity->Modified();
	vtkColorTransferFunction *colorFun=
			test->GetProperty()->GetRGBTransferFunction();
	colorFun->RemoveAllPoints();
	colorFun->AddRGBPoint( minValue, 0, 0, 0, 0.5, 0.0 );
	colorFun->AddRGBPoint(maxValue, 0.9, 0.9, 0.8, 0.49, .61 );
	colorFun->Modified();

	test->Modified();
	vtkwindow->Render();
}

void VolumeInfo::on_ClipPlaneBtn_clicked()
{
	initPlaneInfo();
	int dim[3];double spacing[3];
	oriImageData->GetDimensions(dim);
	oriImageData->GetSpacing(spacing);
	double center[3] = {(dim[0]-1)*spacing[0]/2,(dim[1]-1)*spacing[1]/2,
					   (dim[2]-1)*spacing[2]/2};
	double* direction=renderer->GetActiveCamera()->GetDirectionOfProjection();
	/**
	 * 用面对体进行切割*/
	vtkSmartPointer<vtkPlane> plane =
	  vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(center);
	plane->SetNormal(direction);

	vtkVolume* test =  (vtkVolume*)(renderer->GetVolumes()->GetLastProp());
	vtkGPUVolumeRayCastMapper* gmapper =
			(vtkGPUVolumeRayCastMapper*)(test->GetMapper());

	vtkSmartPointer<vtkPlaneCollection> planes =
	  vtkSmartPointer<vtkPlaneCollection>::New();
	planes->AddItem(plane);
	gmapper->SetClippingPlanes(planes);
	gmapper->Modified();
	vtkwindow->Render();
}

void VolumeInfo::on_SeedGrowthBtn_clicked()
{
	QString pos = ui->lineEdit_SeedPos->text();
	short threshold = ui->lineEdit_SeedThreshold->text().toShort();
	QStringList tmplist = pos.split(",");
	if(tmplist.size() == 3){
		int dimstart[3]= { tmplist.at(0).toInt(),
						   tmplist.at(1).toInt(),tmplist.at(2).toInt() };
		MyFunc::VolumeSeedGrowth(dimstart,oriImageData,threshold);
		qDebug() << "grow end!";
	}
}
