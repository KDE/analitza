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

#ifndef ANALITZAPLOT_FUNCTION_H
#define ANALITZAPLOT_FUNCTION_H

#include "shapeinterface.h"

#include <QSharedDataPointer>
#include <QVector>


#define GL_GLEXT_PROTOTYPES
#include <GL/glu.h>

class QPointF;
class QLineF;
class QVector3D;

namespace Analitza
{

/**
 * \class Curve
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Represents a curve in 2D or 3D space.
 *
 * This class contains all necessary data that Plotter2D will use to draw the curve.
 */

class ANALITZAPLOT_EXPORT Curve : public ShapeInterface<Curve>
{
public:
    Curve();
    Curve(const Curve &other);
    Curve(const Analitza::Expression &expresssion, Variables* vars = 0);// { if is builtin else is eq, expr, lambda,etc }
    ~Curve();
    
    //virtual void animate  (       )    virtualslot
    
    QColor color() const;
    CoordinateSystem coordinateSystem() const;
    void createGeometry();
    Dimension dimension() const;
    QStringList errors() const;
    Expression expression() const;
    QString iconName() const;
    bool isValid() const;
    bool isVisible() const;
    QString name() const;
    void plot(/*const QGLContext * context = 0*/);
    void setColor(const QColor &color);
    void setName(const QString &name);
    void setVisible(bool visible);
    QString typeName() const;
    Variables *variables() const;
    
    bool operator==(const Curve &other) const;
    bool operator!=(const Curve &other) const;
    Curve & operator=(const Curve &other);
    
//     QStringList arguments() const;// X->F(X) : Kn->Km, K: real or complex so args := X in X->F(X)
//     QPair<double, double> argumentLimits(const QString &arg) const;
//     void setArgumentLimits(const QString &arg, double min, double max);//x,y,z also bvars like theta and vars in Variables ...update geometry
//     QStringList parameters() const;// a,b,c... in X->F(X,a,b,c,...)
//     QPair<QPointF, QString> image(const QPointF &mousepos); // calculate the image of the curve based on the mouse postion 
//     QPair<double, double> tangent(const QPointF &mousepos);// calculate the tangent to the curve based on the mouse
    
private:
    class CurveData;
    QSharedDataPointer<CurveData> d;
        GLhandleARB shader_programme;
        GLuint vbo;
    QVector<double> points;
    bool cancelnow;
    int *m_cancont;

};

}

#endif // ANALITZAPLOT_FUNCTION_H
