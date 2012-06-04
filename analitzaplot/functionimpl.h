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


#ifndef KEOMATH_SOLVER_H
#define KEOMATH_SOLVER_H

#include <QtCore/QPointF>
#include <QtGui/QVector3D>


#include <KDE/KLocalizedString>

#include "analitza/analyzer.h"

#include "function.h"

namespace Keomath
{

class ANALITZAPLOT_EXPORT FunctionGraph //strategy pattern for curves and surfaces
{
public:
	FunctionGraph();
	virtual ~FunctionGraph();

	virtual void setFixedGradient(const VectorXd &funcvalargs) = 0; //mustrerun generate
	virtual void clearFixedGradients() = 0;

	virtual void generateData(Function *function) = 0;
	virtual FunctionGraphData * data() const = 0;

	QStringList errors() const;

	virtual FunctionGraph * copy() = 0;

protected:
	QStringList m_errors;
};

}

class ANALITZAPLOT_EXPORT FunctionImpl
{
public:
    explicit FunctionImpl(const Analitza::Expression &expression, Analitza::Variables *variables);
    FunctionImpl(const FunctionImpl &solver);
    virtual ~FunctionImpl();

    RealInterval::List domain() const
    {
        return m_domain;
    }

    void setDomain(const RealInterval::List &domain)
    {
        m_domain.clear();

        foreach (const RealInterval &i, domain)
        m_domain.append(i);
    }

    int resolution() const
    {
        return m_resolution;
    }

    virtual void setResolution(int resolution) = 0;

    QStringList errors() const
    {
        return m_errors;
    }
    bool isCorrect() const
    {
        return m_errors.isEmpty() && m_evaluator.isCorrect();
    }

    virtual const Analitza::Expression & lambda() const
    {
        return m_evaluator.expression();
    }

    virtual int dimension() = 0;
    virtual QStringList arguments() const = 0;
    virtual Function::Axe axeType() const = 0;
    virtual void solve(const RealInterval::List &spaceBounds = RealInterval::List()) = 0;
    virtual FunctionImpl * copy() = 0;

protected:
    RealInterval::List m_domain;
    int m_resolution;
    QStringList m_errors;


    Analitza::Analyzer m_evaluator;
    QVector<Analitza::Object*> m_runStack;

};





class ANALITZAPLOT_EXPORT FunctionImpl2D : public FunctionImpl
{
public:
    explicit FunctionImpl2D(const Analitza::Expression &expression, Analitza::Variables *variables);
    FunctionImpl2D(const FunctionImpl2D &solver2d);
    virtual ~FunctionImpl2D();

    void setResolution(int resolution);

    static int fDimension()
    {
        return 2;
    }

    int dimension()
    {
        return fDimension();
    }

    int typeFunc2D()
    {
        return m_type2D;
    }

    virtual Function::Axe axeType() const
    {
        return Function::Cartesian;
    }

    virtual QPair<QPointF, QString> calc(const QPointF &dp) = 0;
    virtual QLineF derivative(const QPointF &point) = 0;

    QVector<QLineF> paths() const
    {
        return m_paths;
    }

protected:
    void buildPaths(const QRectF &viewport, const QList<QPointF> &points);
    int m_type2D;

    QVector<QLineF> m_paths;
};


class ANALITZAPLOT_EXPORT FunctionImpl3D : public FunctionImpl
{
public:

    virtual bool isCurve() const
    {
        return false;
    }
    virtual QVector3D evalCurve(qreal t)
    {
        return QVector3D();
    }




    explicit FunctionImpl3D(const Analitza::Expression &expression, Analitza::Variables *variables);
    FunctionImpl3D(const FunctionImpl3D &solver3D);
    virtual ~FunctionImpl3D();

    void setResolution(int resolution)
    {
        m_resolution = resolution;
    }

    static int fDimension()
    {
        return 3;
    }

    int dimension()
    {
        return fDimension();
    }
    virtual Function::Axe axeType() const
    {
        return Function::Cartesian;
    }

    virtual QVector3D evalSurface(qreal u, qreal v) = 0;
};

#endif
