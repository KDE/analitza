// /*************************************************************************************
//  *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
//  *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
//  *                                                                                   *
//  *  This program is free software; you can redistribute it and/or                    *
//  *  modify it under the terms of the GNU General Public License                      *
//  *  as published by the Free Software Foundation; either version 2                   *
//  *  of the License, or (at your option) any later version.                           *
//  *                                                                                   *
//  *  This program is distributed in the hope that it will be useful,                  *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
//  *  GNU General Public License for more details.                                     *
//  *                                                                                   *
//  *  You should have received a copy of the GNU General Public License                *
//  *  along with this program; if not, write to the Free Software                      *
//  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
//  *************************************************************************************/
// 
// 
#ifndef ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
#define ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H

#include "abstractfunctiongraphfactory.h"

// 
// #include <QVector>
// #include <QString>
// #include <QMap>
// #include <QStringList>
// #include "analitza/analyzer.h"
// 
// #include "mathutils.h"
// #include "abstractfunctiongraphfactory.h"
// extern Analitza::Analyzer a;
// namespace Analitza
// {
// class Expression;
// class ExpressionType;
// class Variables;
// }
// 
// 
// #define REGISTER_PLANECURVE(name) \
//         static AbstractPlaneCurve * vcreate##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
//         static AbstractPlaneCurve * create##name(const Analitza::Expression &exp) { return new name (exp); } \
//         namespace { bool _##name=PlaneCurveFactory::self()->registerPlaneCurve(vcreate##name, create##name, \
//         name ::TypeName, name ::ExpressionType, name ::CoordSystem, name ::Parameters, \
//         name ::IconName, name ::Examples); }
// 
// class AbstractPlaneCurve;
// 
// 
// 
// 
// // plugins must expose this static members
// 
// // QString TypeName
// // Analitza::ExpressionType ExpressionType
// // int SpaceDimension
// // CoordinateSystem CoordinateSystem
// // QStringList Arguments
// // QString IconName
// // QStringList Examples
// // bool IsImplicit()
// // bool IsParametric()
// 
// 
// 
// ///
// 
// class ANALITZAPLOT_EXPORT PlaneCurveFactory : public AbstractFunctionGraphFactory
// {
// public:
//     typedef AbstractPlaneCurve* (*BuilderFunctionWithVars)(const Analitza::Expression&, Analitza::Variables* );
//     typedef AbstractPlaneCurve* (*BuilderFunctionWithoutVars)(const Analitza::Expression&);
// 
// 
//     static PlaneCurveFactory* self();
// 
//     bool registerPlaneCurve(BuilderFunctionWithVars builderFunctionWithVars, BuilderFunctionWithoutVars builderFunctionWithoutVars, TypeNameFunction typeNameFunction,
//                          ExpressionTypeFunction expressionTypeFunction, 
//                          CoordinateSystemFunction coordinateSystemFunction, ArgumentsFunction argumentsFunction,
//                          IconNameFunction iconNameFunction, ExamplesFunction examplesFunction);
//     QString id(const QStringList& args) const;
//     bool contains(const QString &id) const;
//     AbstractPlaneCurve * build(const QString& id, const Analitza::Expression& exp, Analitza::Variables* v) const;
//     AbstractPlaneCurve * build(const QString& id, const Analitza::Expression& exp) const;
// 
// private:
//     static PlaneCurveFactory* m_self;
//     PlaneCurveFactory() {
//         Q_ASSERT(!m_self);
//         m_self = this;
//     }
// 
//     QMap<QString, BuilderFunctionWithVars> builderFunctionsWithVars;
//     QMap<QString, BuilderFunctionWithoutVars> builderFunctionsWithoutVars;
// };
// 
// 
// 
#endif // ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
