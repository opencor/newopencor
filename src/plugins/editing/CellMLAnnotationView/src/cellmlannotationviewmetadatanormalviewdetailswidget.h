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
// CellML annotation view metadata normal view details widget
//==============================================================================

#ifndef CELLMLANNOTATIONVIEWMETADATANORMALVIEWDETAILSWIDGET_H
#define CELLMLANNOTATIONVIEWMETADATANORMALVIEWDETAILSWIDGET_H

//==============================================================================

#include "cellmlfile.h"
#include "commonwidget.h"

//==============================================================================

#include <QMap>
#include <QScrollArea>

//==============================================================================

namespace Ui {
    class CellmlAnnotationViewMetadataNormalViewDetailsWidget;
}

//==============================================================================

class QGridLayout;
class QLabel;
class QMenu;
class QStackedWidget;

//==============================================================================

namespace OpenCOR {
namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewEditingWidget;

//==============================================================================

class CellmlAnnotationViewMetadataNormalViewDetailsWidget : public QScrollArea,
                                                            public Core::CommonWidget
{
    Q_OBJECT

private:
    enum Information {
        None,
        First,
        Any,
        Last
    };

    enum Type {
        No,
        Qualifier,
        Resource,
        Id
    };

public:
    explicit CellmlAnnotationViewMetadataNormalViewDetailsWidget(CellmlAnnotationViewEditingWidget *pParent);
    ~CellmlAnnotationViewMetadataNormalViewDetailsWidget();

    virtual void retranslateUi();

    void updateGui(iface::cellml_api::CellMLElement *pElement,
                   const QString &pRdfTripleInformation = QString(),
                   const Type &pType = No,
                   const Information &pLookUpInformation = First,
                   const int &pVerticalScrollBarPosition = 0,
                   const bool &pRetranslate = false);

    void addRdfTriple(CellMLSupport::CellmlFileRdfTriple *pRdfTriple);

private:
    CellMLSupport::CellmlFile *mCellmlFile;

    Ui::CellmlAnnotationViewMetadataNormalViewDetailsWidget *mGui;

    QStackedWidget *mWidget;

    QWidget *mGridWidget;
    QGridLayout *mGridLayout;

    ObjRef<iface::cellml_api::CellMLElement> mElement;

    QString mRdfTripleInformation;
    Type mType;

    Information mLookUpInformation;

    int mVerticalScrollBarPosition;
    int mNeighbourRow;

    QMap<QObject *, CellMLSupport::CellmlFileRdfTriple *> mRdfTriplesMapping;

    QLabel *mCurrentResourceOrIdLabel;

    QMenu *mContextMenu;

    void genericLookUp(const QString &pRdfTripleInformation = QString(),
                       const Type &pType = No,
                       const bool &pRetranslate = false);

    QString rdfTripleInformation(const int &pRow) const;

Q_SIGNALS:
    void qualifierLookUpRequested(const QString &pQualifier,
                                  const bool &pRetranslate);
    void resourceLookUpRequested(const QString &pResource,
                                 const bool &pRetranslate);
    void idLookUpRequested(const QString &pResource, const QString &pId,
                           const bool &pRetranslate);
    void noLookUpRequested();

    void rdfTripleRemoved(CellMLSupport::CellmlFileRdfTriple *pRdfTriple);

private Q_SLOTS:
    void on_actionCopy_triggered();

    void disableLookUpInformation();

    void lookUpQualifier(const QString &pRdfTripleInformation);
    void lookUpResource(const QString &pRdfTripleInformation);
    void lookUpId(const QString &pRdfTripleInformation);

    void removeRdfTriple();

    void showNeighbourRdfTriple();
    void showLastRdfTriple();

    void trackVerticalScrollBarPosition(const int &pPosition);

    void showCustomContextMenu(const QPoint &pPosition);
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
