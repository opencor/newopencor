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
// File Organiser window plugin
//==============================================================================

#include "coreguiutils.h"
#include "fileorganiserwindowplugin.h"
#include "fileorganiserwindowwindow.h"

//==============================================================================

#include <QMainWindow>
#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace FileOrganiserWindow {

//==============================================================================

PLUGININFO_FUNC FileOrganiserWindowPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin to virtually organise files."));
    descriptions.insert("fr", QString::fromUtf8("une extension pour organiser virtuellement des fichiers."));

    return new PluginInfo(PluginInfo::Organisation, true, false,
                          QStringList() << "Core",
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void FileOrganiserWindowPlugin::retranslateUi()
{
    // Retranslate our file organiser window action

    retranslateAction(mFileOrganiserWindowAction, tr("File Organiser"),
                      tr("Show/hide the File Organiser window"));

    // Retranslate our file organiser window

    mFileOrganiserWindowWindow->retranslateUi();
}

//==============================================================================
// Plugin interface
//==============================================================================

void FileOrganiserWindowPlugin::initializePlugin()
{
    // Create an action to show/hide our file organiser window

    mFileOrganiserWindowAction = Core::newAction(true, Core::mainWindow());

    // Create our file organiser window

    mFileOrganiserWindowWindow = new FileOrganiserWindowWindow(Core::mainWindow());
}

//==============================================================================

void FileOrganiserWindowPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void FileOrganiserWindowPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins);

    // We don't handle this interface...
}

//==============================================================================

void FileOrganiserWindowPlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve our file organiser window settings

    pSettings->beginGroup(mFileOrganiserWindowWindow->objectName());
        mFileOrganiserWindowWindow->loadSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void FileOrganiserWindowPlugin::saveSettings(QSettings *pSettings) const
{
    // Keep track of our file organiser window settings

    pSettings->beginGroup(mFileOrganiserWindowWindow->objectName());
        mFileOrganiserWindowWindow->saveSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void FileOrganiserWindowPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// Window interface
//==============================================================================

Qt::DockWidgetArea FileOrganiserWindowPlugin::windowDefaultDockArea() const
{
    // Return our default dock area

    return Qt::LeftDockWidgetArea;
}

//==============================================================================

QAction * FileOrganiserWindowPlugin::windowAction() const
{
    // Return our window action

    return mFileOrganiserWindowAction;
}

//==============================================================================

QDockWidget * FileOrganiserWindowPlugin::windowWidget() const
{
    // Return our window widget

    return mFileOrganiserWindowWindow;
}

//==============================================================================

}   // namespace FileOrganiserWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
