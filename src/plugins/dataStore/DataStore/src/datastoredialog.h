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

#pragma once

//==============================================================================

#include "datastoreglobal.h"
#include "datastoreinterface.h"

//==============================================================================

#include <QDialog>
#include <QStyledItemDelegate>

//==============================================================================

namespace Ui {
    class DataStoreDialog;
}

//==============================================================================

class QStandardItem;
class QStandardItemModel;

//==============================================================================

namespace OpenCOR {
namespace DataStore {

//==============================================================================

class DataStore;

//==============================================================================

class DataItemDelegate : public QStyledItemDelegate
{
public:
    virtual void paint(QPainter *pPainter, const QStyleOptionViewItem &pOption,
                       const QModelIndex &pIndex) const;
};

//==============================================================================

class DATASTORE_EXPORT DataStoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataStoreDialog(DataStore *pDataStore, const bool &pIncludeVoi,
                             QWidget *pParent);
    ~DataStoreDialog();

    void addWidget(QWidget *pWidget);

    DataStoreVariables selectedData() const;

private:
    Ui::DataStoreDialog *mGui;

    QStandardItemModel *mModel;

    QMap<QStandardItem *, DataStoreVariable*> mData;
    int mNbOfData;

    DataStoreVariables doSelectedData(QStandardItem *pItem) const;

    void updateDataSelectedState(QStandardItem *pItem,
                                 const Qt::CheckState &pCheckState);
    void checkDataSelectedState(QStandardItem *pItem, int &pNbOfselectedData);

private slots:
    void updateDataSelectedState(QStandardItem *pItem = 0);

    void on_allDataCheckBox_clicked();
};

//==============================================================================

}   // namespace DataStore
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
