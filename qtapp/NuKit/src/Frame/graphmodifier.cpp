/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphmodifier.h"
//#include <algorithm> //std::min_element, std::max_element
using namespace QtDataVisualization;

const QString celsiusString = QString(QChar(0xB0)) + "C";

//! [0]
GraphModifier::GraphModifier(Q3DBars *bargraph)
    : m_graph(bargraph),
      m_xRotation(0.0f),
      m_yRotation(0.0f),
      m_fontSize(30),
      m_segments(4),
      m_subSegments(3),
      m_minval(-20.0f),
      m_maxval(20.0f),
      m_timeRow(0),
      //! [1]
      m_densityAxis(new QValue3DAxis),
      m_ZAxis(new QCategory3DAxis),
      m_NAxis(new QCategory3DAxis),
      m_primarySeries(new QBar3DSeries),
      //! [1]
      m_barMesh(QAbstract3DSeries::MeshBevelBar),
      m_smooth(false)
{
    //! [2]
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftMedium);
    m_graph->activeTheme()->setBackgroundEnabled(true);
    m_graph->activeTheme()->setFont(QFont("Times New Roman", m_fontSize));
    m_graph->activeTheme()->setLabelBackgroundEnabled(true);
    m_graph->activeTheme()->setType(Q3DTheme::Theme(2)); // Make the graph default theme "Digia"
    m_graph->setMultiSeriesUniform(true);
    //! [2]

    //! [3]
    m_densityAxis->setTitle("Nucleus Density at Specified Spatio Point");
    m_densityAxis->setSegmentCount(m_segments);
    m_densityAxis->setSubSegmentCount(m_subSegments);
    m_densityAxis->setRange(m_minval, m_maxval);
    //    It is a good example to set special symbols in QString, celsiusString do it
    //    m_densityAxis->setLabelFormat(QString(QStringLiteral("%.1f ") + celsiusString));
    m_densityAxis->setLabelFormat(QString(QStringLiteral("%.1f ")));
    m_densityAxis->setLabelAutoRotation(30.0f);
    m_densityAxis->setTitleVisible(true);

    m_ZAxis->setTitle("Nucleus Z number");
    m_ZAxis->setLabelAutoRotation(30.0f);
    m_ZAxis->setTitleVisible(true);
    m_NAxis->setTitle("Nucleus N number");
    m_NAxis->setLabelAutoRotation(30.0f);
    m_NAxis->setTitleVisible(true);

    m_graph->setValueAxis(m_densityAxis);
    m_graph->setRowAxis(m_ZAxis);
    m_graph->setColumnAxis(m_NAxis);
    //! [3]

    //! [8]
    m_primarySeries->setItemLabelFormat(QStringLiteral("Z:@rowLabel, N:@colLabel, Density:@valueLabel"));
    m_primarySeries->setMesh(QAbstract3DSeries::MeshBevelBar);
    m_primarySeries->setMeshSmooth(false);
    //! [8]

    //! [4]
    m_graph->addSeries(m_primarySeries);
    //! [4]

    //! [6]
    changePresetCamera();
    //! [6]

    //! [9]
    findElementsRange();
    resetData();
    //! [9]

    // Set up property animations for zooming to the selected bar
    //! [12]
    Q3DCamera *camera = m_graph->scene()->activeCamera();
    m_defaultAngleX = camera->xRotation();
    m_defaultAngleY = camera->yRotation();
    m_defaultZoom = camera->zoomLevel();
    m_defaultTarget = camera->target();

    m_animationCameraX.setTargetObject(camera);
    m_animationCameraY.setTargetObject(camera);
    m_animationCameraZoom.setTargetObject(camera);
    m_animationCameraTarget.setTargetObject(camera);

    m_animationCameraX.setPropertyName("xRotation");
    m_animationCameraY.setPropertyName("yRotation");
    m_animationCameraZoom.setPropertyName("zoomLevel");
    m_animationCameraTarget.setPropertyName("target");

    int duration = 1700;
    m_animationCameraX.setDuration(duration);
    m_animationCameraY.setDuration(duration);
    m_animationCameraZoom.setDuration(duration);
    m_animationCameraTarget.setDuration(duration);

    // The zoom always first zooms out above the graph and then zooms in
    qreal zoomOutFraction = 0.3;
    m_animationCameraX.setKeyValueAt(zoomOutFraction, QVariant::fromValue(0.0f));
    m_animationCameraY.setKeyValueAt(zoomOutFraction, QVariant::fromValue(90.0f));
    m_animationCameraZoom.setKeyValueAt(zoomOutFraction, QVariant::fromValue(50.0f));
    m_animationCameraTarget.setKeyValueAt(zoomOutFraction,
                                          QVariant::fromValue(QVector3D(0.0f, 0.0f, 0.0f)));
    //! [12]
}
//! [0]

