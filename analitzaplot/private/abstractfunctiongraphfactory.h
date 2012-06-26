#ifndef ABSTRACTFUNCTIONGRAPHFACTORY_H
#define ABSTRACTFUNCTIONGRAPHFACTORY_H

#include "analitzaplotexport.h"
#include "mathutils.h"
#include "analitza/expressiontype.h"

class ANALITZAPLOT_EXPORT AbstractFunctionGraphFactory
{
public:
    typedef QString (*TypeNameFunction)();
    typedef Analitza::ExpressionType (*ExpressionTypeFunction)();
    typedef CoordinateSystem (*CoordinateSystemFunction)();
    typedef QStringList (*ArgumentsFunction)();
    typedef QString (*IconNameFunction)();
    typedef QStringList (*ExamplesFunction)();

    QString typeName(const QString& id) const;
    Analitza::ExpressionType expressionType(const QString& id) const;
    int spaceDimension(const QString& id) const;
    CoordinateSystem coordinateSystem(const QString& id) const;
    QStringList arguments(const QString& id);
    QString iconName(const QString& id) const;
    QStringList examples(const QString& id) const;
    
    virtual bool contains(const QString &id) const = 0;

protected:
    QString registerFunctionGraphDefs(TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction, int spaceDimension,
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);    
    
    
    QMap<QString, TypeNameFunction> typeNameFunctions;
    QMap<QString, ExpressionTypeFunction> expressionTypeFunctions;
    QMap<QString, int> spaceDimensions;
    QMap<QString, CoordinateSystemFunction> coordinateSystemFunctions;
    QMap<QString, ArgumentsFunction> argumentsFunctions;
    QMap<QString, IconNameFunction> iconNameFunctions;
    QMap<QString, ExamplesFunction> examplesFunctions;
};


#endif // ABSTRACTFUNCTIONGRAPHFACTORY_H
