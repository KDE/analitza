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

#include "mappinggraph.h"

class AbstractFunctionGraph;

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph(const Analitza::Expression &functionExpression, int spaceDimension, const QString &name, const QColor& col);
    FunctionGraph(const Analitza::Expression &functionExpression, Analitza::Variables *variables, int spaceDimension, const QString &name, const QColor& col);
    virtual ~FunctionGraph();

    static bool canDraw(const Analitza::Expression &functionExpression, int spaceDimension);
    //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
    static bool canDraw(const Analitza::Expression &functionExpression, int spaceDimension, QStringList &errors);

    bool reset(const Analitza::Expression &functionExpression, int spaceDimension);

    Analitza::Variables *variables() const;
    void setVariables(Analitza::Variables *variables);

    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    QStringList errors() const;
    bool isCorrect() const;
    
    //if evaluate true then result of expressiones will be strings of the value
    //if evaluate is false then the expressions will not evaluate
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);

    //2 convenience methods to work with doubles instead of Expression->Cn->value ... see above
    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);

    QStringList parameters() const;
    
protected:
    FunctionGraph() {}
    FunctionGraph(const FunctionGraph &other) {}
    
    AbstractFunctionGraph *backend() const { return m_functionGraph; }

private:
    AbstractFunctionGraph *m_functionGraph;

    QStringList m_errors;
};



#endif // FUNCTIONGRAPH2_H
