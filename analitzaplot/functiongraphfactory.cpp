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

CurveFactory* CurveFactory::m_self=0;

CurveFactory* CurveFactory::self()
{
    if(!m_self)
        m_self=new CurveFactory;
    return m_self;
}

bool CurveFactory::registerFunction(registerFunc_fn f, expectedType_fn ft, arguments_fn argsf, 
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

bool CurveFactory::contains(const CurveFactory::Id& bvars) const
{
    return m_items.contains(bvars.join("|"));
}

AbstractCurve* CurveFactory::item(const Id& bvars, const Analitza::Expression & exp, Analitza::Variables* v) const
{
    return m_items[bvars.join("|")](exp, v);
}

Analitza::ExpressionType CurveFactory::type(const CurveFactory::Id& bvars)
{
    return m_types[bvars.join("|")]();
}

CoordinateSystem CurveFactory::coordinateSystem(const Id& id) const
{
    return m_coordsys[id.join("|")]();    
}

QString CurveFactory::iconName(const Id& id) const
{
    return m_icons[id.join("|")]();    
}

QStringList CurveFactory::examples(const Id& id) const
{
    return m_examples[id.join("|")]();    
}


