#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Globe/myfunc.h"
#include "../Globe/GlobeInclude.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void createTestVolume();
private:
	Ui::MainWindow *ui;
	vtkDICOMImageReader* dicomReader;
	vtkImageData* m_oriImageData;
	int m_dim[3];
	double m_spacing[3];
	vtkRenderer* renderer;
};

#endif // MAINWINDOW_H
