#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vtkVersion.h>
#include <vtkSmartPointer.h>

#include <vtkClipDataSet.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkConeSource.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <map>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkStripper.h>
#include <vtkFeatureEdges.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkXMLPolyDataReader.h>

#include <vtkSphereSource.h>

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
    /*createTestVolume();*/

    /*
    char* stlstr = "E:/workspace/Data/stl/dongmai.stl";
    m_stlReader=vtkSTLReader::New();
    m_stlReader->SetFileName(stlstr);
    m_stlReader->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(m_stlReader->GetOutputPort());

    m_oriActor =vtkActor::New();
    m_oriActor->SetMapper(mapper);
//    actor->GetProperty()->SetRepresentationToPoints();

    renderer = vtkRenderer::New();

    m_volumeStyle = MouseInteractorStyle::New();
    m_volumeStyle->SetDefaultRenderer(renderer);

    //renderer->AddActor(m_oriActor);
    renderer->SetBackground(.3, .6, .3); // Background color green
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidget->GetInteractor()->SetInteractorStyle(m_volumeStyle);
    ui->qvtkWidget->GetRenderWindow()->Render();
    renderer->GetActiveCamera()->SetParallelProjection(1);
    renderer->GetActiveCamera()->Modified();
    ui->qvtkWidget->GetRenderWindow()->Render();
    testPlane();
    */
}

MainWindow::~MainWindow()
{
	dicomReader->Delete();
	renderer->Delete();
    delete ui;
}
#include<vtkParametricRandomHills.h>
#include<vtkCurvatures.h>
#include<vtkImplicitBoolean.h>

