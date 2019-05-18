#include "qtbarwindow.h"
QtBarWindow::QtBarWindow(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setWindowTitle(QStringLiteral("NuKit V0.1"));
    //    createWindow();
    Q3DBars *widgetgraph = new Q3DBars();
    QWidget *container = QWidget::createWindowContainer(widgetgraph);
    QSize screenSize = widgetgraph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    if (!widgetgraph->hasContext())
    {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        //        return -1;  TODO: Add error here
    }

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->addWidget(container, 1);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addLayout(vLayout);

    QComboBox *themeList = new QComboBox(this);
    QCheckBox *gridCheckBox = new QCheckBox(this);

    GraphModifier *modifier = new GraphModifier(widgetgraph);

    QPushButton *labelButton = new QPushButton(this);
    QCheckBox *smoothCheckBox = new QCheckBox(this);
    QComboBox *barStyleList = new QComboBox(this);
    QPushButton *zoomToSelectedButton = new QPushButton(this);
    QComboBox *selectionModeList = new QComboBox(this);
    QCheckBox *backgroundCheckBox = new QCheckBox(this);
    QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, this);
    QFontComboBox *fontList = new QFontComboBox(this);
    QComboBox *shadowQuality = new QComboBox(this);
    QCheckBox *axisTitlesVisibleCB = new QCheckBox(this);
    QSlider *axisLabelRotationSlider = new QSlider(Qt::Horizontal, this);
    QSlider *timeSlider = new QSlider(Qt::Horizontal, this);

    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));
    themeList->setCurrentIndex(2);

    labelButton->setText(QStringLiteral("Change label style"));

    smoothCheckBox->setText(QStringLiteral("Smooth bars"));
    smoothCheckBox->setChecked(false);

    barStyleList->addItem(QStringLiteral("Bevel bar"), int(QAbstract3DSeries::MeshBevelBar));
    barStyleList->addItem(QStringLiteral("Pyramid"), int(QAbstract3DSeries::MeshPyramid));
    barStyleList->addItem(QStringLiteral("Cone"), int(QAbstract3DSeries::MeshCone));
    barStyleList->addItem(QStringLiteral("Cylinder"), int(QAbstract3DSeries::MeshCylinder));
    barStyleList->setCurrentIndex(0);

    zoomToSelectedButton->setText(QStringLiteral("Zoom to selected bar"));

    selectionModeList->addItem(QStringLiteral("None"),
                               int(QAbstract3DGraph::SelectionNone));
    selectionModeList->addItem(QStringLiteral("Bar"),
                               int(QAbstract3DGraph::SelectionItem));
    selectionModeList->addItem(QStringLiteral("Row"),
                               int(QAbstract3DGraph::SelectionRow));
    selectionModeList->addItem(QStringLiteral("Bar and Row"),
                               int(QAbstract3DGraph::SelectionItemAndRow));
    selectionModeList->addItem(QStringLiteral("Column"),
                               int(QAbstract3DGraph::SelectionColumn));
    selectionModeList->addItem(QStringLiteral("Bar and Column"),
                               int(QAbstract3DGraph::SelectionItemAndColumn));
    selectionModeList->addItem(QStringLiteral("Row and Column"),
                               int(QAbstract3DGraph::SelectionRowAndColumn));
    selectionModeList->addItem(QStringLiteral("Bar, Row and Column"),
                               int(QAbstract3DGraph::SelectionItemRowAndColumn));
    selectionModeList->addItem(QStringLiteral("Slice into Row"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionRow));
    selectionModeList->addItem(QStringLiteral("Slice into Row and Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndRow));
    selectionModeList->addItem(QStringLiteral("Slice into Column"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionColumn));
    selectionModeList->addItem(QStringLiteral("Slice into Column and Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndColumn));
    selectionModeList->addItem(QStringLiteral("Multi: Bar, Row, Col"),
                               int(QAbstract3DGraph::SelectionItemRowAndColumn | QAbstract3DGraph::SelectionMultiSeries));
    selectionModeList->addItem(QStringLiteral("Multi, Slice: Row, Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndRow | QAbstract3DGraph::SelectionMultiSeries));
    selectionModeList->addItem(QStringLiteral("Multi, Slice: Col, Item"),
                               int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndColumn | QAbstract3DGraph::SelectionMultiSeries));
    selectionModeList->setCurrentIndex(1);

    backgroundCheckBox->setText(QStringLiteral("Show background"));
    backgroundCheckBox->setChecked(false);

    gridCheckBox->setText(QStringLiteral("Show grid"));
    gridCheckBox->setChecked(true);

    fontSizeSlider->setTickInterval(10);
    fontSizeSlider->setTickPosition(QSlider::TicksBelow);
    fontSizeSlider->setMinimum(1);
    fontSizeSlider->setValue(30);
    fontSizeSlider->setMaximum(100);

    fontList->setCurrentFont(QFont("Times New Roman"));

    shadowQuality->addItem(QStringLiteral("None"));
    shadowQuality->addItem(QStringLiteral("Low"));
    shadowQuality->addItem(QStringLiteral("Medium"));
    shadowQuality->addItem(QStringLiteral("High"));
    shadowQuality->addItem(QStringLiteral("Low Soft"));
    shadowQuality->addItem(QStringLiteral("Medium Soft"));
    shadowQuality->addItem(QStringLiteral("High Soft"));
    shadowQuality->setCurrentIndex(5);

    axisTitlesVisibleCB->setText(QStringLiteral("Axis titles visible"));
    axisTitlesVisibleCB->setChecked(true);

    axisLabelRotationSlider->setTickInterval(10);
    axisLabelRotationSlider->setTickPosition(QSlider::TicksBelow);
    axisLabelRotationSlider->setMinimum(0);
    axisLabelRotationSlider->setValue(30);
    axisLabelRotationSlider->setMaximum(90);

    timeSlider->setTickInterval(1);
    timeSlider->setTickPosition(QSlider::TicksBelow);
    timeSlider->setMinimum(0);
    timeSlider->setValue(0);
    timeSlider->setMaximum(400);

    vLayout->addWidget(labelButton, 0, Qt::AlignTop);
    vLayout->addWidget(zoomToSelectedButton, 0, Qt::AlignTop);
    vLayout->addWidget(backgroundCheckBox);
    vLayout->addWidget(gridCheckBox);
    vLayout->addWidget(smoothCheckBox);
    vLayout->addWidget(axisTitlesVisibleCB);
    vLayout->addWidget(new QLabel(QStringLiteral("Change bar style")));
    vLayout->addWidget(barStyleList);
    vLayout->addWidget(new QLabel(QStringLiteral("Change selection mode")));
    vLayout->addWidget(selectionModeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
    vLayout->addWidget(shadowQuality);
    vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
    vLayout->addWidget(fontList);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
    vLayout->addWidget(fontSizeSlider);
    vLayout->addWidget(new QLabel(QStringLiteral("Axis label rotation")));
    vLayout->addWidget(axisLabelRotationSlider, 1, Qt::AlignTop);
    vLayout->addWidget(new QLabel(QStringLiteral("Adjust time point")));
    vLayout->addWidget(timeSlider);

    QObject::connect(labelButton, &QPushButton::clicked, modifier,
                     &GraphModifier::changeLabelBackground);
    QObject::connect(zoomToSelectedButton, &QPushButton::clicked, modifier,
                     &GraphModifier::zoomToSelectedBar);

    QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
                     &GraphModifier::setBackgroundEnabled);
    QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
                     &GraphModifier::setGridEnabled);
    QObject::connect(smoothCheckBox, &QCheckBox::stateChanged, modifier,
                     &GraphModifier::setSmoothBars);

    QObject::connect(modifier, &GraphModifier::backgroundEnabledChanged,
                     backgroundCheckBox, &QCheckBox::setChecked);
    QObject::connect(modifier, &GraphModifier::gridEnabledChanged,
                     gridCheckBox, &QCheckBox::setChecked);

    QObject::connect(barStyleList, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeStyle(int)));

    QObject::connect(selectionModeList, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeSelectionMode(int)));

    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeTheme(int)));

    QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
                     SLOT(changeShadowQuality(int)));

    QObject::connect(modifier, &GraphModifier::shadowQualityChanged, shadowQuality,
                     &QComboBox::setCurrentIndex);
    QObject::connect(widgetgraph, &Q3DBars::shadowQualityChanged, modifier,
                     &GraphModifier::shadowQualityUpdatedByVisual);

    QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
                     &GraphModifier::changeFontSize);
    QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
                     &GraphModifier::changeFont);

    QObject::connect(modifier, &GraphModifier::fontSizeChanged, fontSizeSlider,
                     &QSlider::setValue);
    QObject::connect(modifier, &GraphModifier::fontChanged, fontList,
                     &QFontComboBox::setCurrentFont);

    QObject::connect(axisTitlesVisibleCB, &QCheckBox::stateChanged, modifier,
                     &GraphModifier::setAxisTitleVisibility);
    QObject::connect(axisLabelRotationSlider, &QSlider::valueChanged, modifier,
                     &GraphModifier::changeLabelRotation);

    QObject::connect(timeSlider, &QSlider::valueChanged, modifier,
                     &GraphModifier::changeTime);
    //    createWidgets();
};

