/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "functionsmodel.h"
#include <analitza/localize.h>
#include <QDebug>
#include <QTime>
#include <QPixmap>
#include <QFont>
#include <QIcon>

#include "analitza/expression.h"




MappingGraphModel::MappingGraphModel(Analitza::Variables *v, QObject * parent)
: QAbstractListModel(parent)
{
    Q_ASSERT(v);
    
    variablesModule = v;
    
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}


MappingGraphModel::~MappingGraphModel()
{

}


int MappingGraphModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    
    return 2;
}


Qt::ItemFlags MappingGraphModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsTristate;
    else
        return 0;
}


bool MappingGraphModel::hasChildren(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    
    return false;   
}


QVariant MappingGraphModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal) 
    {
        switch(section) {
            case 0:
                ret=i18nc("@title:column", "Name");
                break;
            case 1:
                ret=i18nc("@title:column", "Function");
                break;
        }
    }
    return ret;
}


Qt::DropActions MappingGraphModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}









///






PlaneCurveModel::PlaneCurveModel(Analitza::Variables *v, QObject * parent)
: MappingGraphModel(v, parent)
{
}


PlaneCurveModel::~PlaneCurveModel()
{
    qDeleteAll(m_items.begin(), m_items.end());
    m_items.clear();
}


QVariant PlaneCurveModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()>=m_items.count())
        return QVariant();
    
    int var=index.row();
    
    PlaneCurve *tmpcurve = m_items.at(var);

    switch(role) 
    {
        case Qt::DisplayRole:
            switch(index.column()) {
                case 0:
                    return tmpcurve->name();
                    break;
                case 1:
                    return tmpcurve->expression().toString();
                    break;
            }
            break;
        case Qt::DecorationRole:
            if(index.column()==0) {
                QPixmap ico(15, 15);
                ico.fill(tmpcurve->color());
                return  QIcon(ico);
            } else {
                return QIcon::fromTheme(tmpcurve->iconName());
            }
            break;
            
        //roles that will show in a view, also works for editing job
        case ExpressionRole: //Variant->QString
            return tmpcurve->expression().toString();
            break;
        case NameRole: //Variant->QString
                return tmpcurve->name();
            break;

        case ColorRole: //Variant->QColor
                return tmpcurve->color();
            break;
            
        case IconNameRole: //Variant->QString
                return tmpcurve->iconName();
            break;
            
        //roles for editing job
        case ArgumentsRole: //Variant->QList<QVariant> ... List: ... QString(argname), double min, double max ...
        {
            QVariantList args;

                foreach (QString arg, tmpcurve->arguments())
                {
                    args.append(arg);
                    args.append(tmpcurve->argumentInterval(arg).lowEndPoint().value());
                    args.append(tmpcurve->argumentInterval(arg).highEndPoint().value());
                }

                return args;
            break;
        }
        
        case DrawingPrecisionRole: //Variant->int
                return tmpcurve->drawingPrecision();
            break;

        case VisibleRole: //Variant->bool
                return tmpcurve->isVisible();
            break;

        //roles for deliver extra data: examples, etc ... read-only roles
        case ExamplesRoles: //Variant->QStringList
                return tmpcurve->examples();
            break;

        case SpaceDimensionRole: //Variant->int
                return tmpcurve->spaceDimension();
            break;

        case CoordinateSystemRole: //Variant->int
                return tmpcurve->coordinateSystem();
            break;
            
        case PlotStyleRole: //Variant->int
                return tmpcurve->plotStyle();
            break;

        case IsCorrectRole: //Variant->bool
                return tmpcurve->isCorrect();
            break;

        case ErrorsRole: //Variant->QStringList
                return tmpcurve->errors();
            break;

        case ArcLengthRole: //Variant->double
                return tmpcurve->arcLength();
            break;

        case IsClosedRole:  //Variant->double
                return tmpcurve->isClosed();
            break;

        case AreaRole: //Variant->double
                return tmpcurve->area();
            break;

            //use item instead
//         case JumpsRole:  //Variant->QList<QVariant> ... list of ints
//         {
//                 QVariantList jumps = tmpcurve->jumps();
//                 return jumps;
//             break;
//         }
//         
//         case PointsRole: //Variant->QList<QVariant> ... list of doubles
//                 return tmpcurve->points();
//             break;

        case IsImplicitRole: //Variant->bool
                return tmpcurve->isImplicit();
            break;

        case IsParametricRole: //Variant->bool
                return tmpcurve->isParametric();
            break;

        case IsAlgebraicRole:  //Variant->bool
                return tmpcurve->isAlgebraic();
            break;
    }

    return QVariant();
}


