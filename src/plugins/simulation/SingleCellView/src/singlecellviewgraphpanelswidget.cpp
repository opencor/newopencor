//==============================================================================
// Single cell view graph panels widget
//==============================================================================

#include "singlecellviewgraphpanelplotwidget.h"
#include "singlecellviewgraphpanelswidget.h"
#include "singlecellviewgraphpanelwidget.h"

//==============================================================================

#include <QSettings>
#include <QWheelEvent>

//==============================================================================

namespace OpenCOR {
namespace SingleCellView {

//==============================================================================

SingleCellViewGraphPanelsWidget::SingleCellViewGraphPanelsWidget(QWidget *pParent) :
    QSplitter(pParent),
    CommonWidget(pParent),
    mSplitterSizes(QList<int>())
{
    // Set our orientation

    setOrientation(Qt::Vertical);

    // Keep track of our movement
    // Note: we need to keep track of our movement so that saveSettings() can
    //       work fine even when we are not visible (which happens when a CellML
    //       file cannot be run for some reason or another)...

    connect(this, SIGNAL(splitterMoved(int,int)),
            this, SLOT(splitterMoved()));
}

//==============================================================================

static const QString SettingsGraphPanelsCount = "GraphPanelsCount";
static const QString SettingsActiveGraphPanel = "ActiveGraphPanel";
static const QString SettingsGraphPanelSize   = "GraphPanelSize%1";

//==============================================================================

void SingleCellViewGraphPanelsWidget::loadSettings(QSettings *pSettings)
{
    // Let the user know of a few default things about ourselves by emitting a
    // few signals

    emit removeGraphPanelsEnabled(false);

    // Retrieve the number of graph panels and create the corresponding number
    // of graphs

    int graphPanelsCount = pSettings->value(SettingsGraphPanelsCount, 1).toInt();

    if (!graphPanelsCount)
        // For some reason, the settings for the number of graph panels to be
        // created got messed up, so...

        graphPanelsCount = 1;

    for (int i = 0; i < graphPanelsCount; ++i)
        addGraphPanel();

    // Retrieve and set the size of each graph panel

    mSplitterSizes = QList<int>();

    for (int i = 0; i < graphPanelsCount; ++i)
        mSplitterSizes << pSettings->value(SettingsGraphPanelSize.arg(i)).toInt();

    setSizes(mSplitterSizes);

    // Select the graph panel that used to be active

    qobject_cast<SingleCellViewGraphPanelWidget *>(widget(pSettings->value(SettingsActiveGraphPanel, 0).toInt()))->setActive(true);
}

//==============================================================================

void SingleCellViewGraphPanelsWidget::saveSettings(QSettings *pSettings) const
{
    // Keep track of the number of graph panels, of which graph panel was the
    // active one, and of the size of each graph panel

    pSettings->setValue(SettingsGraphPanelsCount, mSplitterSizes.count());

    for (int i = 0, iMax = mSplitterSizes.count(); i < iMax; ++i)
        if (qobject_cast<SingleCellViewGraphPanelWidget *>(widget(i))->isActive()) {
            // We found the active graph panel, so...

            pSettings->setValue(SettingsActiveGraphPanel, i);

            break;
        }

    for (int i = 0, iMax = mSplitterSizes.count(); i < iMax; ++i)
        pSettings->setValue(SettingsGraphPanelSize.arg(i), mSplitterSizes[i]);
}

//==============================================================================

QList<SingleCellViewGraphPanelWidget *> SingleCellViewGraphPanelsWidget::graphPanels() const
{
    // Return all our graph panels

    QList<SingleCellViewGraphPanelWidget *> res = QList<SingleCellViewGraphPanelWidget *>();

    for (int i = 0, iMax = count(); i < iMax; ++i)
        res << qobject_cast<SingleCellViewGraphPanelWidget *>(widget(i));

    return res;
}

//==============================================================================

SingleCellViewGraphPanelWidget * SingleCellViewGraphPanelsWidget::activeGraphPanel() const
{
    // Return the active graph panel

    for (int i = 0, iMax = count(); i < iMax; ++i) {
        SingleCellViewGraphPanelWidget *graphPanel = qobject_cast<SingleCellViewGraphPanelWidget *>(widget(i));

        if (graphPanel->isActive())
            // We found the active graph panel, so...

            return graphPanel;
    }

    // There are no graph panels, so...
    // Note: we should never reach this point since there should always be at
    //       least one graph panel...

    return 0;
}

//==============================================================================

SingleCellViewGraphPanelWidget * SingleCellViewGraphPanelsWidget::addGraphPanel()
{
    // Keep track of the graph panels' original size

    QList<int> origSizes = sizes();

    // Create a new graph panel

    SingleCellViewGraphPanelWidget *res = new SingleCellViewGraphPanelWidget(this);

    // Add the graph panel to ourselves

    addWidget(res);

    // Resize the graph panels, thus making sure that their size is what it
    // should be (see issue #58)

    double scalingFactor = double(count()-1)/count();

    for (int i = 0, iMax = origSizes.count(); i < iMax; ++i)
        origSizes[i] *= scalingFactor;

    setSizes(origSizes << height()/count());

    // Keep track of whenever a graph panel gets activated

    connect(res, SIGNAL(activated(SingleCellViewGraphPanelWidget *)),
            this, SIGNAL(graphPanelActivated(SingleCellViewGraphPanelWidget *)));

    connect(res, SIGNAL(activated(SingleCellViewGraphPanelWidget *)),
            this, SLOT(updateGraphPanels(SingleCellViewGraphPanelWidget *)));

    // Keep track of the addition and removal of a graph

    connect(res, SIGNAL(graphAdded(SingleCellViewGraphPanelPlotGraph *)),
            this, SIGNAL(graphAdded(SingleCellViewGraphPanelPlotGraph *)));
    connect(res, SIGNAL(graphRemoved(SingleCellViewGraphPanelPlotGraph *)),
            this, SIGNAL(graphRemoved(SingleCellViewGraphPanelPlotGraph *)));

    // Activate the graph panel

    res->setActive(true);

    // Keep track of our new sizes

    splitterMoved();

    // Let people know that we have added a graph panel

    emit graphPanelAdded(res);

    // Let people know whether graph panels can be removed

    emit removeGraphPanelsEnabled(count() > 1);

    // Return our newly created graph panel

    return res;
}

//==============================================================================

void SingleCellViewGraphPanelsWidget::removeGraphPanel()
{
    if (count() == 1)
        // There is only one graph panel left, so...

        return;

    // Remove the current graph panel

    SingleCellViewGraphPanelWidget *graphPanel = 0;

    for (int i = 0, iMax = count(); i < iMax; ++i) {
        graphPanel = qobject_cast<SingleCellViewGraphPanelWidget *>(widget(i));

        if (graphPanel->isActive()) {
            // We are dealing with the currently active graph panel, so remove
            // it

            graphPanel->hide();

            delete graphPanel;

            // Activate the next graph panel or the last one available, if any

            if (i < count())
                // There is a next graph panel, so activate it

                qobject_cast<SingleCellViewGraphPanelWidget *>(widget(i))->setActive(true);
            else
                // We were dealing with the last graph panel, so just activate
                // the new last graph panel

                qobject_cast<SingleCellViewGraphPanelWidget *>(widget(count()-1))->setActive(true);

            // We are all done, so...

            break;
        }
    }

    // Keep track of our new sizes

    splitterMoved();

    // Let people know that we have removed a graph panel
    // Note: the reason we pass a pointer to a now non-existing graph panel is
    //       that some people interested in that signal might have used the
    //       pointer to keep track of some information, as is the case with
    //       SingleCellViewInformationGraphsWidget for example...

    emit graphPanelRemoved(graphPanel);

    // Let people know whether graph panels can be removed

    emit removeGraphPanelsEnabled(count() > 1);
}

//==============================================================================

void SingleCellViewGraphPanelsWidget::splitterMoved()
{
    // Our splitter has been moved, so keep track of its new sizes

    mSplitterSizes = sizes();
}

//==============================================================================

void SingleCellViewGraphPanelsWidget::updateGraphPanels(SingleCellViewGraphPanelWidget *pGraphPanel)
{
    // A graph panel has been activated, so inactivate all the others

    for (int i = 0, iMax = count(); i < iMax; ++i) {
        SingleCellViewGraphPanelWidget *graphPanel = qobject_cast<SingleCellViewGraphPanelWidget *>(widget(i));

        if (graphPanel != pGraphPanel)
            // We are not dealing with the graph panel that just got activated,
            // so inactivate it

            graphPanel->setActive(false);
    }
}

//==============================================================================

}   // namespace SingleCellView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
