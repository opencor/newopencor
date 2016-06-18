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
// Graph Panel widget custom axes window
//==============================================================================

#include "graphpanelwidgetcustomaxeswindow.h"

//==============================================================================

#include "ui_graphpanelwidgetcustomaxeswindow.h"

//==============================================================================

namespace OpenCOR {
namespace GraphPanelWidget {

//==============================================================================

GraphPanelWidgetCustomAxesWindow::GraphPanelWidgetCustomAxesWindow(QWidget *pParent) :
    QDialog(pParent),
    mGui(new Ui::GraphPanelWidgetCustomAxesWindow)
{
    // Set up the GUI

    mGui->setupUi(this);
}

//==============================================================================

GraphPanelWidgetCustomAxesWindow::~GraphPanelWidgetCustomAxesWindow()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void GraphPanelWidgetCustomAxesWindow::on_buttonBox_accepted()
{
    // Confirm that we accepted the changes

    accept();
}

//==============================================================================

void GraphPanelWidgetCustomAxesWindow::on_buttonBox_rejected()
{
    // Simply cancel whatever was done here

    reject();
}

//==============================================================================

}   // namespace GraphPanelWidget
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
