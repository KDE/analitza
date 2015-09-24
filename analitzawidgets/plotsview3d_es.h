/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
 *  Copyright (C) 2007 by Abderrahman Taha: Basic OpenGL calls like scene, lights    *
 *                                          and mouse behaviour taken from K3DSurf   *
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

#ifndef PLOTSVIEW3DES_H
#define PLOTSVIEW3DES_H

#ifdef _WIN32
#include <GL/glew.h>
#endif

#include <QOpenGLWidget>
#include <QMouseEvent>

#include <QModelIndex>
#include "analitzawidgets_export.h"
#include <analitzaplot/plotter3d.h>

class QItemSelectionModel;

namespace Analitza
{
class Surface;
class PlotItem;

/**
 * \class PlotsView3DES
 * 
 * \ingroup AnalitzaGUIModule
 *
 * \brief Widget that allows visualization of 3D plots.
 *
 * This class lets you create a widget that can draw multiple 3D graphs. This widget 
 * use Plotter3D and OpenGL as a backend.
 */

class ANALITZAWIDGETS_EXPORT PlotsView3DES : public  QOpenGLWidget, public Plotter3D
{
    Q_OBJECT

public:
    PlotsView3DES(QWidget* parent = 0);
    virtual ~PlotsView3DES();

    void setSelectionModel(QItemSelectionModel* selection);

public Q_SLOTS:
    void resetView();
    
private Q_SLOTS:
    void updateFuncs(const QModelIndex &indexf,const QModelIndex &indext);
    void addFuncs(const QModelIndex &index,int,int);
    void removeFuncs(const QModelIndex &index,int,int);

private:
    virtual int currentPlot() const { return -1 ;}
    virtual void modelChanged();
    virtual void renderGL();

    virtual void keyPressEvent(QKeyEvent*);
    
    virtual void wheelEvent(QWheelEvent* ev);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    
    virtual void paintGL();
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    
    QItemSelectionModel* m_selection;
    
    Qt::MouseButtons buttons;
    double old_x, old_y;
};

}

#endif
