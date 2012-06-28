// #ifndef SURFACEFACTORY_H
// #define SURFACEFACTORY_H
// 
// #include "abstractfunctiongraphfactory.h"
// 
// #define (name) \
//         static  * create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
//         namespace { bool _##name=::self()->registerSurface(create##name, \
//         name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Parameters, \
//         name ::IconName, name ::Examples); }
// 
// #define REGISTER_SURFACE(name) \
//         static AbstractSurface * vcreate##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
//         static AbstractSurface * create##name(const Analitza::Expression &exp) { return new name (exp); } \
//         namespace { bool _##name=SurfaceFactory::self()->registerPlaneCurve(vcreate##name, create##name, \
//         name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Parameters, \
//         name ::IconName, name ::Examples); }
// 
//         
// class AbstractSurface;
// 
// class ANALITZAPLOT_EXPORT SurfaceFactory : public AbstractFunctionGraphFactory
// {
// public:
//     typedef AbstractSurface* (*BuilderFunctionWithVars)(const Analitza::Expression&, Analitza::Variables* );
//     typedef AbstractSurface* (*BuilderFunctionWithoutVars)(const Analitza::Expression&);
// 
//     static SurfaceFactory* self();
// 
//     bool registerSurface(BuilderFunctionWithVars builderFunctionWithVars, BuilderFunctionWithoutVars builderFunctionWithoutVars, TypeNameFunction typeNameFunction,
//                          ExpressionTypeFunction expressionTypeFunction, 
//                          CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
//                          IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
//     QString id(const QStringList& args) const;
//     bool contains(const QString &id) const;
//     AbstractSurface * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;
//     AbstractSurface * build(const QString& id, const Analitza::Expression& exp) const;
// 
// private:
//     static SurfaceFactory* m_self;
//     SurfaceFactory() {
//         Q_ASSERT(!m_self);
//         m_self = this;
//     }
// 
//     QMap<QString, BuilderFunctionWithVars> builderFunctionsWithVars;
//     QMap<QString, BuilderFunctionWithoutVars> builderFunctionsWithoutVars;};
// 
// #endif // SURFACEFACTORY_H
