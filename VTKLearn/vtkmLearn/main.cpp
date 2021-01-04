#include <QCoreApplication>

//#ifndef INITIAL_OPENGL
//#define INITIAL_OPENGL
//#include "vtkAutoInit.h"
//VTK_MODULE_INIT(vtkRenderingOpenGL2)
//VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
//VTK_MODULE_INIT(vtkRenderingFreeType)
//VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
//#endif

/*vtkNew<vtkInteractorStyleTrackballCamera> style;
iren->SetInteractorStyle(style.GetPointer());*/

//着色器测试类
//#include "shadertest.h"
#include "GlobeInclude.h"


//We first check if VTKM_DEVICE_ADAPTER is defined, so that when TBB and CUDA
//includes this file we use the device adapter that they have set.
#ifndef VTKM_DEVICE_ADAPTER
#define VTKM_DEVICE_ADAPTER VTKM_DEVICE_ADAPTER_SERIAL
#endif

#include <vtkm/Math.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/testing/MakeTestDataSet.h>

//Suppress warnings about glut being deprecated on OSX
#if (defined(VTKM_GCC) || defined(VTKM_CLANG))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <vtkm/rendering/internal/OpenGLHeaders.h> //Required for compile....

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vtkm/rendering/CanvasGL.h>
#include <vtkm/rendering/ColorTable.h>
#include <vtkm/rendering/MapperGL.h>
#include <vtkm/rendering/View3D.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/CanvasRayTracer.h>

vtkm::rendering::View3D* view = nullptr;

const vtkm::Int32 W = 512, H = 512;
int buttonStates[3] = { GLUT_UP, GLUT_UP, GLUT_UP };
bool shiftKey = false;
int lastx = -1, lasty = -1;

void reshape(int, int)
{
  //Don't allow resizing window.
  glutReshapeWindow(W, H);
}

// Render the output using simple OpenGL
void displayCall()
{
  view->Paint();
  glutSwapBuffers();
}

// Allow rotations of the camera
void mouseMove(int x, int y)
{
  const vtkm::Id width = view->GetCanvas().GetWidth();
  const vtkm::Id height = view->GetCanvas().GetHeight();

  //Map to XY
  y = static_cast<int>(height - y);

  if (lastx != -1 && lasty != -1)
  {
    vtkm::Float32 x1 = vtkm::Float32(lastx * 2) / vtkm::Float32(width) - 1.0f;
    vtkm::Float32 y1 = vtkm::Float32(lasty * 2) / vtkm::Float32(height) - 1.0f;
    vtkm::Float32 x2 = vtkm::Float32(x * 2) / vtkm::Float32(width) - 1.0f;
    vtkm::Float32 y2 = vtkm::Float32(y * 2) / vtkm::Float32(height) - 1.0f;

    if (buttonStates[0] == GLUT_DOWN)
    {
      if (shiftKey)
        view->GetCamera().Pan(x2 - x1, y2 - y1);
      else
        view->GetCamera().TrackballRotate(x1, y1, x2, y2);
    }
    else if (buttonStates[1] == GLUT_DOWN)
      view->GetCamera().Zoom(y2 - y1);
  }

  lastx = x;
  lasty = y;
  glutPostRedisplay();
}

// Respond to mouse button
void mouseCall(int button, int state, int vtkmNotUsed(x), int vtkmNotUsed(y))
{
  int modifiers = glutGetModifiers();
  shiftKey = modifiers & GLUT_ACTIVE_SHIFT;
  buttonStates[button] = state;

  //std::cout<<"Buttons: "<<buttonStates[0]<<" "<<buttonStates[1]<<" "<<buttonStates[2]<<" SHIFT= "<<shiftKey<<std::endl;

  //mouse down, reset.
  if (buttonStates[button] == GLUT_DOWN)
  {
    lastx = -1;
    lasty = -1;
  }
}

// Compute and render an isosurface for a uniform grid example
#include <vtkm/cont/testing/Testing.h>
#include <vtkm/io/reader/VTKDataSetReader.h>

#include <string>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    /*
     * 着色器测试
    ShaderTest st;
    st.CreateShaderTest();*/

    //vtkm::cont::testing::MakeTestDataSet maker;
    //vtkm::cont::DataSet ds = maker.Make3DUniformDataSet0();

    vtkm::cont::DataSet ds;
    vtkm::io::reader::VTKDataSetReader reader("E:/workspace/Data/vtk/carotid.vtk");
    try
    {
      ds = reader.ReadDataSet();
    }
    catch (vtkm::io::ErrorIO& e)
    {
      std::string message("Error reading: ");
      message += ", ";
      message += e.GetMessage();

      VTKM_TEST_FAIL(message.c_str());
    }
    qDebug() << "num:"<<ds.GetNumberOfFields() << ds.GetNumberOfCoordinateSystems();
    std::string fieldname = ds.GetField(0).GetName() ;
    std::cout << "name:" << fieldname << "  " << ds.GetCoordinateSystem().GetName();


    lastx = lasty = -1;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutCreateWindow("VTK-m Rendering");
    glutDisplayFunc(displayCall);
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseCall);
    glutReshapeFunc(reshape);

    vtkm::rendering::Color bg(0.2f, 0.2f, 0.2f, 1.0f);
    vtkm::rendering::CanvasGL canvas;
    vtkm::rendering::MapperGL mapper;
    vtkm::rendering::ColorTable colortable;
    colortable.AddControlPoint(0,vtkm::rendering::Color(1,0,0),0.0);
    colortable.AddControlPoint(50,vtkm::rendering::Color(0,1,0),0.2);
    colortable.AddControlPoint(100,vtkm::rendering::Color(0,0,1),0.4);
    colortable.AddControlPoint(150,vtkm::rendering::Color(1,1,0),0.8);
    colortable.AddControlPoint(200,vtkm::rendering::Color(1,0,1),1.0);
    mapper.SetActiveColorTable(colortable);
    vtkm::rendering::CanvasRayTracer canvas1;
    vtkm::rendering::MapperRayTracer mapper1;

    vtkm::rendering::ColorTable colorTablegg = vtkm::rendering::ColorTable("default");

    vtkm::rendering::Scene scene;
    scene.AddActor(vtkm::rendering::Actor(ds.GetCellSet(),
                                          ds.GetCoordinateSystem(),
                                          ds.GetField(fieldname),colorTablegg));

    //Create vtkm rendering stuff.
    view = new vtkm::rendering::View3D(scene, mapper, canvas, bg);
    view->Initialize();
    glutMainLoop();

    return a.exec();
}
#if (defined(VTKM_GCC) || defined(VTKM_CLANG))
#pragma GCC diagnostic pop
#endif
