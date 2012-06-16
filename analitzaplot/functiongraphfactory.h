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

#define REGISTER_FUNCTION2D(name) \
        static AbstractCurve* create##name(const Analitza::Expression &exp, Analitza::Variables* v) { return new name (exp, v); } \
        namespace { bool _##name=CurveFactory::self()->registerFunction(create##name, \
        name::expressionType, name ::arguments, name ::coordinateSystem, name ::iconName, name ::examples); }

class AbstractCurve;

class ANALITZAPLOT_EXPORT CurveFactory
{
    public:
        typedef AbstractCurve* (*registerFunc_fn)(const Analitza::Expression&, Analitza::Variables* );
        typedef Analitza::ExpressionType (*expectedType_fn)();
        typedef QStringList (*arguments_fn)();
        typedef CoordinateSystem (*coordinateSystem_fn)();
        typedef QString (*iconName_fn)();
        typedef QStringList (*examples_fn)();
        
        typedef QStringList Id;
        static CurveFactory* self();
        bool registerFunction(registerFunc_fn f, expectedType_fn ft, arguments_fn argsf, 
                              coordinateSystem_fn coordsysf, iconName_fn iconf, examples_fn egf);
        bool contains(const Id& id) const;

        AbstractCurve* item(const Id& id, const Analitza::Expression& exp, Analitza::Variables* v) const;
        Analitza::ExpressionType type(const Id& id);
        CoordinateSystem coordinateSystem(const Id& id) const;
        QString iconName(const Id& id) const;
        QStringList examples(const Id& id) const;

    private:
        static CurveFactory* m_self;
        CurveFactory() { Q_ASSERT(!m_self); m_self=this; }
        QMap<QString, registerFunc_fn> m_items;
        QMap<QString, expectedType_fn> m_types;
        QMap<QString, coordinateSystem_fn> m_coordsys;
        QMap<QString, examples_fn> m_examples;
        QMap<QString, iconName_fn> m_icons;
};

#endif // ANALITZAPLOT_FUNCTIONGRAPHFACTORY_H
