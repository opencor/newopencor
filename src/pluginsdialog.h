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
// Plugins dialog
//==============================================================================

#pragma once

//==============================================================================

#include "coreinterface.h"
#include "plugininfo.h"

//==============================================================================

#include <QDialog>
#include <QStyledItemDelegate>

//==============================================================================

class QSettings;
class QStandardItem;
class QStandardItemModel;

//==============================================================================

namespace Ui {
    class PluginsDialog;
}

//==============================================================================

namespace OpenCOR {

//==============================================================================

class PluginManager;

//==============================================================================

class PluginItemDelegate : public QStyledItemDelegate
{
public:
    virtual void paint(QPainter *pPainter, const QStyleOptionViewItem &pOption,
                       const QModelIndex &pIndex) const;
};

//==============================================================================

class PluginsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PluginsDialog(PluginManager *pPluginManager, QWidget *pParent);
    ~PluginsDialog();

    void loadSettings(QSettings *pSettings);
    void saveSettings(QSettings *pSettings) const;

private:
    Ui::PluginsDialog *mGui;

    PluginManager *mPluginManager;

    QStandardItemModel *mModel;

    QMap<QString, PluginInfo::Category> mMappedCategories;

    QList<QStandardItem *> mSelectablePluginItems;
    QList<QStandardItem *> mUnselectablePluginItems;

    QMap<QString, bool> mInitialLoadingStates;

    QMap<PluginInfo::Category, QStandardItem *> mPluginCategories;

    void newPluginCategory(const PluginInfo::Category &pCategory,
                           const QString &pName);

    QString statusDescription(Plugin *pPlugin) const;

    void selectFirstVisibleCategory();

private slots:
    void on_selectablePluginsCheckBox_toggled(bool pChecked);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void updateInformation(const QModelIndex &pNewIndex,
                           const QModelIndex &pOldIndex);
    void updatePluginsSelectedState(QStandardItem *pItem,
                                    const bool &pInitializing = false);

    void openLink(const QString &pLink) const;

    void apply();
};

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
