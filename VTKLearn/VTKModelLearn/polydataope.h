#ifndef POLYDATAOPE_H
#define POLYDATAOPE_H

#include <QDialog>
#include "../Globe/GlobeInclude.h"

namespace Ui {
class PolyDataOpe;
}

class PolyDataOpe : public QDialog
{
    Q_OBJECT

public:
    explicit PolyDataOpe(QWidget *parent = 0);
    ~PolyDataOpe();

private slots:
    void on_BooleanOperationPolyDataBtn_clicked();

    void on_CloseBtm_clicked();

private:
    Ui::PolyDataOpe *ui;
    vtkRenderer* m_render;
};

#endif // POLYDATAOPE_H
