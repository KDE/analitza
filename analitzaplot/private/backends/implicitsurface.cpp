/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include <analitza/value.h>
#include <analitza/vector.h>

#include "private/utils/marchingcubes.h"

using namespace Analitza;

class ImplicitSurf : public AbstractSurface , public MarchingCubes/*, static class? better macros FooClass*/
{
public:
    ImplicitSurf(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v);

    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Implicit Surface"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda)
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value))
        .addParameter(Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("x")) << QStringLiteral("y") << QStringLiteral("z"))
    ICON_NAME(QStringLiteral("draw-square-inverted-corners"))
    EXAMPLES(QStringList())

    //Own
    virtual ~ImplicitSurf() {  }
    void update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2) override;
    
    double evalScalarField(double x, double y, double z) override;
};

double ImplicitSurf::evalScalarField(double x, double y, double z)
{
    arg(QStringLiteral("x"))->setValue(x);
    arg(QStringLiteral("y"))->setValue(y);
    arg(QStringLiteral("z"))->setValue(z);
    
    return analyzer->calculateLambda().toReal().value();
}


ImplicitSurf::ImplicitSurf(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v): AbstractSurface(e, v)
{

}

void ImplicitSurf::update(const QVector3D & /*oppositecorner1*/, const QVector3D & /*oppositecorner2*/)
{
    SpaceLimits spaceLimits;
    
    double tmpsize = 6.0;
    
    spaceLimits.minX = -tmpsize;
    spaceLimits.maxX = tmpsize;
    spaceLimits.minY = -tmpsize;
    spaceLimits.maxY = tmpsize;
    spaceLimits.minZ = -tmpsize;
    spaceLimits.maxZ = tmpsize;
    
    if (hasIntervals())
    {
        QPair<double, double> intervalx = interval(QStringLiteral("x"));
        QPair<double, double> intervaly = interval(QStringLiteral("y"));
        QPair<double, double> intervalz = interval(QStringLiteral("z"));

        spaceLimits.minX = intervalx.first;
        spaceLimits.maxX = intervalx.second;
        spaceLimits.minY = intervaly.first;
        spaceLimits.maxY = intervaly.second;
        spaceLimits.minZ = intervalz.first;
        spaceLimits.maxZ = intervalz.second;
    }
    
    
    setupSpace(spaceLimits);
    
    buildGeometry();
    
//     qDebug() << "A: " << ntrigs() << nverts();

    //TODO find a better way to avoi this loops
    for (int i = 0; i < nverts(); ++i)
    {
        auto v = vert(i);

        vertices.append(QVector3D(v->x, v->y, v->z));
        normals.append(QVector3D(v->nx, v->ny, v->nz));
    }

    for (int i = 0; i < ntrigs(); ++i)
    {
        auto t = trig(i);
        indexes << t->v1 << t->v2 << t->v3;
    }
}

REGISTER_SURFACE(ImplicitSurf)
