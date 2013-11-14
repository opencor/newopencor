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
// Common widget
//==============================================================================

#ifndef COMMONWIDGET_H
#define COMMONWIDGET_H

//==============================================================================

#include "coreglobal.h"
#include "plugin.h"

//==============================================================================

#include <QtGlobal>

//==============================================================================

#include <QColor>
#include <QString>

//==============================================================================

class QSettings;
class QSize;

//==============================================================================

#include "disableunusedvariablewarning.inl"
    namespace OpenCOR {
    namespace Core {
        // Note: the default font family and size below were taken from Qt
        //       Creator...

        #if defined(Q_OS_WIN)
            static const char *DefaultFontFamily = "Courier";
            enum {
                DefaultFontSize = 10
            };
        #elif defined(Q_OS_LINUX)
            static const char *DefaultFontFamily = "Monospace";
            enum {
                DefaultFontSize = 9
            };
        #elif defined(Q_OS_MAC)
            static const char *DefaultFontFamily = "Monaco";
            enum {
                DefaultFontSize = 12
            };
        #else
            #error Unsupported platform
        #endif
    }   // namespace Core
    }   // namespace OpenCOR
#include "enableunusedvariablewarning.inl"

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

class CORE_EXPORT CommonWidget
{
public:
    explicit CommonWidget(QWidget *pParent = 0);

    virtual void loadSettings(QSettings *pSettings);
    virtual void saveSettings(QSettings *pSettings) const;

    virtual void settingsLoaded(const Plugins &pLoadedPlugins);

    virtual void retranslateUi();

    static QColor borderColor();

    static QColor baseColor();
    static QColor windowColor();
    static QColor highlightColor();

protected:
    QSize defaultSize(const double &pRatio) const;

    void drawBorder(const bool &pDockedTop, const bool &pDockedLeft,
                    const bool &pDockedBottom, const bool &pDockedRight,
                    const bool &pFloatingTop, const bool &pFloatingLeft,
                    const bool &pFloatingBottom, const bool &pFloatingRight);

private:
    QWidget *mParent;

    static QColor specificColor(const QString &pColor);
};

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================

#endif

//==============================================================================
// End of file
//==============================================================================
