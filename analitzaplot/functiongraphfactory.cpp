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

#include "functiongraphfactory.h"

#include "analitza/expressiontype.h"

Function2DFactory* Function2DFactory::m_self=0;

Function2DFactory* Function2DFactory::self()
{
    if(!m_self)
        m_self=new Function2DFactory;
    return m_self;
}

bool Function2DFactory::registerFunction(registerFunc_fn f, expectedType_fn ft, arguments_fn argsf, 
                              coordinateSystem_fn coordsysf, iconName_fn iconf, examples_fn egf)
{
    Q_ASSERT(!contains(argsf()));
    QString id = argsf().join("|");
    m_items[id]=f;
    m_types[id]=ft;
    m_coordsys[id]=coordsysf;
    m_icons[id]=iconf;
    m_examples[id]=egf;

    return true;
}

bool Function2DFactory::contains(const Function2DFactory::Id& bvars) const
{
    return m_items.contains(bvars.join("|"));
}

FunctionImpl2D* Function2DFactory::item(const Id& bvars, const Analitza::Expression & exp, Analitza::Variables* v) const
{
    return m_items[bvars.join("|")](exp, v);
}

Analitza::ExpressionType Function2DFactory::type(const Function2DFactory::Id& bvars)
{
    return m_types[bvars.join("|")]();
}

CoordinateSystem Function2DFactory::coordinateSystem(const Id& id) const
{
    return m_coordsys[id.join("|")]();    
}

QString Function2DFactory::iconName(const Id& id) const
{
    return m_icons[id.join("|")]();    
}

QStringList Function2DFactory::examples(const Id& id) const
{
    return m_examples[id.join("|")]();    
}


