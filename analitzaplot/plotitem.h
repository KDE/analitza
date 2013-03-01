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
#include <QSet>
#include <QColor>

#include "analitzaplotexport.h"
#include "plottingenums.h"
#include <analitza/analyzer.h>

namespace Analitza {
class Variables;
class Expression;
class PlotsModel;

/**
 * \class PlotItem
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Represents a visual item.
 *
 * This class containes common properties of any plot object.
 */

class ANALITZAPLOT_EXPORT PlotItem 
{
friend class PlotsModel;
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

    void clearTags();
    void addTags(const QSet<QString>& tag);
	QSet<QString> tags() const;

protected:
    void setModel(PlotsModel *m);
    void emitDataChanged();

private:
    QString m_name;
    QColor m_color;
    bool m_graphVisible;
    QSet<QString> m_tags;
    
    PlotsModel *m_model;
};

///

//TODO move to _p header
class ShapeData : public QSharedData// this class have to be base class of all shareddatas
{
public:
	
	
	QScopedPointer<Analyzer> m_analyzer;
	bool m_builtinMethod;
	QColor m_color;
	CoordinateSystem m_coordinateSystem;
	Expression m_expression;
	QString m_id;
	QString m_iconName;
	QString m_name;
	int m_size;
	Dimension m_spaceDimension;
	QSet<QString> m_tags;
	QString m_typeName;
	bool m_visible;
};

//concrete classes implment explicit memory sharin semantic (and lighweightpatterhn)
template <typename ShapeType>
class ANALITZAPLOT_EXPORT AbstractShape
{
public:
	virtual void clearTags() = 0;
	virtual void addTags(const QSet<QString> &tags) = 0;
	virtual QColor color() const = 0;
	virtual CoordinateSystem coordinateSystem() const = 0;
	virtual QStringList errors() const = 0;
	virtual Expression expression() const = 0;
	virtual QString id() const = 0; //read only attr always shape needs to be unique aboject in canvas and analitza context
	virtual QString iconName() const = 0;
// 	virtual bool isBuiltinMethod() const = 0; // if false is a simple expression
	virtual bool isValid() const = 0; // see if expression match its definition
	virtual bool isVisible() const = 0;
	virtual QString name() const = 0;
	virtual void setColor(const QColor &color) = 0;
	virtual void setExpression(const Expression &expression) = 0; // use definition to valid
	virtual void setName(const QString &name) = 0;
	virtual void setVisible(bool visible) = 0;
	virtual Dimension spaceDimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety dimension
	virtual QSet<QString> tags() const = 0;
	virtual QString typeName() const = 0;
	virtual Variables *variables() const = 0;
	
	virtual bool operator==(const ShapeType &other) const = 0;
	virtual bool operator!=(const ShapeType &other) const = 0;
	virtual ShapeType & operator=(const ShapeType &other) = 0;
	
	//BEGIN static interface
	// static QStringList builtinMethods();
	// static bool canBuild(const Expression &expression, Analitza::Variables* vars = 0);
	//END static interface
};

// for KAlgebra use case, this class will implements toShapeTypeMethod ... sort of a GenericShape class /method factory
// this a class withouth gometric data 
// this class store only relevant data for containers
class ANALITZAPLOT_EXPORT Shape : public AbstractShape<Shape>
{
public:
	Shape();
	Shape(const Shape &other);
	Shape(const Analitza::Expression &expresssion, Variables* vars = 0);// { if is builtin else is eq, expr, lambda,etc }
	Shape(const QString &expresssion, Variables* vars = 0);

	//BEGIN AbstractShape interface
	void clearTags();
	void addTags(const QSet<QString> &tags);
	QColor color() const;
	CoordinateSystem coordinateSystem() const;
	QStringList errors() const;
	Expression expression() const;
	QString id() const;
	QMap<QString, QString> information() const;
	QString iconName() const;
	bool isValid() const;
	bool isVisible() const;
	QString name() const;
	void setColor(const QColor &color);
	void setExpression(const Expression &expression);
	void setName(const QString &name);
	void setVisible(bool visible);
	Dimension spaceDimension() const;
	QSet<QString> tags() const;
	QString typeName() const;
	Variables *variables() const;
	
	bool operator==(const Shape &other) const;
	bool operator!=(const Shape &other) const;
	Shape & operator=(const Shape &other);
	//END AbstractShape interface
	
	template <typename ShapeType>
	static void registerShapeType(const QString &className) // we don't use typeinfo because is a library here
	{
			Q_ASSERT(!m_shapeTypes.contains(className));
			
			m_shapeTypes.insert(className, ShapeType::canBuild);
	}
	
private:
	typedef bool (*TestBuilderMethod)(const Analitza::Expression&, Analitza::Variables*);
	
	static QHash< QString, TestBuilderMethod > m_shapeTypes;
	
	QExplicitlySharedDataPointer<ShapeData> d;
};



}

#endif // ANALITZAPLOT_PLOTITEM_H
