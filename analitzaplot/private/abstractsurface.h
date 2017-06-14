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


#ifndef ABSTRACTSURFACE_H
#define ABSTRACTSURFACE_H

#include "abstractfunctiongraph.h"

#define CONSTRUCTORS(name) \
name (const Analitza::Expression &functionExpression, const QSharedPointer<Analitza::Variables>& variables) : AbstractSurface(functionExpression, variables) { }

namespace Analitza {

class AbstractSurface : public AbstractFunctionGraph //strategy pattern for curves
{
public:
    explicit AbstractSurface(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v = {});
    virtual ~AbstractSurface();

    //Own
    virtual void update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2) = 0;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<unsigned int> indexes;
    
protected:
    virtual QVector3D fromParametricArgs(double u, double v);
    bool buildParametricSurface();

private:
    AbstractSurface();
    AbstractSurface(const AbstractSurface& other);
    
    void doQuad(int n, int m, const QVector3D &p0,  const QVector3D &p1,  const QVector3D &p2,  const QVector3D &p3);
    void doStrip(int n, const QVector3D &p0,  const QVector3D &p1, const QVector3D &p2, const QVector3D &p3);
    void createFace( QVector3D *buffer );
};

}

#endif // ABSTRACTSURFACE_H
