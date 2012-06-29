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
name (const Analitza::Expression &functionExpression) : AbstractSurface(functionExpression) { } \
name (const Analitza::Expression &functionExpression, Analitza::Variables *variables) : AbstractSurface(functionExpression, variables) { }


class ANALITZAPLOT_EXPORT AbstractSurface : public AbstractFunctionGraph //strategy pattern for curves
{
public:
    AbstractSurface(const Analitza::Expression& e);
    AbstractSurface(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractSurface();

    //Own
    virtual void update(/*frumtum*/) = 0;
    QVector<int> indexes;
    QVector<QVector3D> points;
    
protected:
    AbstractSurface() {}
    AbstractSurface(const AbstractSurface& other) {}
};

#endif // ABSTRACTSURFACE_H
