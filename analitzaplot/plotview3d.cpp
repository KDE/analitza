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
#include <QTime>
#include <GL/glu.h>


///



TrackBall::TrackBall(TrackMode mode)
    : m_angularVelocity(0)
    , m_paused(false)
    , m_pressed(false)
    , m_mode(mode)
{
    m_axis = QVector3D(0, 1, 0);
    m_rotation = QQuaternion();
    m_lastTime = QTime::currentTime();
}

TrackBall::TrackBall(float angularVelocity, const QVector3D& axis, TrackMode mode)
    : m_axis(axis)
    , m_angularVelocity(angularVelocity)
    , m_paused(false)
    , m_pressed(false)
    , m_mode(mode)
{
    m_rotation = QQuaternion();
    m_lastTime = QTime::currentTime();
}

void TrackBall::push(const QPointF& p, const QQuaternion &)
{
    m_rotation = rotation();
    m_pressed = true;
    m_lastTime = QTime::currentTime();
    m_lastPos = p;
    m_angularVelocity = 0.0f;
}

void TrackBall::move(const QPointF& p, const QQuaternion &transformation)
{
    if (!m_pressed)
        return;

    QTime currentTime = QTime::currentTime();
    int msecs = m_lastTime.msecsTo(currentTime);
    if (msecs <= 20)
        return;

    switch (m_mode) {
    case Plane:
    {
        QLineF delta(m_lastPos, p);
        m_angularVelocity = 180*delta.length() / (PI*msecs);
        m_axis = QVector3D(-delta.dy(), delta.dx(), 0.0f).normalized();
        m_axis = transformation.rotatedVector(m_axis);
        m_rotation = QQuaternion::fromAxisAndAngle(m_axis, 180 / PI * delta.length()) * m_rotation;
    }
    break;
    case Sphere:
    {
        QVector3D lastPos3D = QVector3D(m_lastPos.x(), m_lastPos.y(), 0.0f);
        float sqrZ = 1 - QVector3D::dotProduct(lastPos3D, lastPos3D);
        if (sqrZ > 0)
            lastPos3D.setZ(sqrt(sqrZ));
        else
            lastPos3D.normalize();

        QVector3D currentPos3D = QVector3D(p.x(), p.y(), 0.0f);
        sqrZ = 1 - QVector3D::dotProduct(currentPos3D, currentPos3D);
        if (sqrZ > 0)
            currentPos3D.setZ(sqrt(sqrZ));
        else
            currentPos3D.normalize();

        m_axis = QVector3D::crossProduct(lastPos3D, currentPos3D);
        float angle = 180 / PI * asin(sqrt(QVector3D::dotProduct(m_axis, m_axis)));

        m_angularVelocity = angle / msecs;
        m_axis.normalize();
        m_axis = transformation.rotatedVector(m_axis);
        m_rotation = QQuaternion::fromAxisAndAngle(m_axis, angle) * m_rotation;
    }
    break;
    }

    m_lastPos = p;
    m_lastTime = currentTime;
}

void TrackBall::release(const QPointF& p, const QQuaternion &transformation)
{
    // Calling move() caused the rotation to stop if the framerate was too low.
    move(p, transformation);
    m_pressed = false;
}

void TrackBall::start()
{
    m_lastTime = QTime::currentTime();
    m_paused = false;
}

void TrackBall::stop()
{
    m_rotation = rotation();
    m_paused = true;
}

QQuaternion TrackBall::rotation() const
{
    if (m_paused || m_pressed)
        return m_rotation;

    QTime currentTime = QTime::currentTime();
    float angle = m_angularVelocity * m_lastTime.msecsTo(currentTime);
    return QQuaternion::fromAxisAndAngle(m_axis, angle) * m_rotation;
}

///


enum ActionsEnum {
    KEYRIGHT=1<<0, KEYLEFT=1<<1, KEYUP=1<<2, KEYDOWN=1<<3, KEYAVPAG=1<<4, KEYREPAG=1<<5, KEYS=1<<6, KEYW=1<<7,
    KEYQ=1<<8, KEYE=1<<9, LCLICK=1<<10, RCLICK=1<<11, MCLICK=1<<12
};

View3D::View3D(QWidget *parent)
    : QGLWidget(parent)
    ,m_model(0), m_selection(0),default_step(0.15f), default_size(10.0f), zoom(1.0f), alpha(60.)
{

    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));


//     setGridIsDrawn(true);
//     setAxisIsDrawn(true);
    setFocusPolicy(Qt::ClickFocus);
    rotation[0] = 90.0;
    rotation[1] = 0.0;
    rotation[2] = 0.0;
//     mCamera = new Camera;

//     // Set up camera
//     camera()->setPosition(QVector3D(0, 0, 5));
//     camera()->setLookAt(QVector3D(0, 0, 0));
//     camera()->setUp(QVector3D(0, 1, 0));
//     camera()->setDepthRange(1, 100);
//     mRotation = 0;

///

    this->_trackball = TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);
}

View3D::View3D(VisualItemsModel* m, QWidget* parent): QGLWidget(parent)
    ,m_model(m), m_selection(0),default_step(0.15f), default_size(10.0f), zoom(1.0f), alpha(60.)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));


//     setGridIsDrawn(true);
//     setAxisIsDrawn(true);
    setFocusPolicy(Qt::ClickFocus);
    rotation[0] = 90.0;
    rotation[1] = 0.0;
    rotation[2] = 0.0;
//     mCamera = new Camera;


    // Set up camera
