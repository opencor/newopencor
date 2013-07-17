//==============================================================================
// Single cell view information simulation widget
//==============================================================================

#ifndef SINGLECELLVIEWINFORMATIONSIMULATIONWIDGET_H
#define SINGLECELLVIEWINFORMATIONSIMULATIONWIDGET_H

//==============================================================================

#include "propertyeditorwidget.h"

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace CellMLSupport {
    class CellmlFileRuntime;
}   // namespace CellMLSupport

//==============================================================================

namespace SingleCellView {

//==============================================================================

class SingleCellViewSimulation;

//==============================================================================

class SingleCellViewInformationSimulationWidget : public Core::PropertyEditorWidget
{
    Q_OBJECT

public:
    explicit SingleCellViewInformationSimulationWidget(QWidget *pParent = 0);
    ~SingleCellViewInformationSimulationWidget();

    virtual void retranslateUi();

    void initialize(const QString &pFileName,
                    CellMLSupport::CellmlFileRuntime *pRuntime,
                    SingleCellViewSimulation *pSimulation);
    void backup(const QString &pFileName);
    void finalize(const QString &pFileName);

    Core::Property * startingPointProperty() const;
    Core::Property * endingPointProperty() const;
    Core::Property * pointIntervalProperty() const;

    double startingPoint() const;
    double endingPoint() const;
    double pointInterval() const;

private:
    Core::Property *mStartingPointProperty;
    Core::Property *mEndingPointProperty;
    Core::Property *mPointIntervalProperty;

    QMap<QString, Core::PropertyEditorWidgetGuiState *> mGuiStates;
    Core::PropertyEditorWidgetGuiState *mDefaultGuiState;

    void updateToolTips();
};

//==============================================================================

}   // namespace SingleCellView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