// void QtBarWindow::createWindow()
// {

//     Q3DBars *widgetgraph = new Q3DBars();
//     QWidget *container = QWidget::createWindowContainer(widgetgraph);
//     QSize screenSize = widgetgraph->screen()->size();
//     container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
//     container->setMaximumSize(screenSize);
//     container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     container->setFocusPolicy(Qt::StrongFocus);

//     if (!widgetgraph->hasContext())
//     {
//         QMessageBox msgBox;
//         msgBox.setText("Couldn't initialize the OpenGL context.");
//         msgBox.exec();
//         //        return -1;  TODO: Add error here
//     }

//     QHBoxLayout *hLayout = new QHBoxLayout(this);
//     hLayout->addWidget(container, 1);
//     QVBoxLayout *vLayout = new QVBoxLayout();
//     hLayout->addLayout(vLayout);

//     QComboBox *themeList = new QComboBox(this);
//     QCheckBox *gridCheckBox = new QCheckBox(this);

//     GraphModifier *modifier = new GraphModifier(widgetgraph);

//     QPushButton *labelButton = new QPushButton(this);
//     QCheckBox *smoothCheckBox = new QCheckBox(this);
//     QComboBox *barStyleList = new QComboBox(this);
//     QPushButton *zoomToSelectedButton = new QPushButton(this);
//     QComboBox *selectionModeList = new QComboBox(this);
//     QCheckBox *backgroundCheckBox = new QCheckBox(this);
//     QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, this);
//     QFontComboBox *fontList = new QFontComboBox(this);
//     QComboBox *shadowQuality = new QComboBox(this);
//     QCheckBox *axisTitlesVisibleCB = new QCheckBox(this);
//     QSlider *axisLabelRotationSlider = new QSlider(Qt::Horizontal, this);
//     QSlider *timeSlider = new QSlider(Qt::Horizontal, this);

