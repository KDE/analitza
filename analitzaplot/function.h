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

class FunctionImpl;
class FunctionImplData;

//function as a value object
// "SINLGE valued" functions of real numbers (only real numbers) THAT can we can view on a 2d or 3d space
//esta clase se debe user si osi con su graph asociado ni no hay uno y se quier evauluar usar analyzer ...

class ANALITZAPLOT_EXPORT Function
{
public:
    Function();

    Function(const Function &f);

    Function(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem,
             Analitza::Variables *variables, bool isImplicit = false, const QString &name = QString());

    virtual ~Function();

    const Analitza::Expression & expression() const;

    bool isImplicit() const { return m_isImplicit; }

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    QString iconName() const { return m_iconName; }
    QStringList examples() const { return m_examples; }

    //no se regenera los puntos si se cambia el "dominio" ... para eso volver a llamar generar puntos
	QPair<double, double> argumentInterval(const QString &argname) const { return m_arguments[argname].interval; }
	void setArgumentInverval(const QString &argname, QPair<double, double> interval) { m_arguments[argname].interval = interval; }
    QStringList arguments() const { return m_arguments.keys(); }
//     Analitza::Cn* argumentValue(const QString &argname) { return m_arguments[argname].value; }

	FunctionType type() { return m_type; }

// 	//falla por el c++funcsover
// // 	//conjunto de evals para funciones que representan curvas y superficies
//     double evaluateRealValue(double arg);
//     double evaluateRealValue(double arg1, double arg2);
//     double evaluateRealValue(double arg1, double arg2, double arg3); // superfcie de nivel
//     QVector2D evaluateVectorValue2(double arg); // curv param
//     QVector3D evaluateVectorValue3(double arg); // curv param3d
//     QVector3D evaluateVectorValue3(double arg1, double arg2); // surf param

//TODO
//     QVector2D evaluate(double arg1, double arg2); //TODO area para n=2
//     QVector2D evaluate(double arg1, double arg2); //TODO volemb para n=3 ... TODO n>3
//     QVector3D evaluate(double arg1, double arg2, double arg2); // transvol TODO n>3


	FunctionGraphDimension graphDimension() const { return m_graphDimension; }
    CoordinateSystem coordinateSystem() { return m_coordinateSystem; }
    FunctionGraphPrecision graphPrecision() { return m_graphPrecision; }
    void setGraphPrecision(FunctionGraphPrecision precision) { m_graphPrecision = precision; }
    PlotStyle plotStyle() { return m_plotStyle; }
    void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
    QColor graphColor() const { return m_graphColor; }
    void setGraphColor(const QColor& newColor) { m_graphColor = newColor; }
    bool isGraphVisible() const { return m_graphVisible; }
    void setGraphVisible(bool show) { m_graphVisible = show; }

    void updateGraphData();
    const FunctionGraphData * graphData() const;

    QStringList errors() const;
    bool isCorrect() const;

    Function operator = (const Function& f);
    bool operator==(const Function& f) const;

private:
    struct ArgumentInfo
    {
        QPair<double, double> interval;
    };

    bool m_isImplicit;
    QMap< QString, ArgumentInfo > m_arguments;

    Analitza::Expression m_expression;

    //gui
	QString m_name;
    QString m_iconName;
    QStringList m_examples;

    //useful info
    FunctionType m_type;

    //graphDescription
    FunctionGraphDimension m_graphDimension;
    CoordinateSystem m_coordinateSystem;
    FunctionGraphPrecision m_graphPrecision;
    PlotStyle m_plotStyle;
    QColor m_graphColor;
    bool m_graphVisible;
	FunctionImpl *m_graph;

	QStringList m_errors;
};

#endif // ANALITZAPLOT_FUNCTION_H
