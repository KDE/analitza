/*************************************************************************************
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


#ifndef KEOMATH_SPACESMODEL_H__k
#define KEOMATH_SPACESMODEL_H__k

#include <QtCore/QAbstractListModel>
#include <KIcon>

#include "dictionaryitem.h"

namespace Analitza
{
class Expression;
}

#include "analitzaplotexport.h"

class ANALITZAPLOT_EXPORT DictionariesModel : public QAbstractListModel 
{

friend class DictionaryItem;   
Q_OBJECT

public:
    DictionariesModel(QObject *parent=0);

    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    QVariant data( const QModelIndex &index, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex& parent) const;
    //TODO implementar removeRows en analitzaplot (plotsmodel)
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

    DictionaryItem * addSpace(Dimension dim, const QString & title = QString(), const QString &description = QString(), 
                         const QPixmap &thumbnail=KIcon("khipu").pixmap(QSize(256,256)));

    DictionaryItem * space(int row) const;
    DictionaryItem * spacebyid(const QString &id) const;

    QModelIndex spaceIndex(DictionaryItem *it) const { return index(m_items.indexOf(it)); }
//     removeitem or removerow????
//     por conveniencia removeitem ... pero debe usar el removerow
//     es additem ... no es addspace nia ddplot
    //DEPRECATED
//     void removeItem(int row);
    
private:
    SpaceItemList m_items;
    bool m_itemCanCallModelRemoveItem; // just a lock para evitar que el item llame recursivamente a removeItem
};

#endif 
