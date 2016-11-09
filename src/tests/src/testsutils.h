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
// Some useful tests-related functions
//==============================================================================

#pragma once

//==============================================================================

#include <QString>

//==============================================================================

#include <QtTest/QtTest>

//==============================================================================

namespace OpenCOR {

//==============================================================================

QString dirName(const QString &pDirName);
QString fileName(const QString &pFilePath);

QByteArray rawFileContents(const QString &pFileName);
QStringList fileContents(const QString &pFileName);

QString fileSha1(const QString &pFileName);

QStringList runCli(const QStringList pArguments);

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
