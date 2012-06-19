/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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


#ifndef ANALITZAPLOT_FUNCTIONUTILS
#define ANALITZAPLOT_FUNCTIONUTILS

#include "analitzaplotexport.h"

#include <cmath>

#include <QPair>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QLineF>

using std::acos;
using std::atan;
using std::fabs;
using std::cos;
using std::sin;
using std::sqrt;

const double PI = std::acos(-1.0);

namespace Analitza
{
class Expression;
}

enum DrawingPrecision { LowPrecision = 1, MediumPrecision, HighPrecision};
enum PlotStyle { Solid = 1, Wired, Dots };
enum CoordinateSystem { Cartesian = 1, Polar, Cylindrical, Spherical };

class EndPoint
{
public:
    EndPoint(double value = 0.0) : m_isInfinite(false), m_value(value) {}

    bool isInfinite() const { return m_isInfinite; }
    void setInfinite(bool infinite) { m_isInfinite = infinite; }
    double value() const { Q_ASSERT(m_isInfinite); return m_value; }
    void setValue(double value) { Q_ASSERT(m_isInfinite); m_value = value; }

private:

    bool m_isInfinite;
    double m_value;
};

class RealInterval
{
public:
    RealInterval(const EndPoint &lowEndPoint = EndPoint(-1.0), 
                 const EndPoint &highEndPoint = EndPoint(1.0))
    : m_lowEndPoint(lowEndPoint), m_highEndPoint(highEndPoint) { }

    EndPoint lowEndPoint() const { return m_lowEndPoint; }
    EndPoint highEndPoint() const { return m_highEndPoint; }
    void setEndPoints(const EndPoint &lowEndPoint, EndPoint &highEndPoint)
    {
        m_lowEndPoint = lowEndPoint;
        m_highEndPoint = highEndPoint;
    }

private:
    EndPoint m_lowEndPoint;
    EndPoint m_highEndPoint;
};

template<typename VectorType>
class LineSegment
{
public:
    LineSegment(const VectorType &position = VectorType(), 
                const VectorType &direction = VectorType())
    : m_position(position), m_direction(direction) {}
    
    VectorType position() const { return m_position; }
    VectorType direction() const { return direction; }
    
private:
    VectorType m_position;
    VectorType m_direction;
  
};

enum FunctionGraphDataFlag { FunctionData = 0x0, GradientData = 0x1, FixedGradientsData = 0x2 };
Q_DECLARE_FLAGS(FunctionGraphDataFlags, FunctionGraphDataFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(FunctionGraphDataFlags)

//TODO
struct FunctionGraphDescription
{
	FunctionGraphDataFlags dataFlags;
};

struct FunctionGraphData
{

};

struct FunctionGraphData2D : public FunctionGraphData
{
    QVector<QVector2D> fixedGradients;
    QVector<QPointF> points;
    QList<int> jumps;
};

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

#endif // ANALITZAPLOT_FUNCTIONUTILS
