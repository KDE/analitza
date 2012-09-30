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

#ifndef ANALITZAPLOT_PLOTITEM_H
#define ANALITZAPLOT_PLOTITEM_H

#include <QStringList>
#include <QColor>

#include "analitzaplotexport.h"
#include "plottingenums.h"

namespace Analitza {
class Variables;
class Expression;
class PlotsModel;
class DictionaryItem;

class ANALITZAPLOT_EXPORT PlotItem 
{
public:
    PlotItem(const QString &name, const QColor& col);
    virtual ~PlotItem();

    virtual const Analitza::Expression & expression() const = 0;
    virtual QString display() const = 0;
    virtual Analitza::Variables * variables() const = 0;
    virtual QString typeName() const = 0;
    virtual QString iconName() const = 0;
    virtual Dimension spaceDimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety dimension
    virtual CoordinateSystem coordinateSystem() const = 0;
    
    QString name() const { return m_name; }
    void setName(const QString &newName);
    QColor color() const { return m_color; }
    void setColor(const QColor& newColor);

    bool isVisible() const { return m_graphVisible; }
    void setVisible(bool v);
    
    void setModel(PlotsModel *m);

protected:
    void emitDataChanged();

private:
    QString m_name;
    QColor m_color;
    bool m_graphVisible;
    
    PlotsModel *m_model;
};

}

#endif // ANALITZAPLOT_PLOTITEM_H
