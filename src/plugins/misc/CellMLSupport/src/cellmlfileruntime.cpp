//==============================================================================
// CellML file runtime class
//==============================================================================

#include "cellmlfile.h"
#include "cellmlfileruntime.h"
#include "compilerengine.h"
#include "compilermath.h"

//==============================================================================

#include <QRegularExpression>
#include <QStringList>

//==============================================================================

#include "CCGSBootstrap.hpp"

//==============================================================================

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DynamicLibrary.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLSupport {

//==============================================================================

CellmlFileRuntimeParameter::CellmlFileRuntimeParameter(const QString &pName,
                                                       const int &pDegree,
                                                       const QString &pUnit,
                                                       const QString &pComponent,
                                                       const ParameterType &pType,
                                                       const int &pIndex) :
    mName(pName),
    mDegree(pDegree),
    mUnit(pUnit),
    mComponent(pComponent),
    mType(pType),
    mIndex(pIndex)
{
}

//==============================================================================

QString CellmlFileRuntimeParameter::name() const
{
    // Return our name

    return mName;
}

//==============================================================================

int CellmlFileRuntimeParameter::degree() const
{
    // Return our degree

    return mDegree;
}

//==============================================================================

QString CellmlFileRuntimeParameter::unit() const
{
    // Return our unit

    return mUnit;
}

//==============================================================================

QString CellmlFileRuntimeParameter::component() const
{
    // Return our component

    return mComponent;
}

//==============================================================================

CellmlFileRuntimeParameter::ParameterType CellmlFileRuntimeParameter::type() const
{
    // Return our type

    return mType;
}

//==============================================================================

int CellmlFileRuntimeParameter::index() const
{
    // Return our index

    return mIndex;
}

//==============================================================================

QString CellmlFileRuntimeParameter::formattedName() const
{
    // Return a formatted version of our name

    return mName+QString(mDegree, '\'');
}

//==============================================================================

QString CellmlFileRuntimeParameter::fullyFormattedName() const
{
    // Return a fully formatted version of our name

    return mComponent+"."+mName+QString(mDegree, '\'');
}

//==============================================================================

QString CellmlFileRuntimeParameter::formattedUnit(const QString &pVoiUnit) const
{
    // Return a formatted version of our unit

    QString perVoiUnitDegree = QString();

    if (mDegree) {
        perVoiUnitDegree += "/"+pVoiUnit;

        if (mDegree > 1)
            perVoiUnitDegree += mDegree;
    }

    return mUnit+perVoiUnitDegree;
}

//==============================================================================

CellmlFileRuntime::CellmlFileRuntime() :
    mOdeCodeInformation(0),
    mDaeCodeInformation(0),
    mCompilerEngine(0),
    mVariableOfIntegration(0),
    mParameters(CellmlFileRuntimeParameters())
{
    // Reset (initialise, here) our properties

    reset(true, true);
}

//==============================================================================

CellmlFileRuntime::~CellmlFileRuntime()
{
    // Reset our properties

    reset(false, false);
}

//==============================================================================

QString CellmlFileRuntime::address() const
{
    // Return our address as a string

    QString res;

    res.sprintf("%p", this);

    return res;
}

//==============================================================================

bool CellmlFileRuntime::isValid() const
{
    // The runtime is valid if no issues were found

    return mIssues.isEmpty();
}

//==============================================================================

CellmlFileRuntime::ModelType CellmlFileRuntime::modelType() const
{
    // Return the type of model the runtime is for

    return mModelType;
}

//==============================================================================

bool CellmlFileRuntime::needOdeSolver() const
{
    // Return whether the model needs an ODE solver

    return mModelType == Ode;
}

//==============================================================================

bool CellmlFileRuntime::needDaeSolver() const
{
    // Return whether the model needs a DAE solver

    return mModelType == Dae;
}

//==============================================================================

bool CellmlFileRuntime::needNlaSolver() const
{
    // Return whether the model needs an NLA solver

    return mAtLeastOneNlaSystem;
}

//==============================================================================

int CellmlFileRuntime::constantsCount() const
{
    // Return the number of constants in the model

    if (mModelType == Ode)
        return mOdeCodeInformation?mOdeCodeInformation->constantIndexCount():0;
    else
        return mDaeCodeInformation?mDaeCodeInformation->constantIndexCount():0;
}

//==============================================================================

int CellmlFileRuntime::statesCount() const
{
    // Return the number of states in the model

    if (mModelType == Ode)
        return mOdeCodeInformation?mOdeCodeInformation->rateIndexCount():0;
    else
        return mDaeCodeInformation?mDaeCodeInformation->rateIndexCount():0;
}

//==============================================================================

int CellmlFileRuntime::ratesCount() const
{
    // Return the number of rates in the model
    // Note: it is obviously the same as the number of states, so this function
    //       is only for user convenience...

    return statesCount();
}

//==============================================================================

int CellmlFileRuntime::algebraicCount() const
{
    // Return the number of algebraic equations in the model

    if (mModelType == Ode)
        return mOdeCodeInformation?mOdeCodeInformation->algebraicIndexCount():0;
    else
        return mDaeCodeInformation?mDaeCodeInformation->algebraicIndexCount():0;
}

//==============================================================================

int CellmlFileRuntime::condVarCount() const
{
    // Return the number of conditional variables in the model

    if (mModelType == Ode)
        return 0;
    else
        return mDaeCodeInformation?mDaeCodeInformation->conditionVariableCount():0;
}

//==============================================================================

CellmlFileRuntime::InitializeConstantsFunction CellmlFileRuntime::initializeConstants() const
{
    // Return the initializeConstants function

    return mInitializeConstants;
}

//==============================================================================

CellmlFileRuntime::ComputeComputedConstantsFunction CellmlFileRuntime::computeComputedConstants() const
{
    // Return the computeComputedConstants function

    return mComputeComputedConstants;
}

//==============================================================================

CellmlFileRuntime::ComputeOdeRatesFunction CellmlFileRuntime::computeOdeRates() const
{
    // Return the computeOdeRates function

    return mComputeOdeRates;
}

//==============================================================================

CellmlFileRuntime::ComputeOdeVariablesFunction CellmlFileRuntime::computeOdeVariables() const
{
    // Return the computeOdeVariables function

    return mComputeOdeVariables;
}

//==============================================================================

CellmlFileRuntime::ComputeDaeEssentialVariablesFunction CellmlFileRuntime::computeDaeEssentialVariables() const
{
    // Return the computeDaeEssentialVariables function

    return mComputeDaeEssentialVariables;
}

//==============================================================================

CellmlFileRuntime::ComputeDaeResidualsFunction CellmlFileRuntime::computeDaeResiduals() const
{
    // Return the computeDaeResiduals function

    return mComputeDaeResiduals;
}

//==============================================================================

CellmlFileRuntime::ComputeDaeRootInformationFunction CellmlFileRuntime::computeDaeRootInformation() const
{
    // Return the computeDaeRootInformation function

    return mComputeDaeRootInformation;
}

//==============================================================================

CellmlFileRuntime::ComputeDaeStateInformationFunction CellmlFileRuntime::computeDaeStateInformation() const
{
    // Return the computeDaeStateInformation function

    return mComputeDaeStateInformation;
}

//==============================================================================

CellmlFileRuntime::ComputeDaeVariablesFunction CellmlFileRuntime::computeDaeVariables() const
{
    // Return the computeDaeVariables function

    return mComputeDaeVariables;
}

//==============================================================================

CellmlFileIssues CellmlFileRuntime::issues() const
{
    // Return the issue(s)

    return mIssues;
}

//==============================================================================

CellmlFileRuntimeParameters CellmlFileRuntime::parameters() const
{
    // Return the parameter(s)

    return mParameters;
}

//==============================================================================

void CellmlFileRuntime::resetOdeCodeInformation()
{
    // Reset the ODE code information
    // Note: setting it to zero will automatically delete the current instance,
    //       if any

    mOdeCodeInformation = 0;
}

//==============================================================================

void CellmlFileRuntime::resetDaeCodeInformation()
{
    // Reset the DAE code information
    // Note: setting it to zero will automatically delete the current instance,
    //       if any

    mDaeCodeInformation = 0;
}

//==============================================================================

void CellmlFileRuntime::resetFunctions()
{
    // Reset the functions

    mInitializeConstants = 0;

    mComputeComputedConstants = 0;

    mComputeOdeRates = 0;
    mComputeOdeVariables = 0;

    mComputeDaeEssentialVariables = 0;
    mComputeDaeResiduals = 0;
    mComputeDaeRootInformation = 0;
    mComputeDaeStateInformation = 0;
    mComputeDaeVariables = 0;
}

//==============================================================================

void CellmlFileRuntime::reset(const bool &pRecreateCompilerEngine,
                              const bool &pResetIssues)
{
    // Reset all of the runtime's properties

    mModelType = Undefined;
    mAtLeastOneNlaSystem = false;

    resetOdeCodeInformation();
    resetDaeCodeInformation();

    delete mCompilerEngine;

    if (pRecreateCompilerEngine)
        mCompilerEngine = new Compiler::CompilerEngine();
    else
        mCompilerEngine = 0;

    resetFunctions();

    if (pResetIssues)
        mIssues.clear();

    foreach (CellmlFileRuntimeParameter *parameter, mParameters)
        delete parameter;

    mVariableOfIntegration = 0;

    mParameters.clear();
}

//==============================================================================

void CellmlFileRuntime::couldNotGenerateModelCodeIssue()
{
    mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                               tr("the model code could not be generated"));
}

