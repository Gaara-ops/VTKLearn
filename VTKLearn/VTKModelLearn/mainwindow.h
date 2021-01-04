#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MouseInteractorStyle.h"
#include "vecontrol.h"
#include "polydataope.h"
class vtkRenderer;
class vtkLight;
class vtkDICOMImageReader;
class vtkActor;
class MouseInteractorStyleTest;
class vtkPolyDataMapper;
class vtkImageData;
class HeartSeg;

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
    //绘制之前删除render中的actor或prop
    void DeleteAllThing();
	void initImagePartData();
	void SetImagePartDataValue(short ctvalue=-2048);
    //添加两个体，设置不同的透明度
    void AddTwoVolume();
private slots:
	void slotTimeOut();
    void TestCreateLine(QString allposstr, double color[]);
    void TestCreatePoint(double center[],double color[]);
    void TestDrawAdjustCenterLine();
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
	void on_actionSeedGrowth_triggered();
    //使用Frustum进行切割
	void on_ClipFrustum_triggered();
	//生成三维切片面
	void on_actionCreateThreeSlice_triggered();
	void on_actionDrawLine_triggered();
	void on_actionClearLine_triggered();
	void on_actionInterpolateVolume_triggered();
	void on_actionCreateThreeView_triggered();
	//显示一系列dicom文件，包含区域增长
	void on_actionSliceShow_triggered();
	//执行VE相关
	void on_actionCreateCurveLine_triggered();
	void on_actionNextView_triggered();
	void on_actionPlayView_triggered();
    void on_actionPolyDataWidget_triggered();

    void on_SwitchBtn_clicked();

private:
    void InitInfo();
    void InitCamera();
	void InitCameraPespective(int dim[], double spacing[]);
private:
    Ui::MainWindow *ui;
	QTimer* m_timer;
	MouseInteractorStyle* m_volumeStyle;
	VolumeInfo* m_volumeInfo;

    int m_dim[3];
    double m_spacing[3];
    vtkRenderer* renderer;
    vtkRenderer* renderer2;
    vtkDICOMImageReader* dicomReader;
    vtkImageData* m_imageData;
	vtkImageData* m_imagePartData;
    //灯光1
    vtkLight* light1;
	//三维切片显示时,在切片上画线并绘制点集
	MouseInteractorStyleTest* m_mouseInter;
	//VE相关
	VEControl* m_vecontrol;
	bool m_veactive;

    //polydata操作相关
    PolyDataOpe* m_polyDataOpe;

    //显示分割心脏相关
    HeartSeg* m_heartseg;
    vtkVolume* m_volume1;
    vtkVolume* m_volume2;
    vtkVolume* m_volume3;
    vtkVolume* m_volume4;
    vtkVolume* m_volume5;
    vtkVolume* m_volume6;

    vtkRenderer* m_render1;
    vtkRenderer* m_render2;
    vtkRenderer* m_render3;
    vtkRenderer* m_render4;
    vtkRenderer* m_render5;
    vtkRenderer* m_render6;
};

#endif // MAINWINDOW_H
