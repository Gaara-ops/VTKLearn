#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//读取文件
	char *dirname = "E:/workspace/DICOM/133ceng";
	dicomReader = vtkDICOMImageReader::New();
	dicomReader->SetDirectoryName(dirname);
	dicomReader->Update();
	m_oriImageData = dicomReader->GetOutput();
	m_oriImageData->GetDimensions(m_dim);
	m_oriImageData->GetSpacing(m_spacing);

	renderer = vtkRenderer::New();
	ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
	ui->qvtkWidget->GetRenderWindow()->Render();

	//创建测试体
	createTestVolume();
}

MainWindow::~MainWindow()
{
	dicomReader->Delete();
	renderer->Delete();
	delete ui;
}

void MainWindow::createTestVolume()
{
	int orivalue = 2048,nowvalue = -2048;
	vtkImageData* imagedata = vtkImageData::New();
	imagedata->DeepCopy(m_oriImageData);
	imagedata->Modified();
	for(int z=0;z<m_dim[2];z++){
		for(int y=0;y<m_dim[1];y++){
			for(int x=0;x<m_dim[0];x++){
				short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
							x,y,z));
				*ptr0 = orivalue;
			}
		}
	}
	QString centerLinePos = "";
	int dim1[3] = {100,100,0};
	int dim2[3] = {100,100,50};
	double tmpPos[3] = {100*m_spacing[0],100*m_spacing[1],0};
	for(int i=0;i<50;i++){
		double tmpz = i*m_spacing[2];
		centerLinePos += QString::number(tmpPos[0])+","+
				QString::number(tmpPos[1])+","+
				QString::number(tmpz)+"|";
	}
	int tmpLen = 20;int tmpLen2 = 10;
	for(int z=dim1[2];z<dim2[2];z++){
		for(int y=-tmpLen;y<=tmpLen;y++){
			for(int x=-tmpLen;x<=tmpLen;x++){
				short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
							dim2[0]+x,dim2[1]+y,z));
				*ptr0 = nowvalue;
			}
		}
	}
	int dim3[3] = {100,200,50};
	double tmpPos3[3] = {100*m_spacing[0],0,50*m_spacing[2]};
	for(int i=100;i<200;i++){
		double tmpy = i*m_spacing[1];
		centerLinePos += QString::number(tmpPos3[0])+","+
				QString::number(tmpy)+","+
				QString::number(tmpPos3[2])+"|";
	}
	for(int z=dim3[2]-tmpLen2;z<=dim3[2]+tmpLen2;z++){
		for(int y=dim2[1]-tmpLen;y<=dim3[1]+tmpLen;y++){
			for(int x=-tmpLen;x<=tmpLen;x++){
				short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
							dim3[0]+x,y,z));
				*ptr0 = nowvalue;
			}
		}
	}
	int dim4[3] = {100,200,100};
	double tmpPos4[3] = {100*m_spacing[0],200*m_spacing[1],0};
	for(int i=50;i<100;i++){
		double tmpz = i*m_spacing[2];
		centerLinePos += QString::number(tmpPos4[0])+","+
				QString::number(tmpPos4[1])+","+
				QString::number(tmpz)+"|";
	}
	for(int z=dim3[2];z<dim4[2];z++){
		for(int y=-tmpLen;y<=tmpLen;y++){
			for(int x=-tmpLen;x<=tmpLen;x++){
				short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
							dim4[0]+x,dim4[1]+y,z));
				*ptr0 = nowvalue;
			}
		}
	}
	int dim5[3] = {100,100,100};
	double tmpPos5[3] = {100*m_spacing[0],0,100*m_spacing[2]};
	for(int i=200;i>100;i--){
		double tmpy = i*m_spacing[1];
		centerLinePos += QString::number(tmpPos5[0])+","+
				QString::number(tmpy)+","+
				QString::number(tmpPos5[2])+"|";
	}
	for(int z=dim5[2]-tmpLen2;z<=dim5[2]+tmpLen2;z++){
		for(int y=dim5[1]-tmpLen;y<=dim4[1]+tmpLen;y++){
			for(int x=-tmpLen;x<=tmpLen;x++){
				short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
							dim5[0]+x,y,z));
				*ptr0 = nowvalue;
			}
		}
	}
	int dim6[3] = {100,100,133};
	double tmpPos6[3] = {100*m_spacing[0],100*m_spacing[1],0};
	for(int i=100;i<133;i++){
		double tmpz = i*m_spacing[2];
		centerLinePos += QString::number(tmpPos6[0])+","+
				QString::number(tmpPos6[1])+","+
				QString::number(tmpz)+"|";
	}
	for(int z=dim5[2];z<dim6[2];z++){
		for(int y=-tmpLen;y<=tmpLen;y++){
			for(int x=-tmpLen;x<=tmpLen;x++){
				short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
							dim6[0]+x,dim6[1]+y,z));
				*ptr0 = nowvalue;
			}
		}
	}
	imagedata->Modified();

	MyFunc::CreateVolume(imagedata,0,1,0,renderer);

	renderer->ResetCamera();
	ui->qvtkWidget->GetRenderWindow()->Render();
	qDebug() << "endl";
}
