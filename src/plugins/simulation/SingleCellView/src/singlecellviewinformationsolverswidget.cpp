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
// Single Cell view information solvers widget
//==============================================================================

#include "cellmlfileruntime.h"
#include "corecliutils.h"
#include "singlecellviewinformationsolverswidget.h"
#include "singlecellviewplugin.h"
#include "singlecellviewsimulation.h"

//==============================================================================

namespace OpenCOR {
namespace SingleCellView {

//==============================================================================

SingleCellViewInformationSolversWidgetData::SingleCellViewInformationSolversWidgetData(const QMap<QString, SolverInterface *> &pSolversInterfaces,
                                                                                       Core::Property *pSolversProperty,
                                                                                       Core::Property *pSolversListProperty,
                                                                                       const QMap<QString, Core::Properties> &pSolversProperties) :
    mSolversInterfaces(pSolversInterfaces),
    mSolversProperty(pSolversProperty),
    mSolversListProperty(pSolversListProperty),
    mSolversProperties(pSolversProperties)
{
}

//==============================================================================

QMap<QString, SolverInterface *> SingleCellViewInformationSolversWidgetData::solversInterfaces() const
{
    // Return our solvers interfaces

    return mSolversInterfaces;
}

//==============================================================================

Core::Property * SingleCellViewInformationSolversWidgetData::solversProperty() const
{
    // Return our solvers property

    return mSolversProperty;
}

//==============================================================================

Core::Property * SingleCellViewInformationSolversWidgetData::solversListProperty() const
{
    // Return our solvers list property

    return mSolversListProperty;
}

//==============================================================================

QMap<QString, Core::Properties> SingleCellViewInformationSolversWidgetData::solversProperties() const
{
    // Return our solvers properties

    return mSolversProperties;
}

//==============================================================================

SingleCellViewInformationSolversWidget::SingleCellViewInformationSolversWidget(SingleCellViewPlugin *pPlugin,
                                                                               QWidget *pParent) :
    PropertyEditorWidget(true, pParent),
    mDescriptions(QMap<Core::Property *, Descriptions>())
{
    // Remove all our properties

    removeAllProperties();

    // Add properties for our different solvers

    mOdeSolverData = addSolverProperties(pPlugin->solverInterfaces(), Solver::Ode);
    mDaeSolverData = addSolverProperties(pPlugin->solverInterfaces(), Solver::Dae);
    mNlaSolverData = addSolverProperties(pPlugin->solverInterfaces(), Solver::Nla);

    // Show/hide the relevant properties

    if (mOdeSolverData)
        doSolverChanged(mOdeSolverData, mOdeSolverData->solversListProperty()->value());

    if (mDaeSolverData)
        doSolverChanged(mDaeSolverData, mDaeSolverData->solversListProperty()->value());

    if (mNlaSolverData)
        doSolverChanged(mNlaSolverData, mNlaSolverData->solversListProperty()->value());

    // Expand all our properties

    expandAll();

    // Keep track of changes to list properties

    connect(this, SIGNAL(propertyChanged(Core::Property *)),
            this, SLOT(solverChanged(Core::Property *)));
}

//==============================================================================

SingleCellViewInformationSolversWidget::~SingleCellViewInformationSolversWidget()
{
    // Delete some internal objects

    delete mOdeSolverData;
    delete mDaeSolverData;
    delete mNlaSolverData;
}

//==============================================================================

void SingleCellViewInformationSolversWidget::retranslateUi()
{
    // Update our property names

    if (mOdeSolverData) {
        mOdeSolverData->solversProperty()->setName(tr("ODE solver"));
        mOdeSolverData->solversListProperty()->setName(tr("Name"));

        mOdeSolverData->solversListProperty()->setEmptyListValue(tr("None available"));
    }

    if (mDaeSolverData) {
        mDaeSolverData->solversProperty()->setName(tr("DAE solver"));
        mDaeSolverData->solversListProperty()->setName(tr("Name"));

        mDaeSolverData->solversListProperty()->setEmptyListValue(tr("None available"));
    }

    if (mNlaSolverData) {
        mNlaSolverData->solversProperty()->setName(tr("NLA solver"));
        mNlaSolverData->solversListProperty()->setName(tr("Name"));

        mNlaSolverData->solversListProperty()->setEmptyListValue(tr("None available"));
    }

    // Update the name of our various properties, should they have a description
    // associated with them
    // Note: this is effectively to have the description of our solvers'
    //       properties properly updated...

    foreach (Core::Property *property, properties()) {
        if (mDescriptions.contains(property)) {
            // The property has a description associated with it, so retrieve
            // the version, if any, which corresponds to our current locale

            Descriptions descriptions = mDescriptions.value(property);
            QString description = descriptions.value(Core::locale());

            if (description.isEmpty()) {
                // No description exists for the current locale, so retrieve the
                // English description (which, hopefully, exists)

                description = descriptions.value("en");
            }

            // Set the name of the property to the description

            property->setName(description);
        }
    }

    // Default retranslation
    // Note: we must do it last since we set the empty list value of some
    //       properties above...

    PropertyEditorWidget::retranslateUi();
}

//==============================================================================

SingleCellViewInformationSolversWidgetData * SingleCellViewInformationSolversWidget::addSolverProperties(const SolverInterfaces &pSolverInterfaces,
                                                                                                         const Solver::Type &pSolverType)
{
    // Make sure that we have at least one solver interface

    if (pSolverInterfaces.isEmpty())
        return 0;

    // Retrieve the name of the solvers which type is the one in which we are
    // interested

    QMap<QString, SolverInterface *> solversInterfaces = QMap<QString, SolverInterface *>();
    Core::Property *solversProperty = 0;
    Core::Property *solversListProperty = 0;
    QStringList solversNames = QStringList();
    QMap<QString, Core::Properties> solversProperties = QMap<QString, Core::Properties>();

    foreach (SolverInterface *solverInterface, pSolverInterfaces) {
        if (solverInterface->solverType() == pSolverType) {
            // Keep track of the solver's interface

            solversInterfaces.insert(solverInterface->solverName(), solverInterface);

            // Keep track of the solver's name

            solversNames << solverInterface->solverName();

            // Add our solvers section property and list property for our
            // solvers, if needed

            if (!solversProperty) {
                solversProperty = addSectionProperty();

                solversListProperty = addListProperty(solversProperty);
            }

            // Add the solver's properties

            Core::Property *property = 0;
            Core::Properties properties = Core::Properties();

            foreach (const Solver::Property &solverInterfaceProperty,
                     solverInterface->solverProperties()) {
                // Add the solver's property and set its default value

                switch (solverInterfaceProperty.type()) {
                case Solver::Property::Boolean:
                    property = addBooleanProperty(solverInterfaceProperty.defaultValue().toBool(),
                                                  solversProperty);

                    break;
                case Solver::Property::Integer:
                    property = addIntegerProperty(solverInterfaceProperty.defaultValue().toInt(),
                                                  solversProperty);

                    break;
                case Solver::Property::Double:
                    property = addDoubleProperty(solverInterfaceProperty.defaultValue().toDouble(),
                                                 solversProperty);

                    break;
                case Solver::Property::List:
                    property = addListProperty(solverInterfaceProperty.listValues(),
                                               solverInterfaceProperty.defaultValue().toString(),
                                               solversProperty);

                    break;
                }

                property->setId(solverInterfaceProperty.id());

                // Set the solver's property's 'unit', if needed

                if (solverInterfaceProperty.hasVoiUnit()) {
                    property->setUnit("???");
                    // Note: to assign a non-empty string to our unit item is
                    //       just a way for us to make sure that the property's
                    //       will get initialised (see setPropertiesUnit())...
                }

                // Keep track of the solver's property

                properties << property;

                // Keep track of the solver's property's descriptions

                mDescriptions.insert(property, solverInterfaceProperty.descriptions());
            }

            // Keep track of the solver's properties

            solversProperties.insert(solverInterface->solverName(), properties);
        }
    }

    // Check whether we have at least one solver

    if (solversListProperty) {
        // We have a solvers list property, which means that we have at least
        // one solver, so sort our list

        solversNames.sort();

        // Assign the list of solvers to our list property

        solversListProperty->setListValues(solversNames);

        // Create and return our solver data

        return new SingleCellViewInformationSolversWidgetData(solversInterfaces,
                                                              solversProperty,
                                                              solversListProperty,
                                                              solversProperties);
    } else {
        return 0;
    }
}

//==============================================================================

void SingleCellViewInformationSolversWidget::setPropertiesUnit(SingleCellViewInformationSolversWidgetData *pSolverData,
                                                               const QString &pVoiUnit)
{
    // Make sure that we have some solver's data

    if (!pSolverData)
        return;

    // Go through the solvers' properties and set the unit of the relevant ones

    foreach (const Core::Properties &properties, pSolverData->solversProperties()) {
        foreach (Core::Property *property, properties) {
            if (!property->unit().isEmpty())
                property->setUnit(pVoiUnit);
        }
    }
}

//==============================================================================

void SingleCellViewInformationSolversWidget::initialize(SingleCellViewSimulation *pSimulation)
{
    // Make sure that the CellML file runtime is valid

    if (pSimulation->runtime()->isValid()) {
        // Show/hide the ODE/DAE/NLA solver information

        if (mOdeSolverData)
            mOdeSolverData->solversProperty()->setVisible(pSimulation->runtime()->needOdeSolver());

        if (mDaeSolverData)
            mDaeSolverData->solversProperty()->setVisible(pSimulation->runtime()->needDaeSolver());

        if (mNlaSolverData)
            mNlaSolverData->solversProperty()->setVisible(pSimulation->runtime()->needNlaSolver());
    }

    // Set the unit of our different properties, if needed

    QString voiUnit = pSimulation->runtime()->variableOfIntegration()->unit();

    setPropertiesUnit(mOdeSolverData, voiUnit);
    setPropertiesUnit(mDaeSolverData, voiUnit);
    setPropertiesUnit(mNlaSolverData, voiUnit);

    // Initialise our simulation's NLA solver's properties, so that we can then
    // properly reset our simulation the first time round

    if (mNlaSolverData) {
        pSimulation->data()->setNlaSolverName(mNlaSolverData->solversListProperty()->value(), false);

        foreach (Core::Property *property, mNlaSolverData->solversProperties().value(pSimulation->data()->nlaSolverName())) {
            pSimulation->data()->addNlaSolverProperty(property->id(),
                                                      (property->type() == Core::Property::Integer)?
                                                          property->integerValue():
                                                          property->doubleValue(),
                                                      false);
        }
    }
}

//==============================================================================

QStringList SingleCellViewInformationSolversWidget::odeSolvers() const
{
    // Return the available ODE solvers, if any

    return mOdeSolverData?mOdeSolverData->solversListProperty()->listValues():QStringList();
}

//==============================================================================

QStringList SingleCellViewInformationSolversWidget::daeSolvers() const
{
    // Return the available DAE solvers, if any

    return mDaeSolverData?mDaeSolverData->solversListProperty()->listValues():QStringList();
}

//==============================================================================

QStringList SingleCellViewInformationSolversWidget::nlaSolvers() const
{
    // Return the available NLA solvers, if any

    return mNlaSolverData?mNlaSolverData->solversListProperty()->listValues():QStringList();
}

//==============================================================================

SingleCellViewInformationSolversWidgetData * SingleCellViewInformationSolversWidget::odeSolverData() const
{
    // Return our ODE solver data

    return mOdeSolverData;
}

//==============================================================================

SingleCellViewInformationSolversWidgetData * SingleCellViewInformationSolversWidget::daeSolverData() const
{
    // Return our DAE solver data

    return mDaeSolverData;
}

//==============================================================================

SingleCellViewInformationSolversWidgetData * SingleCellViewInformationSolversWidget::nlaSolverData() const
{
    // Return our NLA solver data

    return mNlaSolverData;
}

//==============================================================================

void SingleCellViewInformationSolversWidget::updateSolverGui(SingleCellViewInformationSolversWidgetData *pSolverData)
{
    // Make sure that we have some solver data

    if (!pSolverData)
        return;

    // Update our solver properties visibility

    QString solverName = pSolverData->solversListProperty()->value();
    SolverInterface *solverInterface = pSolverData->solversInterfaces().value(solverName);
    Core::Properties solverProperties = pSolverData->solversProperties().value(solverName);

    // Retrieve our solver properties values

    QMap<QString, QString> solverPropertiesValues = QMap<QString, QString>();

    foreach (Core::Property *solverProperty, solverProperties)
        solverPropertiesValues.insert(solverProperty->id(),solverProperty->value());

    // Ask our solver interface to tell us about the visibility of its
    // properties based on their values

    QMap<QString, bool> solverPropertiesVisibility = solverInterface->solverPropertiesVisibility(solverPropertiesValues);

    // Update the visilibity of our solver properties
    // Note: if there is no visibility information for a given solver property,
    //       then it means that it should be visible...

    foreach (Core::Property *solverProperty, solverProperties)
        solverProperty->setVisible(solverPropertiesVisibility.value(solverProperty->id(), true));
}

//==============================================================================

void SingleCellViewInformationSolversWidget::updateGui(SingleCellViewInformationSolversWidgetData *pSolverData)
{
    // Update our solver(s) properties visibility

    if (mOdeSolverData && (!pSolverData || (pSolverData == mOdeSolverData)))
        updateSolverGui(mOdeSolverData);

    if (mDaeSolverData && (!pSolverData || (pSolverData == mDaeSolverData)))
        updateSolverGui(mDaeSolverData);

    if (mNlaSolverData && (!pSolverData || (pSolverData == mNlaSolverData)))
        updateSolverGui(mNlaSolverData);
}

//==============================================================================

void SingleCellViewInformationSolversWidget::doSolverChanged(SingleCellViewInformationSolversWidgetData *pSolverData,
                                                             const QString &pSolverName)
{
    // Make sure that we have some solver data

    if (!pSolverData)
        return;

    // Go through the different properties for the given type of solver and
    // show/hide whatever needs showing/hiding

    for (auto solverProperties = pSolverData->solversProperties().constBegin(),
              solverPropertiesEnd = pSolverData->solversProperties().constEnd();
         solverProperties != solverPropertiesEnd; ++solverProperties) {
        bool solverPropertiesVisible = !solverProperties.key().compare(pSolverName);

        foreach (Core::Property *solverProperty, solverProperties.value())
            solverProperty->setVisible(solverPropertiesVisible);
    }
}

//==============================================================================

void SingleCellViewInformationSolversWidget::solverChanged(Core::Property *pProperty)
{
    // Try, for the ODE/DAE/NLA solvers list property, to handle the change in
    // the list property
    // Note: the ODE/DAE/NLA solvers list property is our first property, hence
    //       we make sure that its row number is equal to zero (in case there is
    //       one or several other list properties, as is the case for the CVODE
    //       and IDA solvers)...

    if (!pProperty->row()) {
        doSolverChanged((mOdeSolverData && (pProperty == mOdeSolverData->solversListProperty()))?
                            mOdeSolverData:
                            (mDaeSolverData && (pProperty == mDaeSolverData->solversListProperty()))?
                                mDaeSolverData:
                                mNlaSolverData,
                        pProperty->value());
    }
}

//==============================================================================

}   // namespace SingleCellView
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
