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


#ifndef KEOMATH_SOLVERFACTORY_H
#define KEOMATH_SOLVERFACTORY_H

#define REGISTER_FUNCTION_2D(name) \
        static FunctionImpl2D* create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=FunctionFactory::self()->registerFunction2D(name::supportedBVars(), create##name, name ::expectedType, name ::fDimension, name ::examples()); }

#define REGISTER_FUNCTION_3D(name) \
        static FunctionImpl3D* create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=FunctionFactory::self()->registerFunction3D(name::supportedBVars(), create##name, name ::expectedType, name ::fDimension, name ::examples()); }

#include <QMap>
#include <QStringList>
#include "analitzaplotexport.h"

namespace Analitza
{
class Expression;
class ExpressionType;
class Variables;
}

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
    static FunctionFactory* m_self;
    FunctionFactory()
    {
        Q_ASSERT(!m_self);
        m_self = this;
    }
    QMap<QString, registerFunc_fn_2d> m_items_2d;
    QMap<QString, registerFunc_fn_3d> m_items_3d;
    QMap<QString, expectedType_fn> m_types_2d;
    QMap<QString, expectedType_fn> m_types_3d;
    QMap<QString, dim_fn> m_dims;
    QStringList m_examples;
};

#endif