GraphModifier::~GraphModifier()
{
    delete m_graph;
}

void GraphModifier::findElementsRange()
{
    const QStringList elementName = CSVParser(QString("../data/vElement.csv")).getHeader();
    const QVector<QVector<int>> elementNums = CSVParser(QString("../data/vElement.csv")).getiDataWHead();
    m_Znum = elementNums[1];
    m_maxZnum = m_Znum[0]; m_minZnum = m_Znum[0];
    m_Nnum = elementNums[2];
    m_maxNnum = m_Nnum[0]; m_minNnum = m_Nnum[0];
//    m_maxZnum = *std::max_element(&m_Znum[0], &m_Znum[-1]);
//    m_minZnum = *std::min_element(&m_Znum[0], &m_Znum[-1]);
//    m_maxNnum = *std::max_element(&m_Nnum[0], &m_Nnum[-1]);
//    m_minNnum = *std::min_element(&m_Nnum[0], &m_Nnum[-1]);
//    qDebug() << "Is it right?" << m_maxNnum;
    for (int i = 0; i < m_Znum.size()-1; i++)
    {
        if (m_minZnum > m_Znum[i])
            m_minZnum = m_Znum[i];
        if (m_maxZnum < m_Znum[i])
            m_maxZnum = m_Znum[i];
        if (m_minNnum > m_Nnum[i])
            m_minNnum = m_Nnum[i];
        if (m_maxNnum < m_Nnum[i])
            m_maxNnum = m_Nnum[i];
    }
}

void GraphModifier::resetData()
{
    //! [5]
    // Set up data
    static const QStringList dataHeader = CSVParser(QString("../data/oderun.csv")).getHeader();
    static const QVector<QVector<double>> spdata = CSVParser(QString("../data/oderun.csv")).getdDataWHead();
    static const QStringList elementName = CSVParser(QString("../data/vElement.csv")).getHeader();
    static const QVector<QVector<int>> elementNums = CSVParser(QString("../data/vElement.csv")).getiDataWHead();

    double max_density = 0, sum_density = 0;
    qDebug() << "I am resetting the data.";

    int Zlen = m_maxZnum - m_minZnum + 1;
    int Nlen = m_maxNnum - m_minNnum + 1;
    qDebug() << "I am resetting the data. 2 ";

    // Create data arrays
    QBarDataArray *dataSet = new QBarDataArray;
    QBarDataRow *dataRow;

    QStringList Zrange, Nrange;
    for (int i = 0; i < Zlen; i++)
        Zrange << QString::number(i + m_minZnum);
    for (int i = 0; i < Nlen; i++)
        Nrange << QString::number(i + m_minNnum);

    dataSet->reserve(Zlen);
    for (int Z = 0; Z < Zlen; Z++)
    {
        dataRow = new QBarDataRow(Nlen);
        for (int N = 0; N < Nlen; N++)
        {
            (*dataRow)[N].setValue(0);
        }
        // Add the row to the set
        dataSet->append(dataRow);
    };
    qDebug() << "This is time_index" << m_timeRow;
    for (int i = 0; i < m_Znum.size()-1; i++)
    {
        if ((m_minZnum <= m_Znum[i]) && (m_maxZnum >= m_Znum[i]))
        {
            if ((m_minNnum <= m_Nnum[i]) && (m_maxNnum >= m_Nnum[i]))
            {
                if (max_density < spdata[m_timeRow][i+2])
                    max_density = spdata[m_timeRow][i+2];
                sum_density += spdata[m_timeRow][i+2];

                int reducedZ = m_Znum[i] - m_minZnum;
                int reducedN = m_Nnum[i] - m_minNnum;
                dataRow = dataSet->at(reducedZ);
                (*dataRow)[reducedN].setValue(spdata[m_timeRow][i+2]);
            }
        }
    }

    // Add data to the data proxy (the data proxy assumes ownership of it)
    m_primarySeries->dataProxy()->resetArray(dataSet, Zrange, Nrange);
    m_densityAxis->setRange(0, sum_density);
    //! [5]
}

