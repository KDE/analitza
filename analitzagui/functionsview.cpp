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


#include "functionsview.h"
// #include "functiondelegate.h"
#include "analitzaplot/functionsmodel.h"

#include <KDebug>
#include <KLocale>
#include <KIcon>
#include <QMouseEvent>
#include <QMenu>
#include <QHeaderView>

#include "analitza/expression.h"




FunctionsView::FunctionsView(QWidget * parent) : QTreeView(parent)
    , m_mainView(false)
{


    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);
    setSortingEnabled(false);

//     setItemDelegate(new FunctionDelegate(this));
}

void FunctionsView::selectionChanged(const QItemSelection & selected, const QItemSelection &)
{
    if(!selected.indexes().isEmpty())
    {
        QModelIndex idx=selected.indexes().first();

        FunctionsModel *sm = qobject_cast<FunctionsModel*>(model());
        sm->setData(idx, QVariant(), FunctionsModel::Selection);
    }
}

void FunctionsView::mousePressEvent(QMouseEvent * e)
{
    QModelIndex clickIdx(indexAt(e->pos()));
    QModelIndex nameIdx(clickIdx.sibling(clickIdx.row(), 0));
    bool implicit=false;
    if(model()->data(nameIdx).toString()=="Esfera" ||
            model()->data(nameIdx).toString()=="Elipsoide" ||
            model()->data(nameIdx).toString()=="Cilindro" ||
            model()->data(nameIdx).toString()=="Hiperboloide una hoja" ||
            model()->data(nameIdx).toString()=="Hiperboloide dos hojas" ||
            model()->data(nameIdx).toString()=="Paraboloide hiperbolico" ||
            model()->data(nameIdx).toString()=="Paraboloide eliptico" ||
            model()->data(nameIdx).toString()=="Cono eliptico")
        implicit=true;


    if(e->button()==Qt::RightButton && clickIdx.isValid())
    {
        QModelIndex nameIdx(clickIdx.sibling(clickIdx.row(), 0));
        bool shown=model()->data(clickIdx, FunctionsModel::Shown).toBool();
        QString actuallyShown;
        QString icon;
        if(shown)
        {
            icon="layer-visible-off";
            actuallyShown=i18n("Ocultar Función '%1'", model()->data(nameIdx).toString());
        }
        else
        {
            icon="layer-visible-on";
            actuallyShown=i18n("Mostrar Función '%1'", model()->data(nameIdx).toString());
        }

        QMenu menu(this);
        QAction* actionShown=0;

        QAction* actionShowOnSpace = 0;
        if (m_mainView && !implicit)
            actionShowOnSpace=menu.addAction(KIcon("page-zoom"), i18n("Mostrar Función '%1'", model()->data(nameIdx).toString()));


        QAction* actionRemove=menu.addAction(KIcon("list-remove"), i18n("Eliminar Función '%1'", model()->data(nameIdx).toString()));

        if (!m_mainView && !implicit)
            actionShown=menu.addAction(KIcon(icon), actuallyShown);


        QAction* result=menu.exec(e->globalPos());

        FunctionsModel *sm = qobject_cast<FunctionsModel*>(model());
        int sourceRow = clickIdx.row();

        if(result==actionShown)
        {
            sm->setData(sm->index(sourceRow,0), !shown, FunctionsModel::Shown);

            if (!shown)
                emit functionShown(sm->funclist[sourceRow]);
            else
                emit functionHided(sm->editFunction(sourceRow)->id(), sm->editFunction(sourceRow)->expression().toString());

        }

        if(result==actionRemove)
        {

            sm->removeRow(sourceRow);

        }

        if (m_mainView)
            if (result==actionShowOnSpace)
            {



                sm->editFunction(sourceRow)->setShown(true);
                emit functionOnSpaceShown(sm->editFunction(sourceRow)->spaceId());

            }



    }
    else
    {
        if(implicit)return;
        QTreeView::mousePressEvent(e);
    }
}

