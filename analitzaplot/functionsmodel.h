/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef ANALITZAPLOT_FUNCTIONSMODEL_H
#define ANALITZAPLOT_FUNCTIONSMODEL_H

#include <QAbstractTableModel>
#include <QStandardItem>
#include <QAbstractListModel>


#include "function.h"






class ANALITZAPLOT_EXPORT MappingGraphModel : public QAbstractListModel
{
Q_OBJECT
    
public:
    MappingGraphModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~MappingGraphModel();
    
    //QAbstractItemModel
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;

    //own
    
    // append gen n valid functions randomly usa insertrow /// insertrow las genera pero invalidadas
    virtual bool magic(int n) = 0; 

protected:
    Analitza::Variables *variablesModule;
};

class ANALITZAPLOT_EXPORT PlaneCurveModel : public MappingGraphModel
{
Q_OBJECT

public:
    enum PlaneCurveDataRole 
    {
        //roles that will show in a view, also works for editing job except iconname
        ExpressionRole = Qt::UserRole, //Variant->QString
        NameRole, //Variant->QString
        ColorRole, //Variant->QColor
        IconNameRole, //Variant->QString

        //roles for editing job
        ArgumentsRole, //Variant->QList<QVariant> ... List: ... QString(argname), double min, double max ...
        DrawingPrecisionRole, //Variant->int
        VisibleRole, //Variant->bool

        //roles for deliver extra data: examples, etc ... read-only roles / except plotstyle
        ExamplesRoles, //Variant->QStringList
        SpaceDimensionRole, //Variant->int
        CoordinateSystemRole, //Variant->int
        PlotStyleRole, //Variant->int
        IsCorrectRole, //Variant->bool
        ErrorsRole, //Variant->QStringList
        ArcLengthRole, //Variant->double
        IsClosedRole,  //Variant->double
        AreaRole, //Variant->double

                //methodroles
        UpdateRole, //variant->qrect
//         JumpsRole,  //Variant->QList<QVariant> ... list of ints ... use const T * item const instead
//         PointsRole, //Variant->QList<QVariant> ... list of doubles ... use const T * item const instead
        IsImplicitRole, //Variant->bool
        IsParametricRole, //Variant->bool
        IsAlgebraicRole  //Variant->bool

    };
    
    PlaneCurveModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~PlaneCurveModel();
    
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
    QMap<int, QVariant> itemData(const QModelIndex & index) const;
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    bool setItemData(const QModelIndex & index, const QMap<int, QVariant> & roles);
    
    bool magic(int n);
    //ro pointer for points values ... stuff too heavy for the role/variant way
    //also points and jumps will change internaly not by the setdata/qobject 
    const PlaneCurve * item(int n) const; 

private:
    QList<PlaneCurve*> m_items;
};





/** Functions model is a model class that has a relation of all operators string with their FunctionType. */
// class ANALITZAPLOT_EXPORT FunctionsModel : public QAbstractTableModel
// {
//     Q_OBJECT
// //     Q_PROPERTY(uint resolution READ resolution WRITE setResolution);
//     public:
//         enum FunctionsModelRoles { Color=Qt::UserRole, Expression=Qt::UserRole+1 , Shown=Qt::UserRole+2 };
//         typedef QList<FunctionGraph2d>::const_iterator const_iterator;
//         friend class PlotView2D;
// 
//         /** Constructor. Creates a new Function Model. */
//         explicit FunctionsModel(QObject *parent=0);
// 
//         Qt::ItemFlags flags ( const QModelIndex & index ) const;
// 
//         QVariant data( const QModelIndex &index, int role=Qt::DisplayRole) const;
//         QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
//         int rowCount(const QModelIndex &parent=QModelIndex()) const;
//         int columnCount(const QModelIndex & =QModelIndex()) const { return 2; }
// 
//         /** Adds another function @p f. Returns whether another function like @p f existed. */
//         bool addFunction(const FunctionGraph2d &func);
// 
//         /** Specifies that the @p exp function is shown.
//             @returns whether another function like @p exp existed. */
//         bool setShown(const QString& exp, bool shown);
// 
//         /** Edits the @p num nth function. The @p num should be less than the number of functions,
//             because you are editing. */
//         void editFunction(int num, const FunctionGraph2d &func);
// 
//         /** Edits the @p exp function. Returns whether another function like @p exp existed. */
//         bool editFunction(const QString &toChange, const FunctionGraph2d &func);
// 
//         /** Returns a pointer to the @p num nth function. */
//         FunctionGraph* editFunction(int num);
// 
//         void setResolution(FunctionGraphPrecision res);
//         uint resolution() const { return m_resolution; }
// 
//         void sendStatus(const QString& msg) { emit status(msg); }
// 
//         void updatePoints(int i, const QRect& viewport);
// 
//         const_iterator constBegin() const { return funclist.constBegin(); }
//         const_iterator constEnd() const { return funclist.constEnd(); }
// 
//         virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
// 
//         virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
// 
//         /** Returns the id for the next function that's not used by any other, starting by f */
//         QString freeId();
// 
//         QPair<QPointF, QString> calcImage(int row, const QPointF& ndp);
//         QLineF slope(int row, const QPointF& dp) const;
//         QModelIndex indexForId(const QString & name);
// 
//     public slots:
//         void clear();
// 
//     signals:
//         /** Emits a status message when something changes. */
//         void status(const QString &msg);
// 
//         void functionModified(const QString& name, const Analitza::Expression& e);
//         void functionRemoved(const QString& name);
// 
//     private:
//         QList<FunctionGraph2d>::const_iterator findFunction(const QString& id) const;
//         QList<FunctionGraph2d>::iterator findFunction(const QString& id);
// 
//         QList<FunctionGraph2d> funclist;
//         FunctionGraphPrecision m_resolution;
// 
//         uint m_fcount;
// };



#endif // ANALITZAPLOT_FUNCTIONSMODEL_H
