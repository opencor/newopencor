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
// QScintillaWidget class
//==============================================================================

#include "filemanager.h"
#include "qscintillawidget.h"

//==============================================================================

#include <QDragEnterEvent>
#include <QMenu>
#include <QMimeData>

//==============================================================================

#include "Qsci/qscilexer.h"

//==============================================================================

namespace OpenCOR {
namespace QScintillaSupport {

//==============================================================================

void QScintillaWidget::constructor(const QString &pContents,
                                   const bool &pReadOnly, QsciLexer *pLexer)
{
    // Remove the frame around our Scintilla editor

    setFrameShape(QFrame::NoFrame);

    // Remove the margin number in our Scintilla editor

    setMarginWidth(SC_MARGIN_NUMBER, 0);

    // Associate a lexer to our Scintilla editor, should one be provided
    // Note: the default font family and size come from Qt Creator...

#if defined(Q_OS_WIN)
    mFont = QFont("Courier", 10);
#elif defined(Q_OS_LINUX)
    mFont = QFont("Monospace", 9);
#elif defined(Q_OS_MAC)
    mFont = QFont("Monaco", 12);
#else
    #error Unsupported platform
#endif

    if (pLexer) {
        // A lexer was provided, so specify its fonts and associate it with our
        // Scintilla editor

        pLexer->setFont(mFont);

        setLexer(pLexer);

        // Specify the type of tree folding to be used. Some lexers may indeed
        // use that feature, so...

        setFolding(QsciScintilla::BoxedTreeFoldStyle);
    } else {
        // No lexer was provided, so simply specify a default font family and
        // size for our Scintilla editor

        setFont(mFont);
    }

    // Set the contents of our Scintilla editor and its read-only property

    setContents(pContents);
    setReadOnly(pReadOnly);

    // Empty context menu by default

    mContextMenu = new QMenu(this);
}

//==============================================================================

QScintillaWidget::QScintillaWidget(const QString &pContents,
                                   const bool &pReadOnly,
                                   QsciLexer *pLexer, QWidget *pParent) :
    QsciScintilla(pParent)
{
    // Construct our object

    constructor(pContents, pReadOnly, pLexer);
}

//==============================================================================

QScintillaWidget::QScintillaWidget(const QString &pContents,
                                   const bool &pReadOnly, QWidget *pParent) :
    QsciScintilla(pParent)
{
    // Construct our object

    constructor(pContents, pReadOnly);
}

//==============================================================================

QScintillaWidget::QScintillaWidget(QsciLexer *pLexer, QWidget *pParent) :
    QsciScintilla(pParent)
{
    // Construct our object

    constructor(QString(), false, pLexer);
}

//==============================================================================

QScintillaWidget::QScintillaWidget(QWidget *pParent) :
    QsciScintilla(pParent)
{
    // Construct our object

    constructor();
}

//==============================================================================

QScintillaWidget::~QScintillaWidget()
{
    // Delete some internal objects

    delete mContextMenu;
}

//==============================================================================

QMenu * QScintillaWidget::contextMenu() const
{
    // Return our context menu

    return mContextMenu;
}

//==============================================================================

void QScintillaWidget::setContextMenu(const QList<QAction *> &pContextMenuActions)
{
    // Set our context menu

    mContextMenu->clear();

    foreach (QAction *action, pContextMenuActions)
        mContextMenu->addAction(action);
}

//==============================================================================

void QScintillaWidget::setContents(const QString &pContents)
{
    // Set our contents

    setText(pContents);
}

//==============================================================================

void QScintillaWidget::contextMenuEvent(QContextMenuEvent *pEvent)
{
    // Show our context menu or QsciScintilla's one, if we don't have one

    if (mContextMenu->isEmpty())
        QsciScintilla::contextMenuEvent(pEvent);
    else
        mContextMenu->exec(pEvent->globalPos());
}

//==============================================================================

void QScintillaWidget::dragEnterEvent(QDragEnterEvent *pEvent)
{
    // Accept the proposed action for the event, but only if we are not dropping
    // URIs
    // Note: this is not (currently?) needed on Windows and OS X, but if we
    //       don't have that check on Linux, then to drop some files on our
    //       Scintilla editor will result in the text/plain version of the data
    //       (e.g. file:///home/me/myFile) to be inserted in the text, so...

    if (!pEvent->mimeData()->hasFormat(Core::FileSystemMimeType))
        pEvent->acceptProposedAction();
    else
        pEvent->ignore();
}

//==============================================================================

void QScintillaWidget::keyPressEvent(QKeyEvent *pEvent)
{
    // Reset the font size, if needed

    if (   !(pEvent->modifiers() & Qt::ShiftModifier)
        &&  (pEvent->modifiers() & Qt::ControlModifier)
        && !(pEvent->modifiers() & Qt::AltModifier)
        && !(pEvent->modifiers() & Qt::MetaModifier)
        &&  (pEvent->key() == Qt::Key_0)) {
        zoomTo(0);
    } else {
        QsciScintilla::keyPressEvent(pEvent);
    }
}

//==============================================================================

void QScintillaWidget::wheelEvent(QWheelEvent *pEvent)
{
    // Increasing/decrease the font size, if needed

    if (pEvent->modifiers() == Qt::ControlModifier) {
        int delta = pEvent->delta();

        if (delta > 0)
            zoomIn();
        else if (delta < 0)
            zoomOut();

        pEvent->accept();
    } else {
        QsciScintilla::wheelEvent(pEvent);
    }
}

//==============================================================================

}   // namespace QScintillaSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
