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
// Data store dialog
//==============================================================================

#include "coreguiutils.h"
#include "datastoreinterface.h"
#include "datastoredialog.h"

//==============================================================================

#include "ui_datastoredialog.h"

//==============================================================================

#include <QPushButton>
#include <QStandardItemModel>

//==============================================================================

namespace OpenCOR {
namespace DataStore {

//==============================================================================

void DataItemDelegate::paint(QPainter *pPainter,
                               const QStyleOptionViewItem &pOption,
                               const QModelIndex &pIndex) const
{
    // Paint the item as normal unless it's a hierarchy item, in which case we
    // make it bold

    QStandardItem *dataItem = qobject_cast<const QStandardItemModel *>(pIndex.model())->itemFromIndex(pIndex);

    QStyleOptionViewItem option(pOption);

    initStyleOption(&option, pIndex);

    if (dataItem->rowCount())
        option.font.setBold(true);

    QStyledItemDelegate::paint(pPainter, option, pIndex);
}

//==============================================================================

DataStoreDialog::DataStoreDialog(DataStore *pDataStore, const bool &pIncludeVoi,
                                 QWidget *pParent) :
    QDialog(pParent),
    mGui(new Ui::DataStoreDialog),
    mData(QMap<QStandardItem *, DataStoreVariable*>()),
    mNbOfData(0)
{
    // Set up the GUI

    mGui->setupUi(this);

#ifdef Q_OS_MAC
    mGui->treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    // Note: the above removes the focus border since it messes up the look of
    //       our tree view widget...
#endif

    mGui->dataLabel->setVisible(false);

    connect(mGui->allDataCheckBox, SIGNAL(toggled(bool)),
            mGui->buttonBox->button(QDialogButtonBox::Ok), SLOT(setEnabled(bool)));

    connect(mGui->buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(mGui->buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    // Populate our tree view with the data store's variables and, or not, the
    // variable of integration
    // Note: indeed, in some cases (e.g. CSV export), we want to list all the
    //       variables including the variable of integration while in some other
    //       cases (e.g. BioSignalML export), we don't want to list the variable
    //       of integration (since, to respect the BioSignalML format, the
    //       variable of integration must absolutely be exported)...

    mModel = new QStandardItemModel(mGui->treeView);

    mGui->treeView->setModel(mModel);
    mGui->treeView->setItemDelegate(new DataItemDelegate());

    QString dataHierarchy = QString();
    QStandardItem *hierarchyItem = 0;

    foreach (DataStoreVariable *variable,
             pIncludeVoi?pDataStore->voiAndVariables():pDataStore->variables()) {
        if (variable->isVisible()) {
            // Check whether the variable is in the same hierarchy as the
            // previous one

            static const QRegularExpression VariableNameRegEx = QRegularExpression("/[^/]+(/prime)*$");

            QString crtDataHierarchy = variable->uri().remove(VariableNameRegEx);

            if (crtDataHierarchy.compare(dataHierarchy)) {
                // The variable is in a different component hierarchy, so create
                // a new section hierarchy for our 'new' component, reusing
                // existing sections, whenever possible

                QStandardItem *parentHierarchyItem = mModel->invisibleRootItem();

                foreach (const QString &hierarchyPart, crtDataHierarchy.split("/")) {
                    hierarchyItem = 0;

                    for (int i = 0, iMax = parentHierarchyItem->rowCount(); i < iMax; ++i) {
                        QStandardItem *childHierarchyItem = parentHierarchyItem->child(i);

                        if (!childHierarchyItem->text().compare(hierarchyPart)) {
                            hierarchyItem = childHierarchyItem;

                            break;
                        }
                    }

                    if (!hierarchyItem) {
                        hierarchyItem = new QStandardItem(hierarchyPart);

                        hierarchyItem->setAutoTristate(true);
                        hierarchyItem->setCheckable(true);
                        hierarchyItem->setEditable(false);

                        parentHierarchyItem->appendRow(hierarchyItem);
                    }

                    parentHierarchyItem = hierarchyItem;
                }

                dataHierarchy = crtDataHierarchy;
            }

            QStandardItem *dataItem = new QStandardItem(variable->icon(),
                                                        variable->label());

            dataItem->setCheckable(true);
            dataItem->setCheckState(Qt::Checked);
            dataItem->setEditable(false);

            hierarchyItem->appendRow(dataItem);

            mData.insert(dataItem, variable);

            ++mNbOfData;
        }
    }

    mGui->treeView->expandAll();

    updateDataSelectedState();
}

//==============================================================================

DataStoreDialog::~DataStoreDialog()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void DataStoreDialog::addWidget(QWidget *pWidget)
{
    // Insert the given widget before our tree view, set our focus to it and
    // show our data selector label

    mGui->layout->insertWidget(0, pWidget);

    setFocusProxy(pWidget);
    setFocus();

    mGui->dataLabel->setVisible(true);

    // Resize ourselves to make sure that the new widget doesn't squash our
    // original contents

    resize(QSize(qMax(qMax(width(), pWidget->sizeHint().width()), mGui->dataLabel->sizeHint().width()),
                 pWidget->sizeHint().height()+mGui->dataLabel->sizeHint().height()+height()));
}

//==============================================================================

DataStoreVariables DataStoreDialog::doSelectedData(QStandardItem *pItem) const
{
    // Return the selected data for the given item

    DataStoreVariables res = DataStoreVariables();

    if (pItem->rowCount()) {
        for (int i = 0, iMax = pItem->rowCount(); i < iMax; ++i)
            res << doSelectedData(pItem->child(i));
    } else if (pItem->checkState() == Qt::Checked) {
        res << mData.value(pItem);
    }

    return res;
}

//==============================================================================

DataStoreVariables DataStoreDialog::selectedData() const
{
    // Return our selected data

    DataStoreVariables res = DataStoreVariables();

    for (int i = 0, iMax = mModel->invisibleRootItem()->rowCount(); i < iMax; ++i)
        res << doSelectedData(mModel->invisibleRootItem()->child(i));

    return res;
}

//==============================================================================

void DataStoreDialog::updateDataSelectedState(QStandardItem *pItem,
                                              const Qt::CheckState &pCheckState)
{
    // Update the selected state of the given item's children

    for (int i = 0, iMax = pItem->rowCount(); i < iMax; ++i) {
        QStandardItem *childItem = pItem->child(i);

        childItem->setCheckState(pCheckState);

        if (childItem->rowCount())
            updateDataSelectedState(childItem, pCheckState);
    }
}

//==============================================================================

void DataStoreDialog::checkDataSelectedState(QStandardItem *pItem,
                                             int &pNbOfselectedData)
{
    // Update the selected state of the given item's children

    int nbOfSelectedChildItems = 0;
    bool partiallySelectedChildItems = false;

    for (int i = 0, iMax = pItem->rowCount(); i < iMax; ++i) {
        QStandardItem *childItem = pItem->child(i);

        if (childItem->rowCount())
            checkDataSelectedState(childItem, pNbOfselectedData);

        if (childItem->checkState() == Qt::Checked) {
            ++nbOfSelectedChildItems;

            if (!childItem->rowCount())
                ++pNbOfselectedData;
        }

        if (childItem->checkState() == Qt::PartiallyChecked)
            partiallySelectedChildItems = true;
    }

    pItem->setCheckState(nbOfSelectedChildItems?
                             (nbOfSelectedChildItems == pItem->rowCount())?
                                 Qt::Checked:
                                 Qt::PartiallyChecked:
                             partiallySelectedChildItems?
                                 Qt::PartiallyChecked:
                                 Qt::Unchecked);
}

//==============================================================================

void DataStoreDialog::updateDataSelectedState(QStandardItem *pItem)
{
    // Disable the handling of the itemChanged() signal (otherwise what we are
    // doing here is going to be completely ineffective)

    disconnect(mModel, SIGNAL(itemChanged(QStandardItem *)),
               this, SLOT(updateDataSelectedState(QStandardItem *)));

    // In case we un/select a hierarchy, then go through its data and un/select
    // it accordingly, or keep track of the number of selected data, if we
    // un/select some data

    if (pItem && pItem->isAutoTristate())
        updateDataSelectedState(pItem, (pItem->checkState() == Qt::Unchecked)?Qt::Unchecked:Qt::Checked);

    // Update the selected state of all our hierarchies

    int nbOfSelectedData = 0;

    for (int i = 0, iMax = mModel->invisibleRootItem()->rowCount(); i < iMax; ++i) {
        QStandardItem *childItem = mModel->invisibleRootItem()->child(i);

        checkDataSelectedState(childItem, nbOfSelectedData);
    }

    mGui->allDataCheckBox->setCheckState(nbOfSelectedData?
                                             (nbOfSelectedData == mNbOfData)?
                                                 Qt::Checked:
                                                 Qt::PartiallyChecked:
                                             Qt::Unchecked);

    // Re-enable the handling of the itemChanged() signal

    connect(mModel, SIGNAL(itemChanged(QStandardItem *)),
            this, SLOT(updateDataSelectedState(QStandardItem *)));
}

//==============================================================================

void DataStoreDialog::on_allDataCheckBox_clicked()
{
    // If our checked state is partially checked, then we want to make it fully
    // so

    if (mGui->allDataCheckBox->checkState() == Qt::PartiallyChecked)
        mGui->allDataCheckBox->setCheckState(Qt::Checked);

    // Un/select all the data
    // Note: we temporally disable the handling of the itemChanged() signal
    //       since we 'manually' set everything ourselves...

    disconnect(mModel, SIGNAL(itemChanged(QStandardItem *)),
               this, SLOT(updateDataSelectedState(QStandardItem *)));

    updateDataSelectedState(mModel->invisibleRootItem(),
                            mGui->allDataCheckBox->isChecked()?Qt::Checked:Qt::Unchecked);

    connect(mModel, SIGNAL(itemChanged(QStandardItem *)),
            this, SLOT(updateDataSelectedState(QStandardItem *)));
}

//==============================================================================

}   // namespace DataStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
