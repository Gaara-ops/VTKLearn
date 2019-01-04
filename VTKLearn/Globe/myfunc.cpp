#include "myfunc.h"

//#include "MyDiocmInteractorStyleImage.h"
MyFunc::MyFunc()
{
}

bool MyFunc::InterpolateImagedata(vtkImageData *oridata,
								  vtkImageData *interpolatedata,
								  int interpolatetype)
{
	int dim[3];
	double spacing[3];
	oridata->GetDimensions(dim);
	oridata->GetSpacing(spacing);
	int type = 2;
	if(interpolatetype == 0){
		if(dim[0] != dim [1]){
			qDebug() << "dim[0] != dim [1]";
			return 0;
		}
		if(dim[0] == dim [2]){
			qDebug() << "dim[0] == dim [2]";
			return 0;
		}
		if(dim[0] < dim[2]){
			type = 0;
		}
	}else if(interpolatetype == 1){
		if(spacing[0] != spacing [1]){
			qDebug() << "spacing[0] != spacing [1]";
			return 0;
		}
		if(spacing[0] == spacing [2]){
			qDebug() << "spacing[0] == spacing [2]";
			return 0;
		}
		if(spacing[0] > spacing[2]){
			type = 0;
		}
	}
	if(type == 0){
		qDebug() << "dim[0] < dim [2]";
		return 0;
	}
	double realzspacing = (dim[2]-1)*spacing[2]/dim[0];
	if(interpolatetype == 0){
		interpolatedata->SetDimensions(dim[0],dim[0],dim[0]);
		interpolatedata->SetSpacing(spacing[0],spacing[0],realzspacing);
	}else if(interpolatetype == 1){
		realzspacing = spacing[0];
		int realDimz = (dim[2]-1)*spacing[2]/spacing[0];
		interpolatedata->SetDimensions(dim[0],dim[0],realDimz+1);
		interpolatedata->SetSpacing(spacing[0],spacing[0],spacing[0]);
	}
	interpolatedata->AllocateScalars(VTK_SHORT,1);

	short *ptr = static_cast<short*>(interpolatedata->GetScalarPointer());
	double tmpzTest = 0;
	int tmprealDim = 0;
	for(int i = 0;i<dim[0];i++){
		for(int j = 0;j<dim[1];j++){
			for(int k = 0;k<dim[2]-1;k++){
				if(tmpzTest > spacing[2]){
					tmpzTest = tmpzTest-spacing[2];
				}
				short* ptr1 = static_cast<short*>(oridata->GetScalarPointer(
							i,j,k));
				short* ptr2 = static_cast<short*>(oridata->GetScalarPointer(
							i,j,k+1));
				short tmpdistance = (*ptr2)-(*ptr1);
				while(tmpzTest <= spacing[2]){
					int tmpindx = tmprealDim*dim[0]*dim[1]+j*dim[0]+i;
					ptr[tmpindx] = (*ptr1)+(tmpzTest/spacing[2])*tmpdistance;
					tmprealDim++;
					tmpzTest = tmpzTest+realzspacing;
				}
			}
			tmprealDim = 0;
		}
	}
	interpolatedata->Modified();
	qDebug() << "interpolate end";
}