//     themeList->addItem(QStringLiteral("Qt"));
//     themeList->addItem(QStringLiteral("Primary Colors"));
//     themeList->addItem(QStringLiteral("Digia"));
//     themeList->addItem(QStringLiteral("Stone Moss"));
//     themeList->addItem(QStringLiteral("Army Blue"));
//     themeList->addItem(QStringLiteral("Retro"));
//     themeList->addItem(QStringLiteral("Ebony"));
//     themeList->addItem(QStringLiteral("Isabelle"));
//     themeList->setCurrentIndex(2);

//     labelButton->setText(QStringLiteral("Change label style"));

//     smoothCheckBox->setText(QStringLiteral("Smooth bars"));
//     smoothCheckBox->setChecked(false);

//     barStyleList->addItem(QStringLiteral("Bevel bar"), int(QAbstract3DSeries::MeshBevelBar));
//     barStyleList->addItem(QStringLiteral("Pyramid"), int(QAbstract3DSeries::MeshPyramid));
//     barStyleList->addItem(QStringLiteral("Cone"), int(QAbstract3DSeries::MeshCone));
//     barStyleList->addItem(QStringLiteral("Cylinder"), int(QAbstract3DSeries::MeshCylinder));
//     barStyleList->setCurrentIndex(0);

//     zoomToSelectedButton->setText(QStringLiteral("Zoom to selected bar"));

