/*************************************************************************************
 *  Copyright (C) 2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef ANALITZAPLOT_PLOTITEM_H_DATA
#define ANALITZAPLOT_PLOTITEM_H_DATA

#include "analitzaplotexport.h"
#include "plottingenums.h"

class QStringList;
class QColor;
class QVector3D;

namespace Analitza {
class Variables;
class Expression;

/**
 * \interface ShapeInterface
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Commmon contract/interface for all shape types.
 *
 * Concrete classes have to implement implicit memory sharing by using 
 * QSharedDataPointer.
 */

template <typename ShapeType>
class ANALITZAPLOT_EXPORT ShapeInterface
{
    
public:
    virtual QColor color() const = 0;
    virtual CoordinateSystem coordinateSystem() const = 0;
    virtual Dimension dimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety/top dimension
    virtual QStringList errors() const = 0;
    virtual Expression expression() const = 0;
    virtual QString iconName() const = 0;
    virtual bool isValid() const = 0; // see if expression match its definition
    virtual bool isVisible() const = 0;
    virtual QString name() const = 0;
    virtual void setColor(const QColor &color) = 0;
    virtual void setName(const QString &name) = 0;
    virtual void setVisible(bool visible) = 0;
    virtual QString typeName() const = 0;
    //PLotter can need do the task in a thread so use  QtConcurrent::run<double>(&ShapeType::plot);
    virtual void plot(/*const QGLContext * context = 0*/) = 0; // draw in openglcontext Update the surfaces's data @p oppositecorner1 and @p oppositecorner2 form an axis-aligned bounding box.
    virtual Variables *variables() const = 0;
    
//     virtual bool operator==(const ShapeType &other) const = 0;
//     virtual bool operator!=(const ShapeType &other) const = 0;
//     virtual ShapeType & operator=(const ShapeType &other) = 0;
    
protected:
//     virtual void geometrize() = 0; // constructs the geometry of shape with QtConcurrent::run then we use the data in plotmethod
};

}

#endif // ANALITZAPLOT_PLOTITEM_H
