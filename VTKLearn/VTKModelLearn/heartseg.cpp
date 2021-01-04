#include "heartseg.h"

#include "../Globe/GlobeInclude.h"
#include <omp.h>
int gtype = 6;
int bounding1[6] = {185,421,159,351,43,183};
int bounding2[6] = {131,358,131,282,39,201};
int bounding3[6] = {106,260,286,370,33,137};
int bounding4[6] = {49 ,203,172,332,52,165};
int bounding5[6] = {188,437,140,370,28,191};
int bounding6[6] = {68 ,219,185,301,0 ,124};

int Vbounding1[6] = {185,421,512-351,512-159,275-183,275-43,};
int Vbounding2[6] = {131,358,512-282,512-131,275-201,275-39,};
int Vbounding3[6] = {106,260,512-370,512-286,275-137,275-33,};
int Vbounding4[6] = {49 ,203,512-332,512-172,275-165,275-52,};
int Vbounding5[6] = {188,437,512-370,512-140,275-191,275-28,};
int Vbounding6[6] = {68 ,219,512-301,512-185,275-124,275-0 ,};
HeartSeg::HeartSeg()
{
    m_image1 = NULL;
    m_image2 = NULL;
    m_image3 = NULL;
    m_image4 = NULL;
    m_image5 = NULL;
    m_image6 = NULL;

//    colorFun1 =vtkColorTransferFunction::New();
//    opacityFun1 =vtkPiecewiseFunction::New();
//    colorFun1->AddRGBPoint(-1024, 0.0, 0.0, 0.0);
//    colorFun1->AddRGBPoint(1000, 1.0, 0.0, 0.0 );
//    opacityFun1->AddPoint(-1024, 0 );
//    opacityFun1->AddPoint(1000, 1.0 );




    vtkMetaImageReader* read1 = vtkMetaImageReader::New();
    read1->SetFileName("E:/save_heart.mhd");
    read1->Update();
    m_imageData = read1->GetOutput();

    vtkMetaImageReader* read2 = vtkMetaImageReader::New();
    read2->SetFileName("E:/mhddata/36649/test_1.mhd");
    read2->Update();
    m_oriImageData = read2->GetOutput();
    m_oriImageData->GetSpacing(m_spacing);
    m_oriImageData->GetDimensions(m_dim);
}

void HeartSeg::ReadmhdData(vtkVolume *&volume, int type)
{
    initSegVolume(type);
    double* range = NULL;
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> mapper1 =
        vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    if(type == 1){
        range = m_image1->GetScalarRange();
        mapper1->SetInputData(m_image1);
    }else if(type == 2){
        range = m_image2->GetScalarRange();
        mapper1->SetInputData(m_image2);
    }else if(type == 3){
        range = m_image3->GetScalarRange();
        mapper1->SetInputData(m_image3);
    }else if(type == 4){
        range = m_image4->GetScalarRange();
        mapper1->SetInputData(m_image4);
    }else if(type == 5){
        range = m_image5->GetScalarRange();
        mapper1->SetInputData(m_image5);
    }else if(type == 6){
        range = m_image6->GetScalarRange();
        mapper1->SetInputData(m_image6);
    }else{
        return;
    }
    qDebug() << range[0] << range[1];

    mapper1->SetBlendMode(0);
    mapper1->SetAutoAdjustSampleDistances(1);

    vtkSmartPointer<vtkColorTransferFunction> colorFun1 =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    vtkSmartPointer<vtkPiecewiseFunction> opacityFun1 =
            vtkSmartPointer<vtkPiecewiseFunction>::New();
    colorFun1->AddRGBPoint(range[0], 0.0, 0.0, 0.0);

//    colorFun1->AddRGBPoint(0, 0.0, 0.0, 0.0);
    if(type == 1){
        colorFun1->AddRGBPoint(range[1], 1.0, 0.0, 0.0 );
    }else if(type == 2){
        colorFun1->AddRGBPoint(range[1], 0.0, 1.0, 0.0 );
    }else if(type == 3){
        colorFun1->AddRGBPoint(range[1], 0.0, 0.0, 1.0 );
    }else if(type == 4){
        colorFun1->AddRGBPoint(range[1], 1.0, 1.0, 0.0 );
    }else if(type == 5){
        colorFun1->AddRGBPoint(range[1], 1.0, 0.0, 1.0 );
    }else if(type == 6){
        colorFun1->AddRGBPoint(range[1], 0.0, 1.0, 1.0 );
    }

    opacityFun1->AddPoint(-1024, 0.0 );
    opacityFun1->AddPoint(-1023, 1.0 );
    opacityFun1->AddPoint(range[1], 1.0 );

/*
    opacityFun1->AddPoint(0, 0 );
    opacityFun1->AddPoint(1, 0 );
    opacityFun1->AddPoint(2, 0 );
    opacityFun1->AddPoint(3, 0 );
    opacityFun1->AddPoint(4, 0 );
    opacityFun1->AddPoint(5, 0 );
    opacityFun1->AddPoint(6, 0 );
    opacityFun1->AddPoint(gtype, 0.5);
*/
    vtkSmartPointer<vtkVolumeProperty> property1 =
            vtkSmartPointer<vtkVolumeProperty>::New();
    property1->SetColor( colorFun1 );
    property1->SetScalarOpacity( opacityFun1 );
    property1->SetInterpolationTypeToLinear();

    volume->SetProperty( property1 );
    volume->SetMapper( mapper1 );
//    tmpData->Delete();
}

