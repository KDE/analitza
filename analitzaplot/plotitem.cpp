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
    , m_model(0)
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

///

Shape::Shape()
{

}

Shape::Shape(const Shape& other)
{

}

Shape::Shape(const Expression& expresssion, Variables* vars)
{

}

Shape::Shape(const QString& expresssion, Variables* vars)
{

}

//BEGIN AbstractShape interface
void Shape::clearTags()
{
	d->m_tags.clear();;
}

void Shape::addTags(const QSet<QString> &tags)
{
	d->m_tags = tags;
}

QColor Shape::color() const
{
	return d->m_color;
}

CoordinateSystem Shape::coordinateSystem() const
{
	return d->m_coordinateSystem;
}

QStringList Shape::errors() const
{
	return QStringList();
}

Expression Shape::expression() const
{
	return d->m_expression;
}

QString Shape::id() const
{
	return d->m_id;
}

QString Shape::iconName() const
{
	return d->m_iconName;
}

bool Shape::isValid() const
{
	return false;
}

bool Shape::isVisible() const
{
	return d->m_visible;
}

QString Shape::name() const
{
	return d->m_name;
}

void Shape::setColor(const QColor &color)
{
	d->m_color = color;
}

void Shape::setExpression(const Expression &expression)
{
	d->m_expression = expression; //TODO
}

void Shape::setName(const QString &name)
{
	d->m_name = name;
}

void Shape::setVisible(bool visible)
{
	d->m_visible = visible;
}

Dimension Shape::spaceDimension() const
{
	return d->m_spaceDimension;
}

QSet<QString> Shape::tags() const
{
	return d->m_tags;
}

QString Shape::typeName() const
{
	return d->m_typeName;
}

Variables * Shape::variables() const
{
	return d->m_analyzer->variables();
}


bool Shape::operator==(const Shape &other) const
{
	return false; // TODO
}

bool Shape::operator!=(const Shape &other) const
{
	return !((*this) == other);
}

Shape & Shape::operator=(const Shape &other)
{
	return *this; //TODO
}

//END AbstractShape interface

QHash< QString, Shape::TestBuilderMethod > Shape::m_shapeTypes = QHash< QString, Shape::TestBuilderMethod >();

