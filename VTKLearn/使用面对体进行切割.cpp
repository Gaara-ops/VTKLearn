vtkSmartPointer<vtkPlane> plane =
  vtkSmartPointer<vtkPlane>::New();
plane->SetOrigin(0,0,0);
plane->SetNormal(0,0,1);//切除的是向量指向的方向
vtkSmartPointer<vtkPlaneCollection> planes =
  vtkSmartPointer<vtkPlaneCollection>::New();
planes->AddItem(plane);
vtkSmartPointer<vtkGPUVolumeRayCastMapper> gmapper =
  vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
gmapper->SetClippingPlanes(planes);
gmapper->Modified();