/*************************************************************************************
*  Copyright (C) 2012 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef PLOTSFACTORY_H
#define PLOTSFACTORY_H

#include "plottingenums.h"
#include "analitzaplotexport.h"
#include <analitza/expression.h>

class QColor;

namespace Analitza {
class Variables;
class FunctionGraph;
class PlotsModel;
class PlotItem;

/**
 * \class PlotBuilder
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Build a plot and exposes its most importat attributes.
 *
 * This class is a builder that is create by PlotsFactory.
 */

class ANALITZAPLOT_EXPORT PlotBuilder
{
    friend class PlotsFactory;
    public:
        QStringList errors() const { return m_errors; }
        bool canDraw() const;
        FunctionGraph* create(const QColor& color, const QString& name) const;
        Analitza::Expression expression() const;
        QString display() const;
        Variables *vars() const;

    protected:
        PlotBuilder() {}
        
        QString m_id;
        QStringList m_errors;
        Analitza::Expression m_expression;
        QString m_display;
        Variables *m_vars;
};

/**
 * \class PlotsFactory
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Allows create plot builders.
 *
 * This is a singleton class that can create PlotBuilder and exposes some 
 * expressions examples of many type of plots (curves, surfaces,etc.).
 *
 * \author Aleix Pol <aleixpol@kde.org>
 */

class ANALITZAPLOT_EXPORT PlotsFactory
{
    public:
        PlotsFactory();

        static PlotsFactory* self();
		
		/** @p vars is necesary because @p expression can contains variables of any Variables module. */
        PlotBuilder requestPlot(const Analitza::Expression& expresssion, Dimension dim, Variables *vars = 0) const;
        QStringList examples(Dimensions s) const;
};

}

#endif // PLOTSFACTORY_H
