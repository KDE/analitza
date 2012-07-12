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


#include "plotview3d.h"
#include "private/functiongraphsmodel.h"
#include "surface.h"
#include "mathutils.h"
#include <QVector3D>
#include <QVector2D>
#include <qitemselectionmodel.h>
#include <QVector>
#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variables.h"
#include "analitza/vector.h"
#include <QDebug>
#include <GL/glu.h>

enum ActionsEnum {
    KEYRIGHT=1<<0, KEYLEFT=1<<1, KEYUP=1<<2, KEYDOWN=1<<3, KEYAVPAG=1<<4, KEYREPAG=1<<5, KEYS=1<<6, KEYW=1<<7,
    KEYQ=1<<8, KEYE=1<<9, LCLICK=1<<10, RCLICK=1<<11, MCLICK=1<<12 };

View3D::View3D(QWidget *parent)
    : QGLWidget(parent)
    ,m_model(0), m_selection(0),default_step(0.15f), default_size(10.0f), zoom(1.0f), alpha(60.)
{
//     setGridIsDrawn(true);
//     setAxisIsDrawn(true);
    setFocusPolicy(Qt::ClickFocus);
    rotation[0] = 90.0;
    rotation[1] = 0.0;
    rotation[2] = 0.0;
}

View3D::View3D(VisualItemsModel* m, QWidget* parent): QGLWidget(parent)
    ,m_model(m), m_selection(0),default_step(0.15f), default_size(10.0f), zoom(1.0f), alpha(60.)
{
//     setGridIsDrawn(true);
//     setAxisIsDrawn(true);
    setFocusPolicy(Qt::ClickFocus);
    rotation[0] = 90.0;
    rotation[1] = 0.0;
    rotation[2] = 0.0;
}

void View3D::setModel(VisualItemsModel *model)
{
    m_model=model;

    //TODO disconnect prev model
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
        this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
        this, SLOT(addFuncs(QModelIndex,int,int)));
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
        this, SLOT(removeFuncs(QModelIndex,int,int)));
    
    updateGL();
}

void View3D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection);
    Q_ASSERT(selection->model() == m_model);
    
    
    m_selection = selection;
    connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(forceRepaint()));
}

void View3D::addFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(start == end); // siempre se agrega un solo item al model

    
    Surface* surf = static_cast<Surface*>(m_model->item(start));
    surf->update(Box());
        
    GLuint dlid = glGenLists(1);
    m_displayLists[surf] = dlid;

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

    //BEGIN display list
    glNewList(dlid, GL_COMPILE);

    // set specular and shiniess using glMaterial (gold-yellow)
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//     glColor3fv(diffuseColor);
    glColor3ub(surf->color().red(), surf->color().green(), surf->color().blue());

    foreach (const Face &face, surf->faces())
    {
        glBegin(GL_TRIANGLES);
        glNormal3d(face.normal.x(), face.normal.y(), face.normal.z());
        glVertex3d(face.p1.x(), face.p1.y(), face.p1.z());
        glVertex3d(face.p2.x(), face.p2.y(), face.p2.z());
        glVertex3d(face.p3.x(), face.p3.y(), face.p3.z());
        glEnd();
    }

    glEndList();
    //END display list
    
}

void View3D::removeFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(start == end); // siempre se agrega un solo item al model
    
    glDeleteLists(m_displayLists[m_model->item(start)], 1);
}

void View3D::updateFuncs(const QModelIndex& start, const QModelIndex& end)
{
//     updateFunctions(start, end);
}

int View3D::currentFunction() const
{
    if (!m_model) return -1; // guard
    
    int ret=-1;
    if(m_selection) {
        ret=m_selection->currentIndex().row();
    }
    
    return ret;
}


void View3D::initializeGL()
{
    
///
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
// //  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//     glShadeModel(GL_SMOOTH);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
//     QImage t, b;
//     GLuint texture[1];
//     b = QImage(8, 8, QImage::Format_Mono);
//     b.fill(QColor(0xff, 0xff, 0xff).rgb());
//     for(int i=0; i<b.height(); i++)
//         b.setPixel(4, i, 0);
//     
//     for(int i=0; i<b.width(); i++)
//         b.setPixel(i, 4, 0);
//         
//     t = QGLWidget::convertToGLFormat( b );
//     glGenTextures( 1, &texture[0] );
//     glBindTexture( GL_TEXTURE_2D, texture[0] );
//     glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
//     
//     glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
///
        glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.1, 2.0, 3.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    
    
}

void View3D::resizeGL(int width, int height)
{
    height = height ? height : 1;
    
    glViewport( 0, 0, width, height );
    
}

void View3D::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        press = e->pos();
        keyspressed |= LCLICK;
    }
}

void View3D::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        keyspressed &= ~LCLICK;
}

void View3D::mouseMoveEvent(QMouseEvent *e)
{
    if(keyspressed & LCLICK){
        QPoint rel = e->pos() - press;
        rotation[0] += rel.y();
        rotation[2] -= rel.x();
        
        press = e->pos();
        repaint();
    }
}


