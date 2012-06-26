
/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef _ABSTRACTFUNCTIONGRAPH_H
#define _ABSTRACTFUNCTIONGRAPH_H

#include "abstractmappinggraph.h"

#define TYPE_NAME(name) \
const QString typeName() const { return TypeName(); } \
static QString TypeName() { return QString(name); } 

#define EXPRESSION_TYPE(name) \
static Analitza::ExpressionType ExpressionType() { return Analitza::ExpressionType(name); }

#define COORDDINATE_SYSTEM(name) \
CoordinateSystem coordinateSystem() const { return CoordSystem(); } \
static CoordinateSystem CoordSystem() { return name; }

//TODO validaciones strim etc 
#define PARAMETERS(name) \
QStringList parameters() const { return Parameters(); } \
static QStringList Parameters() { return QString(name).split(","); }

#define ICON_NAME(name) \
QString iconName() const { return IconName(); } \
static QString IconName() { return QString(name); } 

#define EXAMPLES(name) \
QStringList examples() const { return Examples(); } \
static QStringList Examples() { return QString(name).split(","); }


class ANALITZAPLOT_EXPORT AbstractFunctionGraph : public AbstractMappingGraph
{
public:
    explicit AbstractFunctionGraph(const Analitza::Expression& e, Analitza::Variables* v);
    virtual ~AbstractFunctionGraph();

    //FunctionGraph
    //no lleva const porque se calcularan valores con m_argumentIntervals
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    void setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    
    QPair<double, double> interval(const QString &argname) const;
    void setInterval(const QString &argname, double min, double max);
    
    virtual QStringList parameters() const = 0;

protected:
    AbstractFunctionGraph() {}
    AbstractFunctionGraph(const AbstractFunctionGraph& other);
    
    Analitza::Cn* arg(const QString &argname) { return dynamic_cast<Analitza::Cn*>(m_argumentValues[argname]); }

private:
    QMap<QString, Analitza::Object*> m_argumentValues;
    QMap<QString, RealInterval > m_argumentIntervals;
};


#endif // ABSTRACTFUNCTIONGRAPH_H
