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


#ifndef KEOMATH_MATHHELPER_H
#define KEOMATH_MATHHELPER_H

#include <Eigen/Core>

#include <QPair>
#include <QVector>
#include <QLineF>

namespace Keomath
{

typedef Eigen::Matrix<double,1,1> Vector1d;
typedef Eigen::Vector2d Vector2d;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::VectorXd VectorXd;

typedef QPair<double, double> Interval;
typedef QVector<Interval> IntervalList;

enum FunctionType { RealValued = 1, VectorValued = 2};
enum CoordinateSystem { Cartesian = 1, Polar = 2, Cylindrical = 3, Spherical = 4 };

enum FunctionGraphDimension { Dimension1D = 1, Dimension2D = 2, Dimension3D = 3 };
enum FunctionGraphPrecision { VeryLow = 1, Low = 2, Average = 3, High = 4, VeryHigh = 5 };

enum FunctionGraphDataFlag { FunctionData = 0x0, GradientData = 0x1, FixedGradientsData = 0x2 };
Q_DECLARE_FLAGS(FunctionGraphDataFlags, FunctionGraphDataFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(FunctionGraphDataFlags)

//remove this see above FunctionType ... a better def
// enum FunctionImageType { InvalidValued = 0, RealValued = 1, VectorValued = 2, /*TODO ComplexValued= 3*/ };

struct FunctionGraphDescription
{
	FunctionGraphDataFlags dataFlags;
};

struct FunctionGraphData
{
	QVector<VectorXd> fixedGradients;
};

struct FunctionGraphData2D : public FunctionGraphData
{
	QVector<QPointF> points;
	QList<int> jumps;
};



}

#include <QtCore/QPointF>
#include <QtGui/QVector3D>


#include "analitzaplotexport.h"

#include <cmath>
#include <QLineF>


using std::sin;
using std::cos;
using std::atan;
using std::fabs;

namespace Analitza
{
class Expression;
}

const double PI = std::acos(-1.0);

static bool isSimilar(double a, double b, double diff = .0000001)
{
    return fabs(a-b) < diff;
}





static bool traverse(double p1, double p2, double next)
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

static QLineF slopeToLine(const double &der)
{
    double arcder = atan(der);
    const double len=6.*der;
    QPointF from, to;
    from.setX(len*cos(arcder));
    from.setY(len*sin(arcder));

    to.setX(-len*cos(arcder));
    to.setY(-len*sin(arcder));
    return QLineF(from, to);
}

static QLineF mirrorXY(const QLineF& l)
{
    return QLineF(l.y1(), l.x1(), l.y2(), l.x2());
}








class ANALITZAPLOT_EXPORT RealInterval
{
public:
    typedef QList<RealInterval> List;

    RealInterval();
    RealInterval(qreal lower, qreal upper);
    RealInterval(const RealInterval &realInterval);
    virtual ~RealInterval();

    qreal lower() const
    {
        return m_lower;
    }
    void setLower(qreal lower)
    {
        m_lower = lower;
        m_mid = (m_lower + m_upper)*0.5;
    }

    qreal upper() const
    {
        return m_upper;
    }
    void setUpper(qreal upper)
    {
        m_upper = upper;
        m_mid = (m_lower + m_upper)*0.5;
    }

    qreal mid() const
    {
        return m_mid;
    }

    bool contains(qreal val) const
    {
        if ((m_lower <= val) && (val <= m_upper))
            return true;

        return false;
    }


    bool operator == (const RealInterval &realInterval) const
    {
        return ((m_lower == realInterval.lower()) && (m_upper == realInterval.upper()));
    }

    RealInterval & operator *= (qreal factor)
    {
        m_lower *= factor;
        m_upper *= factor;

        m_mid = (m_lower + m_upper)*0.5;

        return *this;
    }

private:
    qreal m_lower;
    qreal m_upper;
    qreal m_mid;
};

#endif

