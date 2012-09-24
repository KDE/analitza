/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "plotter3d.h"

#include "surface.h"
#include "spacecurve.h"

#include "plotsmodel.h"
#include "private/utils/box3d.h"
#include "private/utils/triangle3d.h"
#include "private/utils/mathutils.h"

#include <QApplication>
#include <cmath>
#include <QDebug>
#include <KLocalizedString>
#include <KColorUtils>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

Q_DECLARE_METATYPE(PlotItem*);

using namespace std;

// #define DEBUG_GRAPH

Plotter3D::Plotter3D(QAbstractItemModel* model)
    : m_model(model)
    , m_rotStrength(0)
    , m_depth(-400)
    , m_rotx(90.0)
    , m_roty(90.0)
    , m_rotz(1.0)
{
}

Plotter3D::~Plotter3D()
{
    //asser geoms == model rowcount
    if (m_model && m_model->rowCount() > 0) {
        for (int i = 0; i < m_model->rowCount(); ++i) {
            glDeleteLists(m_itemGeometries.value(itemAt(i)), 1);
        }
    }

    glDeleteLists(m_sceneObjects.value(Axes).first, 1);
    glDeleteLists(m_sceneObjects.value(RefPlaneXY).first, 1);
    
}

void Plotter3D::initGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_SMOOTH);
//
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    /// For drawing Filled Polygones :
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    glFrontFace (GL_CCW);
    glMaterialf (GL_FRONT, GL_SHININESS, 35.0);
    glMaterialf (GL_BACK, GL_SHININESS, 35.0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0);

//TODO efecto de tranaparente al draw     //pasar a draw para efecto de transparente a current surface plot
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
//     glCallLists(strlen("X"),GL_UNSIGNED_BYTE,"X");

    glColor3f (0.7, 0.7, 0.7);
    glTranslatef(410.0, 4.0, 4.0);
    //gluSphere(sphereObj, .0, 16, 16);
    glTranslatef(-410.0, -4.0, -4.0);

    glColor3f (0., 1., 0.);
    glRasterPos3i(10, 410, 10);
    //printString("Y");
//     glCallLists(strlen("Y"), GL_UNSIGNED_BYTE, "Y");

    glColor3f (1., 1., 0.);
    glTranslatef(4.0, 410.0, 4.0);
    //gluSphere(sphereObj, 8.0, 32, 32);
    glTranslatef(-4.0, -410.0, -4.0);

    glColor3f (0., 0., 1.);
    glRasterPos3i(10, 10, 410);
    //printString("Z");
//     glCallLists(strlen("Z"), GL_UNSIGNED_BYTE, "Z");

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
    
    GLfloat ambient[] = { .0, .0, .0, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat local_view[] = { 1.0 };
    static GLfloat position[4] = {0.0, 0.0,1000.0, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
}

void Plotter3D::setViewport(const QRect& vp)
{
    static int CornerH, CornerW;
    static int heightresult, widthresult;

    int newwidth = vp.size().width();
    int newheight = vp.size().height();

    int tmp, starth, startw;
    //GLdouble mm[16];
    //glGetDoublev(GL_MODELVIEW_MATRIX,mm);
    CornerH = (int)(newheight/2);
    CornerW = (int)(newwidth/2);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(newwidth > newheight) {
        tmp = newwidth;
        starth=(newwidth-newheight)/4;
        startw=0;
    }
    else {
        tmp = newheight;
        startw = (newheight-newwidth)/4;
        starth=0;
    }

    glViewport(vp.left(), vp.top(), tmp, tmp);
    glFrustum(-250+startw, 250+startw, -250+starth, 250+starth, 350.0, 3000.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -800.0 );
    glRotatef(79,-13,-2,6);

    //glMultMatrixd(mm);

    heightresult = tmp/2;
    widthresult = 250+starth;

    m_viewport = QRect(vp.topLeft(), QSize(tmp, tmp));
}

void Plotter3D::drawPlots()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Object Drawing :
    glCallList(m_sceneObjects.value(Axes).first);
    glCallList(m_sceneObjects.value(RefPlaneXY).first);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0); //NOTE we can parametrized this args as members

    glPushMatrix();
    static const double ss = 40; // space size : with scale factor 0 - 80 -> esto hace que cada eje este visible en [0,+/-10]
    glScalef(ss, ss, ss);

    if(!m_model)
    {
        glPopMatrix();
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    
        return ;
    }
    
    //NOTE si esto pasa entonces quiere decir que el proxy empezado a filtrar otros items
    // y si es asi borro todo lo que esta agregado al la memoria de la tarjeta
    //esto se hace pues m_itemGeometries es una copia del estado actual de model
    if (m_model->rowCount() != m_itemGeometries.count())
    {
        foreach (PlotItem *item, m_itemGeometries.keys())
        {
            glDeleteLists(m_itemGeometries.take(item), 1);
        }
    }

    /// luego paso a verificar el map de display list no este vacio ... si lo esta lo reconstruyo

    if (m_itemGeometries.isEmpty())
    {
        //NOTE no llamar a ninguna funcion que ejucute un updategl, esto para evitar una recursividad
        for (int i = 0; i < m_model->rowCount(); ++i) {
            PlotItem* item = dynamic_cast<Surface*>(itemAt(i));

            if (item && item->isVisible())
                addFuncsInternalA(item);
        }
    }

    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        PlotItem *item = itemAt(i);

        if (!item || item->spaceDimension() != Dim3D || !item->isVisible())
            continue;

        //TODO el color ya no se configura en la dlista, en la dlist solo va la geometria
        GLfloat  fcolor[4] = {float(item->color().redF()), float(item->color().greenF()), float(item->color().blueF()), 1.0f};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
        glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, fcolor);

        glCallList(m_itemGeometries[item]);
    }

    glPopMatrix();