void GraphModifier::changeTime(int time_index)
{
    m_timeRow = time_index;
    resetData();
}



void GraphModifier::changeStyle(int style)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox)
    {
        m_barMesh = QAbstract3DSeries::Mesh(comboBox->itemData(style).toInt());
        m_primarySeries->setMesh(m_barMesh);
    }
}

void GraphModifier::changePresetCamera()
{
    m_animationCameraX.stop();
    m_animationCameraY.stop();
    m_animationCameraZoom.stop();
    m_animationCameraTarget.stop();

    // Restore camera target in case animation has changed it
    m_graph->scene()->activeCamera()->setTarget(QVector3D(0.0f, 0.0f, 0.0f));

    //! [10]
    static int preset = Q3DCamera::CameraPresetFront;

    m_graph->scene()->activeCamera()->setCameraPreset((Q3DCamera::CameraPreset)preset);

    if (++preset > Q3DCamera::CameraPresetDirectlyBelow)
        preset = Q3DCamera::CameraPresetFrontLow;
    //! [10]
}

void GraphModifier::changeTheme(int theme)
{
    Q3DTheme *currentTheme = m_graph->activeTheme();
    currentTheme->setType(Q3DTheme::Theme(theme));
    emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
    emit gridEnabledChanged(currentTheme->isGridEnabled());
    emit fontChanged(currentTheme->font());
    emit fontSizeChanged(currentTheme->font().pointSize());
}

void GraphModifier::changeLabelBackground()
{
    m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void GraphModifier::changeSelectionMode(int selectionMode)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox)
    {
        int flags = comboBox->itemData(selectionMode).toInt();
        m_graph->setSelectionMode(QAbstract3DGraph::SelectionFlags(flags));
    }
}

void GraphModifier::changeFont(const QFont &font)
{
    QFont newFont = font;
    m_graph->activeTheme()->setFont(newFont);
}

void GraphModifier::changeFontSize(int fontsize)
{
    m_fontSize = fontsize;
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(font);
}

void GraphModifier::shadowQualityUpdatedByVisual(QAbstract3DGraph::ShadowQuality sq)
{
    int quality = int(sq);
    // Updates the UI component to show correct shadow quality
    emit shadowQualityChanged(quality);
}

void GraphModifier::changeLabelRotation(int rotation)
{
    m_densityAxis->setLabelAutoRotation(float(rotation));
    m_NAxis->setLabelAutoRotation(float(rotation));
    m_ZAxis->setLabelAutoRotation(float(rotation));
}

void GraphModifier::setAxisTitleVisibility(bool enabled)
{
    m_densityAxis->setTitleVisible(enabled);
    m_NAxis->setTitleVisible(enabled);
    m_ZAxis->setTitleVisible(enabled);
}

void GraphModifier::setAxisTitleFixed(bool enabled)
{
    m_densityAxis->setTitleFixed(enabled);
    m_NAxis->setTitleFixed(enabled);
    m_ZAxis->setTitleFixed(enabled);
}

