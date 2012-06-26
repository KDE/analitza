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

class ANALITZAPLOT_EXPORT AbstractSurface : public AbstractFunctionGraph //strategy pattern for curves
{
public:
    explicit AbstractSurface(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractSurface();

    //AbstractMappingGraph
    int spaceDimension() const { return 3; }
    
    //Own
    virtual void update(/*frumtum*/) = 0;
    const QVector<int> & indexes() const;
    const QVector<QVector3D> & points() const;
    
protected:
    AbstractSurface() {}
    AbstractSurface(const AbstractSurface& other) {}
    
private:
    QMap< QString, RealInterval > m_argumentIntervals;
    QVector<int> m_indexes;
    QVector<QVector3D> m_points;
};

#endif // ABSTRACTSURFACE_H
