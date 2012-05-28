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


#include "functionfactory.h"

#include "analitza/expressiontype.h"

using Analitza::Expression;
using Analitza::Variables;

FunctionFactory* FunctionFactory::m_self=0;

bool FunctionFactory::containsFor2D(const FunctionFactory::Id& bvars) const
{
    return m_items_2d.contains(bvars.join("|"));
}

bool FunctionFactory::containsFor3D(const FunctionFactory::Id& bvars) const
{
    return m_items_3d.contains(bvars.join("|"));
}


    Analitza::ExpressionType FunctionFactory::type(const Id& bvars)
    {
        return typeFor2D(bvars);
    }

FunctionImpl2D* FunctionFactory::item2D(const Id& bvars, const Expression& exp, Variables* v) const
{
    return m_items_2d[bvars.join("|")](exp, v);
}

FunctionImpl3D* FunctionFactory::item3D(const Id& bvars, const Expression& exp, Variables* v) const
{
    return m_items_3d[bvars.join("|")](exp, v);
}

Analitza::ExpressionType FunctionFactory::typeFor2D(const FunctionFactory::Id& bvars)
{
    return m_types_2d[bvars.join("|")]();
}

Analitza::ExpressionType FunctionFactory::typeFor3D(const FunctionFactory::Id& bvars)
{
    return m_types_3d[bvars.join("|")]();
}


bool FunctionFactory::registerFunction2D(const Id& bvars, registerFunc_fn_2d f, expectedType_fn ft, dim_fn fd,
                                       const QStringList& examples)
{
    m_items_2d[bvars.join("|")]=f;
    m_types_2d[bvars.join("|")]=ft;
    m_dims[bvars.join("|")]=fd;
    m_examples.append(examples);

    return true;
}

bool FunctionFactory::registerFunction3D(const Id& bvars, registerFunc_fn_3d f, expectedType_fn ft, dim_fn fd,
                                       const QStringList& examples)
{
    m_items_3d[bvars.join("|")]=f;
    m_types_3d[bvars.join("|")]=ft;
    m_dims[bvars.join("|")]=fd;
    m_examples.append(examples);

    return true;
}

QStringList FunctionFactory::examples() const
{
    return m_examples;
}

FunctionFactory* FunctionFactory::self()
{
    if(!m_self)
        m_self=new FunctionFactory;
    return m_self;
}