#ifndef SURFACEFACTORY_H
#define SURFACEFACTORY_H

#include "abstractfunctiongraphfactory.h"

#define REGISTER_SURFACE(name) \
        static AbstractSurface * create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=SurfaceFactory::self()->registerSurface(create##name, \
        name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Parameters, \
        name ::IconName, name ::Examples); }

class AbstractSurface;

class ANALITZAPLOT_EXPORT SurfaceFactory : public AbstractFunctionGraphFactory
{
public:
    typedef AbstractSurface* (*BuilderFunction)(const Analitza::Expression&, Analitza::Variables* );

    static SurfaceFactory* self();

    bool registerSurface(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
                         ExpressionTypeFunction expressionTypeFunction, 
                         CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                         IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
    QString id(const QStringList& args, CoordinateSystem coordsys) const;
    bool contains(const QString &id) const;
    AbstractSurface * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;

private:
    static SurfaceFactory* m_self;
    SurfaceFactory() {
        Q_ASSERT(!m_self);
        m_self = this;
    }

    QMap<QString, BuilderFunction> builderFunctions;
};

#endif // SURFACEFACTORY_H
