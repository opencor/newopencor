//==============================================================================
// Core plugin
//==============================================================================

#ifndef COREPLUGIN_H
#define COREPLUGIN_H

//==============================================================================

#include "coreinterface.h"
#include "guiinterface.h"
#include "i18ninterface.h"
#include "plugininfo.h"

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

PLUGININFO_FUNC CorePluginInfo();

//==============================================================================

static const QString FileGroup = "File";

//==============================================================================

class CentralWidget;

//==============================================================================

class CorePlugin : public QObject, public CoreInterface, public GuiInterface,
                   public I18nInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "OpenCOR.CorePlugin" FILE "coreplugin.json")

    Q_INTERFACES(OpenCOR::CoreInterface)
    Q_INTERFACES(OpenCOR::GuiInterface)
    Q_INTERFACES(OpenCOR::I18nInterface)

public:
    virtual void initialize();

    virtual void initializationsDone(const Plugins &pLoadedPlugins);

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual void loadingOfSettingsDone(const Plugins &pLoadedPlugins);

    virtual void handleArguments(const QStringList &pArguments);

    virtual void changeEvent(QEvent *pEvent);

    virtual void retranslateUi();

    virtual void fileOpened(const QString &pFileName);
    virtual void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);
    virtual void fileClosed(const QString &pFileName);

    virtual bool canClose();

private:
    CentralWidget *mCentralWidget;

    QAction *mFileOpenAction;
    QAction *mFileSaveAction;
    QAction *mFileSaveAsAction;
    QAction *mFileSaveAllAction;
    QAction *mFilePreviousAction;
    QAction *mFileNextAction;
    QAction *mFileCloseAction;
    QAction *mFileCloseAllAction;

    QMenu *mFileReopenSubMenu;

    QAction *mFileReopenSubMenuSeparator;
    QAction *mFileClearReopenSubMenuAction;

    QStringList mRecentFileNames;

    void updateFileReopenMenu();

    void retrieveBorderColor();
    void retrieveColors();

private Q_SLOTS:
    void openRecentFile();

    void clearReopenSubMenu();
};

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
