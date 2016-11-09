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
// CellML Annotation view metadata edit details widget
//==============================================================================

#pragma once

//==============================================================================

#include "cellmlfile.h"
#include "widget.h"

//==============================================================================

#include <QMap>
#include <QModelIndex>
#include <QSslError>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

//==============================================================================

class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QNetworkAccessManager;
class QNetworkReply;
class QPushButton;
class QScrollArea;

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace Core {
    class UserMessageWidget;
}   // namespace Core

//==============================================================================

namespace WebViewerWidget {
    class WebViewerWidget;
}   // namespace WebViewerWidget

//==============================================================================

namespace CellMLAnnotationView {

//==============================================================================

class CellmlAnnotationViewEditingWidget;
class CellmlAnnotationViewWidget;

//==============================================================================

class CellmlAnnotationViewMetadataEditDetailsItem {
public:
    explicit CellmlAnnotationViewMetadataEditDetailsItem(const QString &pName = QString(),
                                                         const QString &pResource = QString(),
                                                         const QString &pId = QString());

    static bool compare(const CellmlAnnotationViewMetadataEditDetailsItem &pItem1,
                        const CellmlAnnotationViewMetadataEditDetailsItem &pItem2);

    QString name() const;
    QString resource() const;
    QString id() const;

private:
    QString mName;
    QString mResource;
    QString mId;
};

//==============================================================================

typedef QList<CellmlAnnotationViewMetadataEditDetailsItem> CellmlAnnotationViewMetadataEditDetailsItems;

//==============================================================================

class CellmlAnnotationViewMetadataEditDetailsWidget : public Core::Widget
{
    Q_OBJECT

public:
    explicit CellmlAnnotationViewMetadataEditDetailsWidget(CellmlAnnotationViewWidget *pViewWidget,
                                                           CellmlAnnotationViewEditingWidget *pViewEditingWidget,
                                                           CellMLSupport::CellmlFile *pCellmlFile,
                                                           QWidget *pParent);

    virtual void retranslateUi();

    void filePermissionsChanged();

private:
    enum InformationType {
        None,
        Qualifier,
        Resource,
        Id
    };

    CellmlAnnotationViewWidget *mViewWidget;
    CellmlAnnotationViewEditingWidget *mViewEditingWidget;

    QNetworkAccessManager *mNetworkAccessManager;

    QLabel *mQualifierLabel;
    QComboBox *mQualifierValue;
    QPushButton *mLookUpQualifierButton;

    QLabel *mTermLabel;
    QLineEdit *mTermValue;
    QPushButton *mAddTermButton;

    QString mTerm;
    QStringList mTerms;

    CellmlAnnotationViewMetadataEditDetailsItems mItems;

    bool mLookUpTerm;
    QString mErrorMessage;
    bool mInternetConnectionAvailable;

    Core::Widget *mOutput;

    QScrollArea *mOutputMessageScrollArea;
    Core::UserMessageWidget *mOutputMessage;

    QString mOutputOntologicalTermsTemplate;
    WebViewerWidget::WebViewerWidget *mOutputOntologicalTerms;

    InformationType mInformationType;

    bool mLookUpInformation;

    QMap<QString, CellmlAnnotationViewMetadataEditDetailsItem> mItemsMapping;
    QMap<QString, bool> mEnabledItems;

    CellMLSupport::CellmlFile *mCellmlFile;

    ObjRef<iface::cellml_api::CellMLElement> mElement;

    QMap<QString, QString> mUrls;
    QStringList mItemInformationSha1s;
    QString mItemInformationSha1;

    QString mLink;
    QString mTextContent;

    QMenu *mContextMenu;

    QAction *mCopyAction;

    QNetworkReply *mNetworkReply;

    void upudateOutputMessage(const bool &pLookUpTerm,
                              const QString &pErrorMessage,
                              const bool &pInternetConnectionAvailable,
                              bool *pShowBusyWidget = 0);
    void updateOutputHeaders();

    void updateItemsGui(const CellmlAnnotationViewMetadataEditDetailsItems &pItems,
                        const bool &pLookUpTerm,
                        const QString &pErrorMessage = QString(),
                        const bool &pInternetConnectionAvailable = true);

    void genericLookUp(const QString &pItemInformation = QString(),
                       const InformationType &pInformationType = None);

    bool isDirectTerm(const QString &pTerm) const;

signals:
    void qualifierLookUpRequested(const QString &pQualifier);
    void resourceLookUpRequested(const QString &pResource);
    void idLookUpRequested(const QString &pResource, const QString &pId);
    void noLookUpRequested();

public slots:
    void updateGui(iface::cellml_api::CellMLElement *pElement,
                   const bool &pResetItemsGui = false,
                   const bool &pFilePermissionsChanged = false);

private slots:
    void copy();

    void disableLookUpInformation();

    void qualifierChanged(const QString &pQualifier);

    void lookUpQualifier();

    void linkClicked();
    void linkHovered();

    void lookUpTerm();

    void termChanged(const QString &pTerm);
    void termLookedUp(QNetworkReply *pNetworkReply = 0);
    void sslErrors(QNetworkReply *pNetworkReply,
                   const QList<QSslError> &pSslErrors);

    void addTerm();

    void showCustomContextMenu();
};

//==============================================================================

}   // namespace CellMLAnnotationView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
