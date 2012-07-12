/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
 *  Copyright (C) 2008 Rivo Laks <rivolaks@hot.ee>                                   *
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

#include <cmath>
#include <limits>

#include "analitzaplotexport.h"

#include "analitza/expression.h"
#include "analitza/value.h"
#include "analitza/analyzer.h"


#include <QPair>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
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

enum CoordinateSystem { Cartesian = 1, Polar, Cylindrical, Spherical };


//math utils

//  default cotrs AxisAlignedBoundingBox 

// an   Oriented Bounding Box class 


class Face
{
public:
    QVector3D p1;
    QVector3D p2;
    QVector3D p3;
    QVector3D normal;
};

class Box
{
    
};

//NOTE reduced version of Camera from kgllib credits: Copyright (C) 2008 Rivo Laks <rivolaks@hot.ee>
// class Camera
// {
// public:
//     Camera();
//     virtual ~Camera();
// 
//     /**
//      * return the viewport that has been specified using the @ref setViewport
//      *  method.
//      **/
//     QRect viewport() const;
// 
//     /**
//      * Specifies the current viewport.
//      **/
//     void setViewport(int x, int y, int width, int height);
// 
//     /**
//      * Sets the aspect ration to @p aspect.
//      * Aspect ratio is usually window's width divided by its height.
//      **/
//     void setAspect(float aspect);
//     /**
//      * Sets the depth buffer's range.
//      *
//      * @p near is distance from the camera to the near clipping plane and
//      *  @p far is distance to the far clipping plane. Everything that is not
//      *  between those two planes will be clipped away (not rendered), so you
//      *  should make sure that all your objects are within that range.
//      * At the same time, the depth range should be as small as possible (i.e.
//      *  @p near should be as big and @p far as small as possible) to increase
//      *  depth buffer precision.
//      **/
//     void setDepthRange(float near, float far);
// 
//     /**
//      * Sets the camera's positionto @p pos.
//      **/
//     void setPosition(const QVector3D& pos);
//     void setPosition(float x, float y, float z)  { setPosition(QVector3D(x, y, z)); }
//     /**
//      * Sets the lookat point to @p lookat.
//      * LookAt is the point at which the camera is looking at.
//      **/
//     void setLookAt(const QVector3D& lookat);
//     void setLookAt(float x, float y, float z)  { setLookAt(QVector3D(x, y, z)); }
//     /**
//      * Sets the up vector to @p up.
//      * Up vector is the one pointing upwards in the viewport.
//      **/
//     void setUp(const QVector3D& up);
//     void setUp(float x, float y, float z)  { setUp(QVector3D(x, y, z)); }
//     /**
//      * Sets the viewing direction of the camera to @p dir.
//      * This method sets lookat point to @ref position() + dir, thus
//      *  you will need to set camera's position before using this method.
//      **/
//     void setDirection(const QVector3D& dir);
//     void setDirection(float x, float y, float z)  { setDirection(QVector3D(x, y, z)); }
// 
//     QVector3D position() const  { return mPosition; }
//     QVector3D lookAt() const  { return mLookAt; }
//     QVector3D up() const  { return mUp; }
// 
//     /**
//      * Sets the modelview matrix.
//      *
//      * If you specify the modelview matrix using this method then parameters
//      *  specified using @ref setPosition, @ref setLookAt and @ref setUp
//      *  methods will be ignored.
//      *
//      * If you call any of @ref setPosition, @ref setLookAt or @ref setUp
//      *  after calling this method, then the modelview matrix specified here
//      *  will be ignored and new modelview matrix will be calculated using
//      *  specified position, lookat and up vectors.
//      **/
//     void setModelviewMatrix(const QMatrix4x4& modelview);
//     /**
//      * Sets the projection matrix.
//      *
//      * If you specify the projection matrix using this method then parameters
//      *  specified using @ref setFoV, @ref setAspect and @ref setDepthRange
//      *  methods will be ignored.
//      *
//      * If you call any of @ref setFoV, @ref setAspect or @ref setDepthRange
//      *  after calling this method, then the projection matrix specified here
//      *  will be ignored and new projection matrix will be calculated using
//      *  specified fov, aspect and depth range parameters.
//      **/
//     void setProjectionMatrix(const QMatrix4x4& projection);
// 
//     /**
//      * @return current modelview matrix.
//      * Modelview matrix is either set using the @ref setModelviewMatrix method
//      *  or automatically calculated using position, lookAt and up vectors.
//      **/
//     QMatrix4x4 modelviewMatrix() const;
//     /**
//      * @return current projection matrix.
//      * Projection matrix is either set using the @ref setProjectionMatrix
//      *   method or automatically calculated using fov, aspect and depth range
//      *   parameters.
//      **/
//     QMatrix4x4 projectionMatrix() const;
// 
// protected:
//     void recalculateProjectionMatrix();
// 
// protected:
//     QVector3D mPosition;
//     QVector3D mLookAt;
//     QVector3D mUp;
//     float mFoV, mAspect, mDepthNear, mDepthFar;
// 
//     QMatrix4x4 mModelviewMatrix;
//     QMatrix4x4 mProjectionMatrix;
//     QRect mViewport;
// };
// 
// //NOTE reduced version of TrackBall from kgllib credits: Copyright (C) 2008 Rivo Laks <rivolaks@hot.ee>
// class TrackBall
// {
// public:
//     TrackBall();
//     virtual ~TrackBall();
// 
//     virtual void rotate(float dx, float dy);
// 
//     QVector3D transform(const QVector3D& v) const;
//     Camera transform(const Camera& c);
// 
//     QVector3D xAxis() const;
//     QVector3D yAxis() const;
//     QVector3D zAxis() const;
// 
//     void rotateX(float degrees);
//     void rotateY(float degrees);
//     void rotateZ(float degrees);
// 
//     void rotate(float degrees, QVector3D axis);
// 
// protected:
//     QMatrix4x4 mMatrix;
// };


static bool isSimilar(double a, double b, double diff = .0000001)
{
    return fabs(a-b) < diff;
}

static bool traverse(double p1, double p2, double next)
{
    static const double delta=3;
    double diff=p2-p1, diff2=next-p2;
    
    bool ret = (diff>0 && diff2<-delta) || (diff<0 && diff2>delta);
    
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
