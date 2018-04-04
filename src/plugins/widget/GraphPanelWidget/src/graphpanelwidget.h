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
// Graph panel widget
//==============================================================================

#pragma once

//==============================================================================

#include "graphpanelplotwidget.h"
#include "graphpanelwidgetglobal.h"
#include "widget.h"

//==============================================================================

#include <QList>

//==============================================================================

namespace OpenCOR {
namespace GraphPanelWidget {

//==============================================================================

class GraphPanelWidget;

//==============================================================================

typedef QList<GraphPanelWidget *> GraphPanelWidgets;

//==============================================================================

class GRAPHPANELWIDGET_EXPORT GraphPanelWidget : public Core::Widget
{
    Q_OBJECT

public:
    explicit GraphPanelWidget(const GraphPanelWidgets &pNeighbors,
                              QAction *pSynchronizeXAxisAction,
                              QAction *pSynchronizeYAxisAction,
                              QWidget *pParent);
    explicit GraphPanelWidget(const GraphPanelWidgets &pNeighbors,
                              QWidget *pParent);
    ~GraphPanelWidget() override;

    void retranslateUi() override;

    bool isActive() const;
    void setActive(bool pActive, bool pForce = false);

    GraphPanelPlotWidget * plot() const;

    GraphPanelPlotGraphs graphs() const;

    void addGraph(GraphPanelPlotGraph *pGraph,
                  const GraphPanelPlotGraphProperties &pGraphProperties = GraphPanelPlotGraphProperties());

    void removeGraphs(const GraphPanelPlotGraphs &pGraphs);
    void removeAllGraphs();

protected:
    void changeEvent(QEvent *pEvent) override;
    void mousePressEvent(QMouseEvent *pEvent) override;

private:
    QFrame *mMarker;
    GraphPanelPlotWidget *mPlot;

    bool mActive;

    void updateMarkerColor();

signals:
    void activated(GraphPanelWidget *pGraphPanel);
    void inactivated(GraphPanelWidget *pGraphPanel);

    void graphAdded(GraphPanelWidget *pGraphPanel, GraphPanelPlotGraph *pGraph,
                    const GraphPanelPlotGraphProperties &pGraphProperties);
    void graphsRemoved(GraphPanelWidget *pGraphPanel,
                       const GraphPanelPlotGraphs &pGraphs);
};

//==============================================================================

}   // namespace GraphPanelWidget
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
