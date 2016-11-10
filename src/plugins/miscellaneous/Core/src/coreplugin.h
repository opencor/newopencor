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
// Core plugin
//==============================================================================

#pragma once

//==============================================================================

#include "coreinterface.h"
#include "filehandlinginterface.h"
#include "filetypeinterface.h"
#include "guiinterface.h"
#include "i18ninterface.h"
#include "plugininfo.h"
#include "plugininterface.h"

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

PLUGININFO_FUNC CorePluginInfo();

//==============================================================================

static const auto FileGroup = QStringLiteral("File");

//==============================================================================

class CentralWidget;

//==============================================================================

class CorePlugin : public QObject, public CoreInterface,
                   public FileHandlingInterface, public GuiInterface,
                   public I18nInterface, public PluginInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.CorePlugin" FILE "coreplugin.json")

    Q_INTERFACES(OpenCOR::CoreInterface)
    Q_INTERFACES(OpenCOR::FileHandlingInterface)
    Q_INTERFACES(OpenCOR::GuiInterface)
    Q_INTERFACES(OpenCOR::I18nInterface)
    Q_INTERFACES(OpenCOR::PluginInterface)

public:
    explicit CorePlugin();

#include "coreinterface.inl"
#include "filehandlinginterface.inl"
#include "guiinterface.inl"
#include "i18ninterface.inl"
#include "plugininterface.inl"

private:
    CentralWidget *mCentralWidget;

    QAction *mFileNewFileAction;

    QAction *mFileOpenAction;
    QAction *mFileOpenRemoteAction;

    QAction *mFileReloadAction;

    QAction *mFileDuplicateAction;

    QAction *mFileLockedAction;

    QAction *mFileSaveAction;
    QAction *mFileSaveAsAction;
    QAction *mFileSaveAllAction;

    QAction *mFilePreviousAction;
    QAction *mFileNextAction;

    QAction *mFileCloseAction;
    QAction *mFileCloseAllAction;

    QAction *mOpenReloadSeparator;

    QMenu *mFileReopenSubMenu;

    QAction *mFileReopenMostRecentFileAction;
    QAction *mFileReopenSubMenuSeparator1;
    QAction *mFileReopenSubMenuSeparator2;
    QAction *mFileClearReopenSubMenuAction;

    FileTypeInterfaces mFileTypeInterfaces;

    QStringList mRecentFileNamesOrUrls;

    void updateNewModifiedSensitiveActions();

    void reopenFile(const QString &pFileName);

private slots:
    void newFile();

    void updateFileReopenMenu(const bool &pEnabled = true);

    void reopenRecentFile();
    void reopenMostRecentFile();
    void clearReopenSubMenu();
};

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
