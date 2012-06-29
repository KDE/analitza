/*************************************************************************************
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#include "analitza/value.h"
#include <analitza/vector.h>

//TODO macros para las prop e abajo


#define MAXAROUND 64
#define MAXSTRIP 64
#define MAXALONG  64

class ANALITZAPLOT_EXPORT CartesianSurface : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    CONSTRUCTORS(CartesianSurface)
    TYPE_NAME("CartesianSurface z=f(x,y)")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    SPACE_DIMENSION(2)
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("x,y")
    ICON_NAME("none")
    EXAMPLES("x*x+y,x+y*sin(x),x*y")
    
    
    //Own
    void update();
    
private:
    double UU(int i);
    double VV(int j);
    QVector3D shape(float u,float v);
    void doSurface();
    void doQuad(int n, int m, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3);
    void doStrip(int n, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3);
    void _emit( QVector3D *buffer );
    
    qreal umin;
    qreal umax;
    qreal vmin;
    qreal vmax;
    int usteps;
    int vsteps;
    
    QVector3D surface[MAXALONG][MAXAROUND];
    QVector3D theStrip[MAXSTRIP][2];
};






void CartesianSurface::update()
{

    usteps=MAXALONG;
    vsteps=MAXAROUND;
}


double CartesianSurface::UU(int i)
{
    return (umin+((umax-umin)/(float)(usteps-1))*(float)(i));
}


double CartesianSurface::VV(int j)
{
    return (vmin+((vmax-vmin)/(float)(vsteps-1))*(float)(j));
}

QVector3D CartesianSurface::shape(float u,float v)
{
    arg("x")->setValue(u);
    arg("x")->setValue(v);

    Analitza::Expression res = analyzer->calculateLambda();

    Analitza::Object* vo = res.tree();
    Analitza::Vector* vec = dynamic_cast<Analitza::Vector*>(vo);

    if (vec)
    {
        Analitza::Cn* x = static_cast<Analitza::Cn*>(vec->at(0));
        Analitza::Cn* y = static_cast<Analitza::Cn*>(vec->at(1));
        Analitza::Cn* z = static_cast<Analitza::Cn*>(vec->at(2));

        return QVector3D(x->value(), y->value(), z->value());
    }

    return QVector3D();
}

void CartesianSurface::doSurface()
{
    int  i, j;
    float u, v;

    for ( i=0; i<usteps; i++ )
        for ( j=0; j<vsteps; j++ )
        {
            u = UU(i);
            v = VV(j);

            QVector3D point = shape(u,v);

            surface[i][j] = QVector3D(point.x(), point.y(), point.z());
        }

    for (i = 0; i < usteps -1; i++ )    
        for ( j=0; j<vsteps-1; j++ )    
        {
            doQuad(1, 1, surface[i][j], surface[i+1][j], surface[i][j+1], surface[i+1][j+1]);
        }
}

void CartesianSurface::doQuad(int n, int m, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3)
{
    int i;

    QVector3D A, B, C, D;   

    for (i=0; i<m; i++)
    {
        A = QVector3D((p0.x()*(float)(m-i) + p1.x()*(float)i)/(float)m, 
                      (p0.y()*(float)(m-i) + p1.y()*(float)i)/(float)m,
                      (p0.z()*(float)(m-i) + p1.z()*(float)i)/(float)m);
        
        B = QVector3D((p0.x()*(float)(m-i-1) + p1.x()*(float)(i+1))/(float)m,
                      (p0.y()*(float)(m-i-1) + p1.y()*(float)(i+1))/(float)m,
                      (p0.z()*(float)(m-i-1) + p1.z()*(float)(i+1))/(float)m);

        C = QVector3D((p2.x()*(float)(m-i)   + p3.x()*(float)i)/(float)m,
                      (p2.y()*(float)(m-i)   + p3.y()*(float)i)/(float)m,
                      (p2.z()*(float)(m-i)   + p3.z()*(float)i)/(float)m);

        D = QVector3D((p2.x()*(float)(m-i-1) + p3.x()*(float)(i+1))/(float)m, 
                      (p2.y()*(float)(m-i-1) + p3.y()*(float)(i+1))/(float)m,
                      (p2.z()*(float)(m-i-1) + p3.z()*(float)(i+1))/(float)m);
        
        doStrip(n, A, B, C, D);
    }
}

void CartesianSurface::doStrip(int n, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3)
{
    int i, j;
    QVector3D A, B, buffer[3];

    for (i=0; i<=n; i++)
    {
        A = QVector3D((p0.x()*(float)(n-i) + p2.x()*(float)i)/(float)n,
                      (p0.y()*(float)(n-i) + p2.y()*(float)i)/(float)n,
                      (p0.z()*(float)(n-i) + p2.z()*(float)i)/(float)n);

        B = QVector3D((p1.x()*(float)(n-i) + p3.x()*(float)i)/(float)n, 
                      (p1.y()*(float)(n-i) + p3.y()*(float)i)/(float)n,
                      (p1.z()*(float)(n-i) + p3.z()*(float)i)/(float)n);

        theStrip[i][0] = A;
        theStrip[i][1] = B;
    }
    
    buffer[0] = theStrip[0][0];
    buffer[1] = theStrip[0][1];
    for (i=1; i<=n; i++)
        for (j=0; j<2; j++)
        {
            buffer[2] = theStrip[i][j];
            _emit(buffer);
            buffer[0] = buffer[1];
            buffer[1] = buffer[2];
        }
}

void CartesianSurface::_emit( QVector3D *buffer )
{
    //TODO normal array too
//     surfpoint Normal, diff1, diff2;
// 
//     diff1.x = buffer[1].x - buffer[0].x;
//     diff1.y = buffer[1].y - buffer[0].y;
//     diff1.z = buffer[1].z - buffer[0].z;
//     diff2.x = buffer[2].x - buffer[1].x;
//     diff2.y = buffer[2].y - buffer[1].y;
//     diff2.z = buffer[2].z - buffer[1].z;
//     Normal.x = diff1.y*diff2.z - diff2.y*diff1.z;
//     Normal.y = diff1.z*diff2.x - diff1.x*diff2.z;
//     Normal.z = diff1.x*diff2.y - diff1.y*diff2.x;
// 
//     switch (m_drawingType)
//     {
//     case Function::Solid:
//         glBegin(GL_POLYGON);
//         break;
//     case Function::Wired:
//         glBegin(GL_LINES);
//         break;
//     case Function::Dots:
//         glBegin(GL_POINTS);
//         break;
// 
//     }
// 
// 
//     glNormal3f(Normal.x,Normal.y,Normal.z);
//     glVertex3f(buffer[0].x,buffer[0].y,buffer[0].z);
//     glVertex3f(buffer[1].x,buffer[1].y,buffer[1].z);
//     glVertex3f(buffer[2].x,buffer[2].y,buffer[2].z);
//     glEnd();
// 
//     glDisable(GL_POLYGON_OFFSET_FILL);

}


REGISTER_SURFACE(CartesianSurface)



