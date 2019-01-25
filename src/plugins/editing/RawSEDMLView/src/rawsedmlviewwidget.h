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
// Raw SED-ML view widget
//==============================================================================

#pragma once

//==============================================================================

#include "corecliutils.h"
#include "viewwidget.h"

//==============================================================================

#include <QMap>

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace SEDMLEditingView {
    class SedmlEditingViewWidget;
}   // namespace SEDMLEditingView

//==============================================================================

namespace EditorWidget {
    class EditorWidget;
}   // namespace EditorWidget

//==============================================================================

namespace RawSEDMLView {

//==============================================================================

class RawSedmlViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit RawSedmlViewWidget(QWidget *pParent);

    void loadSettings(QSettings *pSettings) override;
    void saveSettings(QSettings *pSettings) const override;

    void retranslateUi() override;

    bool isValid(const QString &pFileName) const;

    void initialize(const QString &pFileName, bool pUpdate = true);
    void finalize(const QString &pFileName);

    void fileSaved(const QString &pFileName);
    void fileReloaded(const QString &pFileName);
    void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);

    EditorWidget::EditorWidget * editorWidget(const QString &pFileName) const;

    QWidget * widget(const QString &pFileName) override;

    QList<QWidget *> statusBarWidgets() const override;

    void reformat(const QString &pFileName);

    bool validate(const QString &pFileName, bool pOnlyErrors = false) const;

private:
    bool mNeedLoadingSettings;
    QString mSettingsGroup;

    SEDMLEditingView::SedmlEditingViewWidget *mEditingWidget;
    QMap<QString, SEDMLEditingView::SedmlEditingViewWidget *> mEditingWidgets;
};

//==============================================================================

}   // namespace RawSEDMLView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
