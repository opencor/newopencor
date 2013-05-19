//==============================================================================
// Raw CellML view widget
//==============================================================================

#include "borderedwidget.h"
#include "coreutils.h"
#include "qscintillawidget.h"
#include "rawcellmlviewwidget.h"
#include "viewerwidget.h"

//==============================================================================

#include "ui_rawcellmlviewwidget.h"

//==============================================================================

#include <QDesktopWidget>
#include <QFileInfo>
#include <QSettings>
#include <QSplitter>
#include <QTextStream>

//==============================================================================

#include "Qsci/qscilexerxml.h"

//==============================================================================

namespace OpenCOR {
namespace RawCellMLView {

//==============================================================================

RawCellMLViewWidget::RawCellMLViewWidget(QWidget *pParent) :
    QSplitter(pParent),
    CommonWidget(pParent),
    mGui(new Ui::RawCellMLViewWidget),
    mBorderedEditor(0),
    mBorderedEditors(QMap<QString, Core::BorderedWidget *>()),
    mBorderedViewerHeight(0),
    mBorderedEditorHeight(0)
{
    // Set up the GUI

    mGui->setupUi(this);

    // Keep track of our sizes when moving the splitter

    connect(this, SIGNAL(splitterMoved(int,int)),
            this, SLOT(splitterMoved()));

    // Create our viewer

    mViewer = new Viewer::ViewerWidget(this);
    mBorderedViewer = new Core::BorderedWidget(mViewer,
                                               false, false, true, false);

    // Add the bordered viewer to ourselves

    addWidget(mBorderedViewer);
}

//==============================================================================

RawCellMLViewWidget::~RawCellMLViewWidget()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

static const QString SettingsViewerHeight = "ViewerHeight";
static const QString SettingsEditorHeight = "EditorHeight";

//==============================================================================

void RawCellMLViewWidget::loadSettings(QSettings *pSettings)
{
    // Retrieve the viewer's and editor's height
    // Note #1: the viewer's default height is 19% of the desktop's height while
    //          that of the editor is as big as it can be...
    // Note #2: because the editor's default height is much bigger than that of
    //          our raw CellML view widget, the viewer's default height will
    //          effectively be less than 19% of the desktop's height, but that
    //          doesn't matter at all...

    mBorderedViewerHeight = pSettings->value(SettingsViewerHeight,
                                             0.19*qApp->desktop()->screenGeometry().height()).toInt();
    mBorderedEditorHeight = pSettings->value(SettingsEditorHeight,
                                             qApp->desktop()->screenGeometry().height()).toInt();
}

//==============================================================================

void RawCellMLViewWidget::saveSettings(QSettings *pSettings) const
{
    // Keep track of the viewer's and editor's height
    // Note #1: we must also keep track of the editor's height because when
    //          loading our settings (see above), the widget doesn't yet have a
    //          'proper' height, so we couldn't simply assume that the editor's
    //          initial height is this widget's height minus the viewer's
    //          initial height, so...
    // Note #2: we rely on mBorderedViewerHeight and mBorderedEditorHeight
    //          rather than directly calling the height() method of the viewer
    //          and of the editor, respectively since it may happen that the
    //          user exits OpenCOR without ever having switched to the raw
    //          CellML view, in which case we couldn't retrieve the viewer and
    //          editor's height which in turn would result in OpenCOR crashing,
    //          so...

    pSettings->setValue(SettingsViewerHeight, mBorderedViewerHeight);
    pSettings->setValue(SettingsEditorHeight, mBorderedEditorHeight);
}

//==============================================================================

void RawCellMLViewWidget::initialize(const QString &pFileName)
{
    // Retrieve the editor associated with the file name, if any

    mBorderedEditor = mBorderedEditors.value(pFileName);

    if (!mBorderedEditor) {
        // No editor exists for the file name, so create and set up a Scintilla
        // editor with an XML lexer associated with it

        QFile file(pFileName);
        QString fileContents = QString();
        bool fileIsReadOnly = false;

        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            // We could open the file, so retrieve its contents and whether it
            // can be written to

            fileContents = QTextStream(&file).readAll();
            fileIsReadOnly = !(QFileInfo(pFileName).isWritable());

            // We are done with the file, so close it

            file.close();
        }

        mBorderedEditor = new Core::BorderedWidget(new QScintillaSupport::QScintillaWidget(fileContents,
                                                                                           fileIsReadOnly,
                                                                                           new QsciLexerXML(this),
                                                                                           parentWidget()),
                                                   true, false, false, false);

        // Keep track of our bordered editor and add it to ourselves

        mBorderedEditors.insert(pFileName, mBorderedEditor);

        addWidget(mBorderedEditor);
    }

    // Show/hide our bordered editors and adjust our sizes

    QList<int> newSizes = QList<int>() << mBorderedViewerHeight;

    for (int i = 1, iMax = count(); i < iMax; ++i) {
        Core::BorderedWidget *borderedEditor = static_cast<Core::BorderedWidget *>(widget(i));

        if (borderedEditor == mBorderedEditor) {
            // This is the editor we are after, so show it and set its size

            borderedEditor->show();

            newSizes << mBorderedEditorHeight;
        } else {
            // Not the editor we are after, so hide it and set its size
            // Note: theoretically speaking, we could set its size to whatever
            //       value we want since it's anyway hidden...

            borderedEditor->hide();

            newSizes << 0;
        }
    }

    setSizes(newSizes);

    // Set the raw CellML view widget's focus proxy to our 'new' editor and
    // make sure that it immediately gets the focus
    // Note: if we were not to immediately give our 'new' editor the focus,
    //       then the central widget would give the focus to our 'old' editor
    //       (see CentralWidget::updateGui()), so...

    setFocusProxy(mBorderedEditor->widget());

    mBorderedEditor->widget()->setFocus();
}

//==============================================================================

bool RawCellMLViewWidget::isManaged(const QString &pFileName) const
{
    // Return whether the given file name is managed

    return mBorderedEditors.value(pFileName);
}

//==============================================================================

void RawCellMLViewWidget::finalize(const QString &pFileName)
{
    // Remove the bordered editor, should there be one for the given file name

    Core::BorderedWidget *borderedEditor  = mBorderedEditors.value(pFileName);

    if (borderedEditor) {
        // There is a bordered editor for the given file name, so delete it and
        // remove it from our list

        delete borderedEditor;

        mBorderedEditors.remove(pFileName);

        // Reset our memory of the current bordered editor

        mBorderedEditor = 0;
    }
}

//==============================================================================

void RawCellMLViewWidget::splitterMoved()
{
    // The splitter has moved, so keep track of the viewer and editor's new
    // height

    mBorderedViewerHeight = mBorderedViewer->height();
    mBorderedEditorHeight = mBorderedEditor->height();
}

//==============================================================================

}   // namespace RawCellMLView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
