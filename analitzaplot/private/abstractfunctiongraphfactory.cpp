#include "abstractfunctiongraphfactory.h"


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
    return spaceDimensions[id];
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

QString AbstractFunctionGraphFactory::registerFunctionGraphDefs(TypeNameFunction typeNameFunction,
        ExpressionTypeFunction expressionTypeFunction, int spaceDimension,
        CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
        IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//     Q_ASSERT(!contains(argumentsFunction()));

    QString id = QString::number(spaceDimension)+"|"+
                 QString::number((int)coordinateSystemFunction())+"|"+
                 argumentsFunction().join(",");

    typeNameFunctions[id] = typeNameFunction;
    expressionTypeFunctions[id] = expressionTypeFunction;
    spaceDimensions[id] = spaceDimension;
    coordinateSystemFunctions[id] = coordinateSystemFunction;
    argumentsFunctions[id] = argumentsFunction;
    iconNameFunctions[id] = iconNameFunction;
    examplesFunctions[id] = examplesFunction;

    return id;
}