void HeartSeg::ReadmhdData2(vtkVolume*& volume)
{
    int dim[3];
    double spacing[3];
    m_imageData->GetDimensions(dim);
    m_imageData->GetSpacing(spacing);
    qDebug() << "dim:" << dim[0] << dim[1] << dim[2] << m_imageData->GetScalarType();
    qDebug() << "spacing:" << spacing[0] << spacing[1] << spacing[2];
    QTime time1;
    time1.start();
/*    int bounding1[6] = {99999,0,99999,0,99999,0};
    int bounding2[6] = {99999,0,99999,0,99999,0};
    int bounding3[6] = {99999,0,99999,0,99999,0};
    int bounding4[6] = {99999,0,99999,0,99999,0};
    int bounding5[6] = {99999,0,99999,0,99999,0};
    int bounding6[6] = {99999,0,99999,0,99999,0};
    vtkSmartPointer<vtkPoints> points1 =vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> points2 =vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> points3 =vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> points4 =vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> points5 =vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> points6 =vtkSmartPointer<vtkPoints>::New();
    vtkDataArray* dataarr = image1->GetPointData()->GetScalars();
//    #pragma omp parallel for
    for(int k=0;k<dim[2];k++){
        for(int j=0;j<dim[1];j++){
            for(int i=0;i<dim[0];i++){
                int tmpindex = k*dim[1]*dim[0] + j*dim[0] + i;
                int tmpvalue = (int)dataarr->GetComponent(tmpindex,0);
                if((tmpvalue) == 0 ){
                    continue;
                }

                if((tmpvalue) == 1 ){
                    GetMinValue(bounding1[0],bounding1[2],bounding1[4],
                                i,j,k);
                    GetMaxValue(bounding1[1],bounding1[3],bounding1[5],
                                i,j,k);
                }
                if((tmpvalue) == 2 ){
                    GetMinValue(bounding2[0],bounding2[2],bounding2[4],
                                i,j,k);
                    GetMaxValue(bounding2[1],bounding2[3],bounding2[5],
                                i,j,k);
                }
                if((tmpvalue) == 3 ){
                    GetMinValue(bounding3[0],bounding3[2],bounding3[4],
                                i,j,k);
                    GetMaxValue(bounding3[1],bounding3[3],bounding3[5],
                                i,j,k);
                }
                if((tmpvalue) == 4 ){
                    GetMinValue(bounding4[0],bounding4[2],bounding4[4],
                                i,j,k);
                    GetMaxValue(bounding4[1],bounding4[3],bounding4[5],
                                i,j,k);
                }
                if((tmpvalue) == 5 ){
                    GetMinValue(bounding5[0],bounding5[2],bounding5[4],
                                i,j,k);
                    GetMaxValue(bounding5[1],bounding5[3],bounding5[5],
                                i,j,k);
                }
                if((tmpvalue) == 6 ){
                    GetMinValue(bounding6[0],bounding6[2],bounding6[4],
                                i,j,k);
                    GetMaxValue(bounding6[1],bounding6[3],bounding6[5],
                                i,j,k);
                }
            }
        }
    }

    qDebug() << "time middle:" << time1.elapsed()/1000.0;
    points1->GetBounds(bounding1);
    points2->GetBounds(bounding2);
    points3->GetBounds(bounding3);
    points4->GetBounds(bounding4);
    points5->GetBounds(bounding5);
    points6->GetBounds(bounding6);*/

    qDebug() << "bounding1:" << bounding1[0] << bounding1[1] << bounding1[2]
                << bounding1[3] << bounding1[4] << bounding1[5];
    qDebug() << "bounding2:" << bounding2[0] << bounding2[1] << bounding2[2]
                << bounding2[3] << bounding2[4] << bounding2[5];
    qDebug() << "bounding3:" << bounding3[0] << bounding3[1] << bounding3[2]
                << bounding3[3] << bounding3[4] << bounding3[5];
    qDebug() << "bounding4:" << bounding4[0] << bounding4[1] << bounding4[2]
                << bounding4[3] << bounding4[4] << bounding4[5];
    qDebug() << "bounding5:" << bounding5[0] << bounding5[1] << bounding5[2]
                << bounding5[3] << bounding5[4] << bounding5[5];
    qDebug() << "bounding6:" << bounding6[0] << bounding6[1] << bounding6[2]
                << bounding6[3] << bounding6[4] << bounding6[5];

    qDebug() << "time use:" << time1.elapsed()/1000.0;
    double range[2];
    m_imageData->GetScalarRange(range);

//    int bound1[6] = {int(bounding1[0]),int(bounding1[1]),int(bounding1[2]),
//                    int(bounding1[3]),int(bounding1[4]),int(bounding1[5])};


    /*提取体感兴趣的区域*/
    vtkSmartPointer<vtkExtractVOI> extractVOI =
        vtkSmartPointer<vtkExtractVOI>::New();
    extractVOI->SetInputData(m_imageData);
    if(gtype == 1){
        extractVOI->SetVOI(bounding1);
    }else if(gtype ==2){
        extractVOI->SetVOI(bounding2);
    }else if(gtype ==3){
        extractVOI->SetVOI(bounding3);
    }else if(gtype ==4){
        extractVOI->SetVOI(bounding4);
    }else if(gtype ==5){
        extractVOI->SetVOI(bounding5);
    }else if(gtype ==6){
        extractVOI->SetVOI(bounding6);
    }

    extractVOI->Update();


    qDebug() << "range:" << range[0] << range[1];
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> mapper1 =
        vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    mapper1->SetInputData(m_imageData);
    mapper1->SetBlendMode(0);
    mapper1->SetAutoAdjustSampleDistances(1);
    vtkSmartPointer<vtkColorTransferFunction> colorFun1 =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    vtkSmartPointer<vtkPiecewiseFunction> opacityFun1 =
            vtkSmartPointer<vtkPiecewiseFunction>::New();
    vtkSmartPointer<vtkVolumeProperty> property1 =
            vtkSmartPointer<vtkVolumeProperty>::New();
    colorFun1->AddRGBPoint(range[0], 0.3, 0.3, 0.7);
    colorFun1->AddRGBPoint(range[1], 0.8, 0, 0.2 );
    opacityFun1->AddPoint(range[0], 0 );
    opacityFun1->AddPoint(range[1], 0.5 );
/*
    opacityFun1->AddPoint(0, 0 );
    opacityFun1->AddPoint(1, 0 );
    opacityFun1->AddPoint(2, 0 );
    opacityFun1->AddPoint(3, 0 );
    opacityFun1->AddPoint(4, 0 );
    opacityFun1->AddPoint(5, 0 );
    opacityFun1->AddPoint(6, 0 );
    opacityFun1->AddPoint(gtype, 0.5);
*/

    property1->SetColor( colorFun1 );
    property1->SetScalarOpacity( opacityFun1 );
    property1->SetInterpolationTypeToLinear();

    volume->SetProperty( property1 );
    volume->SetMapper( mapper1 );
}

