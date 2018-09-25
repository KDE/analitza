/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
 *  Copyright (C) 2012 by Aleix Pol Gonzalez <aleixpol@kde.org>                      *
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

#ifndef ANALITZAPLOT_PLOTSDICTIONARYMODEL_H
#define ANALITZAPLOT_PLOTSDICTIONARYMODEL_H

#include <QStandardItemModel>
#include <QPointer>
#include  <analitzaplot/plottingenums.h>
#include "analitzaplotexport.h"
namespace Analitza
{
class PlotsModel;

//WARNING @PlotsDictionaryModel needs dictionary files (*.plots), currently we 
//are working to add plots files for next version coming (kde >= 4.10.x).

class ANALITZAPLOT_EXPORT PlotsDictionaryModel : public QStandardItemModel
{
Q_OBJECT
public:
    enum Roles { ExpressionRole = Qt::UserRole+1, FileRole };
    explicit PlotsDictionaryModel(QObject* parent = nullptr);
    ~PlotsDictionaryModel();
    
    int currentRow() const;
    void setCurrentRow(int row);
    
    PlotsModel* plotModel();
    void createDictionary(const QString& path);
    void createAllDictionaries();
    Analitza::Dimension dimension();

public Q_SLOTS:
    ///convenience class for currentRow
    void setCurrentIndex(const QModelIndex& idx);
    
private:
    void updatePlotsModel();
    QPointer<PlotsModel> m_plots;
    int m_currentItem;
};

}

#endif
