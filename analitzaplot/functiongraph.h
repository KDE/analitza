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

class ANALITZAPLOT_EXPORT FunctionImpl //strategy pattern for curves and surfaces
{
public:
    explicit FunctionImpl(const Analitza::Expression& e, Analitza::Variables* v);
    FunctionImpl(const FunctionImpl& fi);
    virtual ~FunctionImpl();

    FunctionGraphPrecision graphPrecision() { return m_graphPrecision; }
    void setGraphPrecision(FunctionGraphPrecision precs) { m_graphPrecision = precs; }

    virtual QStringList errors() const = 0;

    virtual FunctionImpl * copy() = 0;

protected:
    Analitza::Analyzer analyzer;
    const Analitza::Cn* arg(const QString &argname) const { return dynamic_cast<Analitza::Cn*>(m_argumentValues[argname]); }

private:
    FunctionGraphPrecision m_graphPrecision;

    QMap<QString, Analitza::Object*> m_argumentValues;
};

////
//si tiene 2 vars es implicit si tiene 2 vars y una de ellos es r es polar ... caracterizacion
class ANALITZAPLOT_EXPORT FunctionImpl2D : public FunctionImpl //strategy pattern for curves
{
public:
    explicit FunctionImpl2D(const Analitza::Expression& e, Analitza::Variables* v);
    FunctionImpl2D(const FunctionImpl2D& fi);
    virtual ~FunctionImpl2D();

    QStringList errors() const;

protected:
    QVector<QPointF> points;
    QList<int> jumps;
};

#endif // ANALITZAPLOT_FUNCTIONGRAPH_H