//! [11]
void GraphModifier::zoomToSelectedBar()
{
    m_animationCameraX.stop();
    m_animationCameraY.stop();
    m_animationCameraZoom.stop();
    m_animationCameraTarget.stop();

    Q3DCamera *camera = m_graph->scene()->activeCamera();
    float currentX = camera->xRotation();
    float currentY = camera->yRotation();
    float currentZoom = camera->zoomLevel();
    QVector3D currentTarget = camera->target();

    m_animationCameraX.setStartValue(QVariant::fromValue(currentX));
    m_animationCameraY.setStartValue(QVariant::fromValue(currentY));
    m_animationCameraZoom.setStartValue(QVariant::fromValue(currentZoom));
    m_animationCameraTarget.setStartValue(QVariant::fromValue(currentTarget));

    QPoint selectedBar = m_graph->selectedSeries()
                             ? m_graph->selectedSeries()->selectedBar()
                             : QBar3DSeries::invalidSelectionPosition();

    if (selectedBar != QBar3DSeries::invalidSelectionPosition())
    {
        // Normalize selected bar position within axis range to determine target coordinates
        //! [13]
        QVector3D endTarget;
        float xMin = m_graph->columnAxis()->min();
        float xRange = m_graph->columnAxis()->max() - xMin;
        float zMin = m_graph->rowAxis()->min();
        float zRange = m_graph->rowAxis()->max() - zMin;
        endTarget.setX((selectedBar.y() - xMin) / xRange * 2.0f - 1.0f);
        endTarget.setZ((selectedBar.x() - zMin) / zRange * 2.0f - 1.0f);
        //! [13]

        // Rotate the camera so that it always points approximately to the graph center
        //! [15]
        qreal endAngleX = 90.0 - qRadiansToDegrees(qAtan(qreal(endTarget.z() / endTarget.x())));
        if (endTarget.x() > 0.0f)
            endAngleX -= 180.0f;
        float barValue = m_graph->selectedSeries()->dataProxy()->itemAt(selectedBar.x(),
                                                                        selectedBar.y())
                             ->value();
        float endAngleY = barValue >= 0.0f ? 30.0f : -30.0f;
        if (m_graph->valueAxis()->reversed())
            endAngleY *= -1.0f;
        //! [15]

        m_animationCameraX.setEndValue(QVariant::fromValue(float(endAngleX)));
        m_animationCameraY.setEndValue(QVariant::fromValue(endAngleY));
        m_animationCameraZoom.setEndValue(QVariant::fromValue(250));
        //! [14]
        m_animationCameraTarget.setEndValue(QVariant::fromValue(endTarget));
        //! [14]
    }
    else
    {
        // No selected bar, so return to the default view
        m_animationCameraX.setEndValue(QVariant::fromValue(m_defaultAngleX));
        m_animationCameraY.setEndValue(QVariant::fromValue(m_defaultAngleY));
        m_animationCameraZoom.setEndValue(QVariant::fromValue(m_defaultZoom));
        m_animationCameraTarget.setEndValue(QVariant::fromValue(m_defaultTarget));
    }

    m_animationCameraX.start();
    m_animationCameraY.start();
    m_animationCameraZoom.start();
    m_animationCameraTarget.start();
}
//! [11]

void GraphModifier::changeShadowQuality(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    m_graph->setShadowQuality(sq);
    emit shadowQualityChanged(quality);
}

//! [7]
void GraphModifier::rotateX(int rotation)
{
    m_xRotation = rotation;
    m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}

void GraphModifier::rotateY(int rotation)
{
    m_yRotation = rotation;
    m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}
//! [7]

void GraphModifier::setBackgroundEnabled(int enabled)
{
    m_graph->activeTheme()->setBackgroundEnabled(bool(enabled));
}

void GraphModifier::setGridEnabled(int enabled)
{
    m_graph->activeTheme()->setGridEnabled(bool(enabled));
}

void GraphModifier::setSmoothBars(int smooth)
{
    m_smooth = bool(smooth);
    m_primarySeries->setMeshSmooth(m_smooth);
}

void GraphModifier::setReverseValueAxis(int enabled)
{
    m_graph->valueAxis()->setReversed(enabled);
}

void GraphModifier::setReflection(bool enabled)
{
    m_graph->setReflection(enabled);
}