void View3D::drawAxes()
{
    glColor3f(0.8, 0.8, 0.4);
    renderText(11.0, 0.0, 0.0, "Y");
    renderText(0.0, 11.0, 0.0, "X");
    renderText(0.0, 0.0,-11.0, "Z");
    
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-10.0f, 0.0f, 0.0f);
            glVertex3f( 10.0f, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f( 0.0f, 10.0f, 0.0f);
            glVertex3f( 0.0f,-10.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f( 0.0f, 0.0f, 10.0f);
            glVertex3f( 0.0f, 0.0f,-10.0f);
    glEnd();
}

void View3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(keyspressed & KEYDOWN)   rotation[0]+=3.f;
    if(keyspressed & KEYUP)     rotation[0]-=3.f;
    if(keyspressed & KEYAVPAG)  rotation[1]+=3.f;
    if(keyspressed & KEYREPAG)  rotation[1]-=3.f;
    if(keyspressed & KEYLEFT)   rotation[2]+=3.f;
    if(keyspressed & KEYRIGHT)  rotation[2]-=3.f;
    if(keyspressed & KEYW)      alpha/=2.f;
    if(keyspressed & KEYS && alpha<=90.f)   alpha*=2.f;
    if(keyspressed & KEYQ)      { zoom/=2.0f;  }
    if(keyspressed & KEYE)      { zoom*=2.0f;  }
    
    if(rotation[0]>=360.f) rotation[0]-=360.f;
    if(rotation[1]>=360.f) rotation[1]-=360.f;
    if(rotation[2]>=360.f) rotation[2]-=360.f;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(alpha,(GLfloat)width()/(GLfloat)height(),0.1f,100.0f);
    
    float z=25.f;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-z);
    glRotatef(rotation[0], 1.0, 0.0, 0.0);
    glRotatef(rotation[1], 0.0, 1.0, 0.0);
    glRotatef(rotation[2], 0.0, 0.0, 1.0);
        drawAxes();
    foreach (VisualItem *item, m_displayLists.keys())
    {
        glCallList(m_displayLists[item]);
//         qDebug() << itemid;
    }
    
    glFlush();
}

QPixmap View3D::toPixmap()
{
    return renderPixmap();
}

void View3D::wheelEvent(QWheelEvent * e)
{
    if(e->delta()>0)
        alpha/=2.f;
    else if(alpha<=90.f)
        alpha*=2.f;
    repaint();
}


void View3D::keyPressEvent( QKeyEvent *e )
{
    switch(e->key()) {
        case Qt::Key_Up:
            keyspressed |= KEYUP;
            break;
        case Qt::Key_Down:
            keyspressed |= KEYDOWN;
            break;
        case Qt::Key_Left:
            keyspressed |= KEYLEFT;
            break;
        case Qt::Key_Right:
            keyspressed |= KEYRIGHT;
            break;
        case Qt::Key_PageUp:
            keyspressed |= KEYREPAG;
            break;
        case Qt::Key_PageDown:
            keyspressed |= KEYAVPAG;
            break;
        case Qt::Key_W:
            keyspressed |= KEYW;
            break;
        case Qt::Key_S:
            keyspressed |= KEYS;
            break;
        case Qt::Key_Q: //Be careful
            keyspressed |= KEYQ;
            break;
        case Qt::Key_E: //Be careful
            keyspressed |= KEYE;
            break;
    }
//  sendStatus(QString("-%1-").arg(keyspressed, 16));
    repaint();
}


void View3D::keyReleaseEvent( QKeyEvent *e )
{
    switch(e->key()) {
        case Qt::Key_Up:
            keyspressed &= ~KEYUP;
            break;
        case Qt::Key_Down:
            keyspressed &= ~KEYDOWN;
            break;
        case Qt::Key_Left:
            keyspressed &= ~KEYLEFT;
            break;
        case Qt::Key_Right:
            keyspressed &= ~KEYRIGHT;
            break;
        case Qt::Key_PageUp:
            keyspressed &= ~KEYREPAG;
            break;
        case Qt::Key_PageDown:
            keyspressed &= ~KEYAVPAG;
            break;
        case Qt::Key_W:
            keyspressed &= ~KEYW;
            break;
        case Qt::Key_S:
            keyspressed &= ~KEYS;
            break;
        case Qt::Key_Q: //Be careful
            keyspressed &= ~KEYQ;
            break;
        case Qt::Key_E: //Be careful
            keyspressed &= ~KEYE;
            break;
        
    }
//  sendStatus(QString(".%1.").arg(keyspressed, 16));
    repaint();
}

void View3D::timeOut()
{
    rotation[0] += 20.0;
    rotation[1] += 20.0;
    rotation[2] += 20.0;
    repaint();
}

void View3D::setSize(double newSize)
{
    default_size = newSize;
    repaint();
}

void View3D::resetView()
{
    default_step=0.15f;
    default_size=8.0f;
    if(zoom!=1.0f) {
        zoom=1.0f;
    }
    alpha=60.;
    
    rotation[0] = 90.0;
    rotation[1] = 0.0;
    rotation[2] = 0.0;
    updateGL();
}
