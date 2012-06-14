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


#ifndef ANALITZAPLOT_FUNCTIONGRAPH_H
#define ANALITZAPLOT_FUNCTIONGRAPH_H

#include "analitza/analyzer.h"
#include "function.h"

namespace Analitza
{
class Variables;
class Analyzer;
class Object;
class Cn;
class Expression;
}

class ANALITZAPLOT_EXPORT FunctionImpl //strategy pattern for curves and surfaces
{
public:
	FunctionImpl(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem,
             Analitza::Variables *variables, bool isImplicit = false);
    FunctionImpl(const FunctionImpl& fi);
	virtual ~FunctionImpl();

    //TODO to fungrap2d
// 	virtual void setFixedGradient(const VectorXd &funcvalargs) = 0; //mustrerun generate
	virtual void clearFixedGradients() = 0;

	virtual void updateGraphData(Function *function) = 0;
	virtual FunctionGraphData * data() const = 0;

	QStringList errors() const;

	virtual FunctionImpl * copy() = 0;


    //iface for func
    const FunctionGraphData * graphData() const;


protected:
    //analitza
    Analitza::Analyzer m_analyzer;
    QVector<Analitza::Object*> m_runStack;
    QVector<Analitza::Cn *> m_values;

    QStringList m_errors;
    bool m_isImplicit;

private:
    FunctionGraphData *m_graphData;
};

#endif // ANALITZAPLOT_FUNCTIONGRAPH_H
