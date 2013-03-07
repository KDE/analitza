/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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


#ifndef ANALITZAPLOT_FUNCTION_H
#define ANALITZAPLOT_FUNCTION_H

#include "functiongraph.h"
#include <analitza/expression.h>
#include <analitza/analyzer.h>

#include <QMap>
#include <QPoint>
#include <QRectF>
#include <QLineF>
#include <QExplicitlySharedDataPointer>
#include <QVector>
#include <QColor>

namespace Analitza
{
class Variables;
class Expression;
class AbstractPlaneCurve;

/**
 * \class PlaneCurve
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Represents a curve in 2D space.
 *
 * This class contains all necessary data that Plotter2D will use to draw the curve.
 */

class ANALITZAPLOT_EXPORT PlaneCurveOld : public FunctionGraph 
{
public:
	PlaneCurveOld(AbstractFunctionGraphOld* g);
	virtual ~PlaneCurveOld();

	QVector<int> jumps() const;
	const QVector<QPointF> & points() const;
	void update(const QRectF& viewport);
	QPair<QPointF, QString> image(const QPointF &mousepos); // calculate the image of the curve based on the mouse postion 
	QLineF tangent(const QPointF &mousepos);// calculate the tangent to the curve based on the mouse 
};

///

class PlaneCurveData : public ShapeData
{
public:
	PlaneCurveData();
	PlaneCurveData(const PlaneCurveData &other);
	PlaneCurveData(const Analitza::Expression& expresssion, Variables* vars);
	~PlaneCurveData() {}
	
	QMap< QString, QPair< Expression, Expression > > m_arguments; // from old functiongraphiface

	QVector<int> m_jumps;
	QVector<QPointF> m_points;
};

class ANALITZAPLOT_EXPORT PlaneCurve : public ShapeInterface<PlaneCurve>
{
public:
	PlaneCurve();
	PlaneCurve(const PlaneCurve &other);
	PlaneCurve(const Analitza::Expression &expresssion, Variables* vars = 0);// { if is builtin else is eq, expr, lambda,etc }
	PlaneCurve(const QString &expresssion, Variables* vars = 0);// { if is builtin else is eq, expr, lambda,etc }

	//BEGIN AbstractShape interface
	void clearTags();
	void addTags(const QSet<QString> &tags);
	QColor color() const;
	CoordinateSystem coordinateSystem() const;
	QStringList errors() const;
	Expression expression() const;
	QString id() const;
	QMap<QString, QString> information() const;
	QString iconName() const;
	bool isValid() const;
	bool isVisible() const;
	QString name() const;
	void setColor(const QColor &color);
	void setExpression(const Expression &expression);
	void setName(const QString &name);
	void setVisible(bool visible);
	Dimension dimension() const;
	QSet<QString> tags() const;
	QString typeName() const;
	Variables *variables() const;
	
	bool operator==(const PlaneCurve &other) const;
	bool operator!=(const PlaneCurve &other) const;
	PlaneCurve & operator=(const PlaneCurve &other);
	//END AbstractShape interface
	
	//BEGIN AbstractFunctionGraph interface
	QStringList arguments() const;// X->F(X) : Kn->Km, K: real or complex so args := X in X->F(X)
	QPair<Expression, Expression> limits(const QString &arg) const;
	QStringList parameters() const;// a,b,c... in X->F(X,a,b,c,...)
	void setLimits(const QString &arg, double min, double max);//x,y,z also bvars like theta and vars in Variables ...update geometry
	void setLimits(const QString &arg, const Expression &min, const Expression &max);//x,y,z also bvars like theta and vars in Variables ...update geometry
	//END AbstractFunctionGraph interface
	
	QPair<QPointF, QString> image(const QPointF &mousepos); // calculate the image of the curve based on the mouse postion 
	QVector<int> jumps() const;
	QVector<QPointF> points() const;
	QLineF tangent(const QPointF &mousepos);// calculate the tangent to the curve based on the mouse
	void update(const QRectF& viewport);
	
	//BEGIN static AbstractShape interface
	static QStringList builtinMethods();
	static bool canBuild(const Expression &expression, Analitza::Variables* vars = 0);
	//END static AbstractShape interface
	
private:
	QExplicitlySharedDataPointer<PlaneCurveData> d;
};

// singleton or namespace para el buuilder de las clases concretas

}

#endif // ANALITZAPLOT_FUNCTION_H