void HeartSeg::GetMinValue(int &oldx, int &oldy, int &oldz,
                           int &newx, int &newy, int &newz)
{
    oldx = oldx<newx ? oldx : newx;
    oldy = oldy<newy ? oldy : newy;
    oldz = oldz<newz ? oldz : newz;
}

void HeartSeg::GetMaxValue(int &oldx, int &oldy, int &oldz,
                           int &newx, int &newy, int &newz)
{
    oldx = oldx>newx ? oldx : newx;
    oldy = oldy>newy ? oldy : newy;
    oldz = oldz>newz ? oldz : newz;
}

void HeartSeg::GetOpacityFun(vtkPiecewiseFunction *&opacity, int type)
{
    opacity->AddPoint(0, 0 );
    opacity->AddPoint(1, 0 );
    opacity->AddPoint(2, 0 );
    opacity->AddPoint(3, 0 );
    opacity->AddPoint(4, 0 );
    opacity->AddPoint(5, 0 );
    opacity->AddPoint(6, 0 );
    opacity->AddPoint(type-0.1, 0);
    opacity->AddPoint(type, 0.5);
    opacity->AddPoint(type+0.1, 0);
}

void HeartSeg::GetSectionImageData(vtkImageData *&imageData, int type, int source)
{
    vtkSmartPointer<vtkExtractVOI> extractVOI =
        vtkSmartPointer<vtkExtractVOI>::New();
    if(source == 0){
        extractVOI->SetInputData(m_imageData);
    }else if(source == 1){
        extractVOI->SetInputData(m_oriImageData);
    }

    if(type == 1){
        extractVOI->SetVOI(bounding1);
    }else if(type ==2){
        extractVOI->SetVOI(bounding2);
    }else if(type ==3){
        extractVOI->SetVOI(bounding3);
    }else if(type ==4){
        extractVOI->SetVOI(bounding4);
    }else if(type ==5){
        extractVOI->SetVOI(bounding5);
    }else if(type ==6){
        extractVOI->SetVOI(bounding6);
    }
    extractVOI->Update();
    imageData->DeepCopy(extractVOI->GetOutput());
}

