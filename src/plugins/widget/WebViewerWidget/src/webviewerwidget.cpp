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

#include "webviewerwidget.h"

//==============================================================================

#include <QAction>
#include <QCursor>
#include <QDesktopServices>
#include <QEvent>
#include <QHelpEvent>
#include <QNetworkRequest>
#include <QSettings>
#include <QToolTip>
#include <QWebElement>
#include <QWebHitTestResult>

//==============================================================================

namespace OpenCOR {
namespace WebViewerWidget {

//==============================================================================

WebViewerPage::WebViewerPage(WebViewerWidget *pParent) :
    QWebPage(pParent),
    mOwner(pParent)
{
}

//==============================================================================

bool WebViewerPage::acceptNavigationRequest(QWebFrame *pFrame,
                                            const QNetworkRequest &pRequest,
                                            QWebPage::NavigationType pType)
{
    // Ask our owner whether the URL should be handled by ourselves or just
    // opened the default way

    QUrl url = pRequest.url();
    QString urlScheme = url.scheme();

    if (mOwner->isUrlSchemeSupported(urlScheme)) {
        // We should be handled the URL ourseves, but now let's see whether
        // anything should be delegated
        // Note: this comes (and was adapted) from
        //       QWebPage::acceptNavigationRequest(),
        //       QWebPageAdapter::treatSchemeAsLocal(),
        //       SchemeRegistry::shouldTreatURLSchemeAsLocal() and
        //       localURLSchemes()...

        if (pType == NavigationTypeLinkClicked) {
            static QStringList localSchemes = QStringList();

            if (localSchemes.isEmpty()) {
                localSchemes << "file";
#ifdef Q_OS_MAC
                localSchemes << "applewebdata";
#endif
                localSchemes << "qrc";
            }

            switch (linkDelegationPolicy()) {
            case DontDelegateLinks:
                return true;
            case DelegateExternalLinks:
                if (   urlScheme.isEmpty()
                    && localSchemes.contains(pFrame->baseUrl().scheme())) {
                    return true;
                }

                if (localSchemes.contains(urlScheme))
                    return true;

                emit linkClicked(pRequest.url());

                return false;
            case DelegateAllLinks:
                emit linkClicked(pRequest.url());

                return false;
            }
        }

        return true;
    } else {
        QDesktopServices::openUrl(url);

        return false;
    }
}

//==============================================================================

enum {
    MinimumZoomLevel =  1,
    DefaultZoomLevel = 10
};

//==============================================================================

WebViewerWidget::WebViewerWidget(QWidget *pParent) :
    QWebView(pParent),
    Core::CommonWidget(this),
    mResettingCursor(false),
    mLinkToolTip(QString()),
    mHomePage("about:blank"),
    mZoomingEnabled(true),
    mZoomLevel(-1)   // This will ensure that mZoomLevel gets initialised by our
                     // first call to setZoomLevel
{
    // Use our own page

    setPage(new WebViewerPage(this));

    // Customise ourselves

    setAcceptDrops(false);
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Some connections

    connect(this, SIGNAL(urlChanged(const QUrl &)),
            this, SLOT(urlChanged(const QUrl &)));

    connect(pageAction(QWebPage::Back), SIGNAL(changed()),
            this, SLOT(pageChanged()));
    connect(pageAction(QWebPage::Forward), SIGNAL(changed()),
            this, SLOT(pageChanged()));

    connect(page(), SIGNAL(selectionChanged()),
            this, SLOT(selectionChanged()));

    // Set our initial zoom level to its default value
    // Note: to set mZoomLevel directly is not good enough since one of the
    //       things setZoomLevel does is to set our zoom factor...

    setZoomLevel(DefaultZoomLevel);
}

//==============================================================================

static const auto SettingsZoomLevel = QStringLiteral("ZoomLevel");

//==============================================================================

void WebViewerWidget::loadSettings(QSettings *pSettings)
{
    // Retrieve the zoom level

    setZoomLevel(pSettings->value(SettingsZoomLevel, DefaultZoomLevel).toInt());

    emitZoomRelatedSignals();

    // Let the user know of a few default things about ourselves by emitting a
    // few signals

    emit homePage(true);

    emit backEnabled(false);
    emit forwardEnabled(false);

    emit copyTextEnabled(false);
}

//==============================================================================

void WebViewerWidget::saveSettings(QSettings *pSettings) const
{
    // Keep track of the zoom level

    pSettings->setValue(SettingsZoomLevel, mZoomLevel);
}

//==============================================================================

void WebViewerWidget::setLinkToolTip(const QString &pLinkToolTip)
{
    // Set our link tool tip

    mLinkToolTip = pLinkToolTip;
}

//==============================================================================

bool WebViewerWidget::event(QEvent *pEvent)
{
    // Override the change of the cursor and tool tip when hovering a link

    if (mResettingCursor) {
        return true;
    } else if (    (pEvent->type() == QEvent::CursorChange)
               &&  (cursor().shape() == Qt::IBeamCursor)
               && !mResettingCursor) {
        mResettingCursor = true;

        setCursor(Qt::ArrowCursor);

        mResettingCursor = false;

        return true;
    } else if (pEvent->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(pEvent);

        if (!mLinkToolTip.isEmpty()) {
            QToolTip::showText(helpEvent->globalPos(), mLinkToolTip);
        } else {
            QToolTip::hideText();

            pEvent->ignore();
        }

        return true;
    } else {
        return QWebView::event(pEvent);
    }
}

//==============================================================================

void WebViewerWidget::wheelEvent(QWheelEvent *pEvent)
{
    // Handle the wheel mouse button for zooming in/out the help document
    // contents

    if (mZoomingEnabled && (pEvent->modifiers() == Qt::ControlModifier)) {
        int delta = pEvent->delta();

        if (delta > 0)
            zoomIn();
        else if (delta < 0)
            zoomOut();

        pEvent->accept();
    } else {
        // Not the modifier we were expecting, so call the default handling of
        // the event

        QWebView::wheelEvent(pEvent);
    }
}

//==============================================================================

bool WebViewerWidget::isUrlSchemeSupported(const QString &pUrlScheme)
{
    Q_UNUSED(pUrlScheme);

    // By default, we support all URL schemes

    return true;
}

//==============================================================================

QWebElement WebViewerWidget::retrieveLinkInformation(QString &pLink,
                                                     QString &pTextContent)
{
    // Retrieve the link and text content values for the link, if any, below our
    // mouse pointer
    // Note: normally, one would want to handle the linkHovered() signal, but it
    //       may provide the wrong information. Indeed, say that you are over a
    //       link and then scroll down/up using your mouse wheel, and end up
    //       over another link and click it. Now, because your mouse didn't
    //       move, no linkHovered() message will have been emitted (and
    //       handled), which means that if we need that information to process
    //       the click, then we will have the wrong information...

    QWebHitTestResult hitTestResult = page()->mainFrame()->hitTestContent(mapFromGlobal(QCursor::pos()));
    QWebElement res = hitTestResult.element();

    while (!res.isNull() && res.tagName().compare("A"))
        res = res.parent();

    if (res.isNull()) {
        pLink = QString();
        pTextContent = QString();
    } else {
        pLink = res.attribute("href");
        pTextContent = hitTestResult.linkText();
    }

    return res;
}

//==============================================================================

void WebViewerWidget::setHomePage(const QString &pHomePage)
{
    // Set our home page

    mHomePage = pHomePage;
}

//==============================================================================

QString WebViewerWidget::homePage() const
{
    // Return our home page

    return mHomePage;
}

//==============================================================================

void WebViewerWidget::goToHomePage()
{
    // Go to our home page

    load(mHomePage);
}

//==============================================================================

void WebViewerWidget::resetZoom()
{
    // Reset the zoom level

    setZoomLevel(DefaultZoomLevel);
}

//==============================================================================

void WebViewerWidget::zoomIn()
{
    // Zoom in the help document contents

    setZoomLevel(mZoomLevel+1);
}

//==============================================================================

void WebViewerWidget::zoomOut()
{
    // Zoom out the help document contents

    setZoomLevel(qMax(int(MinimumZoomLevel), mZoomLevel-1));
}

//==============================================================================

void WebViewerWidget::setZoomingEnabled(const bool &pZoomingEnabled)
{
    // Set whether zooming in/out is enabled

    mZoomingEnabled = pZoomingEnabled;
}

//==============================================================================

void WebViewerWidget::emitZoomRelatedSignals()
{
    // Let the user know whether we are not at the default zoom level and
    // whether we can still zoom out

    emit defaultZoomLevel(mZoomLevel == DefaultZoomLevel);
    emit zoomingOutEnabled(mZoomLevel != MinimumZoomLevel);
}

//==============================================================================

void WebViewerWidget::setZoomLevel(const int &pZoomLevel)
{
    if (!mZoomingEnabled || (pZoomLevel == mZoomLevel))
        return;

    // Set the zoom level of the help document contents to a particular value

    mZoomLevel = pZoomLevel;

    setZoomFactor(0.1*mZoomLevel);

    // Emit a few zoom-related signals

    emitZoomRelatedSignals();
}

//==============================================================================

void WebViewerWidget::urlChanged(const QUrl &pUrl)
{
    // The URL has changed, so let the user know whether it's our home page

    emit homePage(pUrl == mHomePage);
}

//==============================================================================

void WebViewerWidget::selectionChanged()
{
    // The text selection has changed, so let the user know whether some text is
    // now selected

    emit copyTextEnabled(!selectedText().isEmpty());
}

//==============================================================================

void WebViewerWidget::pageChanged()
{
    // We have a new page, resulting in the previous or next page becoming
    // either available or not

    QAction *action = qobject_cast<QAction *>(sender());

    if (action == pageAction(QWebPage::Back))
        emit backEnabled(action->isEnabled());
    else if (action == pageAction(QWebPage::Forward))
        emit forwardEnabled(action->isEnabled());
}

//==============================================================================

}   // namespace WebViewerWidget
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