//==============================================================================

void CellmlFileRuntime::unexpectedProblemDuringModelCompilationIssue()
{
    mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                               tr("an unexpected problem occurred while trying to compile the model"));
}

//==============================================================================

void CellmlFileRuntime::checkCodeInformation(iface::cellml_services::CodeInformation *pCodeInformation)
{
    if (!pCodeInformation)
        // No code information was provided, so...

        return;

    // Retrieve the code information's latest error message

    QString codeGenerationErrorMessage = QString::fromStdWString(pCodeInformation->errorMessage());

    if (codeGenerationErrorMessage.isEmpty()) {
        // The code generation went fine, so check the model's constraint level

        iface::cellml_services::ModelConstraintLevel constraintLevel = pCodeInformation->constraintLevel();

        if (constraintLevel == iface::cellml_services::UNDERCONSTRAINED) {
            mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                                       tr("the model is underconstrained (i.e. some variables need to be initialised or computed)"));
        } else if (constraintLevel == iface::cellml_services::UNSUITABLY_CONSTRAINED) {
            mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                                       tr("the model is unsuitably constrained (i.e. some variables could not be found and/or some equations could not be used)"));
        } else if (constraintLevel == iface::cellml_services::OVERCONSTRAINED) {
            mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                                       tr("the model is overconstrained (i.e. some variables are either both initialised and computed or computed more than once)"));
        }
    } else {
        // The code generation didn't work, so...

        mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                                   tr("a problem occurred during the compilation of the model"));
    }
}

