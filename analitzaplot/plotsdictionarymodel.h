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



#ifndef ANALITZAPLOT_FUNCTION_H_DICT
#define ANALITZAPLOT_FUNCTION_H_DICT


#include "plotsmodel.h"
#include "plotsproxymodel.h"
#include "plotitem.h"
#include <QStandardItemModel>


//si se agrega nuevos archvivos se debe cargar mediante un filewatch ... en general QAbstractItemModel es mas flexible que qstandaritemmodel
//too complex rescandiction on signals like export as dic ...
//cunado lo use el dicmanager borrar los models luego de cargar la data
class ANALITZAPLOT_EXPORT PlotsDictionariesModel : public PlotsModel
{
// Q_OBJECT    
// 
// // friend PlotsDictionaryModel * getDictionary(QObject *); // for private slots
// 
public:
    //primero lee el dicmodel y crea los parents
    //luego lee el plostmodel para llenar los childs
    PlotsDictionariesModel(QObject* parent = 0);
    ~PlotsDictionariesModel();
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent) const;
};

#endif
