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
// Pretty CellML view widget
//==============================================================================

#ifndef PRETTYCELLMLVIEWWIDGET_H
#define PRETTYCELLMLVIEWWIDGET_H

//==============================================================================

#include "corecliutils.h"
#include "viewwidget.h"

//==============================================================================

#include <QMap>

//==============================================================================

namespace Ui {
    class PrettyCellmlViewWidget;
}

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace CoreCellMLEditing {
    class CoreCellmlEditingWidget;
}   // namespace CoreCellMLEditing

//==============================================================================

namespace Editor {
    class EditorWidget;
}   // namespace Editor

//==============================================================================

namespace QScintillaSupport {
    class QScintillaWidget;
}   // namespace QScintillaSupport

//==============================================================================

namespace PrettyCellMLView {

//==============================================================================

class PrettyCellmlViewWidgetData
{
public:
    explicit PrettyCellmlViewWidgetData(CoreCellMLEditing::CoreCellmlEditingWidget *pEditingWidget = 0,
                                        const QString &pSha1 = QString(),
                                        const bool &pValid = false,
                                        QDomDocument pRdfNodes = QDomDocument());

    CoreCellMLEditing::CoreCellmlEditingWidget * editingWidget() const;
    QString sha1() const;
    bool isValid() const;
    QDomDocument rdfNodes() const;

private:
    CoreCellMLEditing::CoreCellmlEditingWidget *mEditingWidget;
    QString mSha1;
    bool mValid;
    QDomDocument mRdfNodes;
};

//==============================================================================

class PrettyCellmlViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit PrettyCellmlViewWidget(QWidget *pParent);
    ~PrettyCellmlViewWidget();

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual void retranslateUi();

    bool contains(const QString &pFileName) const;

    void initialize(const QString &pFileName, const bool &pUpdate = true);
    void finalize(const QString &pFileName);

    void fileReloaded(const QString &pFileName);
    void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);

    Editor::EditorWidget * editor(const QString &pFileName) const;

    bool isEditorUseable(const QString &pFileName) const;
    bool isEditorContentsModified(const QString &pFileName) const;

    virtual QList<QWidget *> statusBarWidgets() const;

private:
    Ui::PrettyCellmlViewWidget *mGui;

    bool mNeedLoadingSettings;
    QString mSettingsGroup;

    CoreCellMLEditing::CoreCellmlEditingWidget *mEditingWidget;

    QMap<QString, PrettyCellmlViewWidgetData> mData;

    void commentOrUncommentLine(QScintillaSupport::QScintillaWidget *editor,
                                const int &pLineNumber,
                                const bool &pCommentLine);

private Q_SLOTS:
    void editorKeyPressed(QKeyEvent *pEvent, bool &pHandled);

    void selectFirstItemInEditorList();
};

//==============================================================================

}   // namespace PrettyCellMLView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
