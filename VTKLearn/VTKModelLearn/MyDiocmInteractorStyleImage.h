#ifndef MYDIOCMINTERACTORSTYLEIMAGE_H
#define MYDIOCMINTERACTORSTYLEIMAGE_H

#include "../Globe/GlobeInclude.h"
#include "myfunc.h"

// helper class to format slice status message
class StatusMessage {
public:
   static std::string Format(int slice, int maxSlice) {
	  std::stringstream tmp;
	  tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;
	  return tmp.str();
   }
};

// Define own interaction style
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
   static myVtkInteractorStyleImage* New();
   vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

   vtkImageData* m_imageData;
protected:
   vtkImageViewer2* _ImageViewer;
   vtkTextMapper* _StatusMapper;
   int _Slice;
   int _MinSlice;
   int _MaxSlice;
   QString m_strpos;
public:
   void SetImageViewer(vtkImageViewer2* imageViewer) {
	  _ImageViewer = imageViewer;
	  _MinSlice = imageViewer->GetSliceMin();
	  _MaxSlice = imageViewer->GetSliceMax();
	  _Slice = _MaxSlice;
   }

   void SetStatusMapper(vtkTextMapper* statusMapper) {
	  _StatusMapper = statusMapper;
   }

protected:
   void OnLeftButtonDown(){
	   int* pos = this->GetInteractor()->GetEventPosition();
	   double pos3d[3];
	   MyFunc::GetPos3DBy2D_1(_ImageViewer->GetRenderer(),pos,pos3d);

	   qDebug() << "pos:" << pos[0] << "," << pos[1]<<","<<_Slice;
	   qDebug() << "pos3d:" << pos3d[0]<< pos3d[1]<< pos3d[2];
	   /*测试区域增长*/
	   vtkSmartPointer<vtkPoints> allPolyPoints =
		 vtkSmartPointer<vtkPoints>::New();
	   vtkSmartPointer<vtkActor> allPointActor =
		 vtkSmartPointer<vtkActor>::New();
	   double spacing[3];
	   m_imageData->GetSpacing(spacing);
	   int dimx = pos3d[0]/spacing[0];
	   int dimy = pos3d[1]/spacing[1];
	   qDebug() << "dim:" << dimx<<dimy;
	   SeedGrowTest(dimx,dimy,allPolyPoints);
	   double poscolor[3] = {1,0,0};
	   MyFunc::CreatePointActor(allPolyPoints,allPointActor,poscolor,3,0.5);
	   _ImageViewer->GetRenderer()->AddActor2D(allPointActor);
	   _ImageViewer->Modified();
	   _ImageViewer->Render();

	   /*测试添加中心点
	   m_strpos += QString::number(pos3d[0])+","+QString::number(pos3d[1])+
			   ","+QString::number(pos3d[2]) + "|";*/

	   vtkInteractorStyleImage::OnLeftButtonDown();
   }
   void OnRightButtonUp(){
		//qDebug() << m_strpos;
		vtkInteractorStyleImage::OnRightButtonUp();
   }

   void MoveSliceForward() {
	  if(_Slice < _MaxSlice) {
		 _Slice += 1;
		 _ImageViewer->SetSlice(_Slice);
		 std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
		 _StatusMapper->SetInput(msg.c_str());
		 _ImageViewer->Render();
	  }
   }
   void MoveSliceBackward() {
	  if(_Slice > _MinSlice) {
		 _Slice -= 1;
		 _ImageViewer->SetSlice(_Slice);
		 std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
		 _StatusMapper->SetInput(msg.c_str());
		 _ImageViewer->Render();
	  }
   }
   virtual void OnKeyDown() {
	  std::string key = this->GetInteractor()->GetKeySym();
	  if(key.compare("Up") == 0) {
		 MoveSliceForward();
	  }
	  else if(key.compare("Down") == 0) {
		 MoveSliceBackward();
	  }
	  vtkInteractorStyleImage::OnKeyDown();
   }
   virtual void OnMouseWheelForward() {
	  MoveSliceForward();
   }
   virtual void OnMouseWheelBackward() {
	  if(_Slice > _MinSlice) {
		 MoveSliceBackward();
	  }
   }
   //区域增长测试
   void SeedGrowTest(int x,int y,vtkPoints* points){
	   qDebug() << "begin";
	   int numpos = 0,outnumpos=0;
	   QPoint ptGrowing;//待生长点的位置
	   int threshold=200;//是否被标记
	   int nScrValue=0;//生长起始点的灰度值
	   int nCurValue=0;//当前生长点的灰度值
	   int DIR[8][2]={{-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}};
	   QVector<QPoint> vcGrowpt;//生长点的堆栈
	   vcGrowpt.push_back(QPoint(x,y));//将初始生长点压入堆栈
	   //标记初始生长点
	   short *ptr0 = static_cast<short*>(m_imageData->GetScalarPointer(x,y,_Slice));
	   nScrValue = (int)(*ptr0);
	   qDebug() << "nScrValue:"<<nScrValue;
	   double spacing[3];
	   m_imageData->GetSpacing(spacing);
	   while(!vcGrowpt.empty())
	   {
		   QPoint curpt=vcGrowpt.back();//在堆栈中取出一个生长点
		   vcGrowpt.pop_back();

		   for(int i=0;i<8;i++)
		   {
			   ptGrowing.setX(curpt.x()+DIR[i][0]);
			   ptGrowing.setY(curpt.y()+DIR[i][1]);
			   if(ptGrowing.x()<0||ptGrowing.y()<0||
					   (ptGrowing.x()>=512)||(ptGrowing.y()>=512)){
				   continue;
			   }
			   short *ptr = static_cast<short*>(m_imageData->GetScalarPointer(
											ptGrowing.x(),ptGrowing.y(),_Slice));
			   nCurValue=*ptr;
			   if (nCurValue!=-2048)//表示还未标记过
			   {
				   if(abs(nCurValue-nScrValue)<=threshold)
				   {
					   points->InsertNextPoint(ptGrowing.x()*spacing[0],
							   ptGrowing.y()*spacing[1],
							   _Slice*spacing[2]);
					   ++numpos;
					   *ptr = -2048;
					   vcGrowpt.push_back(ptGrowing);
				   }else{
					   ++outnumpos;
						//qDebug() << "nCurValue:"<<nCurValue;
				   }
			   }
		   }
	   }
	   qDebug() <<"numpos:"<<numpos<<",outnumpos:"<<outnumpos;
   }
};

#endif // MYDIOCMINTERACTORSTYLEIMAGE_H
