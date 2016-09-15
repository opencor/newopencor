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
// KINSOL solver plugin
//==============================================================================

#include "kinsolsolver.h"
#include "kinsolsolverplugin.h"

//==============================================================================

namespace OpenCOR {
namespace KINSOLSolver {

//==============================================================================

PLUGININFO_FUNC KINSOLSolverPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", QString::fromUtf8("a plugin that uses <a href=\"http://computation.llnl.gov/projects/sundials/kinsol\">KINSOL</a> to solve non-linear systems."));
    descriptions.insert("fr", QString::fromUtf8("une extension qui utilise <a href=\"http://computation.llnl.gov/projects/sundials/kinsol\">KINSOL</a> pour résoudre des systèmes non-linéaires."));

    return new PluginInfo(PluginInfo::Solver, true, false,
                          QStringList() << "SUNDIALS",
                          descriptions);
}

//==============================================================================
// Solver interface
//==============================================================================

Solver::Solver * KINSOLSolverPlugin::solverInstance() const
{
    // Create and return an instance of the solver

    return new KinsolSolver();
}

//==============================================================================

QString KINSOLSolverPlugin::id(const QString &pKisaoId) const
{
    // Return the id for the given KiSAO id

    if (!pKisaoId.compare("KISAO:0000282"))
        return solverName();

    return QString();
}

//==============================================================================

QString KINSOLSolverPlugin::kisaoId(const QString &pId) const
{
    // Return the KiSAO id for the given id

    if (!pId.compare(solverName()))
        return "KISAO:0000282";

    return QString();
}

//==============================================================================

Solver::Type KINSOLSolverPlugin::solverType() const
{
    // Return the type of the solver

    return Solver::Nla;
}

//==============================================================================

QString KINSOLSolverPlugin::solverName() const
{
    // Return the name of the solver

    return "KINSOL";
}

//==============================================================================

Solver::Properties KINSOLSolverPlugin::solverProperties() const
{
    // Return the properties supported by the solver, i.e. none in our case

    return Solver::Properties();
}

//==============================================================================

QMap<QString, bool> KINSOLSolverPlugin::solverPropertiesVisibility(const QMap<QString, QString> &pSolverPropertiesValues) const
{
    Q_UNUSED(pSolverPropertiesValues);

    // We don't handle this interface...

    return QMap<QString, bool>();
}

//==============================================================================

}   // namespace KINSOLSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
