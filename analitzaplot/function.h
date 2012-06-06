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

#include <QtCore/QUuid>
#include <QPair>

#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QVector2D>
#include <QVector3D>
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

//function as a value object
// "SINLGE valued" functions of real numbers (only real numbers) THAT can we can view on a 2d or 3d space
//esta clase se debe user si osi con su graph asociado ni no hay uno y se quier evauluar usar analyzer ...

class ANALITZAPLOT_EXPORT Function
{
public:
    explicit Function(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem,
                      bool isImplicit = false, const QString &name = QString());

    explicit Function(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem,
                      Analitza::Variables *variables, bool isImplicit = false, const QString &name = QString());

    Function(const Function &f);

    virtual ~Function();

    const Analitza::Expression & expression() const;

    const bool isImplicit() const { return m_isImplicit; }

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    QString iconName() const;

    //no se regenera los puntos si se cambia el "dominio" ... para eso volver a llamar generar puntos
	QPair<double, double> argumentInterval(const QString &argname) const { return m_arguments[argname].interval; }
	void setArgumentInverval(const QString &argname, QPair<double, double> interval) { m_arguments[argname].interval = interval; }

	FunctionType type() { return m_type; }

	//falla por el c++funcsover
// 	//conjunto de evals para funciones que representan curvas y superficies
    double evaluateRealValue(double arg);
    double evaluateRealValue(double arg1, double arg2);
    double evaluateRealValue(double arg1, double arg2, double arg3); // superfcie de nivel
    QVector2D evaluateVectorValue2(double arg); // curv param
    QVector3D evaluateVectorValue3(double arg); // curv param3d
    QVector3D evaluateVectorValue3(double arg1, double arg2); // surf param

//TODO
//     QVector2D evaluate(double arg1, double arg2); //TODO area para n=2
//     QVector2D evaluate(double arg1, double arg2); //TODO volemb para n=3 ... TODO n>3
//     QVector3D evaluate(double arg1, double arg2, double arg2); // transvol TODO n>3

	QStringList arguments() const { return m_arguments.keys(); }
	Analitza::Cn* argumentValue(const QString &argname) { return m_arguments[argname].value; }

	FunctionGraphDimension graphDimension() const { return m_graphDimension; }
    CoordinateSystem coordinateSystem() { return m_coordinateSystem; }
    FunctionGraphPrecision graphPrecision() { return m_graphPrecision; }
    void setGraphPrecision(FunctionGraphPrecision precision) { m_graphPrecision = precision; }
    QColor graphColor() const { return m_graphColor; }
    void setGraphColor(const QColor& newColor) { m_graphColor = newColor; }

    QStringList errors() const;
    bool isCorrect() const;

private:
    struct ArgumentInfo
    {
        QPair<double, double> interval;
        Analitza::Cn *value;
    };

    void setExpression(const Analitza::Expression &functionExpression, Keomath::CoordinateSystem coordinateSystem, bool isImplicitExpression);

    bool m_isImplicit;
    QMap< QString, ArgumentInfo > m_arguments;

    //gui
	QString m_name;

    //useful info
    FunctionType m_type;

    //analitza
	Analitza::Analyzer m_analyzer;
	QVector<Analitza::Object*> m_runStack;

    //graphDescription
    FunctionGraphDimension m_graphDimension = Dimension2D;
    CoordinateSystem m_coordinateSystem = Cartesian;
    FunctionGraphPrecision m_graphPrecision = Average;
    QColor m_graphColor;

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

#endif // ANALITZAPLOT_FUNCTION_H

