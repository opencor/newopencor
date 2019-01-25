/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// Simulation Experiment view plugin
//==============================================================================

#include "cellmlfilemanager.h"
#include "cellmlsupportplugin.h"
#include "combinefilemanager.h"
#include "combinesupportplugin.h"
#include "sedmlfilemanager.h"
#include "sedmlsupportplugin.h"
#include "simulation.h"
#include "simulationexperimentviewplugin.h"
#include "simulationexperimentviewpreferenceswidget.h"
#include "simulationexperimentviewwidget.h"

//==============================================================================

#include <QMainWindow>
#include <QSettings>

//==============================================================================

namespace OpenCOR {
namespace SimulationExperimentView {

//==============================================================================

PLUGININFO_FUNC SimulationExperimentViewPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin to edit and run a simulation experiment."));
    descriptions.insert("fr", QString::fromUtf8("une extension pour éditer et exécuter une expérience de simulation."));

    return new PluginInfo(PluginInfo::Simulation, true, false,
                          QStringList() << "GraphPanelWidget" << "SimulationSupport",
                          descriptions);
}

//==============================================================================
// File handling interface
//==============================================================================

bool SimulationExperimentViewPlugin::saveFile(const QString &pOldFileName,
                                              const QString &pNewFileName,
                                              bool &pNeedFeedback)
{
    Q_UNUSED(pNeedFeedback);

    // Let our view widget know that we want to save a file

    return mViewWidget->saveFile(pOldFileName, pNewFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileOpened(const QString &pFileName)
{
    // Let our view widget know that a file has been opened

    mViewWidget->fileOpened(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::filePermissionsChanged(const QString &pFileName)
{
    // The given file has been un/locked, so let our view widget know about it

    mViewWidget->filePermissionsChanged(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileModified(const QString &pFileName)
{
    Q_UNUSED(pFileName);

    // We don't handle this interface...
}

//==============================================================================

void SimulationExperimentViewPlugin::fileSaved(const QString &pFileName)
{
    // The given file has been saved, so let our view widget know about it

    mViewWidget->fileSaved(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileReloaded(const QString &pFileName)
{
    // The given file has been reloaded, so let our view widget know about it

    mViewWidget->fileReloaded(pFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileRenamed(const QString &pOldFileName,
                                                 const QString &pNewFileName)
{
    // Let our view widget know that a file has been renamed

    mViewWidget->fileRenamed(pOldFileName, pNewFileName);
}

//==============================================================================

void SimulationExperimentViewPlugin::fileClosed(const QString &pFileName)
{
    // Let our view widget know that a file has been closed

    mViewWidget->fileClosed(pFileName);
}

//==============================================================================
// I18n interface
//==============================================================================

void SimulationExperimentViewPlugin::retranslateUi()
{
    // Retranslate our Simulation Experiment view widget

    mViewWidget->retranslateUi();
}

//==============================================================================
// Plugin interface
//==============================================================================

bool SimulationExperimentViewPlugin::definesPluginInterfaces()
{
    // We don't handle this interface...

    return false;
}

//==============================================================================

bool SimulationExperimentViewPlugin::pluginInterfacesOk(const QString &pFileName,
                                                        QObject *pInstance)
{
    Q_UNUSED(pFileName);
    Q_UNUSED(pInstance);

    // We don't handle this interface...

    return false;
}

//==============================================================================

void SimulationExperimentViewPlugin::initializePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SimulationExperimentViewPlugin::finalizePlugin()
{
    // We don't handle this interface...
}

//==============================================================================

void SimulationExperimentViewPlugin::pluginsInitialized(const Plugins &pLoadedPlugins)
{
    // Retrieve the different CellML capable editing and simulation views

    Plugins cellmlEditingViewPlugins = Plugins();
    Plugins cellmlSimulationViewPlugins = Plugins();

    for (auto plugin : pLoadedPlugins) {
        ViewInterface *viewInterface = qobject_cast<ViewInterface *>(plugin->instance());

        if (   viewInterface
            && (   (viewInterface->viewMode() == EditingMode)
                || (viewInterface->viewMode() == SimulationMode))) {
            QStringList viewMimeTypes = viewInterface->viewMimeTypes();

            if (   viewMimeTypes.isEmpty()
                || viewMimeTypes.contains(CellMLSupport::CellmlMimeType)) {
                if (viewInterface->viewMode() == EditingMode)
                    cellmlEditingViewPlugins << plugin;
                else
                    cellmlSimulationViewPlugins << plugin;
            }
        }
    }

    // Create our Simulation Experiment view widget

    mViewWidget = new SimulationExperimentViewWidget(this,
                                                     cellmlEditingViewPlugins,
                                                     cellmlSimulationViewPlugins,
                                                     Core::mainWindow());

    mViewWidget->setObjectName("SimulationExperimentViewWidget");

    // Hide our Simulation Experiment view widget since it may not initially be
    // shown in our central widget

    mViewWidget->hide();
}

//==============================================================================

void SimulationExperimentViewPlugin::loadSettings(QSettings *pSettings)
{
    // Retrieve our Simulation Experiment view settings

    pSettings->beginGroup(mViewWidget->objectName());
        mViewWidget->loadSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void SimulationExperimentViewPlugin::saveSettings(QSettings *pSettings) const
{
    // Keep track of our Simulation Experiment view settings

    pSettings->beginGroup(mViewWidget->objectName());
        mViewWidget->saveSettings(pSettings);
    pSettings->endGroup();
}

//==============================================================================

void SimulationExperimentViewPlugin::handleUrl(const QUrl &pUrl)
{
    Q_UNUSED(pUrl);

    // We don't handle this interface...
}

//==============================================================================
// Preferences interface
//==============================================================================

Preferences::PreferencesWidget * SimulationExperimentViewPlugin::preferencesWidget()
{
    // Return our preferences widget

    return new SimulationExperimentViewPreferencesWidget(Core::mainWindow());
}

//==============================================================================

void SimulationExperimentViewPlugin::preferencesChanged(const QStringList &pPluginNames)
{
    Q_UNUSED(pPluginNames);

    // We don't handle this interface...
}

//==============================================================================
// View interface
//==============================================================================

ViewInterface::Mode SimulationExperimentViewPlugin::viewMode() const
{
    // Return our mode

    return SimulationMode;
}

//==============================================================================

QStringList SimulationExperimentViewPlugin::viewMimeTypes() const
{
    // Return the MIME types we support

    return QStringList() << CellMLSupport::CellmlMimeType
                         << SEDMLSupport::SedmlMimeType
                         << COMBINESupport::CombineMimeType;
}

//==============================================================================

QString SimulationExperimentViewPlugin::viewMimeType(const QString &pFileName) const
{
    // Return the MIME type for the given CellML file
    // Note: we should never return an empty string...

    if (CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName))
        return CellMLSupport::CellmlMimeType;
    else if (SEDMLSupport::SedmlFileManager::instance()->sedmlFile(pFileName))
        return SEDMLSupport::SedmlMimeType;
    else if (COMBINESupport::CombineFileManager::instance()->combineArchive(pFileName))
        return COMBINESupport::CombineMimeType;
    else
        return QString();
}

//==============================================================================

QString SimulationExperimentViewPlugin::viewDefaultFileExtension() const
{
    // Return the default file extension we support, based on the file type of
    // our simulation

    return (mViewWidget->simulationWidget()->simulation()->fileType() == SimulationSupport::Simulation::CellmlFile)?
               CellMLSupport::CellmlFileExtension:
               SEDMLSupport::SedmlFileExtension;
}

//==============================================================================

QWidget * SimulationExperimentViewPlugin::viewWidget(const QString &pFileName)
{
    // Make sure that we are dealing with a valid file

    if (!mViewWidget->isValid(pFileName))
        return nullptr;

    // Update and return our simulation view widget using the given CellML file,
    // SED-ML file or COMBINE archive

    mViewWidget->initialize(pFileName);

    return mViewWidget;
}

//==============================================================================

void SimulationExperimentViewPlugin::removeViewWidget(const QString &pFileName)
{
    // Ask our view widget to finalise the given CellML file

    mViewWidget->finalize(pFileName);
}

//==============================================================================

QString SimulationExperimentViewPlugin::viewName() const
{
    // Return our Simulation Experiment view's name

    return tr("Simulation Experiment");
}

//==============================================================================

QIcon SimulationExperimentViewPlugin::fileTabIcon(const QString &pFileName) const
{
    // Return the requested file tab icon

    return mViewWidget->fileTabIcon(pFileName);
}

//==============================================================================

}   // namespace SimulationExperimentView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
