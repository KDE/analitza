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


#ifndef KEOMATH_SOLVERFACTORY_H
#define KEOMATH_SOLVERFACTORY_H

#include <QVector>
#include <QString>
#include <QMap>
#include <QStringList>
#include "analitza/analyzer.h"

#include "functionutils.h"
#include "analitzaplotexport.h"
extern Analitza::Analyzer a;
namespace Analitza
{
class Expression;
class ExpressionType;
class Variables;
}
namespace Keomath
{


#define REGISTER_FUNCTIONGRAPH(name) \
        static FunctionGraph* create##name() { return new name (); } \
        namespace { bool _##name=FunctionGraphFactory::self()->registerFunctionGraph(create##name, \
        name::expressionType(), name ::dimension(), name ::coordinateSystem(), \
        name ::hasImplicitExpression(), name ::examples()); }



class FunctionGraph;

class ANALITZAPLOT_EXPORT FunctionGraphFactory
{
	public:
		typedef FunctionGraph* (*registerFunctionGraph_fn)();

		bool registerFunctionGraph(registerFunctionGraph_fn functionGraphConstructor,
								   const Analitza::ExpressionType &expressionType,
								   FunctionGraphDimension functionGraphDimension,
								   FunctionGraphCoordinateSystem functionGraphCoordinateSystem,
								   bool hasImplicitExpression, const QStringList& examples);

		FunctionGraph* create(int functionGraphIndex) const;

		static FunctionGraphFactory* self();

		QVector< registerFunctionGraph_fn > functionGraphConstructors;
		QVector< Analitza::ExpressionType > functionGraphExpressionTypes;
		QVector< FunctionGraphDimension > functionGraphDimensions;
		QVector< FunctionGraphCoordinateSystem > functionGraphCoordinateSystems;
		QVector< bool > functionGraphImplicitExpressionFlags;
		QVector< QStringList > functionGraphExamples;

	private:
		static FunctionGraphFactory* m_self;
		FunctionGraphFactory() { Q_ASSERT(!m_self); m_self=this; }
};

}


