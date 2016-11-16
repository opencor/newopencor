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
// File Browser window
//==============================================================================

#pragma once

//==============================================================================

#include "organisationwidget.h"

//==============================================================================

namespace Ui {
    class FileBrowserWindowWindow;
}

//==============================================================================

class QMenu;
class QModelIndex;

//==============================================================================

namespace OpenCOR {
namespace FileBrowserWindow {

//==============================================================================

class FileBrowserWindowWidget;

//==============================================================================

class FileBrowserWindowWindow : public Core::OrganisationWidget
{
    Q_OBJECT

public:
    explicit FileBrowserWindowWindow(QWidget *pParent);
    ~FileBrowserWindowWindow();

    virtual void retranslateUi();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

private:
    Ui::FileBrowserWindowWindow *mGui;

    FileBrowserWindowWidget *mFileBrowserWindowWidget;

    QMenu *mContextMenu;

private slots:
    void on_actionHome_triggered();
    void on_actionParent_triggered();
    void on_actionPrevious_triggered();
    void on_actionNext_triggered();

    void showCustomContextMenu() const;
    void itemDoubleClicked();
};

//==============================================================================

}   // namespace FileBrowserWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
