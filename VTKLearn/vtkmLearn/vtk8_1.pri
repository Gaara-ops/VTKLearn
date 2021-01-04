
#vtkm--release
VTK_M_DIR = F:/vtk-m/vtkm1.1.0-lib-5.3-release
#freeglut
FreeGLUT_DIR = F:/opengl/freeglut-3.0.0/qt5.3/lib-release
#glew
GLEW_DIR = F:/opengl/glew-2.1.0/qt5.3/lib-release
#TBB
TBB_DIR = F:/vtk-m/tbb-2018_U5


#vtkm
INCLUDEPATH += $${VTK_M_DIR}/include/vtkm-1.1
#freeglut
INCLUDEPATH += $${FreeGLUT_DIR}/include
#glew
INCLUDEPATH += $${GLEW_DIR}/include
#tbb
INCLUDEPATH += $${TBB_DIR}/include

#tbb
LIBS += -L$${TBB_DIR}/build/windows_ia32_gcc_mingw_release/ -ltbb
#vtkm
LIBS += -L$${VTK_M_DIR}/lib/ libvtkm_cont-1.1 libvtkm_rendering-1.1
#glew
LIBS += -L$${GLEW_DIR}/lib/ libglew32
#freeglut
LIBS += -L$${FreeGLUT_DIR}/lib libfreeglut
LIBS += -lopengl32 -lwinmm -lgdi32 -lglu32
