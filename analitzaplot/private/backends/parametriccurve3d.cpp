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

#include "private/abstractspacecurve.h"
#include "private/functiongraphfactory.h"

#include <analitza/value.h>
#include <analitza/vector.h>

using namespace Analitza;

class ParametricCurve3D : public AbstractSpaceCurve
{
public:
    CONSTRUCTORS(ParametricCurve3D)
    TYPE_NAME(QT_TRANSLATE_NOOP("Function type", "Parametric Curve 3D"))
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Vector,
                                            Analitza::ExpressionType(Analitza::ExpressionType::Value), 3)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS(QStringList(QStringLiteral("t")))
    ICON_NAME(QStringLiteral("newparametric3d"))
    EXAMPLES(QStringList(QStringLiteral("t->vector {t,t**2,t}")))
    
    void update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2) override;

private:
    int resolution() { return 5000; }
    Cn findTValueForPoint(const QPointF& p);
};

void ParametricCurve3D::update(const QVector3D & /*oppositecorner1*/, const QVector3D & /*oppositecorner2*/)
{
    QPair< double, double > theInterval;
    if(hasIntervals())
         theInterval = interval(QStringLiteral("t"));
    else
        theInterval = qMakePair(-3.1415*5, 3.1415*5);
    double dlimit=theInterval.first;
    double ulimit=theInterval.second;
    
    points.clear();
    jumps.clear();
    points.reserve(resolution());

    double inv_res = double((ulimit-dlimit)/resolution());
    
    QVector3D curp;
    
    arg(QStringLiteral("t"))->setValue(dlimit);
    Expression res;
    
    for(double t=dlimit; t<ulimit; t+=inv_res) {
        arg(QStringLiteral("t"))->setValue(t);
        res=analyzer->calculateLambda();
        
        Cn x=res.elementAt(0).toReal();
        Cn y=res.elementAt(1).toReal();
        Cn z=res.elementAt(2).toReal();
        
        curp = QVector3D(x.value(), y.value(), z.value());
        
        points.append(curp);
    }
}

REGISTER_SPACECURVE(ParametricCurve3D)
