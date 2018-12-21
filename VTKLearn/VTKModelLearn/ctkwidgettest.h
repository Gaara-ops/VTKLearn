#ifndef CTKWIDGETTEST_H
#define CTKWIDGETTEST_H

#include <QWidget>

#include <QGridLayout>

#include <ctkCheckablePushButton.h>
#include <ctkCollapsibleButton.h>
#include <ctkColorPickerButton.h>
#include <ctkRangeWidget.h>
#include <ctkCollapsibleGroupBox.h>
#include <ctkCoordinatesWidget.h>
#include <ctkTreeComboBox.h>
#include <ctkSearchBox.h>
#include <ctkDirectoryButton.h>
#include <ctkCheckableComboBox.h>
#include <ctkMaterialPropertyWidget.h>
#include <ctkMatrixWidget.h>
#include <ctkPathLineEdit.h>

class CTKWidgetTest : public QWidget
{
    Q_OBJECT
public:
    explicit CTKWidgetTest(QWidget *parent = 0);

signals:

public slots:

private:
    QGridLayout* m_layout;
    ctkCheckablePushButton* m_ctkCheckableButton;
    ctkCollapsibleButton* m_ctkCollapsibleButton;
    ctkColorPickerButton* m_ctkColorPickerButton;
    ctkRangeWidget* m_ctkRangeWidget;
    ctkCollapsibleGroupBox* m_ctkCollapsibleGroupBox;
    ctkCoordinatesWidget* m_ctkCoordinatesWidget;
    ctkTreeComboBox* m_ctkTreeComboBox;
    ctkSearchBox* m_ctkSearchBox;
    ctkDirectoryButton* m_ctkDirectoryButton;
    ctkCheckableComboBox* m_ctkCheckableComboBox;
    ctkMaterialPropertyWidget* m_ctkMaterialPropertyWidget;
    ctkMatrixWidget* m_ctkMatrixWidget;
    ctkPathLineEdit* m_ctkPathLineEdit;
};

#endif // CTKWIDGETTEST_H
