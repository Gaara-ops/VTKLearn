#include "polydataope.h"
#include "ui_polydataope.h"

PolyDataOpe::PolyDataOpe(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PolyDataOpe)
{
    ui->setupUi(this);

    m_render = vtkRenderer::New();
    ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_render);
    ui->qvtkWidget->GetRenderWindow()->Render();
}

PolyDataOpe::~PolyDataOpe()
{
    delete ui;
}

void PolyDataOpe::on_BooleanOperationPolyDataBtn_clicked()
{
    vtkSmartPointer<vtkSphereSource> sphereSource1 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource1->SetCenter(0.0, 0.0, 0.0);
    sphereSource1->SetRadius(2.0f);
    sphereSource1->Update();
    vtkSmartPointer<vtkPolyDataMapper> sphere1Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphere1Mapper->SetInputConnection( sphereSource1->GetOutputPort() );
    sphere1Mapper->ScalarVisibilityOff();
    vtkSmartPointer<vtkActor> sphere1Actor = vtkSmartPointer<vtkActor>::New();
    sphere1Actor->SetMapper( sphere1Mapper );
    sphere1Actor->GetProperty()->SetOpacity(.3);
    sphere1Actor->GetProperty()->SetColor(1,0,0);

    vtkSmartPointer<vtkSphereSource> sphereSource2 = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource2->SetCenter(1.0, 0.0, 0.0);
    sphereSource2->SetRadius(2.0f);
    vtkSmartPointer<vtkPolyDataMapper> sphere2Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphere2Mapper->SetInputConnection( sphereSource2->GetOutputPort() );
    sphere2Mapper->ScalarVisibilityOff();
    vtkSmartPointer<vtkActor> sphere2Actor = vtkSmartPointer<vtkActor>::New();
    sphere2Actor->SetMapper( sphere2Mapper );
    sphere2Actor->GetProperty()->SetOpacity(.3);
    sphere2Actor->GetProperty()->SetColor(0,1,0);

    vtkSmartPointer<vtkIntersectionPolyDataFilter> intersectionPolyDataFilter =
      vtkSmartPointer<vtkIntersectionPolyDataFilter>::New();
    intersectionPolyDataFilter->SetInputConnection( 0, sphereSource1->GetOutputPort() );
    intersectionPolyDataFilter->SetInputConnection( 1, sphereSource2->GetOutputPort() );
    intersectionPolyDataFilter->Update();

    vtkSmartPointer<vtkPolyDataMapper> intersectionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    intersectionMapper->SetInputConnection( intersectionPolyDataFilter->GetOutputPort() );
    intersectionMapper->ScalarVisibilityOff();

    vtkSmartPointer<vtkActor> intersectionActor = vtkSmartPointer<vtkActor>::New();
    intersectionActor->SetMapper( intersectionMapper );


//    m_render->AddViewProp(sphere1Actor);
//    m_render->AddViewProp(sphere2Actor);
    m_render->AddViewProp(intersectionActor);

    m_render->ResetCamera();
    ui->qvtkWidget->GetRenderWindow()->Render();
    qDebug() << "BooleanOperationPolyData end";
}

void PolyDataOpe::on_CloseBtm_clicked()
{
    this->close();
}
