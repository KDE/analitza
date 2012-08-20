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

#include "plotsdictionarymodel.h"
#include "dictionariesmodel.h"
#include <analitza/expression.h>
#include <KLocale>
#include <QFile>
#include <QDomDocument>

PlotsDictionariesModel::PlotsDictionariesModel(DictionariesModel *dmodel, PlotsModel *pmodel , QObject* parent)
    : QStandardItemModel(parent)
    , m_dictionaryModel(dmodel)
    , m_plotsModel(pmodel)
{
    //TODO newdic if models notempy load data
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList() << i18nc("@title:column", "Name") << i18nc("@title:column", "Description"));
    
    connect(m_dictionaryModel, SIGNAL(rowsInserted (QModelIndex , int , int )), SLOT(addParent(QModelIndex,int,int)));
    connect(m_dictionaryModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(setParentData(QModelIndex,QModelIndex)));    
    connect(m_dictionaryModel, SIGNAL(rowsAboutToBeRemoved (QModelIndex, int, int)), SLOT(removeParent(QModelIndex,int,int)));
    
    connect(m_plotsModel, SIGNAL(rowsInserted (QModelIndex , int , int )), SLOT(addChild(QModelIndex,int,int)));
    connect(m_plotsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(setChildData(QModelIndex,QModelIndex)));    
    connect(m_plotsModel, SIGNAL(rowsAboutToBeRemoved (QModelIndex, int, int)), SLOT(removeChild(QModelIndex,int,int)));
}

void PlotsDictionariesModel::addParent(const QModelIndex& parent, int start, int end)
{
    DictionaryItem *dictionary = m_dictionaryModel->space(start);
    
    QStandardItem *dictionaryName = new QStandardItem();
    dictionaryName->setText(dictionary->title());
    dictionaryName->setIcon(dictionary->thumbnail());
    //TODO
//     dictionaryName->setStatusTip(dictionary->timestamp().toString("%A %l:%M %p %B %Y"));
    dictionaryName->setData(dictionary->id().toString(), Qt::UserRole);
    
    QStandardItem *dictionaryDescription = new QStandardItem();
    dictionaryDescription->setText(dictionary->description());
//     dictionaryDescription->setData(dictionary->id().toString(), Qt::UserRole);

    invisibleRootItem()->appendRow(dictionaryName);
    invisibleRootItem()->setChild(dictionaryName->row(), 1, dictionaryDescription);
    
    m_parentsMap[dictionary] = dictionaryName;
}

void PlotsDictionariesModel::setParentData(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    DictionaryItem *dictionary = m_dictionaryModel->space(topLeft.row());

    QStandardItem *dictionaryEntry = m_parentsMap.value(dictionary);

    if (!dictionaryEntry) 
        return ;

    dictionaryEntry->setText(dictionary->title());
    dictionaryEntry->setIcon(dictionary->thumbnail());
    invisibleRootItem()->child(dictionaryEntry->row(), 1)->setText(dictionary->description());
}

void PlotsDictionariesModel::removeParent(const QModelIndex& parent, int start, int end)
{
    //TODO remove many
    QStandardItem *dictionaryEntry = m_parentsMap.value(m_dictionaryModel->space(start));

    if (!dictionaryEntry) 
        return ;

    invisibleRootItem()->removeRow(dictionaryEntry->row());
    m_parentsMap.remove(m_dictionaryModel->space(start));
}

void PlotsDictionariesModel::addChild(const QModelIndex& parent, int start, int end)
{
    PlotItem *plot = m_plotsModel->plot(start); // se que siempre se agrega 1 en 1 no se inserta varios
    
    if (!m_parentsMap.contains(plot->space())) //l padre no corresponde con el dic del plot assert?
        return ;

    QStandardItem *dictionaryEntry = m_parentsMap.value(plot->space());

    if (!dictionaryEntry)
        return ;

    QPixmap ico(16, 16);
    ico.fill(plot->color());

    QStandardItem *plotName = new QStandardItem();
    plotName->setText(plot->name());
    plotName->setIcon(QIcon(ico));
    plotName->setData(plot->id().toString(), Qt::UserRole);

    QStandardItem *plotExpression = new QStandardItem();
    plotExpression->setText(plot->expression().toString());
    plotExpression->setIcon(KIcon(plot->iconName()));
//     plotExpression->setData(plot->id().toString(), Qt::UserRole);

    dictionaryEntry->setChild(dictionaryEntry->rowCount(), 0, plotName);
    dictionaryEntry->setChild(dictionaryEntry->rowCount()-1, 1, plotExpression);
    
    m_childrenMap[plot] = plotName;
}

void PlotsDictionariesModel::setChildData(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    PlotItem *plot = m_plotsModel->plot(topLeft.row()); // se que siempre se agrega 1 en 1 no se inserta varios
    
    if (!m_parentsMap.contains(plot->space())) //l padre no corresponde con el dic del plot assert?
        return ;

    QStandardItem *dictionaryEntry = m_parentsMap.value(plot->space());

    if (!dictionaryEntry)
        return ;

    QStandardItem *plotEntry = m_childrenMap.value(plot);

    if (!plotEntry)
        return ;

    QPixmap ico(16, 16);
    ico.fill(plot->color());

    plotEntry->setText(plot->name());
    plotEntry->setIcon(ico);
    dictionaryEntry->child(plotEntry->row(), 1)->setText(plot->expression().toString());
    dictionaryEntry->child(plotEntry->row(), 1)->setIcon(KIcon(plot->iconName()));


}

void PlotsDictionariesModel::removeChild(const QModelIndex& parent, int start, int end)
{
    PlotItem *plot = m_plotsModel->plot(start); // se que siempre se agrega 1 en 1 no se inserta varios
    
    if (!m_parentsMap.contains(plot->space())) //l padre no corresponde con el dic del plot assert?
        return ;

    QStandardItem *dictionaryEntry = m_parentsMap.value(plot->space());

    if (!dictionaryEntry)
        return ;

    QStandardItem *plotEntry = m_childrenMap.value(plot);

    if (!plotEntry)
        return ;

    dictionaryEntry->removeRow(plotEntry->row());
    m_childrenMap.remove(plot);
}

PlotsDictionariesModel::~PlotsDictionariesModel()
{
// delete rootItem;
}