//==============================================================================

void CellmlFileRuntime::getOdeCodeInformation(iface::cellml_api::Model *pModel)
{
    // Get a code generator bootstrap and create an ODE code generator

    ObjRef<iface::cellml_services::CodeGeneratorBootstrap> codeGeneratorBootstrap = CreateCodeGeneratorBootstrap();
    ObjRef<iface::cellml_services::CodeGenerator> codeGenerator = codeGeneratorBootstrap->createCodeGenerator();

    // Generate some code for the model (i.e. 'compile' the model)

    try {
        mOdeCodeInformation = codeGenerator->generateCode(pModel);

        // Check that the code generation went fine

        checkCodeInformation(mOdeCodeInformation);
    } catch (iface::cellml_api::CellMLException &) {
        couldNotGenerateModelCodeIssue();
    } catch (...) {
        unexpectedProblemDuringModelCompilationIssue();
    }

    // Check the outcome of the ODE code generation

    if (mIssues.count())
        // Something went wrong at some point, so...

        resetOdeCodeInformation();
}

//==============================================================================

void CellmlFileRuntime::getDaeCodeInformation(iface::cellml_api::Model *pModel)
{
    // Get a code generator bootstrap and create a DAE code generator

    ObjRef<iface::cellml_services::CodeGeneratorBootstrap> codeGeneratorBootstrap = CreateCodeGeneratorBootstrap();
    ObjRef<iface::cellml_services::IDACodeGenerator> codeGenerator = codeGeneratorBootstrap->createIDACodeGenerator();

    // Generate some code for the model (i.e. 'compile' the model)

    try {
        mDaeCodeInformation = codeGenerator->generateIDACode(pModel);

        // Check that the code generation went fine

        checkCodeInformation(mDaeCodeInformation);
    } catch (iface::cellml_api::CellMLException &) {
        couldNotGenerateModelCodeIssue();
    } catch (...) {
        unexpectedProblemDuringModelCompilationIssue();
    }

    // Check the outcome of the DAE code generation

    if (mIssues.count())
        // Something went wrong at some point, so...

        resetDaeCodeInformation();
}