bool MyFunc::CreateImageDataXYZSlice(vtkImageData *data)
{
	QString xslicepath="./xslice/",yslicepath="./yslice/",zslicepath="./zslice/";
	QDir tempDir;
	if(!tempDir.exists(xslicepath)){
		tempDir.mkpath(xslicepath);
	}
	if(!tempDir.exists(yslicepath)){
		tempDir.mkpath(yslicepath);
	}
	if(!tempDir.exists(zslicepath)){
		tempDir.mkpath(zslicepath);
	}
	int dim[3];
	data->GetDimensions(dim);
	int width=dim[0],height=dim[1];
	double range[2];
	data->GetScalarRange(range);
	for(int z = 0;z<dim[2];z++){
		QImage image( width, height, QImage::Format_RGB32 );
		QRgb *rgbPtr =
		  reinterpret_cast<QRgb *>( image.bits() ) + width * ( height - 1 );
		for ( int row = 0; row < height; row++ )
		{
		  for ( int col = 0; col < width; col++ )
		  {
			  short *ptr0 = static_cast<short*>(data->GetScalarPointer(
							  col,row,z));
			  int realv = (*ptr0-range[0])/(range[1]-range[0])*255;
			*( rgbPtr++ ) = QColor( realv, realv, realv).rgb();
		  }
		  rgbPtr -= width * 2;
		}
		QString fileName = zslicepath + QString::number(z) + ".png";
		image.save(fileName);
	}
	qDebug() << "write z end";
	width=dim[0],height=dim[2];
	for(int y = 0;y<dim[1];y++){
		QImage image( width, height, QImage::Format_RGB32 );
		QRgb *rgbPtr =
		  reinterpret_cast<QRgb *>( image.bits() ) + width * ( height - 1 );
		for ( int row = 0; row < height; row++ )
		{
		  for ( int col = 0; col < width; col++ )
		  {
			  short *ptr0 = static_cast<short*>(data->GetScalarPointer(
							  col,y,row));
			  int realv = (*ptr0-range[0])/(range[1]-range[0])*255;
			*( rgbPtr++ ) = QColor( realv, realv, realv).rgb();
		  }
		  rgbPtr -= width * 2;
		}
		QString fileName = yslicepath + QString::number(y) + ".png";
		image.save(fileName);
	}
	qDebug() << "write y end";
	width=dim[1],height=dim[2];
	for(int x = 0;x<dim[0];x++){
		QImage image( width, height, QImage::Format_RGB32 );
		QRgb *rgbPtr =
		  reinterpret_cast<QRgb *>( image.bits() ) + width * ( height - 1 );
		for ( int row = 0; row < height; row++ )
		{
		  for ( int col = 0; col < width; col++ )
		  {
			  short *ptr0 = static_cast<short*>(data->GetScalarPointer(
							  x,col,row));
			  int realv = (*ptr0-range[0])/(range[1]-range[0])*255;
			*( rgbPtr++ ) = QColor( realv, realv, realv).rgb();
		  }
		  rgbPtr -= width * 2;
		}
		QString fileName = xslicepath + QString::number(x) + ".png";
		image.save(fileName);
	}
	qDebug() << "write x end";
}

void MyFunc::VolumeThresholding(vtkImageData *data, double minV,
							double maxV, vtkImageData *resData, bool saveData)
{
	if(!saveData){
		vtkImageThreshold* imageThreshold =vtkImageThreshold::New();
		imageThreshold->SetInputData(data);
		imageThreshold->ThresholdBetween(minV, maxV);
		imageThreshold->ReplaceInOn();
		imageThreshold->SetInValue(2048);
		imageThreshold->SetOutValue(-2048);
		imageThreshold->Update();
		resData->DeepCopy(imageThreshold->GetOutput());
		imageThreshold->Delete();
	}else{
		resData->DeepCopy(data);
		vtkDataArray* dataarr =resData->GetPointData()->GetScalars();
		int nums = dataarr->GetNumberOfTuples();
		for(int i=0;i<nums;i++){
			double value = dataarr->GetComponent(i,0);
			if(value < minV || value > maxV){
				dataarr->SetComponent(i,0,-2048);
			}
		}
	}
	qDebug() << "end";
}

bool MyFunc::GetPos3DBy2D_1(vtkRenderer *render, int *pos, double pos3d[])
{
	vtkSmartPointer<vtkCellPicker> picker =
	  vtkSmartPointer<vtkCellPicker>::New();
	picker->SetTolerance(0.0005);
	bool isPicked = picker->Pick(pos[0], pos[1], 0, render);
	if(isPicked){
		double* temppos = picker->GetPickPosition();
		pos3d[0]=temppos[0];pos3d[1]=temppos[1];pos3d[2]=temppos[2];
	}else{
		return false;
	}
	return true;
}

bool MyFunc::GetPos2DBy3D(vtkRenderer *render, double pos3d[], int pos2d[])
{
	render->SetWorldPoint(pos3d[0],pos3d[1],pos3d[2],1);
	render->WorldToDisplay();
	double *displayCoord = render->GetDisplayPoint();
	pos2d[0] = (int)displayCoord[0];
	pos2d[1] = (int)displayCoord[1];
}