bool PlaneCurveModel::insertRows(int row, int count, const QModelIndex & parent)
{
    beginInsertRows(parent, row, row+count-1);

    for (int i = 0; i < count; ++i) 
        m_items.insert(row, new PlaneCurve(Analitza::Expression("x->0"), variablesModule, QString(), QColor()));

    endInsertRows();
    
    return true;
}

QMap<int, QVariant> PlaneCurveModel::itemData(const QModelIndex & index) const
{
    QMap<int, QVariant> ret;
    
    ret[ExpressionRole] = data(index, ExpressionRole);
    ret[NameRole] = data(index, NameRole);
    ret[ColorRole] = data(index, ColorRole);
    ret[IconNameRole] = data(index, IconNameRole);

    //roles for editing job
    ret[ArgumentsRole] = data(index, ArgumentsRole);
    ret[DrawingPrecisionRole] = data(index, DrawingPrecisionRole);
    ret[VisibleRole] = data(index, VisibleRole);

    //roles for deliver extra data: examples, etc ... read-only roles
    ret[ExamplesRoles] = data(index, ExamplesRoles);
    ret[SpaceDimensionRole] = data(index, SpaceDimensionRole);
    ret[CoordinateSystemRole] = data(index, CoordinateSystemRole);
    ret[PlotStyleRole] = data(index, PlotStyleRole);
    ret[IsCorrectRole] = data(index, IsCorrectRole);
    ret[ErrorsRole] = data(index, ErrorsRole);
    ret[ArcLengthRole] = data(index, ArcLengthRole);
    ret[IsClosedRole] = data(index, IsClosedRole);
    ret[AreaRole] = data(index, AreaRole);
//         JumpsRole,  //Variant->QList<QVariant> ... list of ints ... use const T * item const instead
//         PointsRole, //Variant->QList<QVariant> ... list of doubles ... use const T * item const instead
    ret[IsImplicitRole] = data(index, IsImplicitRole);
    ret[IsParametricRole] = data(index, IsParametricRole);
    ret[IsAlgebraicRole] = data(index, IsAlgebraicRole);

    return ret;
}


bool PlaneCurveModel::removeRows(int row, int count, const QModelIndex & parent)
{
    Q_ASSERT(row+count-1<m_items.count());
    
    if(parent.isValid())
        return false;
    
    beginRemoveRows(parent, row, row+count-1);

    for (int i = 0; row < count; ++i) 
    {
        PlaneCurve *tmpcurve = m_items.at(row);
        delete tmpcurve;
        m_items.removeAt(row);
    }

    endRemoveRows();

    return true;   
}


int PlaneCurveModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return m_items.count();
}


bool PlaneCurveModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (index.isValid())
    {
        switch (role)
        {
            //roles that will show in a view, also works for editing job
            case ExpressionRole: //Variant->QString
            {
                Analitza::Expression fexp(value.toString());
                if (m_items[index.row()]->canReset(fexp))
                {
                    m_items[index.row()]->reset(fexp);
                    
                    emit dataChanged(index, index);
                    
                    return true;
                }
                else
                    return false;
            
                break;
            }
                
            case NameRole: //Variant->QString
            {
                m_items[index.row()]->setName(value.toString());
                emit dataChanged(index, index);
                
                return true;
            
                break;
            }

            case ColorRole: //Variant->QColor
            {
                    

                m_items[index.row()]->setColor(value.value<QColor>());
                emit dataChanged(index, index);
                return true;
            
                break;
            }
                
                
            //roles for editing job
            case ArgumentsRole: //Variant->QList<QVariant> ... List: ... QString(argname), double min, double max ...
            {
//                 QVariantList args;
// 
//                     foreach (QString arg, tmpcurve->arguments())
//                     {
//                         args.append(arg);
//                         args.append(tmpcurve->argumentInterval(arg).lowEndPoint().value());
//                         args.append(tmpcurve->argumentInterval(arg).highEndPoint().value());
//                     }
// 
//                     return args;
                break;
            }
            
            case DrawingPrecisionRole: //Variant->int
            {
                m_items[index.row()]->setDrawingPrecision((DrawingPrecision)value.toInt());
                emit dataChanged(index, index);
                
                return true;
            
                break;
            }

            case VisibleRole: //Variant->bool
            {
                m_items[index.row()]->setVisible(value.toBool());
                emit dataChanged(index, index);
                
                return true;
            
                break;
            }

            case PlotStyleRole: //Variant->int
            {
                m_items[index.row()]->setPlotStyle((PlotStyle)value.toInt());
                emit dataChanged(index, index);
                
                return true;
            
                break;
            }
        }
    }
    
    return false;
}


bool PlaneCurveModel::setItemData(const QModelIndex & index, const QMap<int, QVariant> & roles)
{
    bool allvalid = true;
    
    foreach (int role, roles.keys())
        if (!setData(index, roles[role], role))
        {
            allvalid = false;
            
            break;
        }
        
    return allvalid;
}

