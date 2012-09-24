/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "plotter3d.h"

#include "surface.h"
#include "spacecurve.h"

#include "plotsmodel.h"
#include "private/utils/box3d.h"
#include "private/utils/triangle3d.h"

#include <QApplication>
#include <cmath>
#include <QDebug>
#include <KLocalizedString>
#include <KColorUtils>
#include <GL/gl.h>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

Q_DECLARE_METATYPE(PlotItem*);

using namespace std;

// #define DEBUG_GRAPH

Plotter3D::Plotter3D(QAbstractItemModel* model)
    : m_model(model)
{}

Plotter3D::~Plotter3D()
{}

void Plotter3D::initGL()
{
//     qglClearColor(Qt::black);
//
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
//     glEnable(GL_NORMALIZE);
    glEnable(GL_POLYGON_SMOOTH);
//
    glShadeModel(GL_SMOOTH);
//     glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
//     static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
//     glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    /*
        //Texture initialisation :
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, 3, tex1.width(), tex1.height(), 0,
        GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits() );
     */
    /// For drawing Filled Polygones :
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    glFrontFace (GL_CCW);
//     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LocalScene.frontcol);
//     glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, LocalScene.backcol);
//     glMaterialf (GL_FRONT, GL_SHININESS, 35.0);
//     glMaterialf (GL_BACK, GL_SHININESS, 35.0);
// 
//     glEnable(GL_DEPTH_TEST);
//     glClearColor(LocalScene.groundcol[0], LocalScene.groundcol[1],LocalScene.groundcol[2], LocalScene.groundcol[3]);
// 
//     //pasar a draw para efecto de transparente a current surface plot
//     /// For drawing Lines :
//     if(LocalScene.smoothline == 1) {
//         glEnable (GL_LINE_SMOOTH);
//         glEnable (GL_BLEND);
//         glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
//     }
//     else {
//         glDisable(GL_LINE_SMOOTH);
//         glDisable(GL_BLEND);
//     }

    if (m_sceneObjects.contains(Axes)) 
        glDeleteLists(m_sceneObjects.value(Axes).first, 1);
    
    m_sceneObjects[Axes] = qMakePair(glGenLists(1), true);
    glNewList(m_sceneObjects.value(Axes).first, GL_COMPILE );
    glLineWidth(1);
    glBegin( GL_LINES );
    glColor3f (1., 0., 0.);
    glVertex3f( 400.0, 0.0, 0.0);
    glVertex3f(   0.0, 0.0, 0.0);
    glColor3f (0., 1., 0.);
    glVertex3f(0.0, 400.0, 0.0);
    glVertex3f(0.0,   0.0, 0.0);
    glColor3f (0., 0., 1.);
    glVertex3f(0.0, 0.0, 400.0);
    glVertex3f(0.0, 0.0,   0.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f (0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 400.0);
    glVertex3f(10.0, 0.0, 380.0);
    glVertex3f(0.0, 10.0, 380.0);
    glColor3f (0.0, 0.0, 0.3);
    glVertex3f(-10.0, 0.0, 380.0);
    glColor3f (0.0, 0.0, 1.0);
    glVertex3f(0.0, -10.0, 380.0);
    glColor3f (0.0, 0.0, 0.3);
    glVertex3f(10.0, 0.0, 380.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f (0.0, 1., 0.);
    glVertex3f(0.0, 400.0, 0.0);
    glVertex3f(10.0, 380.0, 0.0);
    glVertex3f(0.0, 380.0, 10.0);
    glColor3f (0.0, 0.3, 0.0);
    glVertex3f(-10.0, 380.0, 0.0);
    glColor3f (0.0, 1.0, 0.0);
    glVertex3f(.0, 380.0, -10.0);
    glColor3f (0.0, 0.3, 0.0);
    glVertex3f(10.0, 380.0, 0.0);
    glEnd();

/// Axe X :
    glBegin(GL_TRIANGLE_FAN);
    glColor3f (1.0, 0.0, 0.0);
    glVertex3f(400.0, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glVertex3f(380.0, 0.0, 10.0);
    glColor3f (0.3, 0.0, 0.0);
    glVertex3f(380.0, -10.0, 0.0);
    glColor3f (1.0, 0.0, 0.0);
    glVertex3f(380.0, 0.0, -10.0);
    glColor3f (0.3, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glEnd();

    glColor3f (1.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(400.0, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glVertex3f(380.0, 0.0, 10.0);
    glVertex3f(380.0, -10.0, 0.0);
    glVertex3f(380.0, 0.0, -10.0);
    glVertex3f(380.0, 10.0, 0.0);
    glEnd();

    glColor3f (1., 0., 0.);
    glRasterPos3i(410, 10, 10);
    glCallLists(strlen("X"),GL_UNSIGNED_BYTE,"X");

    glColor3f (0.7, 0.7, 0.7);
    glTranslatef(410.0, 4.0, 4.0);
    //gluSphere(sphereObj, .0, 16, 16);
    glTranslatef(-410.0, -4.0, -4.0);

    glColor3f (0., 1., 0.);
    glRasterPos3i(10, 410, 10);
    //printString("Y");
    glCallLists(strlen("Y"),
                GL_UNSIGNED_BYTE,
                "Y");

    glColor3f (1., 1., 0.);
    glTranslatef(4.0, 410.0, 4.0);
    //gluSphere(sphereObj, 8.0, 32, 32);
    glTranslatef(-4.0, -410.0, -4.0);

    glColor3f (0., 0., 1.);
    glRasterPos3i(10, 10, 410);
    //printString("Z");
    glCallLists(strlen("Z"),
                GL_UNSIGNED_BYTE,
                "Z");

    glColor3f (0., 0.7, 0.7);
    glTranslatef(4.0, 4.0, 410.0);
    //gluSphere(sphereObj, 10.0, 16, 16);
    glTranslatef(-4.0, -4.0, -410.0);
    glLineWidth(0.9);
    glEndList();

    if (m_sceneObjects.contains(RefPlaneXY)) 
        glDeleteLists(m_sceneObjects.value(RefPlaneXY).first, 1);
    
    m_sceneObjects[RefPlaneXY] = qMakePair(glGenLists(1), true);

    glNewList(m_sceneObjects.value(RefPlaneXY).first, GL_COMPILE );
    glLineWidth(1);
    glColor3f (0.8, 0., 0.7);
    glBegin( GL_LINES );
    glVertex3f(-150.0, 600.0, m_depth);
    glVertex3f(-150.0,-600.0, m_depth);
    glVertex3f(0.0, 600.0, m_depth);
    glVertex3f(0.0,-600.0, m_depth);

    glVertex3f(150.0, 600.0, m_depth);
    glVertex3f(150.0,-600.0, m_depth);
    glVertex3f(600.0, -150.0, m_depth);
    glVertex3f(-600.0,-150.0, m_depth);

    glVertex3f(600.0, 0.0, m_depth);
    glVertex3f(-600.0, 0.0, m_depth);
    glVertex3f(600.0, 150.0, m_depth);
    glVertex3f(-600.0, 150.0, m_depth);

    glVertex3f(-75.0, 600.0, m_depth);
    glVertex3f(-75.0,-600.0, m_depth);
    glVertex3f(-225.0, 600.0, m_depth);
    glVertex3f(-225.0,-600.0, m_depth);
    glVertex3f(-300.0, 600.0, m_depth);
    glVertex3f(-300.0,-600.0, m_depth);
    glVertex3f(-375.0, 600.0, m_depth);
    glVertex3f(-375.0,-600.0, m_depth);
    glVertex3f(-450.0, 600.0, m_depth);
    glVertex3f(-450.0,-600.0, m_depth);
    glVertex3f(-525.0, 600.0, m_depth);
    glVertex3f(-525.0,-600.0, m_depth);

    glVertex3f(75.0, 600.0, m_depth);
    glVertex3f(75.0,-600.0, m_depth);
    glVertex3f(225.0, 600.0, m_depth);
    glVertex3f(225.0,-600.0, m_depth);
    glVertex3f(300.0, 600.0, m_depth);
    glVertex3f(300.0,-600.0, m_depth);
    glVertex3f(375.0, 600.0, m_depth);
    glVertex3f(375.0,-600.0, m_depth);
    glVertex3f(450.0, 600.0, m_depth);
    glVertex3f(450.0,-600.0, m_depth);
    glVertex3f(525.0, 600.0, m_depth);
    glVertex3f(525.0,-600.0, m_depth);

    glVertex3f(600.0,-75.0, m_depth);
    glVertex3f(-600.0,-75.0, m_depth);
    glVertex3f(600.0,-225.0, m_depth);
    glVertex3f(-600.0,-225.0, m_depth);
    glVertex3f(600.0,-300.0, m_depth);
    glVertex3f(-600.0,-300.0, m_depth);
    glVertex3f(600.0,-375.0, m_depth);
    glVertex3f(-600.0,-375.0, m_depth);
    glVertex3f(600.0,-450.0, m_depth);
    glVertex3f(-600.0,-450.0, m_depth);
    glVertex3f(600.0,-525.0, m_depth);
    glVertex3f(-600.0,-525.0, m_depth);

    glVertex3f(600.0,75.0, m_depth);
    glVertex3f(-600.0,75.0, m_depth);
    glVertex3f(600.0,225.0, m_depth);
    glVertex3f(-600.0,225.0, m_depth);
    glVertex3f(600.0,300.0, m_depth);
    glVertex3f(-600.0,300.0, m_depth);
    glVertex3f(600.0,375.0, m_depth);
    glVertex3f(-600.0,375.0, m_depth);
    glVertex3f(600.0,450.0, m_depth);
    glVertex3f(-600.0,450.0, m_depth);
    glVertex3f(600.0,525.0, m_depth);
    glVertex3f(-600.0,525.0, m_depth);
    glEnd();
    glLineWidth(0.9);
    glEndList();
}

void Plotter3D::setViewport(const QRect& vp)
{

}

void Plotter3D::drawPlots()
{

}

void Plotter3D::updatePlots(const QModelIndex & parent, int start, int end)
{

}

void Plotter3D::setModel(QAbstractItemModel* f)
{
    m_model=f;
    modelChanged();
}

PlotItem* Plotter3D::itemAt(int row) const
{
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim3D)
        return 0;

    return plot;
}