void MyFunc::CreatePointActor(vtkPoints *points, vtkActor *pActor,
							   double color[], float pSize, double opacity)
{
	vtkSmartPointer<vtkPolyData> polydata =
	  vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(points);

	vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
		vtkSmartPointer<vtkVertexGlyphFilter>::New();
	glyphFilter->SetInputData(polydata);
	glyphFilter->Update();
	vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(glyphFilter->GetOutput());
	pActor->SetMapper(mapper);
	pActor->GetProperty()->SetPointSize(pSize);
	pActor->GetProperty()->SetColor(color);
	pActor->GetProperty()->SetOpacity(opacity);
}

int MyFunc::GetCrossVec(double vec1[], double vec2[], double &crossvalue)
{
	double temp = vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
	crossvalue = temp;
	if(temp > 0){
		return 1;
	}else if(temp < 0){
		return -1;
	}else{
		return 0;
	}
}

void MyFunc::CreateLineActor(vtkPoints *points, vtkActor *lactor,
							 double color[], int linewidth)
{
	vtkSmartPointer<vtkPolyLine> polyLine =
	  vtkSmartPointer<vtkPolyLine>::New();
	polyLine->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());
	for(unsigned int i = 0; i < points->GetNumberOfPoints(); i++)
	{
	  polyLine->GetPointIds()->SetId(i,i);
	}

	vtkSmartPointer<vtkCellArray> cells =
	  vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(polyLine);
	vtkSmartPointer<vtkPolyData> polyData =
	  vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetLines(cells);
	vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);

	lactor->SetMapper(mapper);
	lactor->GetProperty()->SetLineWidth(linewidth);
	lactor->GetProperty()->SetColor(color);
}

void MyFunc::ReadDicomData(vtkDICOMImageReader *reader, char *path,
						   int dim[], double spacing[], double range[],
						   vtkImageData *&imagedata, vtkDataArray *&scalars)
{
	reader->SetDirectoryName(path);
	reader->Update();
	reader->GetOutput()->GetDimensions(dim);
	reader->GetOutput()->GetSpacing(spacing);
	reader->GetOutput()->GetScalarRange(range);
	imagedata = reader->GetOutput();
	scalars = reader->GetOutput()->GetPointData()->GetScalars();
}

void MyFunc::ResampleData(vtkImageData *imagedata, float factor[])
{
	vtkSmartPointer<vtkImageResample> resamle =
			vtkSmartPointer<vtkImageResample> ::New();
	resamle->SetInputData(imagedata);
	resamle->SetAxisMagnificationFactor(0,factor[0]);//x轴抽样
	resamle->SetAxisMagnificationFactor(1,factor[1]);//y轴抽样
	resamle->SetAxisMagnificationFactor(2,factor[2]);//z轴抽样
	resamle->Update();
	imagedata = resamle->GetOutput();
}

