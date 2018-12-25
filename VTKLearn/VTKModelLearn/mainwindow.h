#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class vtkRenderer;
class vtkLight;
class vtkDICOMImageReader;
class vtkActor;
class MouseInteractorStyleTest;
class vtkPolyDataMapper;
class vtkImageData;

class VolumeInfo;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	///所有测试接口
	//测试延中心线移动
	QVector<QVector3D> m_posVec;
	void testReadPoint();
	void testBtnResponse1();
	//测试三个切片和部分体,移动部分体
	void testCreateContext();
	void testBtnResponse2();
	///end
    //绘制之前删除render中的actor或prop
    void DeleteAllThing();
private slots:
	//测试按钮
	void on_pushButton_clicked();

    //弹出更改体信息面板
    void on_actionVoumeInfo_triggered();
    //生成体
    void on_actionVolume_triggered();
    //生成面
    void on_actionSurface_triggered();
    //提取面的最大联通区域
    void on_actionLargestRegion_triggered();
    //清除所有绘制的信息
    void on_actionClear_triggered();
    //生成最大密度投影体
    void on_actionMIP_triggered();
    //使用Frustum进行切割
	void on_ClipFrustum_triggered();
	//生成三维切片面
	void on_actionCreateThreeSlice_triggered();
	void on_actionDrawLine_triggered();
	void on_actionClearLine_triggered();

	void on_actionCannyEdge_triggered();

	void on_actionHistogram_triggered();

	void on_actionSliceShow_triggered();

private:
    void InitInfo();
    void InitCamera();
private:
    Ui::MainWindow *ui;
	VolumeInfo* m_volumeInfo;

    int m_dim[3];
    double m_spacing[3];
    vtkRenderer* renderer;
    vtkDICOMImageReader* dicomReader;
    vtkImageData* m_imageData;
    //灯光1
    vtkLight* light1;
	//dicom交互style,包含区域增长算法
	MouseInteractorStyleTest* m_mouseInter;
};

#endif // MAINWINDOW_H
