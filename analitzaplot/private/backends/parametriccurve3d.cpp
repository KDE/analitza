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

#include "analitza/value.h"
#include <analitza/vector.h>


#include <QRectF>
#include "analitza/value.h"
#include "analitza/vector.h"


#include <QDebug>
#include <analitza/localize.h>

using Analitza::Expression;
using Analitza::ExpressionType;
using Analitza::Variables;
using Analitza::Object;
using Analitza::Cn;

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
static const double pi=M_PI;


namespace
{
    /// The @p p1 and @p p2 parameters are the last 2 values found
    /// @p next is the next value found
    /// @returns whether we've found the gap

    bool traverse(double p1, double p2, double next)
    {
        static const double delta=3;
        double diff=p2-p1, diff2=next-p2;
        bool ret=false;

        if(diff>0 && diff2<-delta)
            ret=true;
        else if(diff<0 && diff2>delta)
            ret=true;

        return ret;
    }
}



class ParametricCurve3D : public AbstractSpaceCurve
{
public:
    CONSTRUCTORS(ParametricCurve3D)
    TYPE_NAME("Parametric Curve 3D")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Vector,
                                            Analitza::ExpressionType(Analitza::ExpressionType::Value), 3)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("t")
    ICON_NAME("newparametric3d")
    EXAMPLES("t->vector {t,t**2,t}")    
    
    void update(const Box3D& viewport);
    
    
    //
    

private:
    Cn findTValueForPoint(const QPointF& p);

};

// 

void ParametricCurve3D::update(const Box3D& viewport)
{
  Q_UNUSED(viewport);
    Q_ASSERT(analyzer->expression().isCorrect());
    //if(int(resolution())==points.capacity())
    //  return;
    
    
            //TODO CACHE en intervalvalues!!!
    QPair<double, double> c_limits = interval("t");
    
//     if ()
//     
//     static QPair<double, double> o_limits = c_limits;
    
    
    double ulimit=c_limits.second;
    double dlimit=c_limits.first;
    
    
    
    points.clear();
    jumps.clear();
    //points.reserve(resolution());

    
//  double inv_res= double((ulimit-dlimit)/resolution());
    double inv_res= 0.01; 
//  double final=ulimit-inv_res;
    
        //by percy
    Box3D vp(viewport);
    
//     vp.setTop(viewport.top() - 2);
//     vp.setBottom(viewport.bottom() + 2);
//     vp.setLeft(viewport.left() + 2);
//     vp.setRight(viewport.right() - 2);
    
    QVector3D curp;
    
    arg("t")->setValue(dlimit);
    Expression res;
    
    int i = 0;
    bool jlock = false;
    
    for(double t=dlimit; t<ulimit; t+=inv_res, ++i) {
        arg("t")->setValue(t);
        res=analyzer->calculateLambda();
        
        Cn x=res.elementAt(0).toReal();
        Cn y=res.elementAt(1).toReal();
        Cn z=res.elementAt(2).toReal();
        
        curp = QVector3D(x.value(), y.value(), z.value());
        
        if (/*vp.contains(curp)*/1)
        {
//             addPoint(curp);
            jlock = false;
        }
        else if (!jlock)
        {
            jumps.append(i);
            jlock = true;
        }
        
        //      objectWalker(vo);
        Q_ASSERT(res.isVector());
    }
}


REGISTER_SPACECURVE(ParametricCurve3D)