//     selectionModeList->addItem(QStringLiteral("None"),
//                                int(QAbstract3DGraph::SelectionNone));
//     selectionModeList->addItem(QStringLiteral("Bar"),
//                                int(QAbstract3DGraph::SelectionItem));
//     selectionModeList->addItem(QStringLiteral("Row"),
//                                int(QAbstract3DGraph::SelectionRow));
//     selectionModeList->addItem(QStringLiteral("Bar and Row"),
//                                int(QAbstract3DGraph::SelectionItemAndRow));
//     selectionModeList->addItem(QStringLiteral("Column"),
//                                int(QAbstract3DGraph::SelectionColumn));
//     selectionModeList->addItem(QStringLiteral("Bar and Column"),
//                                int(QAbstract3DGraph::SelectionItemAndColumn));
//     selectionModeList->addItem(QStringLiteral("Row and Column"),
//                                int(QAbstract3DGraph::SelectionRowAndColumn));
//     selectionModeList->addItem(QStringLiteral("Bar, Row and Column"),
//                                int(QAbstract3DGraph::SelectionItemRowAndColumn));
//     selectionModeList->addItem(QStringLiteral("Slice into Row"),
//                                int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionRow));
//     selectionModeList->addItem(QStringLiteral("Slice into Row and Item"),
//                                int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndRow));
//     selectionModeList->addItem(QStringLiteral("Slice into Column"),
//                                int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionColumn));
//     selectionModeList->addItem(QStringLiteral("Slice into Column and Item"),
//                                int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndColumn));
//     selectionModeList->addItem(QStringLiteral("Multi: Bar, Row, Col"),
//                                int(QAbstract3DGraph::SelectionItemRowAndColumn | QAbstract3DGraph::SelectionMultiSeries));
//     selectionModeList->addItem(QStringLiteral("Multi, Slice: Row, Item"),
//                                int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndRow | QAbstract3DGraph::SelectionMultiSeries));
//     selectionModeList->addItem(QStringLiteral("Multi, Slice: Col, Item"),
//                                int(QAbstract3DGraph::SelectionSlice | QAbstract3DGraph::SelectionItemAndColumn | QAbstract3DGraph::SelectionMultiSeries));
//     selectionModeList->setCurrentIndex(1);

//     backgroundCheckBox->setText(QStringLiteral("Show background"));
//     backgroundCheckBox->setChecked(false);

//     gridCheckBox->setText(QStringLiteral("Show grid"));
//     gridCheckBox->setChecked(true);

//     fontSizeSlider->setTickInterval(10);
//     fontSizeSlider->setTickPosition(QSlider::TicksBelow);
//     fontSizeSlider->setMinimum(1);
//     fontSizeSlider->setValue(30);
//     fontSizeSlider->setMaximum(100);

//     fontList->setCurrentFont(QFont("Times New Roman"));

//     shadowQuality->addItem(QStringLiteral("None"));
//     shadowQuality->addItem(QStringLiteral("Low"));
//     shadowQuality->addItem(QStringLiteral("Medium"));
//     shadowQuality->addItem(QStringLiteral("High"));
//     shadowQuality->addItem(QStringLiteral("Low Soft"));
//     shadowQuality->addItem(QStringLiteral("Medium Soft"));
//     shadowQuality->addItem(QStringLiteral("High Soft"));
//     shadowQuality->setCurrentIndex(5);

//     axisTitlesVisibleCB->setText(QStringLiteral("Axis titles visible"));
//     axisTitlesVisibleCB->setChecked(true);

//     axisLabelRotationSlider->setTickInterval(10);
//     axisLabelRotationSlider->setTickPosition(QSlider::TicksBelow);
//     axisLabelRotationSlider->setMinimum(0);
//     axisLabelRotationSlider->setValue(30);
//     axisLabelRotationSlider->setMaximum(90);

//     timeSlider->setTickInterval(1);
//     timeSlider->setTickPosition(QSlider::TicksBelow);
//     timeSlider->setMinimum(0);
//     timeSlider->setValue(0);
//     timeSlider->setMaximum(400);

