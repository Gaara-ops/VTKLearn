#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Globe/myfunc.h"
#include "../Globe/GlobeInclude.h"

#include "MouseInteractorStyle.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
    void testPlane();
	void createTestVolume();
private slots:
    void on_pushButton_clicked();

    void on_getPosBtn_clicked();

private:
	Ui::MainWindow *ui;
	vtkDICOMImageReader* dicomReader;
	vtkImageData* m_oriImageData;
	int m_dim[3];
	double m_spacing[3];
	vtkRenderer* renderer;

    MouseInteractorStyle* m_volumeStyle;
    vtkSTLReader* m_stlReader;
    vtkActor* m_oriActor;
};

#endif // MAINWINDOW_H