//     camera()->setPosition(QVector3D(0, 0, 5));
//     camera()->setLookAt(QVector3D(0, 0, 0));
//     camera()->setUp(QVector3D(0, 1, 0));
//     camera()->setDepthRange(1, 100);
//     mRotation = 0;

    ///

    this->_trackball = TrackBall(0.05f, QVector3D(0, 1, 0), TrackBall::Sphere);

}
View3D::~View3D()
{
//     delete mCamera;
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
    qglColor(Qt::black);
//     glEnable(GL_DEPTH_TEST);
// glEnable(GL_CULL_FACE);
//     glShadeModel(GL_SMOOTH);

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
//     glEnable(GL_DEPTH_TEST);
// 
    glShadeModel(GL_SMOOTH);
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    //static GLfloat lightPosition[4] = { 0.1, 2.0, 3.0, 1.0 };
    GLfloat lightPosition[] = { 2.0, 0.0, 8.0, 0.5};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);



}

void View3D::resizeGL(int width, int height)
{
    height = height ? height : 1;

//     camera()->setViewport(0, 0, width, height);
//     glViewport(camera()->viewport().x(),camera()->viewport().y(),camera()->viewport().width(), camera()->viewport().height());

    glViewport(0,0,width,height);

//     camera()->setAspect(width/(float)height);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     glMultMatrixf((GLfloat*)mCamera->projectionMatrix().data());
//     gluPerspective(alpha,(GLfloat)width()/(GLfloat)height(),0.1f,100.0f);

//     glMatrixMode(GL_MODELVIEW);
}



QPointF View3D::pixelPosToViewPos(const QPointF& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}



void View3D::mousePressEvent(QMouseEvent *e)
{
//     if(e->button() == Qt::LeftButton) {
//         press = e->pos();
//         keyspressed |= LCLICK;
//     }

//     lastMousePos = e->pos();

///

    if(e->buttons() & Qt::LeftButton)
    {
        this->_trackball.push(pixelPosToViewPos(e->posF()), QQuaternion());
//this->_trackball.push(pixelPosToViewPos(e->posF()), this->_trackball2.rotation().conjugate());
        e->accept();
    }
}

void View3D::mouseReleaseEvent(QMouseEvent *e)
{
//     if(e->button() == Qt::LeftButton)
//         keyspressed &= ~LCLICK;

///
    if(e->buttons() & Qt::LeftButton)
    {
        this->_trackball.release(pixelPosToViewPos(e->posF()),QQuaternion());
        e->accept();
    }
}

void View3D::mouseMoveEvent(QMouseEvent *e)
{
//     if(keyspressed & LCLICK){
//         QPoint rel = e->pos() - press;
//         rotation[0] += rel.y();
//         rotation[2] -= rel.x();
//
// //         press = e->pos();
// //         repaint();
// //     }
//
//
//     int dx = e->x() - lastMousePos.x();
//     int dy = e->y() - lastMousePos.y();
// //     if (e->buttons() & Qt::LeftButton) {
// //         trackball.rotate(dx, dy);
// //     }
//     lastMousePos = e->pos();


///

    if(e->buttons() & Qt::LeftButton)
    {
        this->_trackball.move(pixelPosToViewPos(e->posF()), QQuaternion());
        updateGL();
        e->accept();
    } else {
        this->_trackball.release(pixelPosToViewPos(e->posF()), QQuaternion());
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

static void multMatrix(const QMatrix4x4& m)
{
// static to prevent glMultMatrixf to fail on certain drivers
    static GLfloat mat[16];
    const qreal *data = m.constData();
    for (int index = 0; index < 16; ++index)
        mat[index] = data[index];
    glMultMatrixf(mat);
}

void View3D::paintGL()
{
//     glMultMatrixf((GLfloat*)trackball.transform(*camera()).modelviewMatrix().data());

// glRotatef(mRotation, 0, 1, 0);




    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

//     if(keyspressed & KEYDOWN)   rotation[0]+=3.f;
//     if(keyspressed & KEYUP)     rotation[0]-=3.f;
//     if(keyspressed & KEYAVPAG)  rotation[1]+=3.f;
//     if(keyspressed & KEYREPAG)  rotation[1]-=3.f;
//     if(keyspressed & KEYLEFT)   rotation[2]+=3.f;
//     if(keyspressed & KEYRIGHT)  rotation[2]-=3.f;
//     if(keyspressed & KEYW)      alpha/=2.f;
//     if(keyspressed & KEYS && alpha<=90.f)   alpha*=2.f;
//     if(keyspressed & KEYQ)      {
//         zoom/=2.0f;
//     }
//     if(keyspressed & KEYE)      {
//         zoom*=2.0f;
//     }

//     if(rotation[0]>=360.f) rotation[0]-=360.f;
//     if(rotation[1]>=360.f) rotation[1]-=360.f;
//     if(rotation[2]>=360.f) rotation[2]-=360.f;

//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluPerspective(alpha,(GLfloat)width()/(GLfloat)height(),0.1f,100.0f);

//     float z=25.f;
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();
//     glTranslatef(0,0,-z);
//     glRotatef(rotation[0], 1.0, 0.0, 0.0);
//     glRotatef(rotation[1], 0.0, 1.0, 0.0);
//     glRotatef(rotation[2], 0.0, 0.0, 1.0);

//     glMultMatrixf((GLfloat*)camera()->modelviewMatrix().data());

///


    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(alpha, width() / (float)height(), 0.01, 24.0);
    gluLookAt(0.0,0.0,15.0,0,0,0,0,1,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
//      glTranslatef(0,0,-z);

    {
        QMatrix4x4 m;
        m.rotate(this->_trackball.rotation());
        multMatrix(m);
    }



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
    
    
    updateGL();
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
