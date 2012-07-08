/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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


#ifndef FUNCTIONGRAPHMODEL_H
#define FUNCTIONGRAPHMODEL_H


#include <QAbstractListModel>

#include "analitzaplot/analitzaplotexport.h"
#include <analitzaplot/mathutils.h>


class FunctionGraph;

namespace Analitza
{
class Variables;
class Expression;
}

//TODO NOTE 
// crear una carpeta llamda models?

// class MappingModel;
// hijo FunctionGraphsModel 
// hijos de conveniencias
// - 2dspacilmodel ... hereda de gapmodel add curve add inquealty add geomtry eytc
// - planecurve model ... hereda de fun
// - surface model idem
// - space curve model idem
// - curve ... hereda de fun agece cur2d y 3d
// - geomtri etc ... autoLOS MODELOS POR CONVENIENCIA



class ANALITZAPLOT_EXPORT FunctionGraphsModel : public QAbstractListModel
{
Q_OBJECT
    
public:
    FunctionGraphsModel(QObject * parent = 0);
    FunctionGraphsModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~FunctionGraphsModel();
    
    void setVariables(Analitza::Variables *v); // set variables for all this items this not emit setdata signal
    
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    void removeItem(int curveIndex);

    //planecurve getters ... 
    
//     y si elimino el const?
//     misma iface de 
//     
// QGraphicsLineItem * 
// addLine ( const QLineF & line, const QPen & pen = QPen() )
// QGraphicsLineItem * 
// addLine ( qreal x1, qreal y1, qreal x2, qreal y2, const QPen & pen = QPen() )
// QGraphicsPathItem * 
// addPath ( const QPainterPath & path, const QPen & pen = QPen(), const QBrush & brush = QBrush() )
// 
// 
// en el model no se deberia dar el "son semanticas distintas"
// 
// haer a fungrap pure virtual al menos un metodo

    const FunctionGraph * item(int curveIndex) const; //read only pointer the data CAN NOT be changed (is a good thing :) )... use this instead of roles ... razon: el uso de roles hace que el cliente deba hacer casts largos

    //planecurve setters and calculation/evaluation methods  .. don't forget to emit setdata signal' ... ninguno de estos metodos tiene cont al final

    void setItemName(int curveIndex, const QString &p);
    void setItemColor(int curveIndex, const QColor &p);
    void setItemVisible(int curveIndex, bool f);
    void setItemPlotStyle(int curveIndex, PlotStyle ps);


    void setItemParameterInterval(int curveIndex, const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    void setItemParameterInterval(int curveIndex, const QString &argname, double min, double max);

    //setItem por conv
    //TODO poner en hijos, no aqui 
//     bool setItem(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col);

    void updateItem(int curveIndex, const Box &viewport);

protected:    
//     bool addItem(const Analitza::Expression &functionExpression,int spaceDimension, const QString &name, const QColor& col);
//     bool addItem(const Analitza::Expression &functionExpression,int spaceDimension, const QString &name, const QColor& col, QStringList &errors);
//     bool setItemExpression(int curveIndex, const Analitza::Expression &functionExpression,int spaceDimension);

    QList<FunctionGraph*> items;
    
private:
    Analitza::Variables *m_variables;
};

#endif // FUNCTIONGRAPHMODEL_H
