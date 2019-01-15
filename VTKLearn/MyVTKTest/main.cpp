#include "mainwindow.h"
#include <QApplication>
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)

#include "../Globe/GlobeInclude.h"
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
//	MainWindow w;
//	w.show();
	vtkSmartPointer<vtkSphereSource> sphereSource =
	  vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->Update();

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
	  vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor =
	  vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	// A renderer and render window
	vtkSmartPointer<vtkRenderer> renderer =
	  vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
	  vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(1650,940);
	renderWindow->AddRenderer(renderer);
	renderer->AddActor(actor);

	renderer->SetViewport(0,0,540.0f/1650,889.0f/940);
	renderer->SetBackground(1,0,0);
	// An interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
	  vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
	  vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	renderWindow->Render();

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	renderWindowInteractor->Start();

	return a.exec();
}
