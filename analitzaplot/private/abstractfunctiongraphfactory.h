#ifndef ABSTRACTFUNCTIONGRAPHFACTORY_H
#define ABSTRACTFUNCTIONGRAPHFACTORY_H

#include "analitzaplotexport.h"
#include "mathutils.h"
#include "analitza/expressiontype.h"

#define REGISTER_FUNCTIONGRAPH(name) \
        static AbstractFunctionGraph * vcreate##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        static AbstractFunctionGraph * create##name(const Analitza::Expression &exp) { return new name (exp); } \
        namespace { bool _##name=AbstractFunctionGraphFactory::self()->registerFunctionGraph(vcreate##name, create##name, \
        name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Parameters, \
        name ::IconName, name ::Examples); }

        
#define REGISTER_PLANECURVE REGISTER_FUNCTIONGRAPH
#define REGISTER_SURFACE REGISTER_FUNCTIONGRAPH

        
class AbstractFunctionGraph;

class ANALITZAPLOT_EXPORT AbstractFunctionGraphFactory
{
public:
    typedef AbstractFunctionGraph* (*BuilderFunctionWithVars)(const Analitza::Expression&, Analitza::Variables* );
    typedef AbstractFunctionGraph* (*BuilderFunctionWithoutVars)(const Analitza::Expression&);
    
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
    
    static AbstractFunctionGraphFactory* self();

    bool registerFunctionGraph(BuilderFunctionWithVars builderFunctionWithVars, BuilderFunctionWithoutVars builderFunctionWithoutVars, 
                  TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction,
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
    QString id(const QStringList& args) const;
    bool contains(const QString &id) const;
    
    AbstractFunctionGraph * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;
    AbstractFunctionGraph * build(const QString& id, const Analitza::Expression& exp) const;
    
private:
    QMap<QString, TypeNameFunction> typeNameFunctions;
    QMap<QString, ExpressionTypeFunction> expressionTypeFunctions;
    QMap<QString, int> spaceDimensions;
    QMap<QString, CoordinateSystemFunction> coordinateSystemFunctions;
    QMap<QString, ArgumentsFunction> argumentsFunctions;
    QMap<QString, IconNameFunction> iconNameFunctions;
    QMap<QString, ExamplesFunction> examplesFunctions;
    
    static AbstractFunctionGraphFactory* m_self;
    AbstractFunctionGraphFactory() {
        Q_ASSERT(!m_self);
        m_self = this;
    }

    QMap<QString, BuilderFunctionWithVars> builderFunctionsWithVars;
    QMap<QString, BuilderFunctionWithoutVars> builderFunctionsWithoutVars;
};


#endif // ABSTRACTFUNCTIONGRAPHFACTORY_H
