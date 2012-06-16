/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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


#ifndef ANALITZAPLOT_FUNCTIONGRAPH_H
#define ANALITZAPLOT_FUNCTIONGRAPH_H

#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "function.h"

namespace Analitza
{
class Variables;
class Analyzer;
class Object;
class Cn;
class Expression;
}

class ANALITZAPLOT_EXPORT AbstractMappingGraph //strategy pattern for curves and surfaces
{
public:
    explicit AbstractMappingGraph(const Analitza::Expression& e, Analitza::Variables* v);
    AbstractMappingGraph(const AbstractMappingGraph& fi);
    virtual ~AbstractMappingGraph();

    DrawingPrecision drawingPrecision() { return m_drawingPrecision; }
    void setDrawingPrecision(DrawingPrecision precision) { m_drawingPrecision = precision; }

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;
    virtual AbstractMappingGraph * copy() = 0;

protected:
    Analitza::Analyzer analyzer;
    Analitza::Cn* arg(const QString &argname) { return dynamic_cast<Analitza::Cn*>(m_argumentValues[argname]); }

private:

    DrawingPrecision m_drawingPrecision;

    QMap<QString, Analitza::Object*> m_argumentValues;
};

////
//si tiene 2 vars es implicit si tiene 2 vars y una de ellos es r es polar ... caracterizacion
class ANALITZAPLOT_EXPORT AbstractCurve : public AbstractMappingGraph //strategy pattern for curves
{
public:
    explicit AbstractCurve(const Analitza::Expression& e, Analitza::Variables* v);
    AbstractCurve(const AbstractCurve& fi);
    virtual ~AbstractCurve();

//     RealInterval argumentInterval(const QString &argname) const { return m_argumentIntervals[argname]; }
//     void setArgumentInverval(const QString &argname, QPair<double, double> interval) { m_argumentIntervals[argname] = interval; }

    virtual void updateGraph(const QRect& viewport) = 0;
    QVector<QPointF> points() const;
    QList<int> jumps() const;
    virtual QPair<QPointF, QString> calc(const QPointF& dp)=0;
    virtual QLineF derivative(const QPointF& p)=0;

protected:
    bool addValue(const QPointF& p);
    void setJump(int jump);
    
private:
    QMap< QString, RealInterval > m_argumentIntervals;
    QVector<QPointF> m_points;
    QList<int> m_jumps;
};

#endif // ANALITZAPLOT_FUNCTIONGRAPH_H