void HeartSeg::initSegVolume(int type)
{
    if(type == 1){
        if(m_image1 == NULL){
            m_image1 = vtkImageData::New();
            GetVolumeByBounding(m_image1,bounding1,1);
        }
    }else if(type == 2){
        if(m_image2 == NULL){
            m_image2 = vtkImageData::New();
            GetVolumeByBounding(m_image2,bounding2,2);
        }
    }else if(type == 3){
        if(m_image3 == NULL){
            m_image3 = vtkImageData::New();
            GetVolumeByBounding(m_image3,bounding3,3);
        }
    }else if(type == 4){
        if(m_image4 == NULL){
            m_image4 = vtkImageData::New();
            GetVolumeByBounding(m_image4,bounding4,4);
        }
    }else if(type == 5){
        if(m_image5 == NULL){
            m_image5 = vtkImageData::New();
            GetVolumeByBounding(m_image5,bounding5,5);
        }
    }else if(type == 6){
        if(m_image6 == NULL){
            m_image6 = vtkImageData::New();
            GetVolumeByBounding(m_image6,bounding6,6);
        }
    }
}

void HeartSeg::GetVolumeByBounding(vtkImageData *&imagedata, int bound[], int type)
{
    int dim[3] = {bound[1]-bound[0]+1,bound[3]-bound[2]+1,bound[5]-bound[4]+1};
    imagedata->SetDimensions(dim);
    qDebug() << "tmpdim:" << type << dim[0]<<dim[1]<<dim[2];
    imagedata->SetSpacing(m_spacing);
    imagedata->AllocateScalars(VTK_SHORT,1);
    int extent[6] = {bound[0],bound[1],m_dim[1]-bound[3]-1,m_dim[1]-bound[2]-1,
                                       m_dim[2]-bound[5]-1,m_dim[2]-bound[4]-1};
    imagedata->SetExtent(extent);
//    imagedata->SetOrigin(extent[0]*m_spacing[0],extent[0]*m_spacing[0],extent[0]*m_spacing[0]);
    vtkDataArray* dataarr = imagedata->GetPointData()->GetScalars();
    vtkDataArray* dataarrbin = m_imageData->GetPointData()->GetScalars();
    vtkDataArray* dataarrori = m_oriImageData->GetPointData()->GetScalars();
    qDebug() << "NumberOfTuples:"<< dataarr->GetNumberOfTuples();
    for(int i=0;i<dataarr->GetNumberOfTuples();i++){
        dataarr->InsertComponent(i,0,-1024);
    }
    int allnum = 0;
    int nowindex = 0;
    for(int k=bound[4];k<=bound[5];k++){
        for(int j=bound[2];j<=bound[3];j++){
            for(int i=bound[0];i<=bound[1];i++){
                int tmpindex = k*m_dim[1]*m_dim[0] + j*m_dim[0] + i;
                int tmpvalue = (int)dataarrbin->GetComponent(tmpindex,0);
                if((tmpvalue) == 0 ){
                    //dataarr->InsertComponent(nowindex,0,-1024);
                    ++nowindex;
                    continue;
                }
                if(tmpvalue==type){
                    ++allnum;
                    int realindex = (m_dim[2]-k-1)*m_dim[1]*m_dim[0] +
                                    (m_dim[1]-j-1)*m_dim[0] + i;
                    short realvalue = (short)dataarrori->GetComponent(realindex,0);

                    int partindex = ((dim[2]-(k-bound[4])-1)*dim[1]*dim[0])+
                            ((dim[1]-(j-bound[2])-1)*dim[0])+(i-bound[0]);
                    if(partindex<=dataarr->GetNumberOfTuples()){
                        if(partindex>=0){
                            dataarr->InsertComponent(partindex,0,realvalue);
                        }else{
                            qDebug() << "error0:" << k-bound[4] <<j-bound[2] <<i-bound[0] << partindex;
                        }
                    }else{
                        qDebug() << "error:" << k-bound[4] <<j-bound[2] <<i-bound[0] << partindex;
                    }
                }
                ++nowindex;
            }
        }
    }
    qDebug() << "allnum:" << allnum;
    imagedata->Modified();
}