//==============================================================================

QString CellmlFileRuntime::functionCode(const QString &pFunctionSignature,
                                        const QString &pFunctionBody,
                                        const bool &pHasDefines)
{
    QString res = pFunctionSignature+"\n"
                  "{\n";

    if (pFunctionBody.isEmpty()) {
        res += "    return 0;\n";
    } else {
        res += "    int ret = 0;\n"
                     "    int *pret = &ret;\n"
                     "\n";

        if (pHasDefines)
            res += "#define VOI 0.0\n"
                   "#define ALGEBRAIC 0\n"
                   "\n";

        res += pFunctionBody;

        if (!pFunctionBody.endsWith("\n"))
            res += "\n";

        if (pHasDefines)
            res += "\n"
                   "#undef ALGEBRAIC\n"
                   "#undef VOI\n";

        res += "\n"
               "    return ret;\n";
    }

    res += "}\n";

    return res;
}

//==============================================================================

bool sortParameters(CellmlFileRuntimeParameter *pParameter1,
                    CellmlFileRuntimeParameter *pParameter2)
{
    // Determine which of the two parameters should be first
    // Note: the two comparisons which result we return are case insensitive,
    //       so that it's easier for people to search a parameter...

    if (!pParameter1->component().compare(pParameter2->component())) {
        // The parameters are in the same component, so check their name

        if (!pParameter1->name().compare(pParameter2->name()))
            // The parameters have the same name, so check their degree

            return pParameter1->degree() < pParameter2->degree();
        else
            // The parameters have different names, so...

            return pParameter1->name().compare(pParameter2->name(), Qt::CaseInsensitive) < 0;
    } else {
        // The parameters are in different components, so...

        return pParameter1->component().compare(pParameter2->component(), Qt::CaseInsensitive) < 0;
    }
}

//==============================================================================

