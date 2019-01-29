
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

using namespace Analitza;

PlotItem::PlotItem(const QString &n, const QColor& col)
    : m_name(n)
    , m_color(col)
    , m_graphVisible(true)
    , m_model(nullptr)
{}

PlotItem::~PlotItem()
{}

void PlotItem::emitDataChanged()
{
    if (m_model) {
        m_model->emitChanged(this);
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

void PlotItem::setName(const QString& newName)
{
    m_name = newName;
    emitDataChanged();
}

void PlotItem::setVisible(bool v)
{
    m_graphVisible = v;
    emitDataChanged();
}

void PlotItem::addTags(const QSet<QString>& tag)
{
    m_tags += tag;
}

void PlotItem::clearTags()
{
    m_tags.clear();
}

QSet<QString> PlotItem::tags() const
{
    return m_tags;
}