void MyFunc::CreateVolume(vtkImageData *imagedata, int blendmode,
						  bool autosample, float sampledis, vtkRenderer *render)
{
	{
	/*
	 * 高斯平滑
	vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter =
	  vtkSmartPointer<vtkImageGaussianSmooth>::New();
	gaussianSmoothFilter->SetInputConnection(dicomReader->GetOutputPort());
	gaussianSmoothFilter->Update();*/
	/**
	 * 高清体渲染
	double spacing[3];
	dicomReader->GetOutput()->GetSpacing(spacing);
	dicomReader->GetOutput()->SetSpacing(spacing[0]*2,spacing[1]*2,spacing[2]*2);*/
	}
	vtkSmartPointer<vtkGPUVolumeRayCastMapper> mapper =
		vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
	mapper->SetInputData(imagedata);
	mapper->SetBlendMode(blendmode);
	if(autosample){
		mapper->SetAutoAdjustSampleDistances(1);
	}else{
		mapper->SetAutoAdjustSampleDistances(0);
		mapper->SetSampleDistance(sampledis);
	}

	/**
	 * 用面对体进行切割
	vtkSmartPointer<vtkPlane> plane =
	  vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(m_dim[0]*m_spacing[0]/4,m_dim[1]*m_spacing[1]/2,
											 m_dim[2]*m_spacing[2]/2);
	plane->SetNormal(1, 0, 0.0);
	vtkSmartPointer<vtkPlane> plane2 =
	  vtkSmartPointer<vtkPlane>::New();
	plane2->SetOrigin(3*m_dim[0]*m_spacing[0]/4,m_dim[1]*m_spacing[1]/2,
												m_dim[2]*m_spacing[2]/2);
	plane2->SetNormal(-1, 0, 0.0);

	vtkSmartPointer<vtkPlaneCollection> planes =
	  vtkSmartPointer<vtkPlaneCollection>::New();
	planes->AddItem(plane);
	planes->AddItem(plane2);
	mapper->SetClippingPlanes(planes);*/

	vtkSmartPointer<vtkColorTransferFunction> colorFun =
			vtkSmartPointer<vtkColorTransferFunction>::New();
	vtkSmartPointer<vtkPiecewiseFunction> opacityFun =
			vtkSmartPointer<vtkPiecewiseFunction>::New();
	vtkSmartPointer<vtkVolumeProperty> property =
			vtkSmartPointer<vtkVolumeProperty>::New();
	if(blendmode == 0){
		colorFun->AddRGBPoint( -3024, 0, 0, 0, 0.5, 0.0 );
		colorFun->AddRGBPoint( -16, 0.73, 0.25, 0.30, 0.49, .61 );
		colorFun->AddRGBPoint( 641, .90, .82, .56, .5, 0.0 );
		colorFun->AddRGBPoint( 3071, 1, 1, 1, .5, 0.0 );
		opacityFun->AddPoint(-3024, 0, 0.5, 0.0 );
		opacityFun->AddPoint(-16, 0, .49, .61 );
		opacityFun->AddPoint(641, .72, .5, 0.0 );
		opacityFun->AddPoint(3071, .71, 0.5, 0.0);
		property->SetIndependentComponents(true);
		property->SetColor( colorFun );
		property->SetScalarOpacity( opacityFun );
		property->SetInterpolationTypeToLinear();
		property->ShadeOn();
		property->SetAmbient(0.5);
		property->SetDiffuse(0.5);
		property->SetSpecular(0.5);
		property->SetSpecularPower(50.0);
	}
	if(blendmode == 1){
		colorFun->AddRGBPoint( 0, 0, 0, 0);
		colorFun->AddRGBPoint( 1023, 0.5, 0.5, 0.5);
		colorFun->AddRGBPoint( 1024, 1, 0, 0);
		colorFun->AddRGBPoint( 1025, 0.5, 0.5, 0.5 );
		colorFun->AddRGBPoint( 2048, 1, 1, 1 );
		opacityFun->AddPoint(0, 0 );
		opacityFun->AddPoint(2048, 1 );
		property->SetColor( colorFun );
		property->SetScalarOpacity( opacityFun );
		property->SetInterpolationTypeToLinear();
	}
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetProperty( property );
	volume->SetMapper( mapper );
	render->AddVolume( volume );
}

