/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "private/abstractsurface.h"
// #include "private/surfacefactory.h"
#include "private/functiongraphfactory.h"
#include <private/utils/mathutils.h>

#include <analitza/value.h>
#include <analitza/vector.h>

using namespace Analitza;

class SphericalSurface : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    explicit SphericalSurface(const Analitza::Expression& e);
    SphericalSurface(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v);
    
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Spherical Surface Radial=F(t:Azimuth, p: Polar)"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("t"))<< QStringLiteral("p")) // t azimuth p polar
    ICON_NAME(QStringLiteral("newspherical"))
    EXAMPLES(QStringList())

    //Own
    virtual bool setInterval(const QString& argname, const Analitza::Expression& min, const Analitza::Expression& max) override;
    virtual bool setInterval(const QString& argname, double min, double max) override;
    
    QVector3D fromParametricArgs(double u, double v) override;
    void update(const QVector3D& oppositecorner1, const QVector3D& oppositecorner2) override;
    
    
};

bool SphericalSurface::setInterval(const QString& argname, const Analitza::Expression& min, const Analitza::Expression& max)
{
    
    Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());

    QPair<Analitza::Expression, Analitza::Expression> ival = interval(argname, true);
    
    double min_val = ival.first.toReal().value();
    double max_val = ival.second.toReal().value();

    delete intervalsAnalizer;
    
    if (min_val<0 || max_val < 0) // el radio es distancia por tanto es positivo, el angulo va de 0 hasta +inf
        return false; 
    
    if (argname == QStringLiteral("t") && max_val >= 2*M_PI)
        return false;
    
    if (argname == QStringLiteral("p") && max_val > M_PI)
        return false;
    
    return AbstractFunctionGraph::setInterval(argname, min, max);
}

bool SphericalSurface::setInterval(const QString& argname, double min, double max)
{
    if (min<0 || max < 0) // el radio es distancia por tanto es positivo, el angulo va de 0 hasta +inf
        return false; 
    
    
    if (argname == QStringLiteral("t") && max >= 2*M_PI)
        return false;
    
    if (argname == QStringLiteral("p") && max > M_PI)
        return false;
    
    return AbstractFunctionGraph::setInterval(argname, min, max);
}

SphericalSurface::SphericalSurface(const Analitza::Expression& e): AbstractSurface(e)
{
    setInterval(QStringLiteral("t"), 0, M_PI);
    setInterval(QStringLiteral("p"), 0, M_PI);
}

SphericalSurface::SphericalSurface(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v)
    : AbstractSurface(e, v)
{}

QVector3D SphericalSurface::fromParametricArgs(double a, double p)
{
    arg(QStringLiteral("t"))->setValue(a);
    arg(QStringLiteral("p"))->setValue(p);    

    double r = analyzer->calculateLambda().toReal().value();

    return sphericalToCartesian(r,a,p);
}

void SphericalSurface::update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2)
{
    Q_UNUSED(oppositecorner1);
    Q_UNUSED(oppositecorner2);
    buildParametricSurface();
}


REGISTER_SURFACE(SphericalSurface)