/*
if((tmpvalue) == 1 ){
    GetMinValue(bounding1[0],bounding1[2],bounding1[4],
                i,j,k);
    GetMaxValue(bounding1[1],bounding1[3],bounding1[5],
                i,j,k);
}
if((tmpvalue) == 2 ){
    GetMinValue(bounding2[0],bounding2[2],bounding2[4],
                i,j,k);
    GetMaxValue(bounding2[1],bounding2[3],bounding2[5],
                i,j,k);
}
if((tmpvalue) == 3 ){
    GetMinValue(bounding3[0],bounding3[2],bounding3[4],
                i,j,k);
    GetMaxValue(bounding3[1],bounding3[3],bounding3[5],
                i,j,k);
}
if((tmpvalue) == 4 ){
    GetMinValue(bounding4[0],bounding4[2],bounding4[4],
                i,j,k);
    GetMaxValue(bounding4[1],bounding4[3],bounding4[5],
                i,j,k);
}
if((tmpvalue) == 5 ){
    GetMinValue(bounding5[0],bounding5[2],bounding5[4],
                i,j,k);
    GetMaxValue(bounding5[1],bounding5[3],bounding5[5],
                i,j,k);
}
if((tmpvalue) == 6 ){
    GetMinValue(bounding6[0],bounding6[2],bounding6[4],
                i,j,k);
    GetMaxValue(bounding6[1],bounding6[3],bounding6[5],
                i,j,k);
}
*/


/*
if((tmpvalue) == 1 ){
    points1->InsertNextPoint(i,j,k);
}
if((tmpvalue) == 2 ){
    points2->InsertNextPoint(i,j,k);
}
if((tmpvalue) == 3 ){
    points3->InsertNextPoint(i,j,k);
}
if((tmpvalue) == 4 ){
    points4->InsertNextPoint(i,j,k);
}
if((tmpvalue) == 5 ){
    points5->InsertNextPoint(i,j,k);
}
if((tmpvalue) == 6 ){
    points6->InsertNextPoint(i,j,k);
}
*/
