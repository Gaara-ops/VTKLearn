#ifndef GLOBEINCLUDE_H
#define GLOBEINCLUDE_H

//vtk
#include <vtkAxesActor.h>
#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkAbstractMapper3D.h>
#include <vtkAssemblyPath.h>
#include <vtkAppendPolyData.h>

#include <vtkBox.h>
#include <vtkBarChartActor.h>
#include <vtkBooleanOperationPolyDataFilter.h>

#include <vtkCamera.h>
#include <vtkCoordinate.h>
#include <vtkCellPicker.h>
#include <vtkClipVolume.h>
#include <vtkClipDataSet.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkCellCenters.h>
#include <vtkCleanPolyData.h>
#include <vtkCylinderSource.h>
#include <vtkClipPolyData.h>
#include <vtkClipClosedSurface.h>
#include <vtkCellLocator.h>
#include <vtkCutter.h>

#include <vtkDICOMImageReader.h>
#include <vtkDataSetMapper.h>
#include <vtkDistancePolyDataFilter.h>

#include <vtkExtractVOI.h>

#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkFrustumSource.h>

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkGlyph3DMapper.h>
#include <vtkGeometryFilter.h>

#include <vtkImageData.h>
#include <vtkImageShrink3D.h>
#include <vtkImageResample.h>
#include <vtkInformation.h>
#include <vtkImageMagnify.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkImageActor.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapToColors.h>
#include <vtkImageSlice.h>
#include <vtkImageViewer2.h>
#include <vtkImageThreshold.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageLuminance.h>
#include <vtkImageCast.h>
#include <vtkImageGradient.h>
#include <vtkImageNonMaximumSuppression.h>
#include <vtkImageConstantPad.h>
#include <vtkImageToStructuredPoints.h>
#include <vtkImageMagnitude.h>
#include <vtkImageAccumulate.h>
#include <vtkIntersectionPolyDataFilter.h>

#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkLookupTable.h>
#include <vtkLineSource.h>
#include <vtkLine.h>
#include <vtkLODProp3D.h>
#include <vtkLinkEdgels.h>
#include <vtkLegendBoxActor.h>

#include <vtkMarchingCubes.h>
#include <vtkMetaImageReader.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkMapper.h>
#include <vtkMetaImageWriter.h>

#include <vtkNamedColors.h>

#include <vtkOpenGLRenderWindow.h>
#include <vtkOutlineFilter.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>

#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPlanes.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPointData.h>
#include <vtkPolygon.h>
#include <vtkPoints.h>
#include <vtkPolyLine.h>
#include <vtkProp3DCollection.h>
#include <vtkPolyDataNormals.h>

#include <vtkQuadraticHexahedron.h>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkProperty2D.h>
#include <vtkPointHandleRepresentation2D.h>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkSmartPointer.h>
#include <vtkStripper.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtksys/SystemTools.hxx>
#include <vtkShortArray.h>
#include <vtkSubPixelPositionEdgels.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkScalarBarActor.h>

#include <vtkTransform.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkThreshold.h>

#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>

#include <vtkVolumeProperty.h>
#include <vtkVolume16Reader.h>
#include <vtkVertex.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkVoxel.h>


#include <vtkWindowedSincPolyDataFilter.h>

#include <QVTKWidget.h>

//文件读取
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>

//c++
#include <iostream>
#include <array>
#include <vector>
#include <math.h>
#include <synchapi.h>
#include <sstream>
#include <cstdlib>
#include <string>
#include <map>
#include <fstream>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <share.h>
#include <stdio.h>
#include <stdlib.h>

//qt
#include <QDebug>
#include <QTime>
#include <QWaitCondition>
#include <QVector>
#include <QMap>
#include <QList>
#include <QVector3D>
#include <QFile>
#include <QMouseEvent>
#include <QDir>
#include <QDesktopWidget>

//ctk
//#include <ctkCheckablePushButton.h>
//#include <ctkCollapsibleButton.h>
//#include <ctkColorPickerButton.h>
//#include <ctkRangeWidget.h>
//#include <ctkCollapsibleGroupBox.h>
//#include <ctkCoordinatesWidget.h>
//#include <ctkTreeComboBox.h>
//#include <ctkSearchBox.h>
//#include <ctkDirectoryButton.h>
//#include <ctkCheckableComboBox.h>
//#include <ctkMaterialPropertyWidget.h>
//#include <ctkMatrixWidget.h>
//#include <ctkPathLineEdit.h>

#endif // GLOBEINCLUDE_H
