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

#ifndef ANALITZAPLOT_ABSTRACTPLANECURVE_H
#define ANALITZAPLOT_ABSTRACTPLANECURVE_H

#include "abstractfunctiongraph.h"
#include <QLineF>

#define CONSTRUCTORS(name) \
name (const Analitza::Expression &functionExpression, const QSharedPointer<Analitza::Variables>& variables) :AbstractPlaneCurve(functionExpression, variables) { }

namespace Analitza {
class Variables;

class AbstractPlaneCurve : public AbstractFunctionGraph 
{
public:
    explicit AbstractPlaneCurve(const Analitza::Expression& e, const QSharedPointer<Analitza::Variables>& v = {});
    ~AbstractPlaneCurve() override;

    QList<QPointF> points;
    QList<int> jumps;

    virtual void update(const QRectF& viewport) = 0;
    virtual QPair<QPointF, QString> image(const QPointF &mousepos) = 0;
    virtual QLineF tangent(const QPointF &mousepos) = 0;

protected:
    bool addPoint(const QPointF& p);
};

}

#endif // ANALITZAPLOT_ABSTRACTPLANECURVE_H
