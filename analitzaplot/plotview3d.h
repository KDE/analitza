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



#ifndef KEOMATH_VIEW3D_H
#define KEOMATH_VIEW3D_H

#include "QGLViewer/qglviewer.h"
#include <QMouseEvent>
#include <analitzaplot/mathutils.h>
#include <analitzaplot/private/functiongraph.h>
#include <QModelIndex>

class VisualItemsModel;
class QItemSelectionModel;

// class Solver3D;
class ANALITZAPLOT_EXPORT View3D : public QGLViewer
{
    Q_OBJECT

public:
    View3D(QWidget *parent = 0);
    View3D(VisualItemsModel *m, QWidget *parent = 0);

    void setModel(VisualItemsModel *m);
    void setSelectionModel(QItemSelectionModel* selection);

public slots:
    void updateFuncs(const QModelIndex &indexf,const QModelIndex &indext);
    void addFuncs(const QModelIndex &index,int,int);
    void removeFuncs(const QModelIndex &index,int,int);

private:
    int currentFunction() const;
    void draw();
    void init();

    VisualItemsModel *m_model;
    QItemSelectionModel* m_selection;
    
//     <graphid, displaylistid>
    QMap<QString, GLuint> m_displayLists;
};

#endif


