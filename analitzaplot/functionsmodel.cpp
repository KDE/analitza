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
#include <QPixmap>
#include <QFont>
#include <QIcon>

#include "analitza/expression.h"

FunctionGraphModel::FunctionGraphModel(Analitza::Variables *v, QObject * parent)
    : QAbstractListModel(parent)
{
    Q_ASSERT(v);

    variablesModule = v;
}

FunctionGraphModel::~FunctionGraphModel()
{

}


int FunctionGraphModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 2;
}


Qt::ItemFlags FunctionGraphModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsTristate;
    else
        return 0;
}


bool FunctionGraphModel::hasChildren(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return false;
}


QVariant FunctionGraphModel::headerData(int section, Qt::Orientation orientation, int role) const
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


Qt::DropActions FunctionGraphModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

///

PlaneCurveModel::PlaneCurveModel(Analitza::Variables *v, QObject * parent)
    : FunctionGraphModel(v, parent)
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

    case ErrorsRole: //Variant->QStringList
        return tmpcurve->errors();
        break;

        ///arrrggg
        /*
        case ArcLengthRole: //Variant->double
            return tmpcurve->arcLength();
        break;

        case IsClosedRole:  //Variant->double
            return tmpcurve->isClosed();
        break;

        case AreaRole: //Variant->double
            return tmpcurve->area();
        break;*/

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

    }

    return QVariant();
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
                
                if (PlaneCurve::canDraw(fexp))
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
        }
    }

    return false;
}

void PlaneCurveModel::addItem(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
//     beginInsertRows(parent, row, row+count-1);
//
//     for (int i = 0; i < count; ++i)
//         m_items.insert(row, new PlaneCurve(Analitza::Expression("x->0"), variablesModule, QString(), QColor()));
//
//     endInsertRows();
//
//     return true;
}

void PlaneCurveModel::removeItem(int row)
{
//     Q_ASSERT(row+count-1<m_items.count());
//
//     if(parent.isValid())
//         return false;
//
//     beginRemoveRows(parent, row, row+count-1);
//
//     for (int i = 0; row < count; ++i)
//     {
//         PlaneCurve *tmpcurve = m_items.at(row);
//         delete tmpcurve;
//         m_items.removeAt(row);
//     }
//
//     endRemoveRows();
//
//     return true;
}

const PlaneCurve* PlaneCurveModel::item(int row) const
{
    Q_ASSERT(row<m_items.count());

    return m_items[row];
}

void PlaneCurveModel::update(int row, const QRect& viewport)
{
    m_items[row]->update(viewport);

    emit dataChanged(index(row), index(row));
}


QPair< QPointF, QString > PlaneCurveModel::calcItem(int row, const QPointF& mousepos)
{
    Q_ASSERT(row<m_items.count());

    return m_items[row]->calc(mousepos);
}

QLineF PlaneCurveModel::derivativeItem(int row, const QPointF& mousepos) const
{
    Q_ASSERT(row<m_items.count());

    return m_items[row]->derivative(mousepos);
}

