#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class vtkDICOMImageReader;
class vtkRenderer;
class vtkRenderWindow;
class QVTKWidget;
class vtkImageSlice;
class vtkInteractorStyleImage;
class vtkImageResliceMapper;
//class vtkInteractorStyleTrackballCamera;
class MouseInteractorStyleTest;
class vtkCamera;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initView1();//轴位图
    void initView2();//冠位图
    void initView3();//矢位图
    void initView4Surface();//平面重建
    void initView4Curve();//曲面重建

private slots:
    void slotUpdateView1();
    //获取窗宽窗位
    void on_actionGetCL_triggered();

    void slot_slider1_valueChanged(int position);

    void slot_slider2_valueChanged(int position);

    void slot_slider3_valueChanged(int position);

    //绕垂直与屏幕旋转
    void on_actionRoll_triggered();
    //左右运动
    void on_actionAzimuth_triggered();
    //上下运动
    void on_actionElevation_triggered();

    void on_actionSetThinkness_triggered();

    void on_doubleSpinBox1_editingFinished();

    void on_doubleSpinBox2_editingFinished();

    void on_doubleSpinBox3_editingFinished();

    void on_actionSurface_triggered();

    void on_actionDiswidget_triggered();

private:
    //设置交互样式
    void setInterStyle();
    //初始化滑动条
    void initSlider();

    //初始化轴冠矢图
    void initView(QVTKWidget* vtkwidget, double *normal, vtkRenderer* render, vtkImageResliceMapper* slicemapper,vtkCamera* camera, double thickness = 10.0);
    //更新轴冠矢图
    void updateSlice(vtkRenderer* render, double* center,vtkImageResliceMapper* slicemapper, double thickness = 10.0);
    //更新切片厚度
    void updateThickness(vtkImageResliceMapper* slicemapper,double thickness);
private:
    Ui::MainWindow *ui;
    //读取dicom文件
    vtkDICOMImageReader* m_dicomReader;
    int m_dimension[3];
    double m_spacing[3];
    //切片中心点
    double m_center[3];
    //切片所在层数
    int m_sliceIndex[3];
    //每个切片的渲染对象
    vtkRenderer* m_render[4];
    //每个切片对象
    vtkImageResliceMapper* m_resliceMapper[4];
    //每个切片图的交互对象
    MouseInteractorStyleTest* m_style[3];
    //切片厚度
    int m_thickness;

};

#endif // MAINWINDOW_H
