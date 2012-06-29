#include "functiongraphfactory.h"

AbstractFunctionGraphFactory* AbstractFunctionGraphFactory::m_self=0;

QString AbstractFunctionGraphFactory::typeName(const QString& id) const
{
    return typeNameFunctions[id]();
}

Analitza::ExpressionType AbstractFunctionGraphFactory::expressionType(const QString& id) const
{
    return expressionTypeFunctions[id]();
}

int AbstractFunctionGraphFactory::spaceDimension(const QString& id) const
{
    return spaceDimensionFunctions[id]();
}

CoordinateSystem AbstractFunctionGraphFactory::coordinateSystem(const QString& id) const
{
    return coordinateSystemFunctions[id]();
}

QStringList AbstractFunctionGraphFactory::arguments(const QString& id)
{
    return argumentsFunctions[id]();
}

QString AbstractFunctionGraphFactory::iconName(const QString& id) const
{
    return iconNameFunctions[id]();
}

QStringList AbstractFunctionGraphFactory::examples(const QString& id) const
{
    return examplesFunctions[id]();
}

AbstractFunctionGraphFactory* AbstractFunctionGraphFactory::self()
{
    if(!m_self)
        m_self=new AbstractFunctionGraphFactory;
    return m_self;
}

bool AbstractFunctionGraphFactory::registerFunctionGraph(BuilderFunctionWithVars builderFunctionWithVars, BuilderFunctionWithoutVars builderFunctionWithoutVars, TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, SpaceDimensionFunction spaceDimensionFunction,
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//     Q_ASSERT(!contains(argumentsFunction()));
    
    Q_ASSERT(expressionTypeFunction().type() == Analitza::ExpressionType::Lambda);
    
    QString id = QString::number(spaceDimensionFunction())+"|"+
                 QString::number((int)coordinateSystemFunction())+"|"+
                 argumentsFunction().join(",");

    typeNameFunctions[id] = typeNameFunction;
    expressionTypeFunctions[id] = expressionTypeFunction;
    spaceDimensionFunctions[id] = spaceDimensionFunction;
    coordinateSystemFunctions[id] = coordinateSystemFunction;
    argumentsFunctions[id] = argumentsFunction;
    iconNameFunctions[id] = iconNameFunction;
    examplesFunctions[id] = examplesFunction;

    builderFunctionsWithVars[id] = builderFunctionWithVars;
    builderFunctionsWithoutVars[id] = builderFunctionWithoutVars;

    return true;
}

QString AbstractFunctionGraphFactory::id(const QStringList& args) const
{
    QString key;
    
    bool found = false;
    
    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
        if (argumentsFunctions.values()[i]() == args)
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);
            found = true;
            break;
        }

        
    if (found)
        return QString("2|")+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");

    return QString();    
}

bool AbstractFunctionGraphFactory::contains(const QString& id) const
{
    return builderFunctionsWithVars.contains(id);
}

AbstractFunctionGraph* AbstractFunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const
{
    return builderFunctionsWithVars[id](exp, v);
}

AbstractFunctionGraph* AbstractFunctionGraphFactory::build(const QString& id, const Analitza::Expression& exp) const
{
    return builderFunctionsWithoutVars[id](exp);
}
