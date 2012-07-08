/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "planecurvesmodel.h"

#include <analitza/localize.h>
#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QIcon>

#include "analitza/expression.h"

///

PlaneCurvesModel::PlaneCurvesModel(QObject* parent): FunctionGraphsModel(parent)
{

}


PlaneCurvesModel::PlaneCurvesModel(Analitza::Variables *v, QObject * parent)
    : FunctionGraphsModel(v, parent)
{
}

PlaneCurvesModel::~PlaneCurvesModel()
{

}

//TODO review see item
const PlaneCurve* PlaneCurvesModel::curve(int curveIndex) const
{
    return static_cast<const PlaneCurve*>(item(curveIndex));
}

bool PlaneCurvesModel::setCurve(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col)
{
//                 if (PlaneCurve::canDraw(fexp))
//                 {
//                     items[index.row()]->reset(fexp);
// 
//                     emit dataChanged(index, index);
// 
//                     return true;
//                 }
return false;
}
    
void PlaneCurvesModel::updateCurve(int curveIndex, const QRect& viewport)
{
    static_cast<PlaneCurve*>(items[curveIndex])->update(viewport);

    //DONT EMIT dataChanged ... remove for next iter ... solo emitir cuando se cambia EXTERNAMENTE la curva NO cuando el BACKEND CAMBIA la data interna
//     emit dataChanged(index(curveIndex), index(curveIndex));
}

QPair< QPointF, QString > PlaneCurvesModel::curveImage(int row, const QPointF& mousepos)
{
    Q_ASSERT(row<items.count());

    return static_cast<PlaneCurve*>(items[row])->image(mousepos);
}

QLineF PlaneCurvesModel::curveTangent(int row, const QPointF& mousepos)
{
    Q_ASSERT(row<items.count());

    return static_cast<PlaneCurve*>(items[row])->tangent(mousepos);
}

