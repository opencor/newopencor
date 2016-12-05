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
// Sample window plugin
//==============================================================================

#include "coreguiutils.h"
#include "samplewindowplugin.h"
#include "samplewindowwindow.h"

//==============================================================================

#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace SampleWindow {

//==============================================================================

PLUGININFO_FUNC SampleWindowPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin that provides an addition window."));
    descriptions.insert("fr", QString::fromUtf8("une extension qui fournit une fenêtre d'addition."));

    return new PluginInfo(PluginInfo::Sample, true, false,
                          QStringList() << "Core" << "Sample",
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void SampleWindowPlugin::retranslateUi()
{
    // Retranslate our sample window action

    retranslateAction(mSampleWindowAction,
                      tr("Sample"),
                      tr("Show/hide the Sample window"));
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SampleWindowPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SampleWindowPlugin::pluginInterfacesOk(const QString &pFileName,
                                            QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SampleWindowPlugin::initializePlugin()
{
    // Create an action to show/hide our sample window

    mSampleWindowAction = Core::newAction(true, Core::mainWindow());

    // Create our sample window

    mSampleWindowWindow = new SampleWindowWindow(Core::mainWindow());
}

//==============================================================================

void SampleWindowPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SampleWindowPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins);

    // We don't handle this interface...
}

//==============================================================================

void SampleWindowPlugin::loadSettings(QSettings *pSettings)
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void SampleWindowPlugin::saveSettings(QSettings *pSettings) const
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void SampleWindowPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// Window interface
//==============================================================================

Qt::DockWidgetArea SampleWindowPlugin::windowDefaultDockArea() const
{
    // Return our default dock area

    return Qt::TopDockWidgetArea;
}

//==============================================================================

QAction * SampleWindowPlugin::windowAction() const
{
    // Return our window action

    return mSampleWindowAction;
}

//==============================================================================

QDockWidget * SampleWindowPlugin::windowWidget() const
{
    // Return our window widget

    return mSampleWindowWindow;
}

//==============================================================================

}   // namespace SampleWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