void MyFunc::CreateSurface(vtkImageData *imagedata, double value,
						   double ocapity, double color[],
						   vtkRenderer *render, bool getLarge, int mode)
{
	vtkSmartPointer<vtkMarchingCubes> skinExtractor =
	  vtkSmartPointer<vtkMarchingCubes>::New();
	skinExtractor->SetInputData(imagedata);
	skinExtractor->SetValue(0, value);
	skinExtractor->SetComputeGradients(0);
	skinExtractor->SetComputeNormals(1);
	skinExtractor->SetComputeScalars(0);
	/*平滑处理
	vtkSmartPointer<vtkSmoothPolyDataFilter> smoother2 =
	  vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	smoother2->SetInputConnection(skinExtractor->GetOutputPort());
	smoother2->Update();*/
	/*设置迭代次数，角度等其他参数
	vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =
	  vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
	smoother->SetInputConnection(skinExtractor->GetOutputPort());
	smoother->SetNumberOfIterations(10);
	smoother->BoundarySmoothingOff();
	smoother->FeatureEdgeSmoothingOff();
	smoother->SetFeatureAngle(120);
	smoother->SetPassBand(0.05);
	smoother->NonManifoldSmoothingOn();
	smoother->NormalizeCoordinatesOn();
	smoother->Update();
	//三角化
	vtkSmartPointer<vtkStripper> skinStripper =
	  vtkSmartPointer<vtkStripper>::New();
	skinStripper->SetInputConnection(smoother->GetOutputPort());*/

	vtkSmartPointer<vtkCleanPolyData> cleanPolyData =
		vtkSmartPointer<vtkCleanPolyData>::New();
	cleanPolyData->SetInputConnection(skinExtractor->GetOutputPort());
	cleanPolyData->Update();

	vtkSmartPointer<vtkPolyDataMapper> skinMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	if(getLarge){
		//连通器,获取最大区域
		vtkSmartPointer<vtkPolyDataConnectivityFilter> confilter =
		  vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
		confilter->SetInputConnection(cleanPolyData->GetOutputPort());
		confilter->SetExtractionMode(mode);
		if(mode==5){//为5时显示所有区域,并对每个区域显示不同的颜色
			confilter->ColorRegionsOn();
		}
		confilter->Update();
		qDebug() << confilter->GetNumberOfExtractedRegions();
		skinMapper->SetInputConnection(confilter->GetOutputPort());
		if(mode == 5){
			skinMapper->SetScalarRange(confilter->GetOutput()->
						GetPointData()->GetArray("RegionId")->GetRange());
		}
		skinMapper->Update();
	}else{
		skinMapper->SetInputConnection(cleanPolyData->GetOutputPort());
		skinMapper->ScalarVisibilityOff();
	}

	vtkSmartPointer<vtkActor> skin = vtkSmartPointer<vtkActor>::New();
	skin->SetMapper(skinMapper);
	if(!getLarge){
		skin->GetProperty()->SetDiffuseColor(color);
		skin->GetProperty()->SetOpacity(ocapity);
	}
//	skin->GetProperty()->SetLighting(1);
	skin->GetProperty()->SetShading(1);
	skin->GetProperty()->SetDiffuse(0.8);
	skin->GetProperty()->SetAmbient(0.2);
	skin->GetProperty()->SetSpecular(0.2);
//	skin->GetProperty()->SetDiffuseColor(1,1,1);
//	skin->GetProperty()->SetAmbientColor(1,1,1);
//	skin->GetProperty()->SetSpecularColor(1,1,1);
//	skin->GetProperty()->SetSpecularPower(10);
//	skin->GetProperty()->EdgeVisibilityOn();
	skin->GetProperty()->SetInterpolationToPhong();
	render->AddActor(skin);
}

void MyFunc::CreateThreeSlice(int dim[], vtkRenderer *render,
							  vtkDICOMImageReader *reader)
{
	int dimx=dim[0]/2,dimy=dim[1]/2,dimz=dim[2]/2;
	vtkSmartPointer<vtkLookupTable> bwLut =
	  vtkSmartPointer<vtkLookupTable>::New();
	bwLut->SetTableRange (-2048, 2048);
	bwLut->SetSaturationRange (0.0, 1.0);
	bwLut->SetHueRange (0.5, 0.5);
	bwLut->SetValueRange (0.0, 1.0);
	bwLut->Build(); //effective built

	vtkSmartPointer<vtkImageMapToColors> sagittalColors =
	  vtkSmartPointer<vtkImageMapToColors>::New();
	sagittalColors->SetInputConnection(reader->GetOutputPort());
	sagittalColors->SetLookupTable(bwLut);
	sagittalColors->Update();

	vtkSmartPointer<vtkImageActor> sagittal =
	  vtkSmartPointer<vtkImageActor>::New();
	sagittal->GetMapper()->SetInputConnection(sagittalColors->GetOutputPort());
	sagittal->SetDisplayExtent(dimx, dimx, 0,dim[1], 0,dim[2]);

	vtkSmartPointer<vtkImageMapToColors> axialColors =
	  vtkSmartPointer<vtkImageMapToColors>::New();
	axialColors->SetInputConnection(reader->GetOutputPort());
	axialColors->SetLookupTable(bwLut);
	axialColors->Update();

	vtkSmartPointer<vtkImageActor> axial =
	  vtkSmartPointer<vtkImageActor>::New();
	axial->GetMapper()->SetInputConnection(axialColors->GetOutputPort());
	axial->SetDisplayExtent(0,dim[0], 0,dim[1], dimz,dimz);

	vtkSmartPointer<vtkImageMapToColors> coronalColors =
	  vtkSmartPointer<vtkImageMapToColors>::New();
	coronalColors->SetInputConnection(reader->GetOutputPort());
	coronalColors->SetLookupTable(bwLut);
	coronalColors->Update();

	vtkSmartPointer<vtkImageActor> coronal =
	  vtkSmartPointer<vtkImageActor>::New();
	coronal->GetMapper()->SetInputConnection(coronalColors->GetOutputPort());
	coronal->SetDisplayExtent(0,dim[0], dimy,dimy, 0,dim[2]);

	render->AddActor(sagittal);
	render->AddActor(axial);
	render->AddActor(coronal);
}

