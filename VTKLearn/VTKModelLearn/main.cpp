#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <sstream>
#include <iostream>
//#ifndef INITIAL_OPENGL
//#define INITIAL_OPENGL
#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)
//#endif
//#include "ctkwidgettest.h"
#include"../Globe/GlobeInclude.h"

#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader.h>
#include <vtkTexturedSphereSource.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow m;
    m.show();
    return a.exec();

    //F:/VTK-Example/VTKData/Data/earth.ppm

    vtkSmartPointer<vtkColorTransferFunction> colorFun2 =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    colorFun2->AddRGBPoint( -100, 0, 0, 0);
    colorFun2->AddRGBPoint( 100, 0.3, 0.3, 0.3);
    colorFun2->AddRGBPoint( 500, 0.6, 0.6, 0.6 );
    colorFun2->AddRGBPoint( 1000, 0.9, 0.9, 0.9 );
    colorFun2->Modified();

    int row=401,col=401;
    vtkSmartPointer<vtkImageData> imagedata =
      vtkSmartPointer<vtkImageData>::New();
    imagedata->SetDimensions(row,col,1);
    imagedata->SetSpacing(1.0,1.0,1.0);
    imagedata->AllocateScalars(VTK_SHORT,1);

    vtkDataArray* dataArr = imagedata->GetPointData()->GetScalars();

    QFile file("H:/SphereGrayTextureMat.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
         return -1;
    }
    QTextStream in(&file);
    int column = 0;
    while(!in.atEnd()){
        QString onecount = in.readLine();
        QStringList ctinfo = onecount.split(' ');
        for(int j = 0;j<ctinfo.size();j++){
            if(ctinfo.at(j).isEmpty() || ctinfo.at(j) == " "){
                continue;
            }
            int partindex = j*row + column;//j*row+column;
            dataArr->InsertComponent(partindex,0,ctinfo.at(j).toShort());
//            double rgbcolor[3] = {0};
//            colorFun2->GetColor(ctinfo.at(j).toShort(),rgbcolor);
//            dataArr->InsertComponent(partindex,0,rgbcolor[0]*255);
//            dataArr->InsertComponent(partindex,1,rgbcolor[1]*255);
//            dataArr->InsertComponent(partindex,2,rgbcolor[2]*255);
        }
        column++;
    }
    file.close();
    imagedata->Modified();
    qDebug() << "set data end!";

    // Read texture file
    vtkSmartPointer<vtkImageReader2Factory> readerFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> imageReader;
    imageReader.TakeReference(
      readerFactory->CreateImageReader2("F:/VTK-Example/VTKData/Data/earth.ppm"));
    imageReader->SetFileName("F:/VTK-Example/VTKData/Data/earth.ppm");

    // Create texture
    vtkSmartPointer<vtkTexture> texture =
      vtkSmartPointer<vtkTexture>::New();
    texture->SetInputConnection(imageReader->GetOutputPort());
//    texture->SetInputData(imagedata);
//    texture->SetLookupTable(colorFun2);
//    texture->RepeatOff();
//    texture->InterpolateOn();
//    texture->EdgeClampOff();

    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetCenter(0,0,0);
    sphereSource->SetRadius(1);
    sphereSource->SetThetaResolution(40);
    sphereSource->SetPhiResolution(40);
    sphereSource->Update();
    //
    vtkSmartPointer<vtkTextureMapToSphere> textureMapToSphere = vtkSmartPointer<vtkTextureMapToSphere>::New();
    textureMapToSphere->SetInputConnection(sphereSource->GetOutputPort());
    //Create mapper and set the mapped texture as input
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(textureMapToSphere->GetOutputPort());

    //Create actor and set the mapper and the texture
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->SetTexture(texture);

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer( renderer );

    vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renWinInteractor->SetRenderWindow( renderWindow );
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renWinInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renWinInteractor->Start();

    return 0;
}
