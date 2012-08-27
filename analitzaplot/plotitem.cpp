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


#include "plotitem.h"

#include "plotsmodel.h"

PlotItem::PlotItem(const QString &n, const QColor& col)
    : m_name(n)
    ,m_color(col),m_space(0),
    m_graphVisible(true), m_model(0), m_inDestructorSoDontDeleteMe(false)
{
}

PlotItem::~PlotItem()
{
    if (m_model && m_model->m_itemCanCallModelRemoveItem)
    {
        m_inDestructorSoDontDeleteMe = true;
        m_model->removePlot(m_model->m_items.indexOf(this));
        m_inDestructorSoDontDeleteMe = false;
    }
}

// VisualItemsModel* VisualItem::model() const
// {
//     return m_model;
// }

void PlotItem::emitDataChanged()
{
    if (m_model)
    {
        int row = m_model->m_items.indexOf(this);
        m_model->dataChanged(m_model->index(row), m_model->index(row));
    }
}


void PlotItem::setModel(PlotsModel* m)
{
    Q_ASSERT(m);
    Q_ASSERT(m != m_model);
    
    m_model = m;
}

void PlotItem::setColor(const QColor& newColor)
{ 
    m_color = newColor; 
   emitDataChanged(); 
}






