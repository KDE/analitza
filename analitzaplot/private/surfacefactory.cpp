#include "surfacefactory.h"


SurfaceFactory* SurfaceFactory::m_self=0;

SurfaceFactory* SurfaceFactory::self()
{
    if(!m_self)
        m_self=new SurfaceFactory;
    return m_self;
}

bool SurfaceFactory::registerSurface(BuilderFunction builderFunction, TypeNameFunction typeNameFunction,
                                     ExpressionTypeFunction expressionTypeFunction, 
                                     CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
                                     IconNameFunction iconNameFunction, ExamplesFunction examplesFunction)
{
//             Q_ASSERT(!contains(argumentsFunction()));

    QString id = registerFunctionGraphDefs(typeNameFunction,expressionTypeFunction, 3,
                                           coordinateSystemFunction, argumentsFunction, iconNameFunction, examplesFunction);

    builderFunctions[id] = builderFunction;

    return true;

}


QString SurfaceFactory::id(const QStringList& args, CoordinateSystem coordsys) const
{
    QString key;

    for (int i = 0; i < argumentsFunctions.values().size(); ++i)
        if ((argumentsFunctions.values()[i]() == args) &&
                (coordinateSystemFunctions.values()[i]() == coordsys))
        {
            key = argumentsFunctions.key(argumentsFunctions.values()[i]);

            break;
        }

    return QString("3|")+QString::number((int)coordinateSystemFunctions[key]())+"|"+argumentsFunctions[key]().join(",");
}

bool SurfaceFactory::contains(const QString& id) const
{
    return builderFunctions.contains(id);
}

AbstractSurface* SurfaceFactory::build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const
{
    return builderFunctions[id](exp,v);
}