// 
// //     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LocalScene.frontcol);
// //     glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, LocalScene.backcol);
// 
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

//     if(scene->line == 1) {
//         glColor4f (scene->gridcol[0], scene->gridcol[1], scene->gridcol[2], scene->gridcol[3]);
//         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//         (scene->typedrawing == 1) ?
//         glDrawElements(GL_TRIANGLES, scene->PolyNumber, GL_UNSIGNED_INT, scene->PolyIndices_localPt)
//         :
//         glDrawElements(GL_QUADS, scene->PolyNumber, GL_UNSIGNED_INT, scene->PolyIndices_localPt);
//         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//     }
}

void Plotter3D::updatePlots(const QModelIndex & parent, int s, int e)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(parent);
    
    if (m_model->rowCount() == 0)
    {
        for(int i=s; i<=e; i++) {
            PlotItem *item = itemAt(i);
            
            if (item && item->spaceDimension() == Dim3D && item->isVisible()) {
                qDebug() << item->name();
                addFuncsInternalA(item);
            }
        }

        renderGL();
        
        return ;
    }
    
    for(int i=s; i<=e; i++) {
        PlotItem *item = itemAt(i);

        if (!item)
            return;

        glDeleteLists(m_itemGeometries[item], 1);

        if (item->isVisible()) {
    //         addFuncs(QModelIndex(), s.row(), s.row());
    //igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data
            addFuncsInternalA(item);
        }
    }

    renderGL();
}

void Plotter3D::setModel(QAbstractItemModel* f)
{
    m_model=f;

    modelChanged();
}

void Plotter3D::scale(GLdouble factor)
{
    m_scale = factor;

    glScalef(m_scale, m_scale, m_scale);

    renderGL();
}

