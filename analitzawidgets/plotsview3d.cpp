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

#include "plotsview3d.h"
#include <analitzaplot/plotsmodel.h>
#include <analitzaplot/surface.h>
#include <analitzaplot/spacecurve.h>
#include <QVector3D>
#include <QItemSelectionModel>
#include <QVector>
#include <QDebug>

using namespace Analitza;

PlotsView3D::PlotsView3D(QWidget *parent)
    : QGLWidget(parent), m_selection(0), old_x(-1), old_y(-1)
{
    setFocusPolicy(Qt::ClickFocus);
}

PlotsView3D::~PlotsView3D()
{}

void PlotsView3D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection);

    m_selection = selection;
//     connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(forceRepaint()));
}

void PlotsView3D::resetView()
{
    //TODO remove qglviewer
//     setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));

//     camera()->setUpVector(qglviewer::Vec(-1,-1,1), false);
//     camera()->setPosition(qglviewer::Vec(7,7,7));
//     camera()->lookAt(qglviewer::Vec(0,0,0));
//     camera()->setRevolveAroundPoint(qglviewer::Vec(0,0,0));
}

void PlotsView3D::addFuncs(const QModelIndex & parent, int start, int end)
{
    updatePlots(parent, start, end);
}

void PlotsView3D::removeFuncs(const QModelIndex & parent, int start, int end)
{
    updatePlots(parent, start, end);
}

//NOTE
//si hay un cambio aki es desetvisible (que no es necesario configurar en el plotitem) o es del
//setinterval (que si es necesario configurarlo en el plotitem)
//el enfoque es: si hay un cambio borro el displaylist y lo genero de nuevo (no lo genero si el item no es visible)
//TODO cache para exp e interval ... pues del resto es solo cuestion de update
void PlotsView3D::updateFuncs(const QModelIndex& start, const QModelIndex& end)
{
    updatePlots(QModelIndex(), start.row(), end.row());
}

void PlotsView3D::paintGL()
{
    drawPlots();
}

void PlotsView3D::initializeGL()
{
    initGL();
}

void PlotsView3D::resizeGL(int newwidth, int newheight)
{
    setViewport(QRectF(0,0,newwidth,newheight));
}

void PlotsView3D::mousePressEvent(QMouseEvent *e)
{
    buttons = e->buttons();

    old_y = e->y();
    old_x = e->x();
    CartesianAxis axis = selectAxisArrow(e->x(), e->y());
    showAxisArrowHint(axis);

    if (isRotationFixed() && axis != InvalidAxis) {
        fixRotation(QVector3D());
        hideAxisHint();
    } else switch (axis)
    {
        case XAxis: 
            fixRotation(QVector3D(1,0,0));
            break;
        case YAxis: 
            fixRotation(QVector3D(0,1,0));
            break;
        case ZAxis: 
            fixRotation(QVector3D(0,0,1));
            break;
        case InvalidAxis:
            break;
    }

}

void PlotsView3D::modelChanged()
{
//     if (model()) {
//         disconnect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
//         disconnect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
//         disconnect(model(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
//     }

//     for (int i = 0; i < model()->rowCount(); ++i) {
//         if (itemAt(i)) {
//             glDeleteLists(m_displayLists[itemAt(i)], 1);
//             addFuncs(QModelIndex(), 0, model()->rowCount()-1);
//         }
//     }

    connect(model(), &QAbstractItemModel::dataChanged, this, &PlotsView3D::updateFuncs);
    connect(model(), &QAbstractItemModel::rowsInserted, this, &PlotsView3D::addFuncs);
    connect(model(), &QAbstractItemModel::rowsRemoved, this, &PlotsView3D::removeFuncs);

    update();
}

void PlotsView3D::renderGL()
{
    update();
}

void PlotsView3D::wheelEvent(QWheelEvent* ev)
{
    scale(1.f-ev->delta()/1000.f);
}

void PlotsView3D::mouseMoveEvent(QMouseEvent *e)
{
//TODO translate with middle btn
// Rotational function :
    if(buttons & Qt::LeftButton)
        rotate(old_x - e->x(), old_y - e->y());

    old_y = e->y();
    old_x = e->x();
}

void PlotsView3D::keyPressEvent(QKeyEvent* ev)
{
    switch(ev->key()) {
        case Qt::Key_S:
            scale(1.1);
            break;
        case Qt::Key_W:
            scale(0.9);
            break;
        case Qt::Key_Left:
            rotate(-10, 0);
            break;
        case Qt::Key_Right:
            rotate(10, 0);
            break;
        case Qt::Key_Up:
            rotate(0, -10);
            break;
        case Qt::Key_Down:
            rotate(0, 10);
            break;
    }
}
