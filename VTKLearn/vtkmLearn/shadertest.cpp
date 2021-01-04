#include "shadertest.h"
#include "GlobeInclude.h"

ShaderTest::ShaderTest()
{

}

ShaderTest::~ShaderTest()
{

}

void ShaderTest::CreateShaderTest()
{
    vtkMetaImageReader *reader = vtkMetaImageReader::New();
    reader->SetFileName("E:/workspace/Data/raw/FullHead.mhd");
    reader->Update();

    vtkDataArray* arr = reader->GetOutput()->GetPointData()->GetScalars();
    double range[2];
    arr->GetRange(range);

    vtkImageData* im = reader->GetOutput();
    double * bounds = im->GetBounds();
    double depthRange[2];
    depthRange[0] = vtkMath::Min(bounds[0], bounds[2]);
    depthRange[0] = vtkMath::Min(depthRange[0], bounds[4]);
    depthRange[1] = vtkMath::Max(bounds[1], bounds[3]);
    depthRange[1] = vtkMath::Max(depthRange[1], bounds[5]);

    vtkNew<vtkVolumeProperty> volumeProperty;
    volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);


    vtkNew<vtkColorTransferFunction> ctf;
    ctf->AddRGBPoint(depthRange[0], 1.0, 0.0, 0.0);
    ctf->AddRGBPoint(0.5*(depthRange[0]+depthRange[1]), 0.5, 0.5, 0.5);
    ctf->AddRGBPoint(0.8*(depthRange[0]+depthRange[1]), 0.5, 0.4, 0.6);
    ctf->AddRGBPoint(depthRange[1], 0.0, 1.0, 1.0);
    /*
     * 1
    vtkNew<vtkColorTransferFunction> ctf;
    ctf->AddRGBPoint(0, 0.0, 0.0, 0.0);
    ctf->AddRGBPoint(510, 0.4, 0.4, 1.0);
    ctf->AddRGBPoint(640, 1.0, 1.0, 1.0);
    ctf->AddRGBPoint(range[1], 0.9, 0.1, 0.1);*/

    vtkNew<vtkPiecewiseFunction> pf;
    pf->AddPoint(0, 0.00);
    pf->AddPoint(510, 0.00);
    pf->AddPoint(640, 0.5);
    pf->AddPoint(range[1], 0.4);

    volumeProperty->SetScalarOpacity(pf.GetPointer());
    volumeProperty->SetColor(ctf.GetPointer());
    volumeProperty->SetShade(1);

    vtkNew<vtkOpenGLGPUVolumeRayCastMapper> mapper;
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->SetUseJittering(1);//1
//    mapper->SetUseJittering(0);//2

    // Modify the shader to color based on the depth of the translucent voxel
    mapper->AddShaderReplacement(
      vtkShader::Fragment,    // Replace in the fragment shader
      "//VTK::Base::Dec",     // Source string to replace
      true,                   // before the standard replacements
      "//VTK::Base::Dec"      // We still want the default
      "\n bool l_updateDepth;"
      "\n vec3 l_opaqueFragPos;",
      false                   // only do it once i.e. only replace the first match
    );
    mapper->AddShaderReplacement(
      vtkShader::Fragment,
      "//VTK::Base::Init",
      true,
      "//VTK::Base::Init\n"
      "\n l_updateDepth = true;"
      "\n l_opaqueFragPos = vec3(0.0);",
      false);
    mapper->AddShaderReplacement(
      vtkShader::Fragment,
      "//VTK::Base::Impl",
      true,
      "//VTK::Base::Impl"
      "\n    if(!g_skip && g_srcColor.a > 0.0 && l_updateDepth)"
      "\n      {"
      "\n      l_opaqueFragPos = g_dataPos;"
      "\n      l_updateDepth = false;"
      "\n      }",
      false);
    /*mapper->AddShaderReplacement(
      vtkShader::Fragment,
      "//VTK::RenderToImage::Exit",
      true,
      "//VTK::RenderToImage::Exit"
      "\n  if (l_opaqueFragPos == vec3(0.0))"
      "\n    {"
      "\n    fragOutput0 = vec4(0.0);"
      "\n    }"
      "\n  else"
      "\n    {"
      "\n    vec4 depthValue = in_projectionMatrix * in_modelViewMatrix *"
      "\n                      in_volumeMatrix[0] * in_textureDatasetMatrix[0] *"
      "\n                      vec4(l_opaqueFragPos, 1.0);"
      "\n    depthValue /= depthValue.w;"
      "\n    fragOutput0.rgb = opacity * vec3(ambientColor +  diffuse + specular);\n"
      "\n    fragOutput0.a = opacity;\n"
      "\n    }",
      false);*/
    mapper->AddShaderReplacement(
      vtkShader::Fragment,  // dummy replacement to test clear method
      "//VTK::ComputeGradient::Dec",
      true,
      "VTK::ComputeGradient::Dec",
      false);
    mapper->ClearShaderReplacement(
      vtkShader::Fragment, // clear the dummy replacement
      "//VTK::ComputeGradient::Dec",
      true);

    /*
     * 2
    mapper->SetColorRangeType(vtkGPUVolumeRayCastMapper::NATIVE);
    // Modify the shader to color based on the depth of the translucent voxel
    std::ifstream shaderFile("H:temp/mytest.glsl");
    std::ostringstream shaderCode;
    shaderCode << shaderFile.rdbuf();

    std::ifstream shaderFile2("H:temp/point.glsl");
    std::ostringstream shaderCode2;
    shaderCode2 << shaderFile2.rdbuf();
    mapper->SetVertexShaderCode(shaderCode2.str().c_str());

    mapper->SetFragmentShaderCode(shaderCode.str().c_str());*/

    vtkNew<vtkVolume> volume;
    volume->SetMapper(mapper.GetPointer());
    volume->SetProperty(volumeProperty.GetPointer());

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetMultiSamples(0);
    renWin->SetSize(1000, 800); // Intentional NPOT size

    vtkNew<vtkRenderer> ren;
    renWin->AddRenderer(ren.GetPointer());

    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style.GetPointer());

    iren->SetRenderWindow(renWin.GetPointer());

    ren->AddVolume(volume.GetPointer());
    ren->GetActiveCamera()->Elevation(-60.0);
    ren->ResetCamera();
    ren->GetActiveCamera()->Zoom(1.3);

    renWin->Render();
    iren->Start();
}
