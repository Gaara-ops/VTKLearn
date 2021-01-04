#include "mainwindow.h"
#include <QApplication>
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)

#include "../Globe/GlobeInclude.h"

#include "tbb/tbb.h"

#include <vtkSmartPointer.h>
#include <vtkAssembly.h>

#include <vtkTransform.h>
#include <vtkCubeSource.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPropAssembly.h>

#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkNamedColors.h>

#include <vtkCullerCollection.h>
#include <vtkFrustumCoverageCuller.h>
#include <vtkCuller.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    vtkSmartPointer<vtkNamedColors> namedColors =
      vtkSmartPointer<vtkNamedColors>::New();

    // Create a sphere
    vtkSmartPointer<vtkSphereSource> sphereSource =
      vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkSmartPointer<vtkActor> sphereActor =
      vtkSmartPointer<vtkActor>::New();
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Banana").GetData());


    // Create a cube
    vtkSmartPointer<vtkCubeSource> cubeSource =
      vtkSmartPointer<vtkCubeSource>::New();
    cubeSource->SetCenter(5.0, 0.0, 0.0);
    cubeSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkSmartPointer<vtkActor> cubeActor =
      vtkSmartPointer<vtkActor>::New();
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(
      namedColors->GetColor3d("Tomato").GetData());
    // Combine the sphere and cube into an assembly
    vtkSmartPointer<vtkAssembly> assembly =
      vtkSmartPointer<vtkAssembly>::New();
    assembly->AddPart(sphereActor);
    assembly->AddPart(cubeActor);

    // Apply a transform to the whole assembly
    vtkSmartPointer<vtkTransform> transform =
      vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply(); //this is the key line
    transform->Translate(5.0, 0, 0);

    assembly->SetUserTransform(transform);

    // Extract each actor from the assembly and change its opacity
    vtkSmartPointer<vtkPropCollection> collection =
      vtkSmartPointer<vtkPropCollection>::New();

    assembly->GetActors(collection);
    collection->InitTraversal();
//    for(vtkIdType i = 0; i < collection->GetNumberOfItems(); i++)
//    {
//      dynamic_cast<vtkActor*>(collection->GetNextProp())->GetProperty()->SetOpacity(0.5);
//    }

    // Visualization
    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(assembly);
    renderer->SetBackground(namedColors->GetColor3d("SlateGray").GetData());

//    qDebug() << renderer->GetCullers()->GetNumberOfItems();
//    vtkCuller* culler = renderer->GetCullers()->GetLastItem();
//    vtkFrustumCoverageCuller* realculler = static_cast<vtkFrustumCoverageCuller*>(culler);
//    realculler->SetSortingStyleToBackToFront();

    renderer->ResetCamera();
    renderWindow->Render();

    renderWindowInteractor->Start();

	return a.exec();
}

