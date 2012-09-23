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

#ifndef FUNCTIONSPAINTER3D_H
#define FUNCTIONSPAINTER3D_H

#include "plotitem.h"
#include <QModelIndex>

class QAbstractItemModel;
class QPainter;
class QPaintDevice;

class QModelIndex;

class ANALITZAPLOT_EXPORT Plotter3D
{
    public:
        Plotter3D(const QSizeF& size, QAbstractItemModel* model = 0);
        virtual ~Plotter3D();
        
        virtual void drawPlots();
        virtual int currentPlot() const = 0;
        virtual void modelChanged() = 0;

        /** Force the functions from @p start to @p end to be recalculated. */
        void updatePlots(const QModelIndex & parent, int start, int end);
        
        PlotItem* currentPlotItem() const;

        void setModel(QAbstractItemModel* f);
        QAbstractItemModel* model() const { return m_model; }
        
    private:
        PlotItem *itemAt(int row) const;

        QAbstractItemModel* m_model;
        
        QMap<PlotItem*, unsigned int> m_displayLists;
};

#endif // FUNCTIONSPAINTER3D_H
