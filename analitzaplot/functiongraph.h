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

class AbstractFunctionGraph;

class ANALITZAPLOT_EXPORT FunctionGraph : public PlotItem
{
    
public:
    FunctionGraph(const Analitza::Expression &functionExpression, Dimension spacedim, const QString &nam, const QColor& col, Analitza::Variables *vars = 0);
    virtual ~FunctionGraph();

    Analitza::Variables *variables() const;
    
    //MappingGraph
    const QString typeName() const;
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
    
    void setExpression(const Analitza::Expression& functionExpression, Dimension spacedim);

    QStringList parameters() const;

    /** given an expression @p exp and a dimension @p spacedim, return the errors we would have if
        we tried to plot this function.
        @returns either the errors or empty if it can be done
    */
    static QStringList canDraw(const Analitza::Expression& exp, Dimension spacedim);

    static QStringList examples(Dimension dim);
    
protected:
    AbstractFunctionGraph *backend() const { return m_functionGraph; }

private:
    FunctionGraph(const FunctionGraph &other);
    
    static QString canDrawInternal(const Analitza::Expression &testexp, Dimension spacedim, QStringList &errs);
    
    AbstractFunctionGraph *m_functionGraph;
    QStringList m_errors;
};



#endif // FUNCTIONGRAPH2_H
