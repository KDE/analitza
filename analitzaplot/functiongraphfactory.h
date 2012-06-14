/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#ifndef ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
#define ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H

#include <QVector>
#include <QString>
#include <QMap>
#include <QStringList>
#include "analitza/analyzer.h"

#include "functionutils.h"
extern Analitza::Analyzer a;
namespace Analitza
{
class Expression;
class ExpressionType;
class Variables;
}


#define REGISTER_FUNCTIONGRAPH(name) \
        static FunctionImpl* create##name(const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem, \
        Analitza::Variables *variables, bool isImplicit) { return new name (functionExpression, \
        coordinateSystem, variables, isImplicit); } \
        namespace { bool _##name=FunctionFactory::self()->registerFunctionGraph(create##name, \
        name::expressionType(), name ::argumentNames(), name ::coordinateSystem(), name ::isImplicit(), \
        name ::examples(), name ::iconName()); }



class FunctionImpl;

class ANALITZAPLOT_EXPORT FunctionFactory
{
	public:
		typedef FunctionImpl* (*registerFunctionGraph_fn)(const Analitza::Expression &, CoordinateSystem, Analitza::Variables *, bool);

		bool registerFunctionGraph(registerFunctionGraph_fn functionGraphConstructor,
								   const Analitza::ExpressionType &expressionType,
                                   const QStringList &argumentNames,
                                   CoordinateSystem coordinateSystem,
								   bool hasImplicitExpression, const QStringList& examples, const QString &iconName);

		FunctionImpl* create(int functionGraphIndex, const Analitza::Expression &functionExpression, CoordinateSystem coordinateSystem,
             Analitza::Variables *variables, bool isImplicit) const;

		static FunctionFactory* self();

		QVector< registerFunctionGraph_fn > constructors;
		QVector< Analitza::ExpressionType > expressionTypes;
		QVector< QStringList > argumentNames;
        QVector< CoordinateSystem > coordinateSystems;
		QVector< bool > implicitFlags;
        QVector< QStringList > examples;
        QVector< QString > iconNames;

	private:
		static FunctionFactory* m_self;
		FunctionFactory() { Q_ASSERT(!m_self); m_self=this; }
};

#endif // ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