vtkSmartPointer<vtkPolyData> MyFunc::ReadPolyData(const char *fileName)
{
	vtkSmartPointer<vtkPolyData> polyData;
	std::string extension =
			vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
	if (extension == ".ply")
	{
	  vtkSmartPointer<vtkPLYReader> reader =
		vtkSmartPointer<vtkPLYReader>::New();
	  reader->SetFileName (fileName);
	  reader->Update();
	  polyData = reader->GetOutput();
	}
	else if (extension == ".vtp")
	{
	  vtkSmartPointer<vtkXMLPolyDataReader> reader =
		vtkSmartPointer<vtkXMLPolyDataReader>::New();
	  reader->SetFileName (fileName);
	  reader->Update();
	  polyData = reader->GetOutput();
	}
	else if (extension == ".obj")
	{
	  vtkSmartPointer<vtkOBJReader> reader =
		vtkSmartPointer<vtkOBJReader>::New();
	  reader->SetFileName (fileName);
	  reader->Update();
	  polyData = reader->GetOutput();
	}
	else if (extension == ".stl")
	{
	  vtkSmartPointer<vtkSTLReader> reader =
		vtkSmartPointer<vtkSTLReader>::New();
	  reader->SetFileName (fileName);
	  reader->Update();
	  polyData = reader->GetOutput();
	}
	else if (extension == ".vtk")
	{
	  vtkSmartPointer<vtkPolyDataReader> reader =
		vtkSmartPointer<vtkPolyDataReader>::New();
	  reader->SetFileName (fileName);
	  reader->Update();
	  polyData = reader->GetOutput();
	}
	else if (extension == ".g")
	{
	  vtkSmartPointer<vtkBYUReader> reader =
		vtkSmartPointer<vtkBYUReader>::New();
	  reader->SetGeometryFileName (fileName);
	  reader->Update();
	  polyData = reader->GetOutput();
	}
	else
	{
	  vtkSmartPointer<vtkSphereSource> source =
		vtkSmartPointer<vtkSphereSource>::New();
	  source->Update();
	  polyData = source->GetOutput();
	}
	return polyData;
}

