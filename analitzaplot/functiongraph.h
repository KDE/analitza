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

class AbstractFunctionGraph;

class ANALITZAPLOT_EXPORT FunctionGraph : public PlotItem
{
public:
    virtual ~FunctionGraph();

    Analitza::Variables *variables() const override;
    
    //MappingGraph
    QString typeName() const override;
    const Analitza::Expression &expression() const override;
    QString iconName() const override;
    Dimension spaceDimension() const override;
    CoordinateSystem coordinateSystem() const override;
    QStringList errors() const;
    bool isCorrect() const;

    //if evaluate true then result of expressiones will be strings of the value
    //if evaluate is false then the expressions will not evaluate
 
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);

    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);
    void clearIntervals();
    bool hasIntervals() const;
    
    /** @returns the parameters that a function expects */
    QStringList parameters() const;

    /**
     * The display property will store the expression like it's been entered by the user.
     * This is useful because sometimes the expression is modified when entered so that
     * we can plot it properly, this remembers what the user entered.
     */
    QString display() const override;
    void setDisplay(const QString& display);
    
    /** 
     * This method gives a hint to the backend of how many @p points we want the plots to have.
     * This is useful for telling the plot implementations an idea of where is this going to be plotted, so
     * we can use lighter computations if we're previewing or if we're in a small device.
     */
    void setResolution(int points);
    
protected:
    FunctionGraph(AbstractFunctionGraph* g);
    AbstractFunctionGraph *backend() const { return m_functionGraph; }

private:
    FunctionGraph(const FunctionGraph &other);
    
    AbstractFunctionGraph *m_functionGraph;
    QStringList m_errors;
    QString m_display;
};

}

#endif // FUNCTIONGRAPH2_H