void Clipper(vtkPolyData* src, double const& dx, double const& dy,
  double const& dz, vtkPolyData* clipped)
{
  double bounds[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  src->GetBounds(bounds);

  vtkSmartPointer<vtkPlane> plane1 =
    vtkSmartPointer<vtkPlane>::New();
  plane1->SetOrigin(bounds[0] + dx, 0, 0);
  plane1->SetNormal(1, 0, 0);

  vtkSmartPointer<vtkPlane> plane2 =
    vtkSmartPointer<vtkPlane>::New();
  plane2->SetOrigin(bounds[1] - dx, 0, 0);
  plane2->SetNormal(-1, 0, 0);

  vtkSmartPointer<vtkPlane> plane3 =
    vtkSmartPointer<vtkPlane>::New();
  plane3->SetOrigin(0, bounds[2] + dy, 0);
  plane3->SetNormal(0, 1, 0);

  vtkSmartPointer<vtkPlane> plane4 =
    vtkSmartPointer<vtkPlane>::New();
  plane4->SetOrigin(0, bounds[3] - dy, 0);
  plane4->SetNormal(0, -1, 0);

  vtkSmartPointer<vtkPlane> plane5 =
    vtkSmartPointer<vtkPlane>::New();
  plane5->SetOrigin(0, 0, bounds[4] + dz);
  plane5->SetNormal(0, 0, 1);

  vtkSmartPointer<vtkPlane> plane6 =
    vtkSmartPointer<vtkPlane>::New();
  plane6->SetOrigin(0, 0, bounds[5] - dz);
  plane6->SetNormal(0, 0, -1);

  vtkSmartPointer<vtkImplicitBoolean> clipFunction =
    vtkSmartPointer<vtkImplicitBoolean>::New();
  clipFunction->SetOperationTypeToUnion();
  clipFunction->AddFunction(plane1);
  clipFunction->AddFunction(plane2);
  clipFunction->AddFunction(plane3);
  clipFunction->AddFunction(plane4);
  clipFunction->AddFunction(plane5);
  clipFunction->AddFunction(plane6);

  // Create clipper for the random hills
  vtkSmartPointer<vtkClipPolyData> clipper =
    vtkSmartPointer<vtkClipPolyData>::New();
  clipper->SetClipFunction(clipFunction);
  clipper->SetInputData(src);
  clipper->GenerateClipScalarsOff();
  clipper->GenerateClippedOutputOff();
  // clipper->GenerateClippedOutputOn();
  clipper->Update();
  clipped->DeepCopy(clipper->GetOutput());
}
void MainWindow::testPlane()
{
    vtkSmartPointer<vtkParametricRandomHills> fn =
      vtkSmartPointer<vtkParametricRandomHills>::New();
    fn->AllowRandomGenerationOn();
    fn->SetRandomSeed(0.1);
    fn->SetHillAmplitude(0.1);
//    fn->SetHillXVariance(0.1);
//    fn->SetHillYVariance(0.1);
    fn->SetNumberOfHills(1);
    // Make the normals face out of the surface.
    // Not needed with VTK 8.0 or later.
    //if (strcmp(fn->GetClassName(), "vtkParametricRandomHills") == 0)
    {
     // fn->ClockwiseOrderingOff();
    }

    vtkSmartPointer<vtkParametricFunctionSource> source =
      vtkSmartPointer<vtkParametricFunctionSource>::New();
    source->SetParametricFunction(fn);
    source->SetUResolution(50);
    source->SetVResolution(50);
    source->SetScalarModeToZ();
    source->Update();
    // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource)
    source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
    // We have calculated the elevation, just rename the scalars.
    source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

    // Calculate the curvature.
    vtkSmartPointer<vtkCurvatures> curvature =
      vtkSmartPointer<vtkCurvatures>::New();
    curvature->SetCurvatureTypeToGaussian();
    curvature->SetInputData(source->GetOutput());
    curvature->Update();

    vtkSmartPointer<vtkPolyData> src =
      vtkSmartPointer<vtkPolyData>::New();
    //Clipper(curvature->GetOutput(), 0.5, 0.5, 0.0, src);


    vtkSmartPointer<vtkPolyDataMapper> srcMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    srcMapper->SetInputData(curvature->GetOutput());

    vtkSmartPointer<vtkActor> srcActor =
      vtkSmartPointer<vtkActor>::New();
    srcActor->SetMapper(srcMapper);

    renderer->AddActor(srcActor);
    renderer->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
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
#include<vtkImplicitSelectionLoop.h>
#include<vtkLODActor.h>

#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkNamedColors.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>

#include <vtkImplicitBoolean.h>
#include <vtkNamedColors.h>

#include <vtkSphere.h>
#include <vtkBox.h>
#include <vtkSampleFunction.h>
#include <vtkContourFilter.h>
void MainWindow::on_pushButton_clicked()
{
    vtkPoints* allpos = m_volumeStyle->allClickPos;

    //Show the base and extrusion vector

    vtkPoints* points2 = vtkPoints::New();
    points2->DeepCopy(allpos);
    points2->InsertNextPoint(allpos->GetPoint(0));
    int id = points2->GetNumberOfPoints();
    vtkSmartPointer<vtkPolyLine> line =
      vtkSmartPointer<vtkPolyLine>::New();
    line->GetPointIds()->SetNumberOfIds(id);
    for(unsigned int i = 0; i < static_cast<unsigned int>(id); ++i)
    {
      line->GetPointIds()->SetId(i,i);
    }

    vtkSmartPointer<vtkCellArray> lines =
      vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(line);

    vtkSmartPointer<vtkPolyData> polyData =
      vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points2);
    polyData->SetLines(lines);

    vtkSmartPointer<vtkLinearExtrusionFilter> extrude =
      vtkSmartPointer<vtkLinearExtrusionFilter>::New();
    extrude->SetInputData(polyData);
    extrude->SetExtrusionTypeToNormalExtrusion();
    double* nnor = renderer->GetActiveCamera()->GetDirectionOfProjection();
    extrude->SetVector(nnor[0]*200,nnor[1]*200,nnor[2]*200);
    extrude->Update();
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(extrude->GetOutputPort());
    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1,0,0);
    actor->GetProperty()->SetOpacity(.7);
    //end

    vtkSmartPointer<vtkImplicitSelectionLoop> loop =
      vtkSmartPointer<vtkImplicitSelectionLoop>::New();
    loop->SetLoop(allpos);
    loop->SetNormal(renderer->GetActiveCamera()->GetDirectionOfProjection());
    loop->SetAutomaticNormalGeneration(0);

    vtkSmartPointer<vtkClipPolyData> clip = //clips out positive region
      vtkSmartPointer<vtkClipPolyData>::New();
    clip->SetInputConnection(m_stlReader->GetOutputPort());
    clip->SetClipFunction(loop);
    clip->Update();
//    clip->SetValue(0.0);

    vtkSmartPointer<vtkPolyDataMapper> clipMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    clipMapper->SetInputConnection(clip->GetOutputPort());
    clipMapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkProperty> backProp =
      vtkSmartPointer<vtkProperty>::New();
    backProp->SetColor(1,1,1);

    vtkLODActor* clipActor =vtkLODActor::New();
    clipActor->SetMapper(clipMapper);
    clipActor->SetBackfaceProperty(backProp);
    clipActor->GetProperty()->SetColor(0,0,1);
    /*
    // Now extract feature edges
      vtkSmartPointer<vtkFeatureEdges> boundaryEdges =
        vtkSmartPointer<vtkFeatureEdges>::New();
      boundaryEdges->SetInputData(clip->GetOutput());
      boundaryEdges->BoundaryEdgesOn();
      boundaryEdges->FeatureEdgesOff();
      boundaryEdges->NonManifoldEdgesOff();
      boundaryEdges->ManifoldEdgesOff();

      vtkSmartPointer<vtkStripper> boundaryStrips =
        vtkSmartPointer<vtkStripper>::New();
      boundaryStrips->SetInputConnection(boundaryEdges->GetOutputPort());
      boundaryStrips->Update();

      // Change the polylines into polygons
      vtkSmartPointer<vtkPolyData> boundaryPoly =
        vtkSmartPointer<vtkPolyData>::New();
      boundaryPoly->SetPoints(boundaryStrips->GetOutput()->GetPoints());
      boundaryPoly->SetPolys(boundaryStrips->GetOutput()->GetLines());

      vtkSmartPointer<vtkPolyDataMapper> boundaryMapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
      boundaryMapper->SetInputData(boundaryPoly);

      vtkSmartPointer<vtkActor> boundaryActor =
        vtkSmartPointer<vtkActor>::New();
      boundaryActor->SetMapper(boundaryMapper);
      boundaryActor->GetProperty()->SetColor(0.8900,0.8100,0.3400);
      //end by gw*/


    renderer->RemoveActor(m_oriActor);
    renderer->Modified();
    ui->qvtkWidget->GetRenderWindow()->Render();

    renderer->AddActor(clipActor);
    //renderer->AddActor(boundaryActor);
    renderer->AddActor(actor);
    ui->qvtkWidget->GetRenderWindow()->Render();

    m_oriActor = clipActor;
    m_volumeStyle->bgetpos = false;
//    m_volumeStyle->allClickPos->Reset();
}

void MainWindow::on_getPosBtn_clicked()
{
    m_volumeStyle->bgetpos = true;
}
