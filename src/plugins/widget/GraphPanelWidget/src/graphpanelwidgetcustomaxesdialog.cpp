/*******************************************************************************

Copyright The University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// Graph Panel widget custom axes dialog
//==============================================================================

#include "coreguiutils.h"
#include "graphpanelwidgetcustomaxesdialog.h"

//==============================================================================

#include "ui_graphpanelwidgetcustomaxesdialog.h"

//==============================================================================

#include <QPushButton>
#include <QRegularExpressionValidator>

//==============================================================================

namespace OpenCOR {
namespace GraphPanelWidget {

//==============================================================================

GraphPanelWidgetCustomAxesDialog::GraphPanelWidgetCustomAxesDialog(const double &pMinX,
                                                                   const double &pMaxX,
                                                                   const double &pMinY,
                                                                   const double &pMaxY,
                                                                   QWidget *pParent) :
    QDialog(pParent),
    mGui(new Ui::GraphPanelWidgetCustomAxesDialog)
{
    // Set up the GUI

    mGui->setupUi(this);

    connect(mGui->buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    // Only allow double numbers

    QRegularExpressionValidator *doubleRegExValidator = new QRegularExpressionValidator(QRegularExpression("^[+-]?(\\d+(\\.\\d*)?|\\.\\d+)([eE][+-]?\\d+)?$"), this);

    mGui->xMinValue->setValidator(doubleRegExValidator);
    mGui->xMaxValue->setValidator(doubleRegExValidator);
    mGui->yMinValue->setValidator(doubleRegExValidator);
    mGui->yMaxValue->setValidator(doubleRegExValidator);

    // Populate ourselves

    mGui->xMinValue->setText(QString::number(pMinX));
    mGui->xMaxValue->setText(QString::number(pMaxX));
    mGui->yMinValue->setText(QString::number(pMinY));
    mGui->yMaxValue->setText(QString::number(pMaxY));

    // Select all of the minimum X value

    mGui->xMinValue->selectAll();
}

//==============================================================================

GraphPanelWidgetCustomAxesDialog::~GraphPanelWidgetCustomAxesDialog()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

double GraphPanelWidgetCustomAxesDialog::minX() const
{
    // Return our minimum X value

    return mGui->xMinValue->text().toDouble();
}

//==============================================================================

double GraphPanelWidgetCustomAxesDialog::maxX() const
{
    // Return our maximum X value

    return mGui->xMaxValue->text().toDouble();
}

//==============================================================================

double GraphPanelWidgetCustomAxesDialog::minY() const
{
    // Return our minimum Y value

    return mGui->yMinValue->text().toDouble();
}

//==============================================================================

double GraphPanelWidgetCustomAxesDialog::maxY() const
{
    // Return our maximum Y value

    return mGui->yMaxValue->text().toDouble();
}

//==============================================================================

void GraphPanelWidgetCustomAxesDialog::checkValue(QLineEdit *pValue)
{
    // Check that we don't have an empty value and if we do then set the value
    // to zero and select it

    if (pValue->text().isEmpty()) {
        pValue->setText("0");
        pValue->selectAll();
    }
}

//==============================================================================


void GraphPanelWidgetCustomAxesDialog::on_xMinValue_textEdited(const QString &pValue)
{
    Q_UNUSED(pValue);

    // Check our X-min value

    checkValue(mGui->xMinValue);
}

//==============================================================================

void GraphPanelWidgetCustomAxesDialog::on_xMaxValue_textEdited(const QString &pValue)
{
    Q_UNUSED(pValue);

    // Check our X-max value

    checkValue(mGui->xMaxValue);
}

//==============================================================================

void GraphPanelWidgetCustomAxesDialog::on_yMinValue_textEdited(const QString &pValue)
{
    Q_UNUSED(pValue);

    // Check our Y-min value

    checkValue(mGui->yMinValue);
}

//==============================================================================

void GraphPanelWidgetCustomAxesDialog::on_yMaxValue_textEdited(const QString &pValue)
{
    Q_UNUSED(pValue);

    // Check our Y-max value

    checkValue(mGui->yMaxValue);
}

//==============================================================================

void GraphPanelWidgetCustomAxesDialog::on_buttonBox_accepted()
{
    // Check that the values make sense

    bool xProblem = minX() >= maxX();
    bool yProblem = minY() >= maxY();

    if (xProblem && yProblem) {
        Core::warningMessageBox(tr("Custom Axes"),
                                tr("X-min and Y-min must be lower than X-max and Y-max, respectively."));
    } else if (xProblem) {
        Core::warningMessageBox(tr("Custom Axes"),
                                tr("X-min must be lower than X-max."));
    } else if (yProblem) {
        Core::warningMessageBox(tr("Custom Axes"),
                                tr("Y-min must be lower than Y-max."));
    } else {
        // Confirm that we accepted the changes

        accept();
    }
}

//==============================================================================

}   // namespace GraphPanelWidget
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
