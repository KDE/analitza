#include "functiongraph.h"

#include <QUuid>

#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"
#include "analitza/variable.h"
#include <analitza/variables.h>

#include "private/abstractplanecurve.h"
#include "private/abstractfunctiongraphfactory.h"



///

///




FunctionGraph::FunctionGraph(const Analitza::Expression &functionExpression, const QString &name, const QColor &col)
    : MappingGraph(name, col), m_varsModule(0), m_functionGraph(0)
{
    reset(functionExpression);
}

FunctionGraph::FunctionGraph(const Analitza::Expression &functionExpression, Analitza::Variables *v, const QString &name, const QColor &col)
    : MappingGraph(name, col), m_varsModule(v), m_functionGraph(0)
{
    reset(functionExpression);
}

FunctionGraph::~FunctionGraph()
{
    delete m_functionGraph;
}

bool FunctionGraph::canDraw(const Analitza::Expression &functionExpression)
{
    QStringList errors;
    
    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
        errors << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a;
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
    {
        errors << i18n("Function type not recognized");
    }
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_functionGraph;
//             m_functionGraph=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool FunctionGraph::canDraw(const Analitza::Expression &functionExpression, QStringList &errors)
{
    Q_ASSERT(errors.isEmpty()); // el usuario deberia ingresar un lista vacia 
    
    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() && !functionExpression.isLambda()) {
        errors << i18n("The expression is not correct");
        return false;
    }

    Analitza::Analyzer a;
    a.setExpression(functionExpression);
    a.setExpression(a.dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a.expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
        errors << i18n("Function type not recognized");
    else if(!a.isCorrect())
        errors << a.errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a.type();

        if(actual.canReduceTo(expected)) {
//             delete m_functionGraph;
//             m_functionGraph=AbstractFunctionGraphFactory::self()->build(bvars, a.expression(), m_varsModule);
        } else
            errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }

    return errors.empty();
}

bool FunctionGraph::reset(const Analitza::Expression& functionExpression)
{
    m_errors.clear();

    //NOTE GSOC see functionExpression.isLambda ask for
    if(!functionExpression.isCorrect() || !functionExpression.isLambda())
    {
        m_errors << i18n("The expression is not correct");
        return false;
    }
    
    Analitza::Analyzer *a;
    if (m_varsModule)
        a = new Analitza::Analyzer(m_varsModule);
    else
        a = new Analitza::Analyzer;

    a->setExpression(functionExpression);
    a->setExpression(a->dependenciesToLambda());

    QStringList bvars;

    foreach (Analitza::Ci *arg, a->expression().parameters())
        bvars.append(arg->name());

    //TODO: turn into assertion
    if(!AbstractFunctionGraphFactory::self()->contains(AbstractFunctionGraphFactory::self()->id(bvars)))
        m_errors << i18n("Function type not recognized");
    else if(!a->isCorrect())
        m_errors << a->errors();
    else {
        Analitza::ExpressionType expected=AbstractFunctionGraphFactory::self()->expressionType(AbstractFunctionGraphFactory::self()->id(bvars));
        Analitza::ExpressionType actual=a->type();

        if(actual.canReduceTo(expected)) {

            delete m_functionGraph;

            if (m_varsModule)
                m_functionGraph= static_cast<AbstractFunctionGraph*>(AbstractFunctionGraphFactory::self()->build(AbstractFunctionGraphFactory::self()->id(bvars), a->expression(), m_varsModule));
            else
                m_functionGraph=static_cast<AbstractFunctionGraph*>(AbstractFunctionGraphFactory::self()->build(AbstractFunctionGraphFactory::self()->id(bvars), a->expression()));
        } else
            m_errors << i18n("Function type not correct for functions depending on %1", bvars.join(i18n(", ")));
    }
    
    delete a;
    
    return m_errors.empty();
}

void FunctionGraph::setVariables(Analitza::Variables* variables)
{
    Q_ASSERT(variables);
    
    m_varsModule = variables;
    
    m_functionGraph->setVariables(variables);
}

const QString FunctionGraph::typeName() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->typeName();
}

const Analitza::Expression & FunctionGraph::expression() const
{
    Q_ASSERT(m_functionGraph);
    
//     return m_functionGraph->
    return m_functionGraph->expression();
}

QString FunctionGraph::iconName() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->iconName();
}

QStringList FunctionGraph::examples() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->examples();
}

int FunctionGraph::spaceDimension() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->spaceDimension();
}

CoordinateSystem FunctionGraph::coordinateSystem() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->coordinateSystem();
}

QStringList FunctionGraph::errors() const
{
    Q_ASSERT(m_functionGraph);
    
    QStringList err(m_errors);
    if(m_functionGraph) {
        err += m_functionGraph->errors();
    }
    return err;
}

bool FunctionGraph::isCorrect() const
{
    Q_ASSERT(m_functionGraph);

    return m_errors.isEmpty() && m_functionGraph->isCorrect();
}

QPair<Analitza::Expression, Analitza::Expression> FunctionGraph::interval(const QString &argname, bool evaluate) const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->interval(argname, evaluate);
}

bool FunctionGraph::setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->setInterval(argname, min, max);
}

QPair<double, double> FunctionGraph::interval(const QString &argname) const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->interval(argname);
}

bool FunctionGraph::setInterval(const QString &argname, double min, double max)
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->setInterval(argname, min, max);
}

QStringList FunctionGraph::parameters() const
{
    Q_ASSERT(m_functionGraph);
    
    return m_functionGraph->parameters();
}
