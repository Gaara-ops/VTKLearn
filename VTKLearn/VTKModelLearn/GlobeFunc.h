#ifndef GLOBEFUNC_H
#define GLOBEFUNC_H
#include"GlobeInclude.h"

//2d转3d点--到物体表面
bool GetPos3DBy2D_1(vtkRenderer* render,int* pos,double pos3d[3]){
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
//向量x乘
int GetCrossVec(double vec1[3],double vec2[3],double& crossvalue){
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
//创建点接口---点集
void CreatePointActor(vtkPoints* points,vtkActor* pActor,double color[3],
                        float pSize=3,double opacity=0.2){
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

//创建线接口
void CreateLineActor(vtkPoints* points,vtkActor* lactor,double color[3],int linewidth=1){
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

//读取dicom文件
void ReadDicomData(vtkDICOMImageReader* reader,char* path,
                   int dim[3],double spacing[3],double range[2],
                    vtkImageData*& imagedata,vtkDataArray*& scalars){
    reader->SetDirectoryName(path);
    reader->Update();
    reader->GetOutput()->GetDimensions(dim);
    reader->GetOutput()->GetSpacing(spacing);
    reader->GetOutput()->GetScalarRange(range);
    imagedata = reader->GetOutput();
    scalars = reader->GetOutput()->GetPointData()->GetScalars();
}
//对数据进行抽样
void ResampleData(vtkImageData* imagedata,float factor[3]){
    vtkSmartPointer<vtkImageResample> resamle =
            vtkSmartPointer<vtkImageResample> ::New();
    resamle->SetInputData(imagedata);
    resamle->SetAxisMagnificationFactor(0,factor[0]);//x轴抽样0.5
    resamle->SetAxisMagnificationFactor(1,factor[1]);//y轴抽样0.5
    resamle->SetAxisMagnificationFactor(2,factor[2]);//z轴抽样0.5
    resamle->Update();
    imagedata = resamle->GetOutput();
}
//生成体
void CreateVolume(vtkImageData* imagedata,int blendmode,bool autosample,
                  float sampledis,vtkRenderer* render){

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
    plane->SetOrigin(m_dim[0]*m_spacing[0]/4,m_dim[1]*m_spacing[1]/2,m_dim[2]*m_spacing[2]/2);
    plane->SetNormal(1, 0, 0.0);
    vtkSmartPointer<vtkPlane> plane2 =
      vtkSmartPointer<vtkPlane>::New();
    plane2->SetOrigin(3*m_dim[0]*m_spacing[0]/4,m_dim[1]*m_spacing[1]/2,m_dim[2]*m_spacing[2]/2);
    plane2->SetNormal(-1, 0, 0.0);

    vtkSmartPointer<vtkPlaneCollection> planes =
      vtkSmartPointer<vtkPlaneCollection>::New();
    planes->AddItem(plane);
    planes->AddItem(plane2);
    mapper->SetClippingPlanes(planes);*/

    vtkColorTransferFunction *colorFun = vtkColorTransferFunction::New();
    vtkPiecewiseFunction *opacityFun = vtkPiecewiseFunction::New();
    vtkVolumeProperty *property = vtkVolumeProperty::New();
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
        colorFun->AddRGBPoint( 100, 1, 1, 1);
        colorFun->AddRGBPoint( 400, 1, 1, 1 );
        colorFun->ClampingOn();
        opacityFun->AddPoint(100, 1 );
        opacityFun->AddPoint(400, 1 );
        opacityFun->ClampingOn();
    }

    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetProperty( property );
    volume->SetMapper( mapper );
    render->AddVolume( volume );
}
//生成面
void CreateSurface(vtkImageData* imagedata,double value,double ocapity,
                   double color[3],vtkRenderer* render,
                   bool getLarge=false,int mode=4){
    vtkSmartPointer<vtkMarchingCubes> skinExtractor =
      vtkSmartPointer<vtkMarchingCubes>::New();
    skinExtractor->SetInputData(imagedata);
    skinExtractor->SetValue(0, value);
    skinExtractor->SetComputeGradients(0);
    skinExtractor->SetComputeNormals(1);
    skinExtractor->SetComputeScalars(0);

//    vtkSmartPointer<vtkSmoothPolyDataFilter> smoother2 =
//      vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
//    smoother2->SetInputConnection(skinExtractor->GetOutputPort());
//    smoother2->Update();

//    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =
//      vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
//    smoother->SetInputConnection(skinExtractor->GetOutputPort());
//    smoother->SetNumberOfIterations(10);
//    smoother->BoundarySmoothingOff();
//    smoother->FeatureEdgeSmoothingOff();
//    smoother->SetFeatureAngle(120);
//    smoother->SetPassBand(0.05);
//    smoother->NonManifoldSmoothingOn();
//    smoother->NormalizeCoordinatesOn();
//    smoother->Update();

//    vtkSmartPointer<vtkStripper> skinStripper =
//      vtkSmartPointer<vtkStripper>::New();
//    skinStripper->SetInputConnection(smoother->GetOutputPort());


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
        skinMapper->SetScalarRange(confilter->GetOutput()->
                                   GetPointData()->GetArray("RegionId")->GetRange());
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
    /*skin->GetProperty()->SetDiffuseColor(g_colors->GetColor3d("SkinColor").GetData());
    skin->GetProperty()->SetSpecular(.2);
    skin->GetProperty()->SetSpecularPower(20);
    skin->GetProperty()->EdgeVisibilityOn();
    skin->GetProperty()->SetInterpolationToFlat();*/
    render->AddActor(skin);
}
//生成三个切片
void CreateThreeSlice(int dim[3],vtkRenderer* render,
						vtkDICOMImageReader* reader){
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
//读取文件
vtkSmartPointer<vtkPolyData> ReadPolyData(const char *fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension = vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
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
//使用Frustum切割
void CreateClipFrustum(vtkRenderer* renderer,const char *fileName){
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
    double bgcolor[3] = {1,1,1};
    renderer->SetBackground(bgcolor);
    renderer->AddActor(frustumActor);
    renderer->AddActor(outActor);
    renderer->ResetCamera();
    renderer->ResetCameraClippingRange();
}

#endif // GLOBEFUNC_H