//     vLayout->addWidget(labelButton, 0, Qt::AlignTop);
//     vLayout->addWidget(zoomToSelectedButton, 0, Qt::AlignTop);
//     vLayout->addWidget(backgroundCheckBox);
//     vLayout->addWidget(gridCheckBox);
//     vLayout->addWidget(smoothCheckBox);
//     vLayout->addWidget(axisTitlesVisibleCB);
//     vLayout->addWidget(new QLabel(QStringLiteral("Change bar style")));
//     vLayout->addWidget(barStyleList);
//     vLayout->addWidget(new QLabel(QStringLiteral("Change selection mode")));
//     vLayout->addWidget(selectionModeList);
//     vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
//     vLayout->addWidget(themeList);
//     vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
//     vLayout->addWidget(shadowQuality);
//     vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
//     vLayout->addWidget(fontList);
//     vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
//     vLayout->addWidget(fontSizeSlider);
//     vLayout->addWidget(new QLabel(QStringLiteral("Axis label rotation")));
//     vLayout->addWidget(axisLabelRotationSlider, 1, Qt::AlignTop);
//     vLayout->addWidget(new QLabel(QStringLiteral("Adjust time point")));
//     vLayout->addWidget(timeSlider);

//     QObject::connect(labelButton, &QPushButton::clicked, modifier,
//                      &GraphModifier::changeLabelBackground);
//     QObject::connect(zoomToSelectedButton, &QPushButton::clicked, modifier,
//                      &GraphModifier::zoomToSelectedBar);

//     QObject::connect(backgroundCheckBox, &QCheckBox::stateChanged, modifier,
//                      &GraphModifier::setBackgroundEnabled);
//     QObject::connect(gridCheckBox, &QCheckBox::stateChanged, modifier,
//                      &GraphModifier::setGridEnabled);
//     QObject::connect(smoothCheckBox, &QCheckBox::stateChanged, modifier,
//                      &GraphModifier::setSmoothBars);

//     QObject::connect(modifier, &GraphModifier::backgroundEnabledChanged,
//                      backgroundCheckBox, &QCheckBox::setChecked);
//     QObject::connect(modifier, &GraphModifier::gridEnabledChanged,
//                      gridCheckBox, &QCheckBox::setChecked);

//     QObject::connect(barStyleList, SIGNAL(currentIndexChanged(int)), modifier,
//                      SLOT(changeStyle(int)));

//     QObject::connect(selectionModeList, SIGNAL(currentIndexChanged(int)), modifier,
//                      SLOT(changeSelectionMode(int)));

//     QObject::connect(themeList, SIGNAL(currentIndexChanged(int)), modifier,
//                      SLOT(changeTheme(int)));

//     QObject::connect(shadowQuality, SIGNAL(currentIndexChanged(int)), modifier,
//                      SLOT(changeShadowQuality(int)));

//     QObject::connect(modifier, &GraphModifier::shadowQualityChanged, shadowQuality,
//                      &QComboBox::setCurrentIndex);
//     QObject::connect(widgetgraph, &Q3DBars::shadowQualityChanged, modifier,
//                      &GraphModifier::shadowQualityUpdatedByVisual);

//     QObject::connect(fontSizeSlider, &QSlider::valueChanged, modifier,
//                      &GraphModifier::changeFontSize);
//     QObject::connect(fontList, &QFontComboBox::currentFontChanged, modifier,
//                      &GraphModifier::changeFont);

//     QObject::connect(modifier, &GraphModifier::fontSizeChanged, fontSizeSlider,
//                      &QSlider::setValue);
//     QObject::connect(modifier, &GraphModifier::fontChanged, fontList,
//                      &QFontComboBox::setCurrentFont);

//     QObject::connect(axisTitlesVisibleCB, &QCheckBox::stateChanged, modifier,
//                      &GraphModifier::setAxisTitleVisibility);
//     QObject::connect(axisLabelRotationSlider, &QSlider::valueChanged, modifier,
//                      &GraphModifier::changeLabelRotation);

//     QObject::connect(timeSlider, &QSlider::valueChanged, modifier,
//                      &GraphModifier::changeTime);
// }
