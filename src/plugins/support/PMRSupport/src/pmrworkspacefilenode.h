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
// PMR workspace file node
//==============================================================================

#pragma once
//==============================================================================

#include "pmrsupportglobal.h"

//==============================================================================

#include <QList>
#include <QObject>
#include <QPair>
#include <QString>

//==============================================================================

namespace OpenCOR {
namespace PMRSupport {

//==============================================================================

class PMRSUPPORT_EXPORT PmrWorkspaceFileNode : public QObject
{
    Q_OBJECT

public:
    PmrWorkspaceFileNode(const QString &shortName, const QString &fullName,
                         const QPair<QChar, QChar> &pStatus=QPair<QChar, QChar>(),
                         PmrWorkspaceFileNode *parent=0);
    PmrWorkspaceFileNode *addChild(const QString &pName,
                                   const QPair<QChar, QChar> &pStatus=QPair<QChar, QChar>());
    const QString &shortName(void) const;
    const QString &fullName(void) const;
    const QPair<QChar, QChar> &status(void) const;
    QList<PmrWorkspaceFileNode *> children(void) const;
    bool hasChildren(void) const;
    void setStatus(const QPair<QChar, QChar> &pStatus);

private:
    QString mShortName;
    QString mFullName;
    QPair<QChar, QChar> mStatus;
    QList<PmrWorkspaceFileNode *> mChildren;
};

//==============================================================================

}   // namespace PMRSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
