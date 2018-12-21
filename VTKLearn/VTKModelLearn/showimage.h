#ifndef SHOWIMAGE_H
#define SHOWIMAGE_H

#include <QLabel>
#include <QStatusBar>
#include <QMouseEvent>

class ShowImage : public QLabel
{
public:
    ShowImage();
    void updateLabel(int width, int height, int *colorptr);
protected:
    void mousePressEvent(QMouseEvent *e);//鼠标按下
    void mouseMoveEvent(QMouseEvent *e);//鼠标移动
    void mouseReleaseEvent(QMouseEvent *e);//鼠标释放
    void mouseDoubleClickEvent(QMouseEvent *e);//鼠标双击
signals:

public slots:

private:
    bool leftBtnDown;
    int lastposx,lastposy;
    int nowposx,nowposy;
};

#endif // SHOWIMAGE_H