void MyFunc::CreateClipFrustum(vtkRenderer *renderer, const char *fileName)
{
	vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(fileName);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);
	mapper->ScalarVisibilityOff();

	vtkSmartPointer<vtkActor> actor =
	  vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	double difcolor[3] = {0,0,0};//相交部分的颜色
	actor->GetProperty()->SetDiffuseColor(difcolor);
	actor->GetProperty()->SetSpecular(.6);
	actor->GetProperty()->SetSpecularPower(30);
	renderer->AddActor(actor);

	vtkSmartPointer<vtkPolyDataMapper> outMapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	outMapper->SetInputData(polyData);
	outMapper->ScalarVisibilityOff();

	vtkSmartPointer<vtkActor> outActor =
	  vtkSmartPointer<vtkActor>::New();
	outActor->SetMapper(outMapper);
	double outdifcolor[3] = {1,1,0};//不相交部分的颜色
	outActor->GetProperty()->SetDiffuseColor(outdifcolor);
	outActor->GetProperty()->SetSpecular(.6);
	outActor->GetProperty()->SetSpecularPower(30);

	double viewUp[3] = {0.0,1.0,0.0};
	double position[3] = {1.0,0.0,0.0};
	renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
	renderer->GetActiveCamera()->SetViewUp(viewUp);
	renderer->GetActiveCamera()->SetPosition(position);
	renderer->ResetCamera();
	renderer->GetActiveCamera()->SetParallelScale(40);
	double planesArray[24];
	renderer->GetActiveCamera()->GetFrustumPlanes(1.0, planesArray);

	vtkSmartPointer<vtkPlanes> planes =
	  vtkSmartPointer<vtkPlanes>::New();
	planes->SetFrustumPlanes(planesArray);

	vtkSmartPointer<vtkFrustumSource> frustumSource =
	  vtkSmartPointer<vtkFrustumSource>::New();
	frustumSource->ShowLinesOff();
	frustumSource->SetPlanes(planes);
	frustumSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> frustumMapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	frustumMapper->SetInputConnection(frustumSource->GetOutputPort());

	vtkSmartPointer<vtkActor> frustumActor =
	  vtkSmartPointer<vtkActor>::New();
	frustumActor->SetMapper(frustumMapper);
	frustumActor->GetProperty()->EdgeVisibilityOn();
	frustumActor->GetProperty()->SetOpacity(.5);
	double fruscolor[3] = {0,0,1};//切割面颜色
	frustumActor->GetProperty()->SetColor(fruscolor);

	vtkSmartPointer<vtkClipPolyData> clip =
	  vtkSmartPointer<vtkClipPolyData>::New();
	clip->SetInputData(polyData);
	clip->SetClipFunction(planes);
	clip->InsideOutOn();
	clip->GenerateClippedOutputOn();
	clip->Update();

	mapper->SetInputConnection(clip->GetOutputPort());
	outMapper->SetInputData(clip->GetClippedOutput());
	renderer->AddActor(frustumActor);
	renderer->AddActor(outActor);
	renderer->ResetCamera();
	renderer->ResetCameraClippingRange();
}

void MyFunc::ShowSeriesDicom(vtkDICOMImageReader *reader)
{
	// Visualize
	/*vtkSmartPointer<vtkImageViewer2> imageViewer =
	   vtkSmartPointer<vtkImageViewer2>::New();
	imageViewer->SetInputData(reader->GetOutput());
	int dim[3];
	double spacing[3];
	reader->GetOutput()->GetDimensions(dim);
	reader->GetOutput()->GetSpacing(spacing);
	imageViewer->SetSize(dim[0],dim[1]);
	// slice status message
	vtkSmartPointer<vtkTextProperty> sliceTextProp =
			vtkSmartPointer<vtkTextProperty>::New();
	sliceTextProp->SetFontFamilyToCourier();
	sliceTextProp->SetFontSize(20);
	sliceTextProp->SetVerticalJustificationToBottom();
	sliceTextProp->SetJustificationToLeft();

	vtkSmartPointer<vtkTextMapper> sliceTextMapper =
			vtkSmartPointer<vtkTextMapper>::New();
	std::string msg = StatusMessage::Format(imageViewer->GetSliceMax(),
											imageViewer->GetSliceMax());
	sliceTextMapper->SetInput(msg.c_str());
	sliceTextMapper->SetTextProperty(sliceTextProp);

	vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
	sliceTextActor->SetMapper(sliceTextMapper);
	sliceTextActor->SetPosition(15, 10);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	   vtkSmartPointer<vtkRenderWindowInteractor>::New();

	vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle =
	   vtkSmartPointer<myVtkInteractorStyleImage>::New();
	myInteractorStyle->SetImageViewer(imageViewer);
	myInteractorStyle->SetStatusMapper(sliceTextMapper);
	myInteractorStyle->m_imageData = reader->GetOutput();

	imageViewer->SetSlice(imageViewer->GetSliceMax());
	imageViewer->SetupInteractor(renderWindowInteractor);
	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);
	imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
	imageViewer->Render();

	imageViewer->GetRenderer()->ResetCamera();
	/*vtkCamera* camera = imageViewer->GetRenderer()->GetActiveCamera();
	double dataCenterPos[3]={(dim[0]-1)*spacing[0]/2,
							 (dim[1]-1)*spacing[1]/2,
							 (dim[2]-1)*spacing[2]/2};
	double viewup[3] = {0,1,0};
	camera->SetParallelProjection(1);
	camera->SetPosition(dataCenterPos[0],dataCenterPos[1],dataCenterPos[2]+500);
	camera->SetFocalPoint(dataCenterPos[0],dataCenterPos[1],dataCenterPos[2]);
	camera->SetViewUp(viewup);
	camera->SetParallelScale(dataCenterPos[1]);
	camera->SetClippingRange(0,10000);
	camera->Modified();*/
	/*imageViewer->Render();
	renderWindowInteractor->Start();*/
}

