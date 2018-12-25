#ifndef VOLUMEINFO_H
#define VOLUMEINFO_H

#include <QDialog>
class vtkLight;
class vtkRenderWindow;
class vtkRenderer;
class vtkImageData;
namespace Ui {
class VolumeInfo;
}

class VolumeInfo : public QDialog
{
    Q_OBJECT

public:
    explicit VolumeInfo(QWidget *parent = 0);
    ~VolumeInfo();
    vtkLight* light;
    vtkRenderWindow* vtkwindow;
    vtkRenderer* renderer;
	vtkImageData* oriImageData;

	void initPlaneInfo();
private slots:
    void on_LightUpdateBtn_clicked();

    void on_closeBtn_clicked();

    void on_ShadeUpdateBtn_clicked();

	void on_ThresholdBtn_clicked();

	void on_ClipPlaneBtn_clicked();

private:
    Ui::VolumeInfo *ui;
	vtkImageData* thresholdImage;
};

#endif // VOLUMEINFO_H
