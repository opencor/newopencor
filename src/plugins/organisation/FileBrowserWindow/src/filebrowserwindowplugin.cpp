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
// File Browser window plugin
//==============================================================================

#include "coreguiutils.h"
#include "filebrowserwindowplugin.h"
#include "filebrowserwindowwindow.h"

//==============================================================================

#include <QMainWindow>
#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace FileBrowserWindow {

//==============================================================================

PLUGININFO_FUNC FileBrowserWindowPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin to access local files."));
    descriptions.insert("fr", QString::fromUtf8("une extension pour accéder aux fichiers locaux."));

    return new PluginInfo(PluginInfo::Organisation, true, false,
                          QStringList() << "Core",
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void FileBrowserWindowPlugin::retranslateUi()
{
    // Retranslate our file browser window action

    retranslateAction(mFileBrowserWindowAction, tr("File Browser"),
                      tr("Show/hide the File Browser window"));

    // Retranslate our file browser window

    mFileBrowserWindowWindow->retranslateUi();
}

//==============================================================================
// Plugin interface
//==============================================================================

void FileBrowserWindowPlugin::initializePlugin()
{
    // Create an action to show/hide our file browser window

    mFileBrowserWindowAction = Core::newAction(true, Core::mainWindow());

    // Create our file browser window

    mFileBrowserWindowWindow = new FileBrowserWindowWindow(Core::mainWindow());
}

//==============================================================================

void FileBrowserWindowPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void FileBrowserWindowPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins);

    // We don't handle this interface...
}

//==============================================================================

void FileBrowserWindowPlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve our file browser window settings

    pSettings->beginGroup(mFileBrowserWindowWindow->objectName());
        mFileBrowserWindowWindow->loadSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void FileBrowserWindowPlugin::saveSettings(QSettings *pSettings) const
{
    // Keep track of our file browser window settings

    pSettings->beginGroup(mFileBrowserWindowWindow->objectName());
        mFileBrowserWindowWindow->saveSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void FileBrowserWindowPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// Window interface
//==============================================================================

Qt::DockWidgetArea FileBrowserWindowPlugin::windowDefaultDockArea() const
{
    // Return our default dock area

    return Qt::LeftDockWidgetArea;
}

//==============================================================================

QAction * FileBrowserWindowPlugin::windowAction() const
{
    // Return our window action

    return mFileBrowserWindowAction;
}

//==============================================================================

QDockWidget * FileBrowserWindowPlugin::windowWidget() const
{
    // Return our window widget

    return mFileBrowserWindowWindow;
}

//==============================================================================

}   // namespace FileBrowserWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
