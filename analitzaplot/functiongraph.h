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

#ifndef FUNCTIONGRAPH2_H
#define FUNCTIONGRAPH2_H

#include "plotitem.h"
#include <QPair>

namespace Analitza
{

/**
 * \class FunctionGraph
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Base class for any object that is a lambda expression.
 *
 * This class is a common interface for functions objects.
 */

class AbstractFunctionGraphOld;

class ANALITZAPLOT_EXPORT FunctionGraph : public PlotItem
{
public:
    virtual ~FunctionGraph();

    Analitza::Variables *variables() const;
    
    //MappingGraph
    QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    Dimension spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    QStringList errors() const;
    bool isCorrect() const;

    //if evaluate true then result of expressiones will be strings of the value
    //if evaluate is false then the expressions will not evaluate
 
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);

    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);
    
    /** @returns the parameters that a function expects */
    QStringList parameters() const;

    /**
     * The display property will store the expression like it's been entered by the user.
     * This is useful because sometimes the expression is modified when entered so that
     * we can plot it properly, this remembers what the user entered.
     */
    QString display() const;
    void setDisplay(const QString& display);
    
    /** 
     * This method gives a hint to the backend of how many @p points we want the plots to have.
     * This is useful for telling the plot implementations an idea of where is this going to be plotted, so
     * we can use lighter computations if we're previewing or if we're in a small device.
     */
    void setResolution(int points);
    
protected:
	FunctionGraph(AbstractFunctionGraphOld* g);
    AbstractFunctionGraphOld *backend() const { return m_functionGraph; }

private:
    FunctionGraph(const FunctionGraph &other);
    
    AbstractFunctionGraphOld *m_functionGraph;
    QStringList m_errors;
    QString m_display;
};

///

class FunctionGraphData : public ShapeData
{
public:
	virtual ~FunctionGraphData() {}
	
	QMap< QString, QPair< Expression, Expression > > m_arguments;
};

template <typename ShapeType>
class AbstractFunctionGraph : public AbstractShape<ShapeType> // X->F(X) : Kn->Km, K: real or complex 
{
public:
	virtual QStringList arguments() const = 0; // X in X->F(X)
	virtual QPair<Expression, Expression> limits(const QString &arg) const = 0;
	virtual QStringList parameters() const = 0; // a,b,c... in X->F(X,a,b,c,...)
	virtual void setLimits(const QString &arg, double min, double max) = 0; //x,y,z also bvars like theta and vars in Variables ...update geometry
	virtual void setLimits(const QString &arg, const Expression &min, const Expression &max) = 0; //x,y,z also bvars like theta and vars in Variables ...update geometry
};



}

#endif // FUNCTIONGRAPH2_H
