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


#ifndef SPACECURVE_H
#define SPACECURVE_H

#include "functiongraph.h"

class ANALITZAPLOT_EXPORT SpaceCurve : public FunctionGraph
{
    //3D
//torsion,   
    //curvature, length of arc, curvature
    
public:
    SpaceCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col, Analitza::Variables *variables = 0);
    virtual ~SpaceCurve();

    static bool canDraw(const Analitza::Expression &functionExpression);
    static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);

    bool reset(const Analitza::Expression& functionExpression);

    //Curve
    QVector<int> jumps() const;

    //Own 
    const QVector<QVector3D> & points() const;
    void update(const Box3D& viewport);

protected:
    SpaceCurve() {}
    SpaceCurve(const SpaceCurve &other) {}
};

#endif // SPACECURVE_H
