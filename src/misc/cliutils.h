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
// CLI utilities
//==============================================================================

#pragma once

//==============================================================================

#include "coreglobal.h"
#include "plugininfo.h"

//==============================================================================

#include <QSslError>
#include <QString>

//==============================================================================

class QCoreApplication;
class QNetworkReply;

//==============================================================================

namespace OpenCOR {

//==============================================================================

#include "corecliutils.h.inl"

//==============================================================================

static const auto HomePageUrl = QStringLiteral("http://www.opencor.ws/");

//==============================================================================
// Note: both cliutils.h and corecliutils.h must specifically define
//       SynchronousFileDownloader. To have it in cliutils.h.inl is NOT good
//       enough since the MOC won't pick it up...

class SynchronousFileDownloader : public QObject
{
    Q_OBJECT

public:
    bool download(const QString &pUrl, QByteArray &pContents,
                  QString *pErrorMessage) const;

private slots:
    void networkAccessManagerSslErrors(QNetworkReply *pNetworkReply,
                                       const QList<QSslError> &pSslErrors);
};

//==============================================================================

void initQtMessagePattern();

void initPluginsPath(const QString &pAppFileName);

void initApplication(QString *pAppDate = 0);

QString applicationDescription(const bool &pGuiMode = true);

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
