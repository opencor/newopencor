/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// Graph panels widget
//==============================================================================

#include "coreguiutils.h"
#include "graphpanelswidget.h"
#include "i18ninterface.h"

//==============================================================================

#include <QAction>
#include <QSettings>

//==============================================================================

#include "sedmlapibegin.h"
    #include "sedml/SedDocument.h"
#include "sedmlapiend.h"

//==============================================================================

namespace OpenCOR {
namespace GraphPanelWidget {

//==============================================================================

GraphPanelsWidget::GraphPanelsWidget(QWidget *pParent) :
    Core::SplitterWidget(pParent),
    mGraphPanels(GraphPanelWidgets()),
    mActiveGraphPanel(0),
    mInternalSizes(QIntList()),
    mUseInternalSizes(true)
{
    // Set our orientation

    setOrientation(Qt::Vertical);

    // Create our actions

    mSynchronizeXAxisAction = Core::newAction(true, this);
    mSynchronizeYAxisAction = Core::newAction(true, this);

    connect(mSynchronizeXAxisAction, SIGNAL(triggered(bool)),
            this, SLOT(synchronizeXAxis()));
    connect(mSynchronizeYAxisAction, SIGNAL(triggered(bool)),
            this, SLOT(synchronizeYAxis()));

    // Check whether one of our splitters has move and, therefore, whether our
    // sizes have changed

    connect(this, SIGNAL(splitterMoved(int, int)),
            this, SLOT(stopUsingInternalSizes()));
}

//==============================================================================

void GraphPanelsWidget::retranslateUi()
{
    // Retranslate our actions

    I18nInterface::retranslateAction(mSynchronizeXAxisAction, tr("Synchonise X Axis"),
                                     tr("Synchronise the X axis of all graph panels"));
    I18nInterface::retranslateAction(mSynchronizeYAxisAction, tr("Synchonise Y Axis"),
                                     tr("Synchronise the Y axis of all graph panels"));

    // Retranslate all our graph panels

    foreach (GraphPanelWidget *graphPanel, mGraphPanels)
        graphPanel->retranslateUi();
}

//==============================================================================

void GraphPanelsWidget::initialize()
{
    // Create a default graph panel, if none exists

    if (mGraphPanels.isEmpty())
        addGraphPanel();
}

//==============================================================================

GraphPanelWidgets GraphPanelsWidget::graphPanels() const
{
    // Return our graph panels

    return mGraphPanels;
}

//==============================================================================

GraphPanelWidget * GraphPanelsWidget::activeGraphPanel() const
{
    // Return our active graph panel

    return mActiveGraphPanel;
}

//==============================================================================

GraphPanelWidget * GraphPanelsWidget::addGraphPanel(const bool &pActive)
{
    // Create a new graph panel, add it to ourselves and keep track of it

    GraphPanelWidget *res = new GraphPanelWidget(mGraphPanels,
                                                 mSynchronizeXAxisAction,
                                                 mSynchronizeYAxisAction,
                                                 this);

    mGraphPanels << res;

    // Resize the graph panels, if needed, making sure that their size is what
    // it should be
    // Note: we use our internal sizes for as long as possible (i.e. as long as
    //       none of our splitters has moved; although, we can reuse our
    //       internal sizes if we have only one graph panel left). Indeed, if we
    //       were to open a CellML file and create two graph panels, then their
    //       height would be based on actual heights since everything would be
    //       visible. On the other hand, if we were to open a SED-ML file with
    //       three graph panels and that we were to rely on their actual height
    //       to resize them, then their new height would be based on 'hidden'
    //       heights since nothing would be visible. Now, the problem is that
    //       those 'hidden' heights tend to be much smaller than actual heights.
    //       The end result is that the two sets of graph panels may end up
    //       being quite different, something that we can prevent by using our
    //       internal heights...

    if (mUseInternalSizes) {
        mInternalSizes << 1;

        setSizes(mInternalSizes);
    } else {
        QIntList oldSizes = sizes();
        QIntList newSizes = QIntList();
        int oldTotalHeight = height()-(mGraphPanels.count()-2)*handleWidth();
        int newTotalHeight = oldTotalHeight-handleWidth();
        double scalingFactor = double(mGraphPanels.count()-1)/mGraphPanels.count()*newTotalHeight/oldTotalHeight;

        for (int i = 0, iMax = oldSizes.count()-1; i < iMax; ++i)
            newSizes << round(scalingFactor*oldSizes[i]);

        setSizes(newSizes << newTotalHeight-std::accumulate(newSizes.begin(), newSizes.end(), 0));
    }

    // Keep track of whenever a graph panel gets activated

    connect(res, SIGNAL(activated(OpenCOR::GraphPanelWidget::GraphPanelWidget *)),
            this, SIGNAL(graphPanelActivated(OpenCOR::GraphPanelWidget::GraphPanelWidget *)));

    connect(res, SIGNAL(activated(OpenCOR::GraphPanelWidget::GraphPanelWidget *)),
            this, SLOT(updateGraphPanels(OpenCOR::GraphPanelWidget::GraphPanelWidget *)));

    // Keep track of the addition and removal of a graph

    connect(res, SIGNAL(graphAdded(OpenCOR::GraphPanelWidget::GraphPanelWidget *, OpenCOR::GraphPanelWidget::GraphPanelPlotGraph *, const OpenCOR::GraphPanelWidget::GraphPanelPlotGraphProperties &)),
            this, SIGNAL(graphAdded(OpenCOR::GraphPanelWidget::GraphPanelWidget *, OpenCOR::GraphPanelWidget::GraphPanelPlotGraph *, const OpenCOR::GraphPanelWidget::GraphPanelPlotGraphProperties &)));
    connect(res, SIGNAL(graphsRemoved(OpenCOR::GraphPanelWidget::GraphPanelWidget *, const OpenCOR::GraphPanelWidget::GraphPanelPlotGraphs &)),
            this, SIGNAL(graphsRemoved(OpenCOR::GraphPanelWidget::GraphPanelWidget *, const OpenCOR::GraphPanelWidget::GraphPanelPlotGraphs &)));

    // In/activate the graph panel

    res->setActive(pActive);

    // Let people know that we have added a graph panel

    emit graphPanelAdded(res, pActive);

    // Let people know whether graph panels can be removed

    emit removeGraphPanelsEnabled(mGraphPanels.count() > 1);

    // Synchronise the axes of our graph panels, if needed, and ensure that they
    // are all aligned with one another by forcing the setting of the axes of
    // our active graph panel
    // Note: at startup, activeGraphPanelPlot is (obviously) null, hence we use
    //       our newly created graph panel's plot instead...

    GraphPanelPlotWidget *activeGraphPanelPlot = mActiveGraphPanel?mActiveGraphPanel->plot():0;

    if (!activeGraphPanelPlot)
        activeGraphPanelPlot = mActiveGraphPanel->plot();

    activeGraphPanelPlot->setAxes(activeGraphPanelPlot->minX(),
                                  activeGraphPanelPlot->maxX(),
                                  activeGraphPanelPlot->minY(),
                                  activeGraphPanelPlot->maxY(),
                                  true, true, true, true, true);

    // Return our newly created graph panel

    return res;
}

//==============================================================================

bool GraphPanelsWidget::removeGraphPanel(GraphPanelWidget *pGraphPanel)
{
    if (!pGraphPanel)
        return false;

    // Retrieve the index of the given graph panel

    int index = mGraphPanels.indexOf(pGraphPanel);

    // Let people know that we have removed it (or, rather, about to remove it)
    // Note: we let people know before we actually delete the graph panel,
    //       because some people interested in that signal might have used the
    //       pointer to keep track of some information, as is done in
    //       SimulationExperimentViewInformationGraphPanelAndGraphsWidget for
    //       example...

    emit graphPanelRemoved(pGraphPanel);

    // Update some trackers
    // Note: mActiveGraphPanel will automatically get updated when another graph
    //       panel gets selected...

    mGraphPanels.removeOne(pGraphPanel);

    if (mUseInternalSizes)
        mInternalSizes.removeLast();

    if (mGraphPanels.count() == 1)
        mUseInternalSizes = true;

    // Now, we can delete our graph panel

    delete pGraphPanel;

    // Let people know whether graph panels can be removed

    emit removeGraphPanelsEnabled(mGraphPanels.count() > 1);

    // Activate the next graph panel or the last one available, if any

    if (index < mGraphPanels.count()) {
        // There is a next graph panel, so activate it

        mGraphPanels[index]->setActive(true);
    } else {
        // We were dealing with the last graph panel, so just activate the new
        // last graph panel

        mGraphPanels[mGraphPanels.count()-1]->setActive(true);
    }

    // Ask our active graph panel's plot to align itself against its neighbours,
    // if any

    if (!mGraphPanels.isEmpty())
        mActiveGraphPanel->plot()->forceAlignWithNeighbors();

    return true;
}

//==============================================================================

bool GraphPanelsWidget::removeCurrentGraphPanel()
{
    // Make sure that we don't have only one graph panel left

    if (mGraphPanels.count() == 1)
        return false;

    // Remove the current graph panel

    return removeGraphPanel(mActiveGraphPanel);
}

//==============================================================================

void GraphPanelsWidget::removeAllGraphPanels()
{
    // Remove all the graph panels, after having created an 'empty' one (since
    // we want at least one graph panel at any given point in time)

    addGraphPanel();

    while (mGraphPanels.count() > 1)
        removeGraphPanel(mGraphPanels.first());
}

//==============================================================================

void GraphPanelsWidget::setActiveGraphPanel(GraphPanelWidget *pGraphPanel)
{
    // Make sure that we own the given graph panel

    if (!mGraphPanels.contains(pGraphPanel))
        return;

    // Make the given graph panel the active one

    pGraphPanel->setActive(true, true);
}

//==============================================================================

QIntList GraphPanelsWidget::sizes() const
{
    // Return either our internal or default sizes, depending on our current
    // state

    return mUseInternalSizes?mInternalSizes:QSplitter::sizes();
}

//==============================================================================

void GraphPanelsWidget::setSizes(const QIntList &pSizes)
{
    // Set our sizes as internal and/or default sizes, depending on whether the
    // given sizes consist of ones

    foreach (const int &size, pSizes) {
        if (size != 1) {
            mUseInternalSizes = false;

            break;
        }
    }

    if (mUseInternalSizes)
        mInternalSizes = pSizes;

    QSplitter::setSizes(pSizes);
}

//==============================================================================

void GraphPanelsWidget::stopUsingInternalSizes()
{
    // Stop using our internal sizes

    mUseInternalSizes = false;
}

//==============================================================================

void GraphPanelsWidget::updateGraphPanels(OpenCOR::GraphPanelWidget::GraphPanelWidget *pGraphPanel)
{
    // Keep track of the newly activated graph panel

    mActiveGraphPanel = pGraphPanel;

    // Inactivate all the other graph panels

    foreach (GraphPanelWidget *graphPanel, mGraphPanels) {
        if (graphPanel != pGraphPanel) {
            // We are not dealing with the graph panel that just got activated,
            // so inactivate it

            graphPanel->setActive(false);
        }
    }
}

//==============================================================================

void GraphPanelsWidget::synchronizeXAxis()
{
    // Synchronise the X axis of our graph panels, if needed, by forcing the
    // setting of the axes of our active graph panel

    if (mSynchronizeXAxisAction->isChecked()) {
        GraphPanelPlotWidget *activeGraphPanelPlot = mActiveGraphPanel->plot();

        activeGraphPanelPlot->setAxes(activeGraphPanelPlot->minX(),
                                      activeGraphPanelPlot->maxX(),
                                      activeGraphPanelPlot->minY(),
                                      activeGraphPanelPlot->maxY(),
                                      true, true, true, true, false);
    }
}

//==============================================================================

void GraphPanelsWidget::synchronizeYAxis()
{
    // Synchronise the Y axis of our graph panels, if needed, by forcing the
    // setting of the axes of our active graph panel

    if (mSynchronizeYAxisAction->isChecked()) {
        GraphPanelPlotWidget *activeGraphPanelPlot = mActiveGraphPanel->plot();

        activeGraphPanelPlot->setAxes(activeGraphPanelPlot->minX(),
                                      activeGraphPanelPlot->maxX(),
                                      activeGraphPanelPlot->minY(),
                                      activeGraphPanelPlot->maxY(),
                                      true, true, true, false, true);
    }
}

//==============================================================================

}   // namespace GraphPanelWidget
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
