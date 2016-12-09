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
// PMR authentication
//==============================================================================

#pragma once

//==============================================================================

#include "o1.h"

//==============================================================================

#include <Qt>

//==============================================================================

#include <QUrl>
#include <QVariantMap>

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

// PMR OAuth 1.0 client
// Constants must match values assigned to the Physiome Model Repository

class PmrOAuthClient: public O1
{
    Q_OBJECT

public:
    explicit PmrOAuthClient(const QString &pUrl, QObject *parent = 0);

private:
    static const char *CallbackUrl(void);
    static int         CallbackPort(void);

    // Authorisation Url templates

    static const QString &AccessTokenUrl(void);
    static const QString &AuthorizeUrl(void);
    static const QString &RequestTokenUrl(void);

    // Scope template for request

    static const QString &RequestScope(void);

    // Keep these out of the header file

    static const char *ConsumerKey(void);
    static const char *ConsumerSecret(void);
    static const char *EncryptionKey(void);
};

//==============================================================================

}   // namespace PMRWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