void Plotter3D::rotate(int xshift, int yshift)
{
    static double anglefinal = 0;
    static double angle = 0;

    GLdouble viewRoty = static_cast<GLdouble>(-yshift);
    GLdouble viewRotx = static_cast<GLdouble>(-xshift);
    m_scale = 1.0;

    GLdouble matrix[16] = {0}; // model view matrix from current OpenGL state

    glGetDoublev(GL_MODELVIEW_MATRIX, matrix);

    GLdouble det; // determinant
    GLdouble d12, d13, d23, d24, d34, d41;
    GLdouble tmp[16] = {0}; // Allow out == in

    // Inverse = adjoint / det. (See linear algebra texts.)
    // pre-compute 2x2 dets for last two rows when computing
    // cofactors of first two rows.

    //column 1
    GLdouble m11 = matrix[0*4 + 0];
    GLdouble m21 = matrix[0*4 + 1];
    GLdouble m31 = matrix[0*4 + 2];
    GLdouble m41 = matrix[0*4 + 3];

    //column 2
    GLdouble m12 = matrix[1*4 + 0];
    GLdouble m22 = matrix[1*4 + 1];
    GLdouble m32 = matrix[1*4 + 2];
    GLdouble m42 = matrix[1*4 + 3];

    //column 3
    GLdouble m13 = matrix[2*4 + 0];
    GLdouble m23 = matrix[2*4 + 1];
    GLdouble m33 = matrix[2*4 + 2];
    GLdouble m43 = matrix[2*4 + 3];

    //column 4
    GLdouble m14 = matrix[3*4 + 0];
    GLdouble m24 = matrix[3*4 + 1];
    GLdouble m34 = matrix[3*4 + 2];
    GLdouble m44 = matrix[3*4 + 3];

    d12 = m31*m42-m41*m32;
    d13 = m31*m43-m41*m33;
    d23 = m32*m43-m42*m33;
    d24 = m32*m44-m42*m34;
    d34 = m33*m44-m43*m34;
    d41 = m34*m41-m44*m31;

    tmp[0] =  (m22 * d34 - m23 * d24 + m24 * d23);
    tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
    tmp[2] =  (m21 * d24 + m22 * d41 + m24 * d12);
    tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

    // Compute determinant as early as possible using these cofactors.
    det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

    // Run singularity test: if det != 0 is not a singular matrix, then all calculations are safe
    if (!isSimilar(det, 0.0)) {
        GLdouble invDet = 1.0 / det;
        // Compute rest of inverse.
        tmp[0] *= invDet;
        tmp[1] *= invDet;
        tmp[2] *= invDet;
        tmp[3] *= invDet;

        tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
        tmp[5] =  (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
        tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
        tmp[7] =  (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

        // Pre-compute 2x2 dets for first two rows when computing
        // cofactors of last two rows.
        d12 = m11*m22-m21*m12;
        d13 = m11*m23-m21*m13;
        d23 = m12*m23-m22*m13;
        d24 = m12*m24-m22*m14;
        d34 = m13*m24-m23*m14;
        d41 = m14*m21-m24*m11;

        tmp[8] =  (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
        tmp[9] = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
        tmp[10] =  (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
        tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
        tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
        tmp[13] =  (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
        tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
        tmp[15] =  (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

        double invMatrix[16] = {0}; // inverse of matrix

        memcpy(invMatrix, tmp, 16*sizeof(GLdouble));

        double ax,ay,az;
        ax = viewRoty;
        ay = viewRotx;
        az = 0.0;
        anglefinal += (angle = sqrt(ax*ax + ay*ay)/(double)(m_viewport.width() + 1)*360.0);

        // Use inverse matrix to determine local axis of rotation
        m_rotx = invMatrix[0]*ax + invMatrix[4]*ay;
        m_roty = invMatrix[1]*ax + invMatrix[5]*ay;
        m_rotz = invMatrix[2]*ax + invMatrix[6]*ay;

        glRotatef(angle, m_rotx, m_roty, m_rotz);

        renderGL();
    }
}

void Plotter3D::addFuncsInternalA(PlotItem* item)
{
    Q_ASSERT(item);

    if (Surface* surf = dynamic_cast<Surface*>(item))
    {
        if (surf->faces().isEmpty())
            surf->update(Box3D());
    }
    else if (SpaceCurve* c = dynamic_cast<SpaceCurve*>(item))
    {
        if (c->points().isEmpty())
            c->update(Box3D());
    }

    GLuint dlid = glGenLists(1);
    //WARNING if same item has dlist then delete it
    m_itemGeometries[item] = dlid;

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

    //BEGIN display list
    glNewList(dlid, GL_COMPILE);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3fv(diffuseColor);
    glColor3ub(item->color().red(), item->color().green(), item->color().blue());

    if (SpaceCurve *curve = dynamic_cast<SpaceCurve*>(item))
    {
        glEnable(GL_LINE_SMOOTH);

        glLineWidth(2.5);

        glBegin(GL_LINES);
        {
            const QVector<QVector3D> points = curve->points();

            //TODO copy approach from plotter 2d and use jumps optimization
            for (int i = 0; i < points.size() -1 ; ++i)
            {
                glVertex3d(points[i].x(), points[i].y(), points[i].z());
                glVertex3d(points[i+1].x(), points[i+1].y(), points[i+1].z());
            }
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);
    }
    else
    {
        Surface* surf = static_cast<Surface*>(item);

        foreach (const Triangle3D &face, surf->faces())
        {
            glBegin(GL_TRIANGLES);
            QVector3D n;

            //TODO no magic numbers
            if (!face.faceNormal().isNull()) n= face.faceNormal().normalized();
            else n = QVector3D(0.5, 0.1, 0.2).normalized();

            glNormal3d(n.x(), n.y(), n.z());
            glVertex3d(face.p().x(), face.p().y(), face.p().z());
            glVertex3d(face.q().x(), face.q().y(), face.q().z());
            glVertex3d(face.r().x(), face.r().y(), face.r().z());
            glEnd();
        }
    }

    glEndList();
    //END display list
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
