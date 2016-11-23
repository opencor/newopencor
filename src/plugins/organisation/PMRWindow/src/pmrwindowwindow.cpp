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

#include "borderedwidget.h"
#include "corecliutils.h"
#include "coreguiutils.h"
#include "pmrwebservice.h"
#include "pmrwindowwidget.h"
#include "pmrwindowwindow.h"
#include "toolbarwidget.h"

//==============================================================================

#include "ui_pmrwindowwindow.h"

//==============================================================================

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QTimer>

//==============================================================================

namespace OpenCOR {
namespace PMRWindow {

//==============================================================================

PmrWindowWindow::PmrWindowWindow(QWidget *pParent) :
    Core::OrganisationWidget(pParent),
    mGui(new Ui::PmrWindowWindow)
{
    // Set up the GUI

    mGui->setupUi(this);

    // Create a tool bar widget with a URL value and refresh button
    // Note: the spacers is a little trick to improve the rendering of our tool
    //       bar widget...

    Core::ToolBarWidget *toolBarWidget = new Core::ToolBarWidget(this);
    QWidget *spacer = new QWidget(toolBarWidget);

    spacer->setMinimumSize(0, 0);
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mFilterLabel = new QLabel(toolBarWidget);
    mFilterValue = new QLineEdit(toolBarWidget);

    QFont font = mFilterLabel->font();

    font.setBold(true);

    mFilterLabel->setFont(font);

#ifdef Q_OS_MAC
    mFilterValue->setAttribute(Qt::WA_MacShowFocusRect, false);
    // Note: the above removes the focus border since it messes up the look of
    //       our URL value widget...
#endif

    connect(mFilterValue, SIGNAL(textChanged(const QString &)),
            this, SLOT(filterValueChanged(const QString &)));

    toolBarWidget->addWidget(spacer);
    toolBarWidget->addWidget(mFilterLabel);
    toolBarWidget->addWidget(mFilterValue);
    toolBarWidget->addAction(mGui->actionReload);

    mGui->layout->addWidget(toolBarWidget);

    // Make the filter value our focus proxy

    setFocusProxy(mFilterValue);

    // Create and add the PMR widget

    mPmrWindowWidget = new PmrWindowWidget(this);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    mGui->layout->addWidget(new Core::BorderedWidget(mPmrWindowWidget,
                                                     true, true, true, true));
#elif defined(Q_OS_MAC)
    mGui->layout->addWidget(new Core::BorderedWidget(mPmrWindowWidget,
                                                     true, false, false, false));
#else
    #error Unsupported platform
#endif

    // Keep track of the window's visibility, so that we can request the list of
    // exposures, if necessary

    connect(this, SIGNAL(visibilityChanged(bool)),
            this, SLOT(retrieveExposuresList(const bool &)));

    // Create and add the PMR web service

    mPmrWebService = new PMRSupport::PmrWebService();

    // Some connections to process responses from the PMR web service

    connect(mPmrWebService, SIGNAL(busy(const bool &)),
            this, SLOT(busy(const bool &)));

    connect(mPmrWebService, SIGNAL(information(const QString &)),
            this, SLOT(showInformation(const QString &)));
    connect(mPmrWebService, SIGNAL(warning(const QString &)),
            this, SLOT(showWarning(const QString &)));

    connect(mPmrWebService, SIGNAL(exposuresList(const PMRSupport::PmrExposures &, const QString &, const bool &)),
            this, SLOT(initializeWidget(const PMRSupport::PmrExposures &, const QString &, const bool &)));

    connect(mPmrWebService, SIGNAL(addExposureFiles(const QString &, const QStringList &)),
            mPmrWindowWidget, SLOT(addExposureFiles(const QString &, const QStringList &)));
    connect(mPmrWebService, SIGNAL(showExposureFiles(const QString &)),
            mPmrWindowWidget, SLOT(showExposureFiles(const QString &)));

    // Some connections to know what our PMR widget wants from us

    connect(mPmrWindowWidget, SIGNAL(cloneWorkspaceRequested(const QString &)),
            this, SLOT(cloneWorkspace(const QString &)));
    connect(mPmrWindowWidget, SIGNAL(showExposureFilesRequested(const QString &)),
            this, SLOT(showExposureFiles(const QString &)));

    connect(mPmrWindowWidget, SIGNAL(openExposureFileRequested(const QString &)),
            this, SLOT(openFile(const QString &)));

    // Some further initialisations that are done as part of retranslating the
    // GUI (so that they can be updated when changing languages)

    retranslateUi();
}

//==============================================================================

PmrWindowWindow::~PmrWindowWindow()
{
    // Delete our PMR web service

    delete mPmrWebService;

    // Delete the GUI

    delete mGui;
}

//==============================================================================

void PmrWindowWindow::retranslateUi()
{
    // Retranslate the whole window and our information note message

    mGui->retranslateUi(this);

    mFilterLabel->setText(tr("Filter:"));

    // Retranslate our PMR widget

    mPmrWindowWidget->retranslateUi();
}

//==============================================================================

void PmrWindowWindow::resizeEvent(QResizeEvent *pEvent)
{
    // Default handling of the event

    Core::OrganisationWidget::resizeEvent(pEvent);

    // Resize our busy widget

    mPmrWindowWidget->resizeBusyWidget();
}

//==============================================================================

void PmrWindowWindow::filterValueChanged(const QString &pText)
{
    // Ask our PMR widget to filter its output using the given regular
    // expression

    mPmrWindowWidget->filter(pText);
}

//==============================================================================

void PmrWindowWindow::busy(const bool &pBusy)
{
    // Show ourselves as busy or not busy anymore

    static int counter = 0;

    counter += pBusy?1:-1;

    if (pBusy && (counter == 1)) {
        mPmrWindowWidget->showBusyWidget();

        mGui->dockWidgetContents->setEnabled(false);
    } else if (!pBusy && !counter) {
        // Re-enable the GUI side and give, within the current window, the focus
        // to mFilterValue, but only if the current window already has the focus

        mPmrWindowWidget->hideBusyWidget();

        mGui->dockWidgetContents->setEnabled(true);

        Core::setFocusTo(mFilterValue);
    }
}

//==============================================================================

void PmrWindowWindow::showWarning(const QString &pMessage)
{
    // Show the given message as a warning

    Core::warningMessageBox(Core::mainWindow(), windowTitle(), pMessage);
}

//==============================================================================

void PmrWindowWindow::showInformation(const QString &pMessage)
{
    // Show the given message as informative text

    Core::informationMessageBox(Core::mainWindow(), windowTitle(), pMessage);
}

//==============================================================================

void PmrWindowWindow::on_actionReload_triggered()
{
    // Get the list of exposures from our PMR web service

    mPmrWebService->requestExposuresList();
}

//==============================================================================

void PmrWindowWindow::initializeWidget(const PMRSupport::PmrExposures &pExposures,
                                       const QString &pErrorMessage,
                                       const bool &pInternetConnectionAvailable)
{
    // Ask our PMR widget to initialise itself

    mPmrWindowWidget->initialize(pExposures, pErrorMessage,
                                 mFilterValue->text(),
                                 pInternetConnectionAvailable);
}

//==============================================================================

void PmrWindowWindow::retrieveExposuresList(const bool &pVisible)
{
    // Retrieve the list of exposures, if we are becoming visible and the list
    // of exposures has never been requested before (through a single shot, this
    // to allow other events, such as the one asking OpenCOR's main window to
    // resize itself, to be handled properly)

    static bool firstTime = true;

    if (pVisible && firstTime) {
        firstTime = false;

        QTimer::singleShot(0, this, SLOT(on_actionReload_triggered()));
    }
}

//==============================================================================

void PmrWindowWindow::cloneWorkspace(const QString &pUrl)
{
    // Retrieve the name of an empty directory

    QString dirName = Core::getExistingDirectory(tr("Select Empty Directory"),
                                                 QString(), true);

    if (!dirName.isEmpty()) {
        // We have got a directory name where we can clone the workspace, so
        // request a clone of it

        mPmrWebService->cloneWorkspace(pUrl, dirName);
    }
}

//==============================================================================

void PmrWindowWindow::showExposureFiles(const QString &pUrl)
{
    // Request a list of the exposure's files from our PMR web service

    mPmrWebService->requestExposureFiles(pUrl);
}

//==============================================================================

}   // namespace PMRWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
