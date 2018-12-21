#include "ctkwidgettest.h"

CTKWidgetTest::CTKWidgetTest(QWidget *parent) :
    QWidget(parent)
{

    m_layout = new QGridLayout(this);
    /*//复选按钮
    m_ctkCheckableButton = new ctkCheckablePushButton(this);
    m_ctkCheckableButton->setText("ctkCheckablePushButton");
    //可折叠按钮
    m_ctkCollapsibleButton = new ctkCollapsibleButton(this);
    m_ctkCollapsibleButton->setText("ctkCollapsibleButton");
    m_layout->addWidget(m_ctkCheckableButton, 0, 0);
    m_layout->addWidget(m_ctkCollapsibleButton, 0, 1);

    //颜色拾取器
    m_ctkColorPickerButton = new ctkColorPickerButton(this);
    m_ctkColorPickerButton->setText("ctkColorPickerButton");
    //滑块
    m_ctkRangeWidget = new ctkRangeWidget(this);
    m_layout->addWidget(m_ctkColorPickerButton, 0, 0);
    m_layout->addWidget(m_ctkRangeWidget, 0, 1);

    //分组框
    m_ctkCollapsibleGroupBox = new ctkCollapsibleGroupBox(this);
    m_ctkCollapsibleGroupBox->setTitle("GroupBox");
    m_layout->addWidget(m_ctkCollapsibleGroupBox, 0, 0);

    //坐标组件
    m_ctkCoordinatesWidget = new ctkCoordinatesWidget(this);

    //树形组合狂
    m_ctkTreeComboBox = new ctkTreeComboBox(this);
    m_ctkTreeComboBox->addItems(QStringList() << "A" << "B");
    m_layout->addWidget(m_ctkCoordinatesWidget, 0, 0);
    m_layout->addWidget(m_ctkTreeComboBox, 0, 1);*/
    //搜索框
    m_ctkSearchBox = new ctkSearchBox(this);
    //目录按钮
    m_ctkDirectoryButton = new ctkDirectoryButton(this);
    m_layout->addWidget(m_ctkSearchBox, 7, 0, 1, 2);
    m_layout->addWidget(m_ctkDirectoryButton, 7, 2, 1, 2);
    //带复选功能的组合框
    m_ctkCheckableComboBox = new  ctkCheckableComboBox(this);
    m_ctkCheckableComboBox->addItems(QStringList() << "A" << "B");
    //路径行编辑框
    m_ctkPathLineEdit = new ctkPathLineEdit(this);
    m_ctkPathLineEdit->addCurrentPathToHistory();
    m_layout->addWidget(m_ctkCheckableComboBox, 8, 0, 1, 2);
    m_layout->addWidget(m_ctkPathLineEdit, 8, 2, 1, 2);
    //材料属性组件
    m_ctkMaterialPropertyWidget = new ctkMaterialPropertyWidget(this);

    ctkVTKVolumePropertyWidget
    //矩阵组件
    m_ctkMatrixWidget = new ctkMatrixWidget(this);
    m_layout->addWidget(m_ctkMaterialPropertyWidget, 9, 0, 1, 2);
    m_layout->addWidget(m_ctkMatrixWidget, 9, 2, 1, 2);/**/

    setLayout(m_layout);
}