bool PlaneCurveModel::magic(int n)
{
    static int defaultNameId = 1;
    
    if (insertRows(m_items.size(), n))
    {
        for (int i = m_items.size()-n; i < m_items.size(); ++i) 
        {
            //TODO rand expression ... from examples?
            QString defaultName = QString("Curve %1").arg(defaultNameId);
            QColor defaultColor(qrand() % 256, qrand() % 256, qrand() % 256);
            
            //yanoisreset se asume que la data que se pondra aca viene de examples o de otra fuente confiable
            //TODO gsoc por el momento solo una sola funcion
            setData(index(i), QString("x->x*x"), ExpressionRole);
            setData(index(i), defaultName, NameRole);
            setData(index(i), defaultColor, ColorRole);
        }

        ++defaultNameId;
        
        return true;
    }
    
    return false;
}

const PlaneCurve* PlaneCurveModel::item(int n) const
{
    Q_ASSERT(n<m_items.count());

    return m_items[n];
}




// FunctionsModel::FunctionsModel(QObject *parent)
//     : QAbstractTableModel(parent), m_resolution(AveragePrecision), m_fcount(1)
// {
//     QHash<int, QByteArray> rolenames=QAbstractTableModel::roleNames();
//     rolenames.insert(Color, "color");
//     rolenames.insert(Expression, "expression");
//     rolenames.insert(Shown, "shown");
// 
//     setRoleNames(rolenames);
// }
// 
// QVariant FunctionsModel::data(const QModelIndex & index, int role) const
// {
//     QVariant ret;
//     if(!index.isValid() || index.row()>=funclist.count())
//         return QVariant();
//     int var=index.row();
//     const Function &f=funclist[var];
// 
//     switch(role) {
//         case Qt::DisplayRole:
//             switch(index.column()) {
//                 case 0:
//                     ret=f.name();
//                     break;
//                 case 1:
//                     ret=f.expression().toString();
//                     break;
//             }
//             break;
//         case Qt::DecorationRole:
//             if(index.column()==0) {
//                 QPixmap ico(15, 15);
//                 ico.fill(f.color());
//                 ret = QIcon(ico);
//             } else {
//                 ret = QIcon::fromTheme(f.iconName());
//             }
//             break;
//         case Expression:
//             ret=f.expression().toString();
//             break;
//         case Shown:
//             ret=f.isGraphVisible();
//             break;
//         case Color:
//             ret=f.color();
//             break;
// 
//     }
// 
// //  qDebug() << "laaaa" << roleNames().value(role) << ret << index;
//     return ret;
// }
// 
// QVariant FunctionsModel::headerData(int section, Qt::Orientation orientation, int role) const
// {
//     QVariant ret;
//     if(role==Qt::DisplayRole && orientation==Qt::Horizontal) {
//         switch(section) {
//             case 0:
//                 ret=i18nc("@title:column", "Name");
//                 break;
//             case 1:
//                 ret=i18nc("@title:column", "Function");
//                 break;
//         }
//     }
//     return ret;
// }
// 
// int FunctionsModel::rowCount(const QModelIndex &idx) const
// {
//     if(idx.isValid())
//         return 0;
//     else
//         return funclist.count();
// }
// 
// //TODO: really need to return that?
// bool FunctionsModel::addFunction(const Function2D& func)
// {
//     Q_ASSERT(func.isCorrect());
// 
//     QList<Function2D>::const_iterator it=const_cast<const FunctionsModel*>(this)->findFunction(func.name());
//     bool exists=it!=funclist.constEnd();
// 
//     if(!exists) {
//         beginInsertRows (QModelIndex(), rowCount(), rowCount());
//         funclist.append(func);
//         funclist.last().setGraphPrecision(m_resolution);
//         endInsertRows();
//         sendStatus(i18n("%1 function added", func.name()));
// 
//         emit functionModified(func.name(), func.expression());
//     }
// 
//     return !exists;
// }
// 
// bool FunctionsModel::removeRows(int row, int count, const QModelIndex & parent)
// {
//     Q_ASSERT(row+count-1<funclist.count());
//     if(parent.isValid())
//         return false;
//     beginRemoveRows(parent, row, row+count-1);
// 
//     QList<Function2D>::iterator it=funclist.begin()+row;
//     for(int i=count-1; i>=0; i--) {
//         QString name=it->name();
//         it=funclist.erase(it);
//         emit functionRemoved(name);
//     }
//     endRemoveRows();
// 
//     return true;
// }
// 
// void FunctionsModel::clear()
// {
//     if(!funclist.isEmpty()) {
//         beginRemoveRows (QModelIndex(), 0, rowCount());
//         foreach(const Function& f, funclist) {
//             emit functionRemoved(f.name());
//         }
//         funclist.clear();
//         endRemoveRows ();
//         reset();
//     }
// }
// 
// bool FunctionsModel::setShown(const QString& f, bool shown)
// {
//     //TODO gsoc
// 
// //     for (QList<Function>::iterator it = funclist.begin(); it != funclist.end(); ++it ){
// //         if(it->name() == f) {
// //             it->setGraphVisible(shown);
// //             return true;
// //         }
// //     }
//     return false;
// }
// 
// //TODO remove me
// Function* FunctionsModel::editFunction(int num)
// {
//     Q_ASSERT(num<funclist.count());
//     return &funclist[num];
// }
// 
// void FunctionsModel::editFunction(int num, const Function2D& func)
// {
//     Q_ASSERT(num<funclist.count());
//     funclist[num]=func;
//     funclist[num].setGraphPrecision(m_resolution);
// 
//     QModelIndex idx=index(num, 0), idxEnd=index(num, columnCount()-1);
//     emit dataChanged(idx, idxEnd);
//     emit functionModified(func.name(), func.expression());
// 
// //  this->repaint(); emit update
// }
// 
// bool FunctionsModel::editFunction(const QString& toChange, const Function2D& func)
// {
//     bool exist=false;
// 
//     QList<Function2D>::iterator it=findFunction(toChange);
//     if(it!=funclist.end()) {
//         exist=true;
//         *it = func;
//         it->setName(toChange);
//         it->setGraphPrecision(m_resolution);
// 
//         int i = funclist.indexOf(*it);
// 
//         QModelIndex idx=index(i, 0), idxEnd=index(i, columnCount()-1);
//         emit dataChanged(idx, idxEnd);
//         emit functionModified(toChange, func.expression());
//     }
// 
//     return exist;
// }
// 
// bool FunctionsModel::setData(const QModelIndex & idx, const QVariant &value, int role)
// {
//     //TODO gsoc
// 
// //     if(role==Shown) {
// //         bool isshown=value.toBool();
// //         funclist[idx.row()].setGraphVisible(isshown);
// //
// //         QModelIndex idx1=index(idx.row(), 0), idxEnd=index(idx.row(), columnCount()-1);
// //         emit dataChanged(idx1, idxEnd);
// //     }
//     return false;
// }
// 
// void FunctionsModel::updatePoints(int i, const QRect & viewport)
// {
//     Q_ASSERT(i<funclist.count());
//     //TODO gsoc
// //     funclist[i].updateGraphData(/*viewport*/);
// }
// 
// QLineF FunctionsModel::slope(int row, const QPointF & dp) const
// {
//     QLineF ret;
//     if(row<0) return ret;
// 
//     const Function & f = funclist[row];
//     if(f.isGraphVisible()) {
// //         ret = f.derivative(dp);
//     }
//     return ret;
// }
// 
// QPair<QPointF, QString> FunctionsModel::calcImage(int row, const QPointF & ndp)
// {
//     QPair<QPointF, QString> ret;
//     ret.first=ndp;
//     if(row<0) return ret;
// 
//     Function & f = funclist[row];
//     if(f.isGraphVisible()) {
// //         ret = f.calc(ndp);
//     }
// 
//     return ret;
// }
// 
// Qt::ItemFlags FunctionsModel::flags(const QModelIndex &idx) const
// {
//     if(idx.isValid())
//         return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsTristate;
//     else
//         return 0;
// }
// 
// void FunctionsModel::setResolution(FunctionGraphPrecision res)
// {
//     m_resolution=res;
//     if(!funclist.isEmpty()) {
//         for (QList<Function2D>::iterator it=funclist.begin(); it!=funclist.end(); ++it)
//             it->setGraphPrecision(res);
//         QModelIndex idx=index(0, 0), idxEnd=index(rowCount()-1, 0);
//         emit dataChanged(idx, idxEnd);
//     }
// }
// 
// QString FunctionsModel::freeId()
// {
//     return QString("f%1").arg(m_fcount++);
// }
// 
// QList<Function2D>::const_iterator FunctionsModel::findFunction(const QString& id) const
// {
//     for (QList<Function2D>::const_iterator it = funclist.constBegin(); it!=funclist.constEnd(); ++it)
//         if(it->name() == id)
//             return it;
// 
//     return funclist.constEnd();
// }
// 
// QList<Function2D>::iterator FunctionsModel::findFunction(const QString& id)
// {
//     for (QList<Function2D>::iterator it = funclist.begin(); it!=funclist.end(); ++it)
//         if(it->name() == id)
//             return it;
// 
//     return funclist.end();
// }
// 
// QModelIndex FunctionsModel::indexForId(const QString& id)
// {
//     int i=0;
//     for (QList<Function2D>::iterator it = funclist.begin(); it!=funclist.end(); ++it, ++i)
//         if(it->name() == id)
//             return index(i,0);
//     return QModelIndex();
// }
