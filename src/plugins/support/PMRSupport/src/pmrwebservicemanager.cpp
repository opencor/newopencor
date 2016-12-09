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
// PMR network manager
//==============================================================================

#include "corecliutils.h"
#include "pmrauthentication.h"
#include "pmrwebservice.h"
#include "pmrwebservicemanager.h"
#include "pmrwebserviceresponse.h"

//==============================================================================

#include <QDesktopServices>
#include <QNetworkReply>
#include <QNetworkRequest>

//==============================================================================

#include "o1requestor.h"

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

PmrWebServiceManager::PmrWebServiceManager(PmrWebService *pPmrWebService) :
    QNetworkAccessManager(pPmrWebService),
    mPmrWebService(pPmrWebService)
{
    // Create an OAuth client for authenticated requests to the Physiome Model Repository

    mPmrOAuthClient = new PmrOAuthClient(pPmrWebService->Url(), this);

    // Make sure that we get told when there are SSL errors (which would happen if the
    // website's certificate is invalid, e.g. it has expired)

    connect(this, SIGNAL(sslErrors(QNetworkReply *, QList<QSslError>)),
            this, SLOT(sslErrors(QNetworkReply *, QList<QSslError>)));

    // Connect some signals

    connect(mPmrOAuthClient, SIGNAL(linkingFailed()), this, SLOT(authenticationFailed()));
    connect(mPmrOAuthClient, SIGNAL(linkingSucceeded()), this, SLOT(authenticationSucceeded()));
    connect(mPmrOAuthClient, SIGNAL(openBrowser(QUrl)), this, SLOT(openBrowser(QUrl)));
}

//==============================================================================

PmrWebServiceManager::~PmrWebServiceManager()
{
}

//==============================================================================

void PmrWebServiceManager::authenticate(const bool &pLink)
{
    if (pLink) mPmrOAuthClient->link();
    else       mPmrOAuthClient->unlink();
}

//==============================================================================

void PmrWebServiceManager::authenticationFailed()
{
    emit authenticated(false);
}

//==============================================================================

void PmrWebServiceManager::authenticationSucceeded()
{
    PmrOAuthClient *o1t = qobject_cast<PmrOAuthClient *>(sender());

    emit authenticated(o1t->linked());
}

//==============================================================================

bool PmrWebServiceManager::isAuthenticated(void) const
{
    return mPmrOAuthClient->linked();
}

//==============================================================================

void PmrWebServiceManager::openBrowser(const QUrl &pUrl)
{
    QDesktopServices::openUrl(pUrl);
}

//==============================================================================

void PmrWebServiceManager::sslErrors(QNetworkReply *pNetworkReply,
                                     const QList<QSslError> &pSslErrors)
{
    // Ignore the SSL errors since we trust the website and therefore its
    // certificate (even if it is invalid, e.g. it has expired)

    pNetworkReply->ignoreSslErrors(pSslErrors);
}

//==============================================================================

PmrWebServiceResponse *PmrWebServiceManager::sendPmrRequest(const QString &pUrl,
                                                            const bool &pSecureRequest,
                                                            const bool &pUsePost,
                                                            const QJsonDocument &pJsonDocument)
{
    // Check that we are connected to the Internet

    if (!Core::internetConnectionAvailable()) {

        emit error(tr("Cannot connect to Internet"), false);
        return nullptr;
    }

    // Let the user know that we are busy

    emit busy(true);

    // Send our request to the PMR, asking for the response to be compressed

    QNetworkRequest networkRequest;

    networkRequest.setRawHeader("Accept", "application/json");
    networkRequest.setRawHeader("Accept-Encoding", "gzip");
//    networkRequest.setRawHeader("Accept-Encoding", "identity");

    networkRequest.setUrl(QUrl(pUrl));

    QNetworkReply *networkReply;

    QByteArray requestData = pJsonDocument.isNull() ? ""
                                                    : pJsonDocument.toJson(QJsonDocument::Compact);

    // Use the authenticated link if it's available

    if (pSecureRequest && mPmrOAuthClient->linked()) {
        O1Requestor *requestor = new O1Requestor(this, mPmrOAuthClient, this);
        if (!pUsePost && pJsonDocument.isNull()) {
            networkReply = requestor->get(networkRequest, QList<O0RequestParameter>());
        }
        else {
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, PmrWebService::RequestMimeType());
            networkReply = requestor->post(networkRequest, QList<O0RequestParameter>(), requestData);
        }

    } else {
        if (!pUsePost && pJsonDocument.isNull()) {
            networkReply = this->get(networkRequest);
        }
        else {
            networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, PmrWebService::RequestMimeType());
            networkReply = this->post(networkRequest, requestData);
        }
    }

    auto pmrWebServiceResponse = new PmrWebServiceResponse(networkReply);
    connect(pmrWebServiceResponse, SIGNAL(busy(bool)),
            mPmrWebService, SIGNAL(busy(bool)));
    connect(pmrWebServiceResponse, SIGNAL(error(QString, bool)),
            mPmrWebService, SIGNAL(error(QString, bool)));
    connect(pmrWebServiceResponse, SIGNAL(unauthorised(QString)),
            mPmrWebService, SLOT(unauthorised(QString)));

    return pmrWebServiceResponse;
}

//==============================================================================

}   // namespace PMRSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

