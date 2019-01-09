#ifndef VECONTROL_H
#define VECONTROL_H
#include "../Globe/GlobeInclude.h"
class VEControl
{
public:
	VEControl();
	~VEControl();
	vtkRenderer* m_render;
	vtkImageData* m_oriData;
	void ReadPointData(QString path);
	void UpdateFocusSphere(double center[]);
	void UpdateViewFocus();

	void ResampleImageData(vtkImageData* oridata,vtkImageData* resdata,
						   double resample=2);
	void CreateCurveLine();
private:
	vtkSphereSource* m_sphere;
	vtkParametricFunctionSource* m_curveLine;
	int m_posindex;
	QVector<QVector3D> m_posVec;
};

#endif // VECONTROL_H