CellmlFileRuntime * CellmlFileRuntime::update(CellmlFile *pCellmlFile)
{
    // Reset the runtime's properties

    reset(true, true);

    // Check that the model is either a 'simple' ODE model or a DAE model
    // Note #1: we don't check whether a model is valid, since all we want is to
    //          update its runtime (which has nothing to do with editing or even
    //          validating a model), so if it can be done then great otherwise
    //          tough luck (so to speak)...
    // Note #2: in order to do so, we need to get a 'normal' code generator (as
    //          opposed to an IDA, i.e. DAE, code generator) since if the model
    //          is correctly constrained, then we can check whether some of its
    //          equations were flagged as needing a Newton-Raphson evaluation,
    //          in which case we would be dealing with a DAE model...
    // Note #3: ideally, there would be a more convenient way to determine the
    //          type of a model, but well... there isn't, so...

    ObjRef<iface::cellml_api::Model> model = pCellmlFile->model();

    if (!model)
        // No model was provided, so...

        return this;

    // Retrieve the model's type
    // Note: this can be done by checking whether some equations were flagged
    //       as needing a Newton-Raphson evaluation...

    getOdeCodeInformation(model);

    if (!mOdeCodeInformation)
        return this;

    // An ODE code information could be retrieved, so we can determine the
    // model's type

    mModelType = mOdeCodeInformation->flaggedEquations()->length()?Dae:Ode;

    // If the model is of DAE type, then we don't want the ODE-specific code
    // information, but the DAE-specific code information

    ObjRef<iface::cellml_services::CodeInformation> genericCodeInformation;

    if (mModelType == Ode) {
        genericCodeInformation = mOdeCodeInformation;
    } else {
        getDaeCodeInformation(model);

        genericCodeInformation = mDaeCodeInformation;
    }

    // Retrieve all the parameters and sort them by component/variable name

    ObjRef<iface::cellml_services::ComputationTargetIterator> computationTargetIterator = genericCodeInformation->iterateTargets();

    for (ObjRef<iface::cellml_services::ComputationTarget> computationTarget = computationTargetIterator->nextComputationTarget();
         computationTarget; computationTarget = computationTargetIterator->nextComputationTarget()) {
        // Determine the type of the parameter

        CellmlFileRuntimeParameter::ParameterType parameterType;

        switch (computationTarget->type()) {
        case iface::cellml_services::VARIABLE_OF_INTEGRATION:
            parameterType = CellmlFileRuntimeParameter::Voi;

            break;
        case iface::cellml_services::CONSTANT: {
            // We are dealing with a constant, but the question is whether that
            // constant is a 'proper' constant or a 'computed' constant and this
            // can be determined by checking whether the computed target has an
            // initial value

            ObjRef<iface::cellml_api::CellMLVariable> variable = computationTarget->variable();

            if (QString::fromStdWString(variable->initialValue()).isEmpty())
                // The computed target doesn't have an initial value, so it must
                // be a 'computed' constant

                parameterType = CellmlFileRuntimeParameter::ComputedConstant;
            else
                // The computed target has an initial value, so it must be a
                // 'proper' constant

                parameterType = CellmlFileRuntimeParameter::Constant;

            break;
        }
        case iface::cellml_services::STATE_VARIABLE:
        case iface::cellml_services::PSEUDOSTATE_VARIABLE:
            parameterType = CellmlFileRuntimeParameter::State;

            break;
        case iface::cellml_services::ALGEBRAIC:
            // We are dealing with either a 'proper' algebraic variable or a
            // rate variable
            // Note: if the variable's degree is equal to zero, then we are
            //       dealing with a 'proper' algebraic variable otherwise we
            //       are dealing with a rate variable...

            if (computationTarget->degree())
                parameterType = CellmlFileRuntimeParameter::Rate;
            else
                parameterType = CellmlFileRuntimeParameter::Algebraic;

            break;
        default:
            // We are dealing with a type of computed target which is of no
            // interest to us, so...

            parameterType = CellmlFileRuntimeParameter::Undefined;
        }

        // Keep track of the parameter, should its type be known

        if (parameterType != CellmlFileRuntimeParameter::Undefined) {
            // Note: we cannot keep track of the parameter using a pointer to a
            //       CellmlFileVariable object since our CellmlFileVariable
            //       objects are for the current CellML file only. In other
            //       words, it would only work for models that don't have
            //       imports while we need a solution that works for any model,
            //       hence we we use CellmlFileRuntimeParameter instead...

            // Retrieve the variable associated with the computation target

            ObjRef<iface::cellml_api::CellMLVariable> variable = computationTarget->variable();

            // Retrieve the component in which the variable was declared
            // Note: for this, we need to check whether the component was
            //       imported which means retrieving the parent element of the
            //       variable (i.e. a component), then the parent of its parent
            //       (i.e. a model) and, finally, the parent of its parent's
            //       parent (i.e. an import, should the component have been
            //       imported)

            QString componentName = QString::fromStdWString(variable->componentName());

            ObjRef<iface::cellml_api::CellMLElement> variableComponent = variable->parentElement();
            ObjRef<iface::cellml_api::CellMLElement> variableComponentModel = variableComponent->parentElement();
            ObjRef<iface::cellml_api::CellMLElement> variableComponentModelImport = variableComponentModel->parentElement();

            if (variableComponentModelImport) {
                // The component has been imported, so retrieve the import's
                // components, and check which one has the name of variable's
                // component

                ObjRef<iface::cellml_api::CellMLImport> import = QueryInterface(variableComponentModelImport);
                ObjRef<iface::cellml_api::ImportComponentSet> importComponents = import->components();
                ObjRef<iface::cellml_api::ImportComponentIterator> importComponentsIterator = importComponents->iterateImportComponents();

                for (ObjRef<iface::cellml_api::ImportComponent> importComponent = importComponentsIterator->nextImportComponent();
                     importComponent; importComponent = importComponentsIterator->nextImportComponent())
                    if (!componentName.compare(QString::fromStdWString(importComponent->componentRef()))) {
                        // This is the imported component we are after, so we
                        // can get the correct name of the variable's component

                        componentName = QString::fromStdWString(importComponent->name());

                        break;
                    }
            }

            // Keep track of the parameter

            CellmlFileRuntimeParameter *parameter = new CellmlFileRuntimeParameter(QString::fromStdWString(variable->name()),
                                                                                   computationTarget->degree(),
                                                                                   QString::fromStdWString(variable->unitsName()),
                                                                                   componentName,
                                                                                   parameterType,
                                                                                   computationTarget->assignedIndex());

            if (parameterType == CellmlFileRuntimeParameter::Voi)
                mVariableOfIntegration = parameter;

            mParameters.append(parameter);
        }
    }

    qSort(mParameters.begin(), mParameters.end(), sortParameters);

    // Generate the model code, after having prepended to it all the external
    // functions which may, or not, be needed
    // Note: indeed, we cannot include header files since we don't (and don't
    //       want in order to avoid complications) deploy them with OpenCOR. So,
    //       instead, we must declare as external functions all the functions
    //       which we would normally use through header files...

    QString modelCode = "extern double fabs(double);\n"
                        "\n"
                        "extern double exp(double);\n"
                        "extern double log(double);\n"
                        "\n"
                        "extern double ceil(double);\n"
                        "extern double floor(double);\n"
                        "\n"
                        "extern double factorial(double);\n"
                        "\n"
                        "extern double sin(double);\n"
                        "extern double cos(double);\n"
                        "extern double tan(double);\n"
                        "extern double sinh(double);\n"
                        "extern double cosh(double);\n"
                        "extern double tanh(double);\n"
                        "extern double asin(double);\n"
                        "extern double acos(double);\n"
                        "extern double atan(double);\n"
                        "extern double asinh(double);\n"
                        "extern double acosh(double);\n"
                        "extern double atanh(double);\n"
                        "\n"
                        "extern double arbitrary_log(double, double);\n"
                        "\n"
                        "extern double pow(double, double);\n"
                        "\n"
                        "extern double gcd_multi(int, ...);\n"
                        "extern double lcm_multi(int, ...);\n"
                        "extern double multi_max(int, ...);\n"
                        "extern double multi_min(int, ...);\n"
                        "\n";

    QString functionsString = QString::fromStdWString(genericCodeInformation->functionsString());

    if (!functionsString.isEmpty()) {
        // We will need to solve at least one NLA system, so...

        mAtLeastOneNlaSystem = true;

        modelCode += "struct rootfind_info\n"
                     "{\n"
                     "    double aVOI;\n"
                     "\n"
                     "    double *aCONSTANTS;\n"
                     "    double *aRATES;\n"
                     "    double *aSTATES;\n"
                     "    double *aALGEBRAIC;\n"
                     "\n"
                     "    int *aPRET;\n"
                     "};\n"
                     "\n"
                     "extern void doNonLinearSolve(char *, void (*)(double *, double *, void*), double *, int *, int, void *);\n";
        modelCode += "\n";
        modelCode += functionsString.replace("do_nonlinearsolve(", QString("doNonLinearSolve(\"%1\", ").arg(address()));
        modelCode += "\n";

        // Note: we rename do_nonlinearsolve() to doNonLinearSolve() because
        //       CellML's CIS service already defines do_nonlinearsolve(), yet
        //       we want to use our own non-linear solve routine defined in our
        //       Compiler plugin. Also, we add a new parameter to all our calls
        //       to doNonLinearSolve() so that doNonLinearSolve() can retrieve
        //       the correct instance of our NLA solver...
    }

    // Retrieve the body of the function that initialises constants and extract
    // the statements that are related to computed variables (since we want to
    // be able to recompute those whenever the user modifies a parameter)
    // Note: ideally, we wouldn't have to do that, but the fact is that the
    //       CellML API doesn't distinguish between 'proper' and 'computed'
    //       constants, so...
    //       (See https://tracker.physiomeproject.org/show_bug.cgi?id=3499)

    QStringList initConstsList = QString::fromStdWString(genericCodeInformation->initConstsString()).split("\r\n");
    QString initConsts = QString();
    QString compCompConsts = QString();

    foreach (const QString &initConst, initConstsList)
        // Add the statement either to our list of 'proper' constants or
        // 'computed' constants

        if (QRegularExpression("^(CONSTANTS|RATES|STATES)\\[[0-9]*\\] = [+-]?[0-9]*\\.?[0-9]+([eE][+-]?[0-9]+)?;$").match(initConst).hasMatch()) {
            // We are dealing with a 'proper' constant (or a rate or a state)

            if (!initConsts.isEmpty())
                initConsts += "\n";

            initConsts += initConst;
        } else {
            // We are dealing with a 'computed' constant

            if (!compCompConsts.isEmpty())
                compCompConsts += "\n";

            compCompConsts += initConst;
        }

    modelCode += functionCode("int initializeConstants(double *CONSTANTS, double *RATES, double *STATES)",
                              initConsts, true);
    modelCode += "\n";
    modelCode += functionCode("int computeComputedConstants(double *CONSTANTS, double *RATES, double *STATES)",
                              compCompConsts, true);
    modelCode += "\n";

    // Retrieve the body of the remaining functions

    if (mModelType == Ode) {
        modelCode += functionCode("int computeOdeRates(double VOI, double *CONSTANTS, double *RATES, double *STATES, double *ALGEBRAIC)",
                                  QString::fromStdWString(mOdeCodeInformation->ratesString()));
        modelCode += "\n";
        modelCode += functionCode("int computeOdeVariables(double VOI, double *CONSTANTS, double *RATES, double *STATES, double *ALGEBRAIC)",
                                  QString::fromStdWString(genericCodeInformation->variablesString()));
    } else {
        modelCode += functionCode("int computeDaeEssentialVariables(double VOI, double *CONSTANTS, double *RATES, double *OLDRATES, double *STATES, double *OLDSTATES, double *ALGEBRAIC, double *CONDVAR)",
                                  QString::fromStdWString(mDaeCodeInformation->essentialVariablesString()));
        modelCode += "\n";
        modelCode += functionCode("int computeDaeResiduals(double VOI, double *CONSTANTS, double *RATES, double *OLDRATES, double *STATES, double *OLDSTATES, double *ALGEBRAIC, double *CONDVAR, double *resid)",
                                  QString::fromStdWString(mDaeCodeInformation->ratesString()));
        modelCode += "\n";
        modelCode += functionCode("int computeDaeRootInformation(double VOI, double *CONSTANTS, double *RATES, double *OLDRATES, double *STATES, double *OLDSTATES, double *ALGEBRAIC, double *CONDVAR)",
                                  QString::fromStdWString(mDaeCodeInformation->rootInformationString()));
        modelCode += functionCode("int computeDaeStateInformation(double *SI)",
                                  QString::fromStdWString(mDaeCodeInformation->stateInformationString()));
        modelCode += "\n";
        modelCode += functionCode("int computeDaeVariables(double VOI, double *CONSTANTS, double *RATES, double *STATES, double *ALGEBRAIC, double *CONDVAR)",
                                  QString::fromStdWString(genericCodeInformation->variablesString()));
    }

    // In the case of Windows, remove all '\r' characters from our model code
    // and then, for all platforms, remove the last '\n'
    // Note: these are only so that it looks better on Windows when we need to
    //       debug things...

#if defined(QT_DEBUG)
    #if defined(Q_OS_WIN)
    modelCode.remove('\r');
    #endif

    modelCode.chop(1);
#endif

    // Compile the model code and check that everything went fine

    if (!mCompilerEngine->compileCode(modelCode))
        // Something went wrong, so output the error that was found

        mIssues << CellmlFileIssue(CellmlFileIssue::Error,
                                   QString("%1").arg(mCompilerEngine->error()));

    // Keep track of the ODE/DAE functions, but only if no issues were reported

    if (mIssues.count()) {
        // Some issues were reported, so...

        reset(true, false);
    } else {
        // Add the symbol of any required external function, if any

        if (mAtLeastOneNlaSystem)
            llvm::sys::DynamicLibrary::AddSymbol("doNonLinearSolve",
                                                 (void *) (intptr_t) doNonLinearSolve);

        // Retrieve the ODE/DAE functions

        mInitializeConstants = (InitializeConstantsFunction) (intptr_t) mCompilerEngine->getFunction("initializeConstants");

        mComputeComputedConstants = (ComputeComputedConstantsFunction) (intptr_t) mCompilerEngine->getFunction("computeComputedConstants");

        if (mModelType == Ode) {
            mComputeOdeRates     = (ComputeOdeRatesFunction) (intptr_t) mCompilerEngine->getFunction("computeOdeRates");
            mComputeOdeVariables = (ComputeOdeVariablesFunction) (intptr_t) mCompilerEngine->getFunction("computeOdeVariables");
        } else {
            mComputeDaeEssentialVariables = (ComputeDaeEssentialVariablesFunction) (intptr_t) mCompilerEngine->getFunction("computeDaeEssentialVariables");
            mComputeDaeResiduals          = (ComputeDaeResidualsFunction) (intptr_t) mCompilerEngine->getFunction("computeDaeResiduals");
            mComputeDaeRootInformation    = (ComputeDaeRootInformationFunction) (intptr_t) mCompilerEngine->getFunction("computeDaeRootInformation");
            mComputeDaeStateInformation   = (ComputeDaeStateInformationFunction) (intptr_t) mCompilerEngine->getFunction("computeDaeStateInformation");
            mComputeDaeVariables          = (ComputeDaeVariablesFunction) (intptr_t) mCompilerEngine->getFunction("computeDaeVariables");
        }
    }

    // We are done, so return ourselves

    return this;
}

//==============================================================================

CellmlFileRuntimeParameter *CellmlFileRuntime::variableOfIntegration() const
{
    // Return our variable of integration, if any

    return mVariableOfIntegration;
}

//==============================================================================

}   // namespace CellMLSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
