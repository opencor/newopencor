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
// Second-order Runge-Kutta solver plugin
//==============================================================================

#include "secondorderrungekuttasolver.h"
#include "secondorderrungekuttasolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace SecondOrderRungeKuttaSolver {

//==============================================================================

PLUGININFO_FUNC SecondOrderRungeKuttaSolverPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin that implements the second-order <a href=\"http://en.wikipedia.org/wiki/Runge–Kutta_methods\">Runge-Kutta method</a> to solve ODEs."));
    descriptions.insert("fr", QString::fromUtf8("une extension qui implémente la <a href=\"http://en.wikipedia.org/wiki/Runge–Kutta_methods\">méthode Runge-Kutta</a> du deuxième ordre pour résoudre des EDOs."));

    return new PluginInfo(PluginInfo::Solver, true, false,
                          QStringList(),
                          descriptions);
}

//==============================================================================
// I18n interface
//==============================================================================

void SecondOrderRungeKuttaSolverPlugin::retranslateUi()
{
    // We don't handle this interface...
    // Note: even though we don't handle this interface, we still want to
    //       support it since some other aspects of our plugin are
    //       multilingual...
}

//==============================================================================
// Solver interface
//==============================================================================

Solver::Solver * SecondOrderRungeKuttaSolverPlugin::solverInstance() const
{
    // Create and return an instance of the solver

    return new SecondOrderRungeKuttaSolver();
}

//==============================================================================

QString SecondOrderRungeKuttaSolverPlugin::id(const QString &pKisaoId) const
{
    // Return the id for the given KiSAO id

    if (!pKisaoId.compare("KISAO:0000381"))
        return solverName();
    else if (!pKisaoId.compare("KISAO:0000483"))
        return StepId;

    return QString();
}

//==============================================================================

QString SecondOrderRungeKuttaSolverPlugin::kisaoId(const QString &pId) const
{
    // Return the KiSAO id for the given id
    // Note: our second-order Runge-Kutta method is effectively the midpoint
    //       method...

    if (!pId.compare(solverName()))
        return "KISAO:0000381";
    else if (!pId.compare(StepId))
        return "KISAO:0000483";

    return QString();
}

//==============================================================================

Solver::Type SecondOrderRungeKuttaSolverPlugin::solverType() const
{
    // Return the type of the solver

    return Solver::Ode;
}

//==============================================================================

QString SecondOrderRungeKuttaSolverPlugin::solverName() const
{
    // Return the name of the solver

    return "Runge-Kutta (2nd order)";
}

//==============================================================================

Solver::Properties SecondOrderRungeKuttaSolverPlugin::solverProperties() const
{
    // Return the properties supported by the solver

    Descriptions stepDescriptions;

    stepDescriptions.insert("en", QString::fromUtf8("Step"));
    stepDescriptions.insert("fr", QString::fromUtf8("Pas"));

    return Solver::Properties() << Solver::Property(Solver::Property::Double, StepId, stepDescriptions, QStringList(), StepDefaultValue, true);
}

//==============================================================================

QMap<QString, bool> SecondOrderRungeKuttaSolverPlugin::solverPropertiesVisibility(const QMap<QString, QString> &pSolverPropertiesValues) const
{
    Q_UNUSED(pSolverPropertiesValues);

    // We don't handle this interface...

    return QMap<QString, bool>();
}

//==============================================================================

}   // namespace SecondOrderRungeKuttaSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
