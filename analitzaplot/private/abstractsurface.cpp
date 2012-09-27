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


#include "abstractsurface.h"
#include "utils/triangle3d.h"
#include <QVector3D>

#define MAXAROUND 32
#define MAXSTRIP 32
#define MAXALONG  32

AbstractSurface::AbstractSurface(const Analitza::Expression& e, Analitza::Variables* v)
: AbstractFunctionGraph(e, v)
{}

AbstractSurface::~AbstractSurface()
{}

bool AbstractSurface::buildParametricSurface()
{
    QStringList bvars = parameters();

    //TODO remove the assert en el caso de implicitas se deberia tratar siempre de crear la superficies parametrica primero
    Q_ASSERT(bvars.size() == 2); // solo para superficies que puedan ser parametrizadas
    
//     static QPair<double, double> intervalx = interval(bvars.at(0));
//     static QPair<double, double> intervaly = interval(bvars.at(1));
// 
//     qreal umin = intervalx.first;
//     qreal umax = intervalx.second;
//     qreal vmin = intervaly.first;
//     qreal vmax = intervaly.second;

    //TODO fix magic numbers
    qreal umin = -5;
    qreal umax = 5;
    qreal vmin = -5;
    qreal vmax = 5;
    
    int usteps = MAXALONG;
    int vsteps = MAXAROUND;
    
    vertices.clear();
    normals.clear();
    indexes.clear();

    QVector3D surface[MAXALONG][MAXAROUND];

    for ( int i=0; i<usteps; i++ )
        for ( int j=0; j<vsteps; j++ )
        {
            float u = (umin+((umax-umin)/(float)(usteps-1))*(float)(i));
            float v = (vmin+((vmax-vmin)/(float)(vsteps-1))*(float)(j));

            surface[i][j] = fromParametricArgs(u,v);
        }

    for (int i = 0; i < usteps -1; i++ )
        for ( int j=0; j<vsteps-1; j++ )
            doQuad(1, 1, surface[i][j], surface[i+1][j], surface[i][j+1], surface[i+1][j+1]);
        
    return !indexes.isEmpty();
}

void AbstractSurface::doQuad(int n, int m, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3)
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

void AbstractSurface::doStrip(int n, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3)
{
    int i, j;
    QVector3D A, B, buffer[3];
    QVector3D theStrip[MAXSTRIP][2];

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
            createFace(buffer);
            buffer[0] = buffer[1];
            buffer[1] = buffer[2];
        }
}

void AbstractSurface::createFace(QVector3D *buffer)
{
    QVector3D Normal, diff1, diff2;

    diff1 = QVector3D(buffer[1].x() - buffer[0].x(),
                      buffer[1].y() - buffer[0].y(),
                      buffer[1].z() - buffer[0].z());
    
    diff2 = QVector3D(buffer[2].x() - buffer[1].x(),
                      buffer[2].y() - buffer[1].y(),
                      buffer[2].z() - buffer[1].z());
    
    Normal = QVector3D(diff1.y()*diff2.z() - diff2.y()*diff1.z(),
                       diff1.z()*diff2.x() - diff1.x()*diff2.z(),
                       diff1.x()*diff2.y() - diff1.y()*diff2.x());

    vertices << buffer[0].x() << buffer[0].y() << buffer[0].z() <<
                buffer[1].x() << buffer[1].y() << buffer[1].z() <<
                buffer[2].x() << buffer[2].y() << buffer[2].z();
                
    normals << Normal.x() << Normal.y() << Normal.z(); 

    indexes.append(indexes.size());
    indexes.append(indexes.size());
    indexes.append(indexes.size());
}

QVector3D AbstractSurface::fromParametricArgs(double, double)
{
    return QVector3D();
}

    
    