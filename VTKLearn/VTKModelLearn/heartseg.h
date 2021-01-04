#ifndef HEARTSEG_H
#define HEARTSEG_H

class vtkVolume;
class vtkPiecewiseFunction;
class vtkImageData;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;

class HeartSeg
{
public:
    HeartSeg();

    void ReadmhdData(vtkVolume *&volume,int type);
    void ReadmhdData2(vtkVolume *&volume);
    void GetMinValue(int& oldx,int& oldy,int& oldz,
                     int& newx,int& newy,int& newz);
    void GetMaxValue(int& oldx,int& oldy,int& oldz,
                     int& newx,int& newy,int& newz);
    void GetOpacityFun(vtkPiecewiseFunction*& opacity,int type);
    void GetSectionImageData(vtkImageData*& imageData,int type,int source=0);

    void initSegVolume(int type);
    void GetVolumeByBounding(vtkImageData*& imagedata,int bound[6],int type);
public:
    vtkImageData* m_imageData;
    vtkImageData* m_oriImageData;
    double m_spacing[3];
    int m_dim[3];

    vtkImageData* m_image1;
    vtkImageData* m_image2;
    vtkImageData* m_image3;
    vtkImageData* m_image4;
    vtkImageData* m_image5;
    vtkImageData* m_image6;

//    vtkColorTransferFunction* colorFun1;
//    vtkPiecewiseFunction* opacityFun1;

};

#endif // HEARTSEG_H
