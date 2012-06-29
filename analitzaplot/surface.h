/*************************************************************************************
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

#ifndef SURFACE_H
#define SURFACE_H

#include "private/functiongraph.h"

class AbstractSurface;

class ANALITZAPLOT_EXPORT Surface : public FunctionGraph 
{
public:
    Surface(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    Surface(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~Surface();

    static bool canDraw(const Analitza::Expression &functionExpression);
    //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
    static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);
    bool reset(const Analitza::Expression &functionExpression);
    void setVariables(Analitza::Variables *variables);
    
    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);

    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);
    
    QStringList parameters() const;

    //Own
    const QVector<int> & indexes() const;
    const QVector<QVector3D> & points() const;
    void update(const Box &viewport);

protected:
    Surface() {}
    Surface(const Surface &other) {}

private:
    Analitza::Variables *m_varsModule;
    AbstractSurface *m_surface;

    QStringList m_errors;
};


#endif // SURFACE_H
