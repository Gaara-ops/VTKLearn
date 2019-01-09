#include "vecontrol.h"
#include "../Globe/myfunc.h"
VEControl::VEControl()
{
	m_sphere = NULL;
	m_render = NULL;
	m_posindex = 0;
}

VEControl::~VEControl()
{
	if(m_sphere != NULL){
		m_sphere->Delete();
	}
}

void VEControl::ReadPointData(QString path)
{
	QVector<QVector3D> m_posVec2;
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))
	{
		 return;
	}
	QTextStream in(&file);
	QString alldata = in.readAll();
	QStringList alldatalist = alldata.split("|");
	int* dim = m_oriData->GetDimensions();
	double* spacing = m_oriData->GetSpacing();
	double realy = (dim[1]-1)*spacing[1];
	for(int i=0;i<alldatalist.size();i++){
		QStringList strlist = alldatalist.at(i).split(",");
		if(strlist.size() == 3){
			double tmpx = strlist.at(0).trimmed().toDouble();
			double tmpy = strlist.at(1).trimmed().toDouble();
			double tmpz = strlist.at(2).trimmed().toDouble();
			m_posVec2.append(QVector3D(tmpx/2,(realy-tmpy)/2,tmpz/2));
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

void VEControl::UpdateFocusSphere(double center[])
{
	if(m_sphere == NULL){
		m_sphere =vtkSphereSource::New();
		m_sphere->SetCenter(center);
		m_sphere->SetRadius(0.1);
		m_sphere->SetPhiResolution(100);
		m_sphere->SetThetaResolution(100);
		vtkSmartPointer<vtkPolyDataMapper> mapper =
		  vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(m_sphere->GetOutputPort());
		vtkSmartPointer<vtkActor> actor =
		  vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(1,0,0);
		m_render->AddActor(actor);
	}else{
		m_sphere->SetCenter(center);
		m_sphere->Modified();
	}
}

void VEControl::UpdateViewFocus()
{
	/*测试视角延中心线移动*/
	int sumpoint = m_curveLine->GetOutput()->GetNumberOfPoints();
	double pos1[3],pos2[3];
	m_curveLine->GetOutput()->GetPoint(m_posindex,pos1);
	m_curveLine->GetOutput()->GetPoint(m_posindex+1,pos2);
	UpdateFocusSphere(pos1);

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

	vtkCamera* camera = m_render->GetActiveCamera();
	double camradis = 8;//1000
	camera->SetPosition(pos1[0]+tmpDir[0]*camradis,pos1[1]+tmpDir[1]*camradis,
			pos1[2]+tmpDir[2]*camradis);
	camera->SetFocalPoint(pos1);
	camera->SetViewUp(projected);
	camera->Modified();
	m_render->Render();
	m_render->GetRenderWindow()->Render();
	m_posindex++;
	if(sumpoint-1 <= m_posindex){
		m_posindex = 0;
	}
}

void VEControl::ResampleImageData(vtkImageData *oridata,
								  vtkImageData *resdata, double resample)
{
	int dim[3];
	double spacing[3];
	oridata->GetDimensions(dim);
	oridata->GetSpacing(spacing);
	int xVSize = dim[0]/resample;
	int yVSize = dim[1]/resample;
	int zVSize = dim[2];
	double volumeSpace = spacing[0];
	double sliceSpace = spacing[2]/resample;
	resdata->SetDimensions(xVSize, yVSize,zVSize);
	resdata->SetSpacing(volumeSpace, volumeSpace, sliceSpace);
	resdata->AllocateScalars(VTK_SHORT,1);
	int* dims = resdata->GetDimensions();
	short *ptr = static_cast<short*>(resdata->GetScalarPointer());
	for(int i=0; i<dims[2]; i++)
	{
		for(int j=0; j<dims[1]; j++)
		{
			for(int k=0; k<dims[0]; k++)
			{
				short *ptr2 = static_cast<short*>(
							oridata->GetScalarPointer(k*resample,
											(dims[1]-j-1)*resample,i));
			   ptr[i * dims[1] *dims[0] + j * dims[0] + k] = *ptr2;

			}
		}
	}
	resdata->Modified();//更新数据后数据刷新
}

void VEControl::CreateCurveLine()
{
	vtkPoints* points = vtkPoints::New();
	for(int i=0;i<m_posVec.size();i++){
		points->InsertNextPoint(m_posVec.at(i).x(),
								m_posVec.at(i).y(),
								m_posVec.at(i).z());
	}
	vtkActor* allPointActor = vtkActor::New();
	double pointcolor[3] = {0,0,1};
	MyFunc::CreatePointActor(points,allPointActor,pointcolor,5,1);
	m_render->AddActor(allPointActor);

	m_curveLine =vtkParametricFunctionSource::New();
	vtkActor* curvelineactor = vtkActor::New();
	double lcolor[3] = {0,1,0};
	MyFunc::CreateCurveLineActor(points,curvelineactor,m_curveLine,lcolor,2.0);
	m_render->AddActor(curvelineactor);
	m_render->GetRenderWindow()->Render();
}
