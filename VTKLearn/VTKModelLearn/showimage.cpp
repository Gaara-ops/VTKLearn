#include "showimage.h"

#include <QImage>
#include <QColor>
#include <QPixmap>

ShowImage::ShowImage()
{
    leftBtnDown=false;
    lastposx=0;
    lastposy=0;
    nowposx = 0;
    nowposy = 0;
}

void ShowImage::updateLabel(int width,int height,int* colorptr)
{
    QImage image( width, height, QImage::Format_RGB32 );
    QRgb *rgbPtr =
      reinterpret_cast<QRgb *>( image.bits() );// + width * ( height - 1 );
    // Loop over the vtkImageData contents.
    int imindex = 0;
    for ( int row = 0; row < height; row++ )
    {
      for ( int col = 0; col < width; col++ )
      {
        // Swap the vtkImageData RGB values with an equivalent QColor
        *( rgbPtr++ ) = QColor( colorptr[imindex],colorptr[imindex+1],colorptr[imindex+2]).rgb();
          imindex += 3;
      }
//      rgbPtr -= width * 2;
    }

    QPixmap pixmapToShow = QPixmap::fromImage( image );
    this->setPixmap(pixmapToShow);
}

void ShowImage::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        lastposx = e->x();
        lastposy = e->y();
        leftBtnDown = true;
    }
}

void ShowImage::mouseMoveEvent(QMouseEvent *e)
{
    nowposx = e->x();
    nowposy = e->y();
    if (leftBtnDown)
    {
        int dx = nowposx - lastposx;
        int dy = nowposy - lastposy;

        /*int *size = renderwindow->GetSize();

        double delta_elevation = -20.0 / size[1];
        double delta_azimuth = -20.0 / size[0];

        double rxf = dx * delta_azimuth * 10.0;
        double ryf = dy * delta_elevation * 10.0;

        vtkCamera *camera = renderer->GetActiveCamera();
        camera->Azimuth(rxf);
        camera->Elevation(ryf);
        camera->OrthogonalizeViewUp();
        renderwindow->Render();
        updateLabel();*/

    }
}

void ShowImage::mouseReleaseEvent(QMouseEvent *e)
{
    leftBtnDown = false;
}

void ShowImage::mouseDoubleClickEvent(QMouseEvent *e)
{

}
