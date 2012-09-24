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
#include "private/utils/mathutils.h"

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
{
    
}

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

    //glMultMatrixd(mm);

    heightresult = tmp/2;
    widthresult = 250+starth;
    
    m_viewport = QRect(vp.topLeft(), QSize(tmp, tmp));
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

void Plotter3D::scale(GLdouble factor)
{

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
    }
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
