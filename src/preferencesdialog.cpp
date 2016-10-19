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
// Preferences dialog
//==============================================================================

#include "cliutils.h"
#include "guiutils.h"
#include "pluginmanager.h"
#include "preferencesdialog.h"
#include "preferencesinterface.h"

//==============================================================================

#include "ui_preferencesdialog.h"

//==============================================================================

#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>

//==============================================================================

namespace OpenCOR {

//==============================================================================

PluginCategoryWidget::PluginCategoryWidget(QWidget *pParent) :
    QWidget(pParent)
{
    // Create a form widget layout

    QFormLayout *formWidgetLayout = new QFormLayout(this);

    formWidgetLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    setLayout(formWidgetLayout);

    // Add both our category and description fields to our form widget layout

    mCategoryValue = new QLabel(this);
    mDescriptionValue = new QLabel(this);

    formWidgetLayout->addRow(label(tr("Category:")), mCategoryValue);
    formWidgetLayout->addRow(label(tr("Description:")), mDescriptionValue);
}

//==============================================================================

void PluginCategoryWidget::setCategory(const QString &pCategory)
{
    // Set our category

    mCategoryValue->setText(pCategory);
}

//==============================================================================

void PluginCategoryWidget::setDescription(const QString &pDescription)
{
    // Set our description

    mDescriptionValue->setText(pDescription);
}

//==============================================================================

QLabel * PluginCategoryWidget::label(const QString &pLabel)
{
    // Create and return a label

    QLabel *res = new QLabel(pLabel, this);

    res->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    QFont newFont = res->font();

    newFont.setBold(true);

    res->setFont(newFont);

    return res;
}

//==============================================================================

void PreferencesItemDelegate::paint(QPainter *pPainter,
                                    const QStyleOptionViewItem &pOption,
                                    const QModelIndex &pIndex) const
{
    // Paint the item as normal, if it is selectable, or bold, if it is a
    // category

    QStandardItem *pluginItem = qobject_cast<const QStandardItemModel *>(pIndex.model())->itemFromIndex(pIndex);

    QStyleOptionViewItem option(pOption);

    initStyleOption(&option, pIndex);

    if (pluginItem->hasChildren())
        option.font.setBold(true);

    QStyledItemDelegate::paint(pPainter, option, pIndex);
}

//==============================================================================

PreferencesDialog::PreferencesDialog(PluginManager *pPluginManager,
                                     QWidget *pParent) :
    QDialog(pParent),
    mGui(new Ui::PreferencesDialog),
    mPluginManager(pPluginManager),
    mCategoryItems(QMap<PluginInfo::Category, QStandardItem *>()),
    mItemCategories(QMap<QStandardItem *, PluginInfo::Category>()),
    mItemPreferencesWidgets(QMap<QStandardItem *, Preferences::PreferencesWidget *>())
{
    // Set up the GUI

    mGui->setupUi(this);

    // Customise our GUI's button box by having both a reset plugin settings and
    // reset all settings buttons

    mResetAllButton = mGui->buttonBox->addButton(tr("Reset All"), QDialogButtonBox::ActionRole);
    mResetPluginButton = mGui->buttonBox->addButton(tr("Reset Plugin"), QDialogButtonBox::ActionRole);

    connect(mResetAllButton, SIGNAL(clicked(bool)),
            this, SLOT(resetAll()));
    connect(mResetPluginButton, SIGNAL(clicked(bool)),
            this, SLOT(resetPlugin()));

    // Create and add our plugin category widget

    mPluginCategoryWidget = new PluginCategoryWidget(this);

    mGui->stackedWidget->addWidget(mPluginCategoryWidget);

    // Set up the tree view widget

    mModel = new QStandardItemModel(mGui->treeView);

#ifdef Q_OS_MAC
    mGui->treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    // Note: the above removes the focus border since it messes up the look of
    //       our plugins tree view widget...
#endif
    mGui->treeView->setModel(mModel);
    mGui->treeView->setItemDelegate(new PreferencesItemDelegate());

    // Populate the data model with our plugins that support the Preferences
    // interface

    foreach (Plugin *plugin, mPluginManager->sortedPlugins()) {
        PreferencesInterface *preferencesInterface = qobject_cast<PreferencesInterface *>(plugin->instance());

        if (preferencesInterface) {
            // Create the item corresponding to the current plugin and add it to
            // its corresponding category

            QStandardItem *pluginItem = new QStandardItem(plugin->name());

            pluginCategoryItem(plugin->info()->category())->appendRow(pluginItem);

            // Retrieve the corresponding preferences widget and add it to our
            // stacked widget, as well as keep track of it

            Preferences::PreferencesWidget *preferencesWidget = preferencesInterface->preferencesWidget();

            mGui->stackedWidget->addWidget(preferencesWidget);

            mItemPreferencesWidgets.insert(pluginItem, preferencesWidget);
        }
    }

    // Expand the whole tree view widget and make sure that it only takes as
    // much width as necessary
    // Note: for some reasons, the retrieved column size gives us a width that
    //       is slightly too small and therefore requires a horizontal scroll
    //       bar, hence we add 15% to it (those extra 15% seems to be enough
    //       to account even for a big number of plugins which would then
    //       require a vertical scroll bar)

    mGui->treeView->expandAll();
    mGui->treeView->resizeColumnToContents(0);

    mGui->treeView->setMinimumWidth(1.15*mGui->treeView->columnWidth(0));
    mGui->treeView->setMaximumWidth(mGui->treeView->minimumWidth());

    // Make, through the stacked widget, sure that the dialog has a minimum
    // width

    mGui->stackedWidget->setMinimumWidth(2.5*mGui->treeView->minimumWidth());

    // Make sure that the dialog has a reasonable starting size

    mGui->layout->setSizeConstraint(QLayout::SetMinimumSize);

    // Connection to handle the change of preferences widget

    connect(mGui->treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(updatePreferencesWidget(const QModelIndex &, const QModelIndex &)));

    // Select our first item

    mGui->treeView->setCurrentIndex(mModel->invisibleRootItem()->child(0)->index());
}

//==============================================================================

PreferencesDialog::~PreferencesDialog()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

QStandardItem * PreferencesDialog::pluginCategoryItem(const PluginInfo::Category &pCategory)
{
    // Return the given category item, after having created it, if it didn't
    // already exist

    QStandardItem *res = mCategoryItems.value(pCategory);

    if (!res) {
        // No category item exists for the given category, so create one and add
        // it to our data model (and this in the right place)

        bool inserted = false;
        QStandardItem *rootItem = mModel->invisibleRootItem();
        QString categoryName = pluginCategoryName(pCategory);
        QString nonDiacriticCategoryName = nonDiacriticString(categoryName);

        res = new QStandardItem(categoryName);

        for (int i = 0, iMax = rootItem->rowCount(); i < iMax; ++i) {
            if (nonDiacriticCategoryName.compare(nonDiacriticString(rootItem->child(i)->text())) < 0) {
                inserted = true;

                mModel->invisibleRootItem()->insertRow(i, res);

                break;
            }
        }

        if (!inserted)
            mModel->invisibleRootItem()->appendRow(res);

        // Keep track of the relationship between our new item and its category

        mCategoryItems.insert(pCategory, res);
        mItemCategories.insert(res, pCategory);
    }

    return res;
}

//==============================================================================

void PreferencesDialog::on_treeView_collapsed(const QModelIndex &pIndex)
{
    // We don't want plugin categories to be collapse, so cancel all collapsings

    mGui->treeView->expand(pIndex);
}

//==============================================================================

void PreferencesDialog::on_buttonBox_accepted()
{
    // Save all of our plugins' preferences

    foreach (Preferences::PreferencesWidget *preferencesWidget, mItemPreferencesWidgets.values())
        preferencesWidget->savePreferences();

    // Confirm that we accepted the changes

    accept();
}

//==============================================================================

void PreferencesDialog::on_buttonBox_rejected()
{
    // Simply cancel whatever was done here

    reject();
}

//==============================================================================

void PreferencesDialog::updatePreferencesWidget(const QModelIndex &pNewIndex,
                                                const QModelIndex &pOldIndex)
{
    Q_UNUSED(pOldIndex);

    // Check whether we are dealing with a plugin category or a plugin's
    // preferences

    QStandardItem *item = mModel->itemFromIndex(pNewIndex);
    bool isPluginCategory = mCategoryItems.values().contains(item);

    mResetPluginButton->setEnabled(!isPluginCategory);

    if (isPluginCategory) {
        // We are dealing with a plugin category, so retrieve and set the name
        // and description of the plugin

        mPluginCategoryWidget->setCategory(item->text());
        mPluginCategoryWidget->setDescription(tr("%1.").arg(formatMessage(pluginCategoryDescription(mItemCategories.value(item)))));

        mGui->stackedWidget->setCurrentWidget(mPluginCategoryWidget);
    } else {
        // We are dealing with a plugin's preferences, so show it

        mGui->stackedWidget->setCurrentWidget(mItemPreferencesWidgets.value(item));
    }

    // Make sure that the current widget has no layout margin (so that not only
    // we don't waste space, but also developers don't have to worry about the
    // layout margin of their preferences widget, assuming it has a layout)

    QLayout *widgetLayout = mGui->stackedWidget->currentWidget()->layout();

    if (widgetLayout)
        widgetLayout->setMargin(0);

    // Make sure that we are big enough to show our contents

    adjustWidgetSize(this);
}

//==============================================================================

void PreferencesDialog::resetAll()
{
    // Reset all of our plugins' preferences

    foreach (Preferences::PreferencesWidget *preferencesWidget, mItemPreferencesWidgets.values())
        preferencesWidget->resetPreferences();
}

//==============================================================================

void PreferencesDialog::resetPlugin()
{
    // Reset all of the current plugin's preferences

    static_cast<Preferences::PreferencesWidget *>(mGui->stackedWidget->currentWidget())->resetPreferences();
}

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
