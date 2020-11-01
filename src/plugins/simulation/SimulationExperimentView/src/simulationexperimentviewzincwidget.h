/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://gnu.org/licenses>.

*******************************************************************************/

//==============================================================================
// Zinc window
//==============================================================================

#pragma once

//==============================================================================

#include <QTimer>
#include <QtMath>

//==============================================================================

#include "qwtbegin.h"
    #include "qwt_wheel.h"
#include "qwtend.h"

//==============================================================================

#include "widget.h"

//==============================================================================

#include "zincbegin.h"
    #include "opencmiss/zinc/context.hpp"
    #include "opencmiss/zinc/field.hpp"
    #include "opencmiss/zinc/fieldvectoroperators.hpp"
    #include "opencmiss/zinc/fieldfiniteelement.hpp"
    #include "opencmiss/zinc/graphics.hpp"
    #include "opencmiss/zinc/timekeeper.hpp"
#include "zincend.h"

//==============================================================================

class QMenu;
class QLabel;
class QCheckBox;
class QSlider;
class QLineEdit;

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Core {
    class ToolBarWidget;
} // namespace Core

//==============================================================================

namespace ZincWidget {
    class ZincWidget;
} // namespace ZincWidget

//==============================================================================

namespace SimulationExperimentView {

//==============================================================================

class SimulationExperimentViewZincWidget : public Core::Widget
{
    Q_OBJECT

public:
    explicit SimulationExperimentViewZincWidget(QWidget *pParent);
    ~SimulationExperimentViewZincWidget() override;

    void retranslateUi() override;

    void loadMappingFile(QString pFileName);
    void loadZincMeshFiles(const QStringList &pZincMeshFiles);

    void initData(quint64 pDataSize, double pMinimumTime, double pMaximumTime,
                      double pTimeInterval, QMap<QString, double *> &pMapVariableValues);
    void addData(int pDataSize);

private:
    enum class GraphicsType {
        All,
        Axes,
        Points,
        Lines,
        Surfaces,
        Isosurfaces,
        Labels
    };

    struct _variable {
        QString component;
        QString variable;
    };

    static constexpr double nodeSizeOrigin = 0;
    static constexpr double nodeSixeExp = 1.1;
    double invLnNodeSizeExp = 1/qLn(nodeSixeExp);

    QAction *mActionAxes;
    QAction *mActionPoints;
    QAction *mActionLines;
    QAction *mActionSurfaces;
    QAction *mActionIsosurfaces;
    QAction *mActionLabels;

    bool mShuttingDown = false;

    ZincWidget::ZincWidget *mZincWidget;
    OpenCMISS::Zinc::Context mZincContext;

    QTimer mTimer;

    QLineEdit *mMaxValueLineEdit;
    QLineEdit *mMinValueLineEdit;
    QLineEdit *mDefaultValueLineEdit;
    QLabel *mMappingFileLabel;
    QLabel *mTimeLabel;
    QwtWheel *mNodeSizeWidget;
    QSlider *mTimeSlider;
    QCheckBox *mTimeCheckBox;
    QCheckBox *mLogCheckBox;
    QLineEdit *mLogAmpliLineEdit;
    Core::ToolBarWidget *mToolBarWidget;
    QwtWheel *mSpeedWidget;


    OpenCMISS::Zinc::Timekeeper mTimeKeeper;
    int mTimeStep = 1;
    double* mTimeValues;
    int mDataSize = 0;
    double mDefaultValue = 0.;
    double mValueMin = mDefaultValue;
    double mValueMax = mDefaultValue;

    QMap<int, double*> *mMapNodeValues = nullptr;
    OpenCMISS::Zinc::FieldFiniteElement mDataField;
    OpenCMISS::Zinc::Spectrum mSpectrum;

    QMap<int, _variable> *mMapNodeVariables = nullptr;

    char *mZincSceneViewerDescription = nullptr;

    OpenCMISS::Zinc::Field mCoordinates;
    OpenCMISS::Zinc::FieldMagnitude mMagnitude;
    OpenCMISS::Zinc::GraphicsPoints mAxes;
    OpenCMISS::Zinc::GraphicsPoints mPoints;
    OpenCMISS::Zinc::GraphicsPoints mLabels;
    OpenCMISS::Zinc::GraphicsLines mLines;
    OpenCMISS::Zinc::GraphicsSurfaces mSurfaces;
    OpenCMISS::Zinc::GraphicsContours mIsosurfaces;

    QStringList mZincMeshFileNames;

    QString mMappingFileName;

    int mAxesFontPointSize = 0;

    void showHideGraphics(GraphicsType pGraphicsType);
    void createAndSetZincContext();
    void initializeZincRegion();
    void updateNodeValues(int pValueBegin, int pValueEnd, bool pReset = false);

private slots:
    void graphicsInitialized();
    void devicePixelRatioChanged(int pDevicePixelRatio);
    void setNodeSizes(double pSize);

    void timeSliderValueChanged(int pTime);
    void timerTimeOut();
    void autoMode();

    void setTimeStep(int value);

    void actionAxesTriggered();
    void actionPointsTriggered();
    void actionLinesTriggered();
    void actionSurfacesTriggered();
    void actionIsosurfacesTriggered();
    void actionLabelsTriggered();

    void setSpectrumExageration(QString pValue);
    void setSpectrumScale();
    void setMaxValue(QString pValue);
    void setMinValue(QString pValue);
    void setDefaultValue(QString pValue);
};

//==============================================================================

} // namespace ZincWindow
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
