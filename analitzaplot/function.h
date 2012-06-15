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

#ifndef ANALITZAPLOT_FUNCTION_H
#define ANALITZAPLOT_FUNCTION_H

#include "analitza/expression.h"
#include "functionutils.h"

#include <QMap>
#include <QColor>

namespace Analitza
{
class Variables;
}

class FunctionImpl2D;

// this class is a sort of a functionitem ...
// it can't use for plot curves/surfaces (is a just a data holder)
class ANALITZAPLOT_EXPORT Function
{
public:
    Function();
    Function(const Function &f);
    Function(const QString &name, const QColor& col);
    virtual ~Function();

    const QString id() const { return m_id; }

    virtual const Analitza::Expression & expression() const = 0; // why pure abstract: couse graphpres go to functionimpl
    virtual bool isImplicit() const = 0;

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    QString iconName() const { return m_iconName; }
    QColor color() const { return m_color; }
    void setColor(const QColor& newColor) { m_color = newColor; }
    QStringList examples() const { return m_examples; }

    QPair<double, double> argumentInterval(const QString &argname) const { return m_argumentIntervals[argname]; }
    void setArgumentInverval(const QString &argname, QPair<double, double> interval) { m_argumentIntervals[argname] = interval; }
    virtual QStringList arguments() const = 0;

    FunctionType type() { return m_type; }
    virtual FunctionGraphDimension graphDimension() const = 0;
    CoordinateSystem coordinateSystem() { return m_coordinateSystem; }
    FunctionGraphPrecision graphPrecision() { return m_graphPrecision; }
    virtual void setGraphPrecision(FunctionGraphPrecision precs) = 0; // why pure abstract: couse graphpres go to functionimpl
    PlotStyle plotStyle() { return m_plotStyle; }
    void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
    bool isGraphVisible() const { return m_graphVisible; }
    void setGraphVisible(bool f) { m_graphVisible = f; }
    void hideGraph() { m_graphVisible = false; }
    void showGraph() { m_graphVisible = true; }

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;

    bool operator == (const Function& f) const { return m_id == f.m_id; }

protected:
    void setId(const QString id) { m_id = id; }

    //gui
    void setIconName(const QString &iconName) { m_iconName = iconName; }
    void setExamples(const QStringList &examples) { m_examples = examples; }

    //useful info
    void setType(FunctionType t) { m_type = t; }

    //graphDescription
    void setGraphDimension(FunctionGraphDimension graphdim) { m_graphDimension = graphdim; }
    void setCoordinateSystem(CoordinateSystem coordsys) { m_coordinateSystem = coordsys; }

private: //TODO pimpl idiom here?
    QString m_id; // from a QUuid

    bool m_isImplicit;

    QMap< QString, QPair<double, double> > m_argumentIntervals;

    //gui
    QString m_name;
    QString m_iconName;
    QColor m_color;
    QStringList m_examples;

    //useful info
    FunctionType m_type;

    //graphDescription
    FunctionGraphPrecision m_graphPrecision;
    PlotStyle m_plotStyle;
    bool m_graphVisible;

    //graphDescription
    FunctionGraphDimension m_graphDimension;
    CoordinateSystem m_coordinateSystem;
};

//si tiene 2 vars es implicit si tiene 2 vars y una de ellos es r es polar ... caracterizacion
//por eso no es necesario que los impls expongan cosas como isimplicit
class ANALITZAPLOT_EXPORT Function2D : public Function
{
public:
    Function2D();

    Function2D(const Function2D &f);

    Function2D(const Analitza::Expression &functionExpression, const QString &name, const QColor& col, Analitza::Variables *variables);

    virtual ~Function2D();

    const Analitza::Expression &expression() const;
    bool isImplicit() const;

    QStringList arguments() const;

    FunctionGraphDimension graphDimension() const { return Dimension2D; }
    void setGraphPrecision(FunctionGraphPrecision precs);

    QStringList errors() const;
    bool isCorrect() const;

    Function2D operator = (const Function2D& f); // copy all:members, "id" and funcimpl instance

    //functionimpl iface
    const QVector<QPointF> & points() const;
    QList<int> jumps() const;
    QLineF derivative(const QPointF &p) const;
    QPair<QPointF, QString> calc(const QPointF &dp);

private:
    Analitza::Expression m_expression;

    FunctionImpl2D* m_function;

    QStringList m_errors;
};


#endif // ANALITZAPLOT_FUNCTION_H
