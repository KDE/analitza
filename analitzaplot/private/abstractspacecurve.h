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

#ifndef ABSTRACTSPACECURVE_H
#define ABSTRACTSPACECURVE_H

#include "abstractfunctiongraph.h"

#define CONSTRUCTORS(name) \
name (const Analitza::Expression &functionExpression, const QSharedPointer<Analitza::Variables>& variables) :AbstractSpaceCurve(functionExpression, variables) { }

namespace Analitza {
class Variables;

class AbstractSpaceCurve : public AbstractFunctionGraph 
{
public:
    explicit AbstractSpaceCurve(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v = {});
    virtual ~AbstractSpaceCurve();

    //Curve ... los expongo como publicos tanto para planecurve como para los backend (
    //para los backends por un tema de performance y flexibilidad) 
    // al final en planecurve todo estara expuesto consistentemente 
    QVector<QVector3D> points;
    QVector<int> jumps;

    //Own
    virtual void update(const QVector3D & oppositecorner1, const QVector3D & oppositecorner2) = 0;

protected:
    bool addPoint(const QVector3D& p);

private:
    AbstractSpaceCurve();
};

}

#endif // ABSTRACTSPACECURVE_H
