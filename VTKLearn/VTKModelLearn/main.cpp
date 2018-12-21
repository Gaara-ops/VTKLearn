#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

//#ifndef INITIAL_OPENGL
//#define INITIAL_OPENGL
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
//#endif
//#include "ctkwidgettest.h"
#include"GlobeInclude.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	MainWindow m;
	m.show();
    return a.exec();
}
