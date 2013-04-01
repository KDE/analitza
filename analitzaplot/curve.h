/*************************************************************************************
 *  Copyright (C) 2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef ANALITZAPLOT_CURVE_H
#define ANALITZAPLOT_CURVE_H

#include "shapeinterface.h"
#include <QVector>
#include <QPointF>


#include <QSharedDataPointer>

namespace Analitza {

namespace MathUtils {
class QuadTree; 
}
    
/**
 * \class Curve
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Represents a curve in 2D or 3D space.
 *
 * This class contains all necessary data that Plotter will use to draw the curve.
 */

/*
NOTE
For implicit curves
1 (fast) space partition
2 detect multiple components: query the tree structure
3 use PC continuation over each component
4 (opt) try to parametrized each component (for analysis)

For curves of real valued functions
1 use the parametric form method

For curves in parametric form 
1 draw (simple)
2 do the all geometric analysis here (normal, curvature, tangent, etc)
*/

class ANALITZAPLOT_EXPORT Curve : public ShapeInterface<Curve>
{
public:
    Curve();
    Curve(const Curve &other);
    Curve(const Analitza::Expression &expresssion, Variables* vars = 0);
    ~Curve();
    
    QColor color() const;
    CoordinateSystem coordinateSystem() const;
    void createGeometry();
    Dimension dimension() const;
    QStringList errors() const;
    Expression expression(bool symbolic = false) const;
    QString iconName() const;
    bool isValid() const;
    bool isVisible() const;
    QString name() const;
    void plot();
    void setColor(const QColor &color);
    void setName(const QString &name);
    void setVisible(bool visible);
    Variables *variables() const;
    
    bool operator==(const Curve &other) const;
    bool operator!=(const Curve &other) const;
    Curve & operator=(const Curve &other);
    
    //TODO
//     qstring type(clasification style/type enum)
//     qlist point singular points
//     QStringList arguments() const;// X->F(X) : Kn->Km, K: real or complex so args := X in X->F(X)
//     QPair<double, double> argumentLimits(const QString &arg) const;
// QPair<Expression, Expression> Curve::limits(const QString &arg) const again??? 
//     void setArgumentLimits(const QString &arg, double min, double max);//x,y,z also bvars like theta and vars in Variables ...update geometry
//     QStringList parameters() const;// a,b,c... in X->F(X,a,b,c,...)
//     QPair<QPointF, QString> image(const QPointF &mousepos); // calculate the image of the curve based on the mouse postion 
//     QPair<double, double> tangent(const QPointF &mousepos);// calculate the tangent to the curve based on the mouse
    
private:
    void adaptiveQuadTreeSubdivisionImplicitCurve();

    
private:
    class CurveData;
    QSharedDataPointer<CurveData> d;
};

}

#endif // ANALITZAPLOT_CURVE_H
