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
// Check for updates dialog
//==============================================================================

#include "checkforupdatesdialog.h"
#include "cliutils.h"
#include "mainwindow.h"

//==============================================================================

#include "ui_checkforupdatesdialog.h"

//==============================================================================

#include <QCoreApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QSettings>
#include <QTextEdit>
#include <QVariantMap>

//==============================================================================

namespace OpenCOR {

//==============================================================================

CheckForUpdatesEngine::CheckForUpdatesEngine(const QString &pApplicationDate) :
    mApplicationDate(pApplicationDate),
    mStatus(QString()),
    mNewerVersions(QStringList())
{
}

//==============================================================================

void CheckForUpdatesEngine::check()
{
    // Retrieve some information about the different versions of OpenCOR that
    // are available

    QByteArray fileVersionsContents = QByteArray();
    QString errorMessage = QString();

    mNewerVersions.clear();

    if (readFileContentsFromUrl("http://www.opencor.ws/downloads/index.js", fileVersionsContents, &errorMessage)) {
        QJsonParseError jsonParseError;
        QJsonDocument versions = QJsonDocument::fromJson(fileVersionsContents.mid(15, fileVersionsContents.length()-17), &jsonParseError);

        if (jsonParseError.error == QJsonParseError::NoError) {
            QVariantMap versionMap;
            int versionMajor, versionMinor, versionPatch;
            int versionDay, versionMonth, versionYear;
            QString versionVersion;
            QString versionDate;

            foreach (const QVariant &version, versions.object().toVariantMap()["versions"].toList()) {
                // Retrieve the version and date of the current version

                versionMap = version.toMap();

                versionMajor = versionMap["major"].toInt();
                versionMinor = versionMap["minor"].toInt();
                versionPatch = versionMap["patch"].toInt();

                versionDay = versionMap["day"].toInt();
                versionMonth = versionMap["month"].toInt();
                versionYear = versionMap["year"].toInt();

                versionDate = QString("%1-%2-%3").arg(versionYear)
                                                 .arg(versionMonth, 2, 10, QChar('0'))
                                                 .arg(versionDay, 2, 10, QChar('0'));

                if (!versionMajor && !versionMinor && !versionPatch) {
                    versionVersion = versionDate;
                } else {
                    versionVersion = QString("%1.%2").arg(versionMajor, versionMinor);

                    if (versionPatch)
                        versionVersion = QString("%1.%2").arg(versionVersion, versionPatch);
                }

                // Check whether the version is newer and, if so, add it to our list

                if (mApplicationDate.compare(versionDate) < 0)
                    mNewerVersions << versionVersion;
            }

            mStatus = QString();
        } else {
            mStatus =  QObject::tr("The version information is mal-formatted.")
                      +"<br/>"
                      +QObject::tr("Please <a href=\"http://opencor.ws/user/contactUs.html\">contact us</a> about this error.");
        }
    } else {
        mStatus = formatMessage(errorMessage, false, true);
    }
}

//==============================================================================

QString CheckForUpdatesEngine::status() const
{
    // Return our status

    return mStatus;
}

//==============================================================================

QStringList CheckForUpdatesEngine::newerVersions() const
{
    // Return our newer versions

    return mNewerVersions;
}

//==============================================================================

bool CheckForUpdatesEngine::hasNewerVersion() const
{
    // Return whether we have a newer version

    return !mNewerVersions.isEmpty();
}

//==============================================================================

bool CheckForUpdatesEngine::hasNewerOfficialVersion() const
{
    // Return whether we have a newer official version

    foreach (const QString &newerVersion, mNewerVersions) {
        if (!newerVersion.contains("-"))
            return true;
    }

    return false;
}

//==============================================================================

void CheckForUpdatesDialog::constructor(const QString &pApplicationDate,
                                        CheckForUpdatesEngine *pEngine)
{
    // We are not yet initialised

    mInitialized = false;

    // Set up the GUI

    mGui = new Ui::CheckForUpdatesDialog;

    mGui->setupUi(this);

    // Create/set our engine and check for updates in the former case

    if (pEngine) {
        mEngine = pEngine;
    } else {
        mEngine = new CheckForUpdatesEngine(pApplicationDate);

        mEngine->check();
    }

    // Retrieve and set some properties

    QSettings settings;

    settings.beginGroup(objectName());

    mGui->checkForUpdatesAtStartupCheckBox->setChecked(settings.value(SettingsCheckForUpdatesAtStartup, true).toBool());
    mGui->includeSnapshotsCheckBox->setChecked(settings.value(SettingsIncludeSnapshots, false).toBool());

    // Update our GUI

    updateGui();

    mInitialized = true;
}

//==============================================================================

CheckForUpdatesDialog::CheckForUpdatesDialog(const QString &pApplicationDate,
                                             QWidget *pParent) :
    QDialog(pParent)
{
    // Construct our dialog

    constructor(pApplicationDate, 0);
}

//==============================================================================

CheckForUpdatesDialog::CheckForUpdatesDialog(CheckForUpdatesEngine *pEngine) :
    QDialog()
{
    // Construct our dialog

    constructor(QString(), pEngine);
}

//==============================================================================

CheckForUpdatesDialog::~CheckForUpdatesDialog()
{
    // Keep track of some properties

    QSettings settings;

    settings.beginGroup(objectName());

    settings.setValue(SettingsCheckForUpdatesAtStartup,
                      mGui->checkForUpdatesAtStartupCheckBox->isChecked());
    settings.setValue(SettingsIncludeSnapshots,
                      mGui->includeSnapshotsCheckBox->isChecked());

    // Delete some internal objects

    delete mEngine;

    // Delete the GUI

    delete mGui;
}

//==============================================================================

void CheckForUpdatesDialog::updateGui()
{
    // Determine the status of our check

    static const QString WhatIsNewUrl = "http://opencor.ws/user/whatIsNew.html?#";

    QString versionInformation = tr("<a href=\"%1\">%2 %3</a> is ready for you to <a href=\"http://opencor.ws/downloads/\">download</a>.");
    QString snapshotInformation = tr("The <a href=\"%1\">latest snapshot</a> is ready for you to <a href=\"http://opencor.ws/downloads/\">download</a>.");

    if (mEngine->status().isEmpty()) {
        if (mGui->includeSnapshotsCheckBox->checkState() == Qt::Checked) {
            if (mEngine->hasNewerVersion()) {
                // The user is after either an official version or a snapshot of
                // OpenCOR, so first look for a snapshot and go for an official
                // version, if no snapshot is available

                QString version = QString();

                foreach (const QString &newerVersion, mEngine->newerVersions()) {
                    if (newerVersion.contains("-")) {
                        version = newerVersion;

                        break;
                    }
                }

                if (version.isEmpty())
                    version = mEngine->newerVersions().first();

                if (version.contains("-"))
                    mGui->statusLabel->setText(snapshotInformation.arg(WhatIsNewUrl+"latest"));
                else
                    mGui->statusLabel->setText(versionInformation.arg(WhatIsNewUrl+version, qAppName(), version));
            } else {
                mGui->statusLabel->setText(tr("No newer version or snapshot of %1 is available.").arg(qAppName()));
            }
        } else if (mEngine->hasNewerOfficialVersion()) {
            // The user is only after an official version of OpenCOR, so look
            // for the first one available

            QString version = QString();

            foreach (const QString &newerVersion, mEngine->newerVersions()) {
                if (!newerVersion.contains("-")) {
                    version = newerVersion;

                    break;
                }
            }

            mGui->statusLabel->setText(versionInformation.arg(WhatIsNewUrl+version, qAppName(), version));
        } else {
            mGui->statusLabel->setText(tr("No newer version of %1 is available.").arg(qAppName()));
        }
    } else {
        mGui->statusLabel->setText(mEngine->status());
    }
}

//==============================================================================

void CheckForUpdatesDialog::on_buttonBox_accepted()
{
    // Simply close ourselves

    close();
}

//==============================================================================

void CheckForUpdatesDialog::on_statusLabel_linkActivated(const QString &pLink)
{
    // Open the link in the user's browser

    QDesktopServices::openUrl(pLink);
}

//==============================================================================

void CheckForUpdatesDialog::on_recheckButton_clicked()
{
    // Recheck for updates and then update our GUI accordingly

    mEngine->check();

    updateGui();
}

//==============================================================================

void CheckForUpdatesDialog::on_includeSnapshotsCheckBox_toggled(bool pChecked)
{
    Q_UNUSED(pChecked);

    // Update the GUI, but only if we are initialised (since updateGui() gets
    // called when initialising ourselves)

    if (mInitialized)
        updateGui();
}

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
