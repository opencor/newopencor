//==============================================================================
// GUI interface
//==============================================================================

#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H

//==============================================================================

#include "interface.h"

//==============================================================================

#include <QIcon>
#include <QList>
#include <QKeySequence>

//==============================================================================

class QAction;
class QDockWidget;
class QMainWindow;
class QMenu;
class QSettings;
class QTabBar;
class QToolBar;

//==============================================================================

namespace OpenCOR {

//==============================================================================

class Plugin;

//==============================================================================

namespace Core {
    class CentralWidget;
    class DockWidget;
}

//==============================================================================

class GuiMenuSettings
{
public:
    enum GuiMenuSettingsType {
        File,
        View
    };

    explicit GuiMenuSettings(const GuiMenuSettingsType &pType, QMenu *pMenu);
    explicit GuiMenuSettings(const GuiMenuSettingsType &pType, QAction *pAction,
                             QMenu *pMenu);

    GuiMenuSettingsType type() const;
    QAction * action() const;
    QMenu * menu() const;

private:
    GuiMenuSettingsType mType;
    QAction *mAction;
    QMenu *mMenu;
};

//==============================================================================

class GuiMenuActionSettings
{
public:
    enum GuiMenuActionSettingsType {
        File,
        FileNew,
        Tools
    };

    explicit GuiMenuActionSettings(const GuiMenuActionSettingsType &pType,
                                   QAction *pAction);

    GuiMenuActionSettingsType type() const;
    QAction * action() const;

private:
    GuiMenuActionSettingsType mType;
    QAction *mAction;
};

//==============================================================================

class GuiViewSettings
{
public:
    enum Mode {
        Editing,
        Simulation,
        Analysis
    };

    explicit GuiViewSettings(const Mode &pMode, const QStringList &pMimeTypes);

    Mode mode() const;
    QStringList mimeTypes() const;

private:
    Mode mMode;
    QStringList mMimeTypes;
};

//==============================================================================

class GuiWindowSettings
{
public:
    enum GuiWindowSettingsType {
        Organisation,
        Editing,
        Help
    };

    explicit GuiWindowSettings(const Qt::DockWidgetArea &pDefaultDockArea,
                               Core::DockWidget *pWindow,
                               const GuiWindowSettingsType &pType,
                               QAction *pAction);

    Qt::DockWidgetArea defaultDockArea() const;
    Core::DockWidget * window() const;
    GuiWindowSettingsType type() const;
    QAction * action() const;

private:
    Qt::DockWidgetArea mDefaultDockArea;
    Core::DockWidget *mWindow;
    GuiWindowSettingsType mType;
    QAction *mAction;
};

//==============================================================================

class GuiSettings
{
public:
    explicit GuiSettings();
    ~GuiSettings();

    void addMenu(const GuiMenuSettings::GuiMenuSettingsType &pType,
                 QMenu *pMenu);
    void addMenu(const GuiMenuSettings::GuiMenuSettingsType &pType,
                 QAction *pAction, QMenu *pMenu);
    void addMenuAction(const GuiMenuActionSettings::GuiMenuActionSettingsType &pType,
                       QAction *pAction = 0);
    void setCentralWidget(Core::CentralWidget *pCentralWidget);
    void addWindow(const Qt::DockWidgetArea &pDefaultDockArea,
                   Core::DockWidget *pWindow,
                   const GuiWindowSettings::GuiWindowSettingsType &pType,
                   QAction *pAction);
    void setView(const GuiViewSettings::Mode &pMode,
                 const QStringList &pMimeTypes);

    QList<GuiMenuSettings *> menus() const;
    QList<GuiMenuActionSettings *> menuActions() const;
    Core::CentralWidget *centralWidget() const;
    QList<GuiWindowSettings *> windows() const;
    GuiViewSettings * view() const;

private:
    QList<GuiMenuSettings *> mMenus;
    QList<GuiMenuActionSettings *> mMenuActions;
    Core::CentralWidget *mCentralWidget;
    GuiViewSettings * mView;
    QList<GuiWindowSettings *> mWindows;
};

//==============================================================================

class GuiInterface : public Interface
{
    friend class MainWindow;

public:
    explicit GuiInterface();

    virtual void changeEvent(QEvent *pEvent);

    virtual void updateGui(Plugin *pViewPlugin, const QString &pFileName);

    virtual QWidget * viewWidget(const QString &pFileName);
    virtual QWidget * removeViewWidget(const QString &pFileName);
    virtual QString viewName() const;

    virtual QIcon fileTabIcon(const QString &pFileName) const;

    virtual bool saveFile(const QString &pOldFileName,
                          const QString &pNewFileName);

    virtual void fileOpened(const QString &pFileName);
    virtual void fileSaved(const QString &pFileName);
    virtual void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);
    virtual void fileClosed(const QString &pFileName);

    virtual bool canClose();

    GuiSettings * guiSettings() const;

    static QMenu * newMenu(QMainWindow *pMainWindow,
                           const QString &pNameOrIconResource,
                           const bool &pProvidedName = true);

    static QAction * newAction(QMainWindow *pMainWindow, const bool &pCheckable,
                               const QString &pIconResource,
                               const QList<QKeySequence> &pKeySequences);
    static QAction * newAction(QMainWindow *pMainWindow, const bool &pCheckable,
                               const QString &pIconResource,
                               const QKeySequence::StandardKey &pStandardKey = QKeySequence::UnknownKey);
    static QAction * newAction(QMainWindow *pMainWindow,
                               const bool &pCheckable = false,
                               const QString &pIconResource = QString(),
                               const QKeySequence &pKeySequence = QKeySequence());

    static void connectDockWidgetToAction(QDockWidget *pDockWidget, QAction *pAction);

    static void retranslateMenu(QMenu *pMenu, const QString &pTitle);
    static void retranslateAction(QAction *pAction, const QString &pText,
                                  const QString &pStatusTip);

protected:
    QMainWindow *mMainWindow;

    GuiSettings *mGuiSettings;

    void loadWindowSettings(QSettings *pSettings,
                            Core::DockWidget *pWindow);
    void saveWindowSettings(QSettings *pSettings,
                            Core::DockWidget *pWindow) const;

    void loadViewSettings(QSettings *pSettings, QObject *pView);
    void saveViewSettings(QSettings *pSettings, QObject *pView) const;

private:
    virtual void destroy();

    void setMainWindow(QMainWindow *pMainWindow);
};

//==============================================================================

}   // namespace OpenCOR

//==============================================================================

Q_DECLARE_INTERFACE(OpenCOR::GuiInterface, "OpenCOR::GuiInterface")

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