#define REGISTER_FUNCTION_2D(name) \
        static FunctionImpl2D* create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=FunctionFactory::self()->registerFunction2D(name::supportedBVars(), create##name, name ::expectedType, name ::fDimension, name ::examples()); }

#define REGISTER_FUNCTION_3D(name) \
        static FunctionImpl3D* create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=FunctionFactory::self()->registerFunction3D(name::supportedBVars(), create##name, name ::expectedType, name ::fDimension, name ::examples()); }

#include <QMap>
#include <QStringList>



class FunctionImpl2D;
class FunctionImpl3D;

class ANALITZAPLOT_EXPORT FunctionFactory
{
public:
    typedef FunctionImpl2D* (*registerFunc_fn_2d)(const Analitza::Expression&, Analitza::Variables* );
    typedef FunctionImpl3D* (*registerFunc_fn_3d)(const Analitza::Expression&, Analitza::Variables* );

    typedef Analitza::ExpressionType (*expectedType_fn)();
    typedef int (*dim_fn)();

    typedef QStringList Id;

    FunctionImpl2D* item2D(const Id& bvars, const Analitza::Expression& exp, Analitza::Variables* v) const;
    FunctionImpl3D* item3D(const Id& bvars, const Analitza::Expression& exp, Analitza::Variables* v) const;

    Analitza::ExpressionType typeFor2D(const Id& bvars);
    Analitza::ExpressionType typeFor3D(const Id& bvars);

    static FunctionFactory* self();

    bool registerFunction2D(const Id& bvars, registerFunc_fn_2d f, expectedType_fn ft, dim_fn fd,
                            const QStringList& examples);

    bool registerFunction3D(const Id& bvars, registerFunc_fn_3d f, expectedType_fn ft, dim_fn fd,
                            const QStringList& examples);

    bool containsFor2D(const Id& bvars) const;
    bool containsFor3D(const Id& bvars) const;

    //(backward comp)
    bool contains(const Id& bvars) const
    {
        return containsFor2D(bvars);
    }

    //(backward comp)
    Analitza::ExpressionType type(const Id& bvars);




    QStringList examples() const;
private:
    static FunctionFactory* mm_self;
    FunctionFactory()
    {
        Q_ASSERT(!mm_self);
        mm_self = this;
    }
    QMap<QString, registerFunc_fn_2d> m_items_2d;
    QMap<QString, registerFunc_fn_3d> m_items_3d;
    QMap<QString, expectedType_fn> m_types_2d;
    QMap<QString, expectedType_fn> m_types_3d;
    QMap<QString, dim_fn> m_dims;
    QStringList m_examples;
};



// c++only
// namespace Keomath
// {
//
// class FunctionGraph;
//
// template<typename FunctionGraphType>
// struct FunctionGraphTypeConstructor
// {
// 	FunctionGraph * operator() ()
// 	{
// 		return new FunctionGraphType();
// 	}
// };
//
// struct FunctionGraphTypeConstructorProxy
// {
// 	template<typename FunctionGraphType>
// 	FunctionGraph * operator() ()
// 	{
// 		return new FunctionGraphType();
// 	}
// };
//
// struct FunctionGraphFactory
// {
//     FunctionGraph * create(const Analitza::Expression& functionExpression,
// 						   FunctionGraphDimension functionGraphDimension,
// 						   FunctionGraphCoordinateSystem functionGraphCoordinateSystem,
// 						   bool hasImplicitExpression, Analitza::Variables *variables)
// 	{
// 		FunctionGraph *ret = 0;
//
// 		Analitza::Analyzer analyzer(variables);
// 		analyzer.setExpression(functionExpression);
// 		analyzer.setExpression(analyzer.dependenciesToLambda());
//
// 		for (int i = 0; i < functionGraphTypeConstructors.size(); ++i)
// 		{
// 			if ((analyzer.type().canReduceTo(functionGraphExpressionTypes[i])) &&
// 				(functionGraphDimension == functionGraphDimensions[i]) &&
// 				(functionGraphCoordinateSystem == functionGraphCoordinateSystems[i]) &&
// 				(hasImplicitExpression == functionGraphImplicitExpressionFlags[i]))
// 				ret = functionGraphTypeConstructors[i].constructNewPointerInstance();
// 		}
//
// 		return ret;
// 	}
//
// 	template<typename FunctionGraphType>
// 	void registerFunctionGraphType()
// 	{
// 		FunctionGraphTypeConstructorProxy proxy;
// 		functionGraphTypeConstructors.append(proxy.operator()<FunctionGraphType>());
// 	}
//
// 	static QVector< FunctionGraphTypeConstructorProxy > functionGraphTypeConstructors;
// 	static QVector< Analitza::ExpressionType > functionGraphExpressionTypes;
// 	static QVector< FunctionGraphDimension > functionGraphDimensions;
// 	static QVector< FunctionGraphCoordinateSystem > functionGraphCoordinateSystems;
// 	static QVector< bool > functionGraphImplicitExpressionFlags;
// 	static QVector< QStringList > functionGraphExamples;
// };
//
// template<typename FunctionGraphType >
// struct RegisterFunctionGraph
// {
// 	RegisterFunctionGraph()
// 	{
// 		FunctionGraphFactory::registerFunctionGraphType<FunctionGraphType>();
// 		FunctionGraphFactory::functionGraphExpressionTypes.append(FunctionGraphType::ExpressionType());
// 		FunctionGraphFactory::functionGraphDimensions.append(FunctionGraphType::Dimension());
// 		FunctionGraphFactory::functionGraphCoordinateSystems.append(FunctionGraphType::CoordinateSystem());
// 		FunctionGraphFactory::functionGraphImplicitExpressionFlags.append(FunctionGraphType::HasImplicitExpression());
// 		FunctionGraphFactory::functionGraphExamples.append(FunctionGraphType::Examples());
// 	}
// };
//
// #define REGISTER_FUNCTIONGRAPH(FunctionGraphTypeName) \
// RegisterFunctionGraph < ##FunctionGraphTypeName > _##FunctionGraphTypeName_gins;
//
// };
//

#endif
