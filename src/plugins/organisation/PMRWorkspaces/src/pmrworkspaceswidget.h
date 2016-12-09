/*******************************************************************************

Licensed to the OpenCOR team under one or more contributor license agreements.
See the NOTICE.txt file distributed with this work for additional information
regarding copyright ownership. The OpenCOR team licenses this file to you under
the Apache License, Version 2.0 (the "License"); you may not use this file
except in compliance with the License. You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

*******************************************************************************/

//==============================================================================
// Workspaces widget
//==============================================================================

#pragma once

//==============================================================================

#include "commonwidget.h"
#include "corecliutils.h"
#include "pmrworkspacesmanager.h"
#include "webviewerwidget.h"

//==============================================================================

#include <QMap>
#include <QFileInfo>

//==============================================================================

class QContextMenuEvent ;
class QTimer;

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace PMRSupport {
    class PmrWebService;
}   // namespace PMRSupport

//==============================================================================

namespace PMRWorkspaces {

//==============================================================================

class PmrWorkspacesWidget : public OpenCOR::WebViewerWidget::WebViewerWidget, public Core::CommonWidget
{
    Q_OBJECT

public:
    explicit PmrWorkspacesWidget(PMRSupport::PmrWebService *pPmrWebService, QWidget *pParent);
    ~PmrWorkspacesWidget();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    void aboutWorkspace(const QString &pUrl);
    void addWorkspace(PMRSupport::PmrWorkspace *pWorkspace, const bool &pOwned=false);
    const QString addWorkspaceFolder(const QString &pFolder);
    void clearWorkspaces(void);
    void refreshWorkspace(const QString &pUrl);
    void refreshWorkspaceFile(const QString &pPath);
    void refreshWorkspaces(const bool &pScanFolders);

protected:
    virtual QSize sizeHint() const;

private:
    PMRSupport::PmrWebService *mPmrWebService;

    OpenCOR::PMRSupport::PmrWorkspacesManager *mWorkspacesManager;

    QMap<QString, QString> mWorkspaceFolders;                 // Folder name --> Url
    QMap<QString, QPair<QString, bool> > mWorkspaceUrls;      // Url --> (Folder name, mine)

    QString mCurrentWorkspaceUrl;
    QSet<QString> mExpandedItems;
    QString mSelectedItem;

    QString mTemplate;

    int mRow;
    int mRowAnchor;
    QMap<QString, int> mAnchors;                               // Item id --> Anchor

    QTimer *mTimer;

    void scanDefaultWorkspaceDirectory(void);

    void displayWorkspaces(void);
    void expandHtmlTree(const QString &pId);

    void scrollToSelected(void);
    void setSelected(QWebElement pNewSelectedRow);

    void setCurrentWorkspaceUrl(const QString &pUrl);

    static const QString actionHtml(const QList<QPair<QString, QString> > &pActions);
    QString containerHtml(const QString &pClass, const QString &pIcon,
                          const QString &pId, const QString &pName,
                          const QString &pStatus,
                          const QList<QPair<QString, QString> > &pActionList);
    QString contentsHtml(const PMRSupport::PmrWorkspaceFileNode *pFileNode, const bool &pHidden);
    QString emptyContentsHtml(void);

    static const QStringList fileStatusActionHtml(const QString &pPath,
                                                  const QPair<QChar, QChar> &pGitStatus);
    static const QStringList fileStatusActionHtml(const PMRSupport::PmrWorkspace *pWorkspace,
                                                  const QString &pPath);
    static const QStringList fileStatusActionHtml(const PMRSupport::PmrWorkspaceFileNode *pFileNode);

    QString fileHtml(const PMRSupport::PmrWorkspaceFileNode *pFileNode);
    QStringList folderHtml(const PMRSupport::PmrWorkspaceFileNode *pFileNode);
    QStringList workspaceHtml(const PMRSupport::PmrWorkspace *pWorkspace);

    static const QWebElement parentWorkspaceElement(const QWebElement &pRowElement);

    void cloneWorkspace(const QString &pUrl);
    void duplicateCloneMessage(const QString &pUrl,
                               const QString &pPath1, const QString &pPath2);

    void commitWorkspace(const QString &pUrl);
    void synchroniseWorkspace(const QString &pUrl, const bool pOnlyPull);

    void showInGraphicalShell(const QString &pPath);

    void startStopTimer();

signals:
    void openFileRequested(const QString &pFile);
    void information(const QString &pMessage);
    void warning(const QString &pMessage);

private slots:
    void focusWindowChanged();
    void refreshCurrentWorkspace();

public slots:
    void initialiseWorkspaceWidget(const PMRSupport::PmrWorkspaceList &pWorkspaces);
    void workspaceCloned(PMRSupport::PmrWorkspace *pWorkspace);
    void workspaceCreated(const QString &pUrl);
    void workspaceSynchronised(PMRSupport::PmrWorkspace *pWorkspace);
};

//==============================================================================

}   // namespace PMRWorkspaces
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
