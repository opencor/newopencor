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
// SED-ML support plugin
//==============================================================================

#include "corecliutils.h"
#include "coreguiutils.h"
#include "filemanager.h"
#include "sedmlfilemanager.h"
#include "sedmlsupportplugin.h"

//==============================================================================

#include <QAction>
#include <QMainWindow>

//==============================================================================

namespace OpenCOR {
namespace SEDMLSupport {

//==============================================================================

PLUGININFO_FUNC SEDMLSupportPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin to support <a href=\"http://www.sed-ml.org/\">SED-ML</a>."));
    descriptions.insert("fr", QString::fromUtf8("une extension pour supporter <a href=\"http://www.sed-ml.org/\">SED-ML</a>."));

    return new PluginInfo(PluginInfo::Support, false, false,
                          QStringList() << "CellMLSupport" << "SEDMLAPI" << "StandardSupport",
                          descriptions);
}

//==============================================================================
// File interface
//==============================================================================

bool SEDMLSupportPlugin::isFile(const QString &pFileName) const
{
    // Return whether the given file is of the type that we support

    return SedmlFileManager::instance()->isFile(pFileName);
}

//==============================================================================

QString SEDMLSupportPlugin::mimeType() const
{
    // Return the MIME type we support

    return SedmlMimeType;
}

//==============================================================================

QString SEDMLSupportPlugin::fileExtension() const
{
    // Return the extension of the type of file we support

    return SedmlFileExtension;
}

//==============================================================================

QString SEDMLSupportPlugin::fileTypeDescription() const
{
    // Return the description of the type of file we support

    return tr("SED-ML File");
}

//==============================================================================

QStringList SEDMLSupportPlugin::fileTypeDefaultViews() const
{
    // Return the default views to use for the type of file we support

    return QStringList() << "SingleCellView" << "RawSEDMLView" << "RawTextView";
}

//==============================================================================
// GUI interface
//==============================================================================

void SEDMLSupportPlugin::updateGui(Plugin *pViewPlugin, const QString &pFileName)
{
    Q_UNUSED(pViewPlugin);
    Q_UNUSED(pFileName);

    // We don't handle this interface...
}

//==============================================================================

Gui::Menus SEDMLSupportPlugin::guiMenus() const
{
    // We don't handle this interface...

    return Gui::Menus();
}

//==============================================================================

Gui::MenuActions SEDMLSupportPlugin::guiMenuActions() const
{
    // Return our menu actions

    return Gui::MenuActions() << Gui::MenuAction(Gui::MenuAction::FileNew, mFileNewSedmlFileAction);
}

//==============================================================================
// I18n interface
//==============================================================================

void SEDMLSupportPlugin::retranslateUi()
{
    // Retranslate our different actions

    retranslateAction(mFileNewSedmlFileAction, tr("SED-ML File"),
                      tr("Create a new SED-ML file"));
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SEDMLSupportPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SEDMLSupportPlugin::pluginInterfacesOk(const QString &pFileName,
                                            QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SEDMLSupportPlugin::initializePlugin()
{
    // Create our different actions

    mFileNewSedmlFileAction = Core::newAction(Core::mainWindow());

    // Some connections to handle our different actions

    connect(mFileNewSedmlFileAction, SIGNAL(triggered(bool)),
            this, SLOT(newSedmlFile()));
}

//==============================================================================

void SEDMLSupportPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SEDMLSupportPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    Q_UNUSED(pLoadedPlugins);

    // Make a call to the instance of the SED-ML file manager so that it gets
    // properly set up (and therefore can start managing SED-ML files) before it
    // actually gets used by other plugins
    // Note: we do it here rather than in initialize() since we need the Core
    //       plugin to be initialised (so we can get access to our 'global' file
    //       manager)...

    SedmlFileManager::instance();
}

//==============================================================================

void SEDMLSupportPlugin::loadSettings(QSettings *pSettings)
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void SEDMLSupportPlugin::saveSettings(QSettings *pSettings) const
{
    Q_UNUSED(pSettings);

    // We don't handle this interface...
}

//==============================================================================

void SEDMLSupportPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// Plugin specific
//==============================================================================

void SEDMLSupportPlugin::newSedmlFile()
{
    // Create a new SED-ML file by asking our file manager to create one

    Core::FileManager *fileManagerInstance = Core::FileManager::instance();
#ifdef QT_DEBUG
    Core::FileManager::Status createStatus =
#endif
    fileManagerInstance->create(QString(),
                                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                "<sedML xmlns=\"http://sed-ml.org/sed-ml/level1/version2\" level=\"1\" version=\"2\">\n"
                                "    <!-- Your code goes here-->\n"
                                "</sedML>\n");

#ifdef QT_DEBUG
    // Make sure that the file has indeed been created

    if (createStatus != Core::FileManager::Created)
        qFatal("FATAL ERROR | %s:%d: the file was not created.", __FILE__, __LINE__);
#endif
}

//==============================================================================

}   // namespace SEDMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
