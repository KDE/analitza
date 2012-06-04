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

#ifndef KEOMATH_FUNCTION_H
#define KEOMATH_FUNCTION_H

#include <QtCore/QUuid>
#include <QPair>

#include <QtGui/QColor>
#include <QtGui/QPen>
#include <qglobal.h>

#include <KDE/KDateTime>

#include "analitzaplotexport.h"

#include "functionutils.h"

namespace Analitza
{
class Expression;
class Variables;
}

#include "analitza/analyzer.h"

namespace Keomath
{

class FunctionGraph;
class FunctionGraphData;

//function is not a value object is an entity object, then their use will be
// with refs and pointers (no copy ctor nor  = optr == optr)
class ANALITZAPLOT_EXPORT Function
{
public:
	Function(const Analitza::Expression &expression, Analitza::Variables *variables,
			 const FunctionGraphDescription &graphDescription = FunctionGraphDescription(),
			 bool hasImplicitExpression = false,
		     const QString &name = QString());

	virtual ~Function();

// 	void redefine()

	const Analitza::Expression& expression() const;

	IntervalList domain() const { return m_domain; }
	void setDomain(const IntervalList &domain);

	const FunctionGraphDescription graphDescription() const { return m_graphDescription; }

	const bool hasImplicitExpression() const { return m_hasImplicitExpression; }

	QString name() const { return m_name; }
	void setName(const QString &newName) { m_name = newName; }

	QColor color() const { return m_color; }
	void setColor(const QColor& newColor) { m_color = newColor; }

	const int argumentCount() { return m_argumentCount; } //Returns the number of arguments passed to the function in this invocation.
	const int valueCount() { return m_valueCount; } // realvalued funcs has one only one comp, others > 1
	const FunctionImageType imageType() const { return m_imageType; }
// int arguments,int imageComponents, ???

	QUuid id() const { return m_id; }

	VectorXd evaluate(const VectorXd &args);

	QVector<Analitza::Cn*> arguments() const
	{
		return m_arguments;
	}

	QStringList errors() const;
	bool isCorrect() const;

private:
	IntervalList m_domain;
	FunctionGraphDescription m_graphDescription;
	bool m_hasImplicitExpression;
	QString m_name;
	QColor m_color;

	int m_argumentCount;
	int m_valueCount;
	FunctionImageType m_imageType;
	QUuid m_id;

	Analitza::Analyzer m_analyzer;
	QVector<Analitza::Object*> m_runStack;
    QVector<Analitza::Cn*> m_arguments;

	FunctionGraph *m_functionGraph;

	QStringList m_errors;
};

}

struct FunctionImpl;

class ANALITZAPLOT_EXPORT Function
{
public:

    enum Axe { Cartesian = 0, Polar, Cylindrical, Spherical };



    enum DrawingType { Solid = 0, Wired, Dots};


    Function();

    //create a 2d fn with just one arg (backward comp)
    Function(const QString& name, const Analitza::Expression& newExp, Analitza::Variables* v, const QPen& m_pen,
                double uplimit, double downlimit);

    Function(const Analitza::Expression &expression, int dimension, const RealInterval::List &domain,
             Analitza::Variables *variables, const QString &name = QString(), const QColor &color = QColor());


    Function(const Function &function);


    ~Function();

    //clac 2d val for 2d curve (backward comp)
    QPair<QPointF, QString> calc(const QPointF& dp);

    //(backward comp)
    void update_points(const QRect& viewport);

    const QUuid & id() const
    {
        return m_id;
    }

    void setId(const QUuid &newid)
    {
        m_id = newid;
    }


    const QUuid & spaceId() const
    {
        return m_spaceId;
    }


    KDateTime dateTime() const
    {
        return m_dateTime;
    }


    void setSpaceId(const QUuid &spaceId)
    {
        m_spaceId = spaceId;
    }

    const Analitza::Expression& expression() const;


    QString name() const
    {
        return m_name;
    }


    void setName(const QString &newName)
    {
        m_name = newName;
    }


    QColor color() const
    {
        return m_color;
    }
    void setColor(const QColor& newColor)
    {
        m_color = newColor;
    }


    int dimension() const;

    Axe axeType() const;
    RealInterval::List domain() const;
    void setDomain(const RealInterval::List &domain);

    int resolution() const;
    void setResolution(int resolution);



    bool isShown() const;
    void setShown(bool newShow)
    {
        m_show=newShow;
    }
    int typeFunction()
    {
        return typeF;
    }
    DrawingType drawingType() const
    {
        return m_drawingType;
    }
    void setDrawingType(DrawingType drawingType)
    {
        m_drawingType = drawingType;
    }


    qreal lineWidth() const
    {
        return m_lineWidth;
    }
    void setLineWidth(qreal lineWidth)
    {
        m_lineWidth = lineWidth;
    }

    FunctionImpl * solver() const
    {
        return m_solver;
    }

    QStringList errors() const;

    bool isCorrect() const;


    bool operator == (const Function &function) const
    {
        return function.id() == m_id;
    }


    Function operator = (const Function& f);

private:
    QUuid m_id;
    QUuid m_spaceId;
    KDateTime m_dateTime;

    QString m_name;
    QColor m_color;
    int typeF;

    bool m_show;
    DrawingType m_drawingType;
    qreal m_lineWidth;

    FunctionImpl *m_solver;

    QStringList m_errors;
};

typedef QList<Function> FunctionList;

#endif
