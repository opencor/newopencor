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
// PMR window
//==============================================================================

#pragma once

//==============================================================================

#include "organisationwidget.h"
#include "pmrexposure.h"

//==============================================================================

namespace Ui {
    class PmrWindowWindow;
}

//==============================================================================

class QLabel;
class QLineEdit;
class QNetworkAccessManager;
class QNetworkReply;

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace PMRSupport {
    class PmrWebService;
}   // namespace PMRSupport

//==============================================================================

namespace PMRWindow {

//==============================================================================

class PmrWindowWidget;

//==============================================================================

class PmrWindowWindow : public Core::OrganisationWidget
{
    Q_OBJECT

public:
    explicit PmrWindowWindow(QWidget *pParent);
    ~PmrWindowWindow();

    virtual void retranslateUi();

protected:
    virtual void resizeEvent(QResizeEvent *pEvent);

private:
    Ui::PmrWindowWindow *mGui;

    QLabel *mFilterLabel;
    QLineEdit *mFilterValue;

    PMRSupport::PmrWebService *mPmrWebService;

    PmrWindowWidget *mPmrWindowWidget;

private slots:
    void on_actionReload_triggered();

    void filterValueChanged(const QString &pText);

    void busy(const bool &pBusy);

    void showWarning(const QString &pMessage);
    void showInformation(const QString &pMessage);

    void retrieveExposuresList(const bool &pVisible);

    void initializeWidget(const PMRSupport::PmrExposures &pExposures,
                          const QString &pErrorMessage,
                          const bool &pInternetConnectionAvailable);

    void showExposureFiles(const QString &pUrl);

    void cloneWorkspace(const QString &pUrl);
};

//==============================================================================

}   // namespace PMRWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