void MyFunc::VolumeSeedGrowth(int startDim[], vtkImageData *imagedata,
							  int threshold)
{
	int numchangepos = 0;
	int replaceValue = 1024;

	int nScrValue=0;//生长起始点的灰度值
	int nCurValue=0;//当前生长点的灰度值
	/*int DIR[26][3]={{-1,-1,0}, {0,-1,0}, {1,-1,0}, {1,0,0},
					{1,1,0}, {0,1,0}, {-1,1,0}, {-1,0,0},
		   {0,0,1}, {-1,-1,1}, {0,-1,1}, {1,-1,1}, {1,0,1},
					{1,1,1}, {0,1,1}, {-1,1,1}, {-1,0,1},
	{0,0,-1}, {-1,-1,-1}, {0,-1,-1}, {1,-1,-1}, {1,0,-1},
					{1,1,-1}, {0,1,-1}, {-1,1,-1}, {-1,0,-1}};*/
	int DIR[6][3]={{0,-1,0}, {1,0,0}, {0,1,0},
					{-1,0,0}, {0,0,1}, {0,0,-1}};
	QVector<QVector3D> vcGrowpt;//生长点的堆栈
	//将初始生长点压入堆栈
	vcGrowpt.push_back(QVector3D(startDim[0],startDim[1],startDim[2]));
	//标记初始生长点
	short *ptr0 = static_cast<short*>(imagedata->GetScalarPointer(
									startDim[0],startDim[1],startDim[2]));
	nScrValue = (int)(*ptr0);
	*ptr0 = replaceValue;
	qDebug() << "nScrValue:"<<nScrValue;
	double spacing[3];
	int dim[3];
	imagedata->GetDimensions(dim);
	imagedata->GetSpacing(spacing);
	QVector3D ptGrowing;//待生长点的位置
	while(!vcGrowpt.empty())
	{
		QVector3D curpt=vcGrowpt.back();//在堆栈中取出一个生长点
		vcGrowpt.pop_back();
		for(int i=0;i<6;i++)
		{
			ptGrowing.setX(curpt.x()+DIR[i][0]);
			ptGrowing.setY(curpt.y()+DIR[i][1]);
			ptGrowing.setZ(curpt.z()+DIR[i][2]);
			if(ptGrowing.x()<0 || ptGrowing.y()<0 ||
					ptGrowing.z()<0 || (ptGrowing.x()>=dim[0]) ||
					(ptGrowing.y()>=dim[1]) || ptGrowing.z()>=dim[2]){
				continue;
			}
			short *ptr = static_cast<short*>(imagedata->GetScalarPointer(
								ptGrowing.x(),ptGrowing.y(),ptGrowing.z()));
			nCurValue=*ptr;
			if (nCurValue!=replaceValue)//表示还未标记过
			{
				if(abs(nCurValue-nScrValue)<=threshold)
				{
					numchangepos++;
					*ptr = replaceValue;
					vcGrowpt.push_back(ptGrowing);
				}
			}
		}
	}
	qDebug() << "end!!" << numchangepos;
}
