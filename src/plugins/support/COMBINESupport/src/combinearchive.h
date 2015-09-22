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
// COMBINE archive class
//==============================================================================

#ifndef COMBINEARCHIVE_H
#define COMBINEARCHIVE_H

//==============================================================================

#include "combinesupportglobal.h"
#include "standardfile.h"

//==============================================================================

#include <QObject>

//==============================================================================

namespace OpenCOR {
namespace COMBINESupport {

//==============================================================================

class CombineArchiveFile
{
public:
    enum Format {
        Cellml,
        Cellml_1_0,
        Cellml_1_1,
        Sedml
    };

    explicit CombineArchiveFile(const QString &pFileName, const QString &pLocation,
                                const Format &pFormat);

    QString fileName() const;

    QString location() const;
    Format format() const;

private:
    QString mFileName;

    QString mLocation;
    Format mFormat;
};

//==============================================================================

typedef QList<CombineArchiveFile> CombineArchiveFiles;

//==============================================================================

class COMBINESUPPORT_EXPORT CombineArchive : public StandardSupport::StandardFile
{
    Q_OBJECT

public:
    explicit CombineArchive(const QString &pFileName);

    virtual bool load();
    virtual bool save(const QString &pNewFileName = QString());

    void addFile(const QString &pFileName, const QString &pLocation,
                 const CombineArchiveFile::Format &pFormat);

private:
    CombineArchiveFiles mCombineArchiveFiles;
};

//==============================================================================

}   // namespace COMBINESupport
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
