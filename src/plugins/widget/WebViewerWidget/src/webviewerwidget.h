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
// Web Viewer widget
//==============================================================================

#pragma once

//==============================================================================

#include "commonwidget.h"
#include "webviewerwidgetglobal.h"

//==============================================================================

#include <QString>
#include <QWebView>

//==============================================================================

namespace OpenCOR {
namespace WebViewerWidget {

//==============================================================================

class WebViewerWidget;

//==============================================================================

class WebViewerPage : public QWebPage
{
public:
    explicit WebViewerPage(WebViewerWidget *pParent);

protected:
    virtual bool acceptNavigationRequest(QWebFrame *pFrame,
                                         const QNetworkRequest &pRequest,
                                         QWebPage::NavigationType pType);

private:
    WebViewerWidget *mOwner;
};

//==============================================================================

class WEBVIEWERWIDGET_EXPORT WebViewerWidget : public QWebView,
                                               public Core::CommonWidget
{
    Q_OBJECT

public:
    explicit WebViewerWidget(QWidget *pParent);

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    QWebElement retrieveLinkInformation(QString &pLink, QString &pTextContent);

    void setLinkToolTip(const QString &pLinkToolTip);

    virtual bool isUrlSchemeSupported(const QString &pUrlScheme);

    void setHomePage(const QString &pHomePage);
    QString homePage() const;

    void goToHomePage();

    void resetZoom();

    void zoomIn();
    void zoomOut();

    void setZoomingEnabled(const bool &pZoomingEnabled);

    void showWebInspector();

protected:
    virtual bool event(QEvent *pEvent);
    virtual void wheelEvent(QWheelEvent *pEvent);

private:
    bool mResettingCursor;

    QString mLinkToolTip;

    QString mHomePage;

    bool mZoomingEnabled;
    int mZoomLevel;

    void emitZoomRelatedSignals();

    void setZoomLevel(const int &pZoomLevel);

signals:
    void homePage(const bool &pHomePage);

    void backEnabled(const bool &pEnabled);
    void forwardEnabled(const bool &pEnabled);

    void copyTextEnabled(const bool &pEnabled);

    void defaultZoomLevel(const bool &pDefault);
    void zoomingOutEnabled(const bool &pEnabled);

private slots:
    void urlChanged(const QUrl &pUrl);

    void selectionChanged();

    void pageChanged();
};

//==============================================================================

}   // namespace WebViewerWidget
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
