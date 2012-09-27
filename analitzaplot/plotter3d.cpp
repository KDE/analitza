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
#include "private/utils/mathutils.h"

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

const GLubyte Plotter3D::XAxisArrowColor[] = {250 -1 , 1, 1};
const GLubyte Plotter3D::YAxisArrowColor[] = {1, 255 - 1, 1};
const GLubyte Plotter3D::ZAxisArrowColor[] = {1, 1, 255 - 1};

//#define DEBUG_GRAPH
QVector<unsigned int> indexes;
void deleteVBO(const GLuint vboId)
{
    glDeleteBuffers(1, &vboId);
}

Plotter3D::Plotter3D(QAbstractItemModel* model)
    : m_model(model)
    , m_plotStyle(Solid)
    , m_plottingFocusPolicy(All)
    , m_depth(-400)
    , m_scale(60)
    , m_currentAxisIndicator(InvalidAxis)
    , m_simpleRotation(false)
{
    resetViewPrivate(QVector3D(-45, 0, -135));
}

Plotter3D::~Plotter3D()
{
    //asser geoms == model rowcount
    for (int i = 0; i < m_itemGeometries.size(); ++i)
    {
        deleteVBO(m_itemGeometries.value(itemAt(i)).first);
        deleteVBO(m_itemGeometries.value(itemAt(i)).second);
    }   

    glDeleteLists(m_sceneObjects.value(Axes), 1);
    glDeleteLists(m_sceneObjects.value(RefPlaneXY), 1);
    glDeleteLists(m_sceneObjects.value(XArrowAxisHint), 1);
    glDeleteLists(m_sceneObjects.value(YArrowAxisHint), 1);
    glDeleteLists(m_sceneObjects.value(ZArrowAxisHint), 1);
}

void Plotter3D::initGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_SMOOTH);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    /// For drawing Filled Polygones :
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glEnable(GL_NORMALIZE);
    glFrontFace (GL_CCW);
    glMaterialf (GL_FRONT, GL_SHININESS, 35.0);
    glMaterialf (GL_BACK, GL_SHININESS, 35.0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,0);

    initAxes();
    initRefPlanes();

    GLfloat ambient[] = { .0, .0, .0, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat local_view[] = { 1.0 };
    static GLfloat position[4] = {0.0, 0.0, 1000.0, 0.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
}

void Plotter3D::resetView()
{
    m_rot.setToIdentity();
    resetViewPrivate(QVector3D(-45, 0, -135));
    renderGL();
}

void Plotter3D::resetViewPrivate(const QVector3D& rot)
{
    m_rot.translate(0,0,-800);
    m_rot.rotate(rot.x(), 1, 0, 0);
    m_rot.rotate(rot.y(), 0, 1, 0);
    m_rot.rotate(rot.z(), 0, 0, 1);
    m_simpleRotationVector = rot;
}

void Plotter3D::setViewport(const QRectF& vp)
{
    float newwidth = vp.size().width();
    float newheight = vp.size().height();
    glViewport( 0, 0, newwidth, newheight);

    m_viewport = vp;
    
    renderGL();
}

void Plotter3D::drawPlots()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_scale, m_viewport.width()/m_viewport.height(), 0.1, 3000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixd(m_rot.data());

    // Object Drawing :
    glCallList(m_sceneObjects.value(Axes));
    glCallList(m_sceneObjects.value(RefPlaneXY));

    if (!m_simpleRotation)
        switch (m_currentAxisIndicator)
        {
            case XAxis:
                glCallList(m_sceneObjects.value(XArrowAxisHint));
                break;
            case YAxis:
                glCallList(m_sceneObjects.value(YArrowAxisHint));
                break;
            case ZAxis:
                glCallList(m_sceneObjects.value(ZArrowAxisHint));
                break;
            case InvalidAxis:
                break;
        }
    
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
            deleteVBO(m_itemGeometries.value(item).first);
            deleteVBO(m_itemGeometries.take(item).second);
        }
    }

    /// luego paso a verificar el map de display list no este vacio ... si lo esta lo reconstruyo

    if (m_itemGeometries.isEmpty())
    {
        //NOTE no llamar a ninguna funcion que ejucute un updategl, esto para evitar una recursividad
        for (int i = 0; i < m_model->rowCount(); ++i) {
            PlotItem* item = dynamic_cast<Surface*>(itemAt(i));

            if (item && item->isVisible())
                addPlots(item);
        }
    }

    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        PlotItem *item = itemAt(i);

        if (!item || item->spaceDimension() != Dim3D || !item->isVisible())
            continue;

        GLfloat  fcolor[4] = {float(item->color().redF()), float(item->color().greenF()), float(item->color().blueF()), 1.0f};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
        glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, fcolor);

        if (SpaceCurve *curv = dynamic_cast<SpaceCurve*>(item))
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_itemGeometries.value(curv).second);
            glVertexPointer(3, GL_DOUBLE, 0, 0);
            glNormalPointer(GL_DOUBLE, sizeof(double)*3, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_itemGeometries.value(curv).first);

            //BEGIN draw
            // start to render polygons
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);

            glDrawElements(GL_LINES, indexes.size(), GL_UNSIGNED_INT, 0);

            fcolor[0] = 0; fcolor[1] = 0; fcolor[2] = 0; fcolor[3] = 1;
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
            glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, fcolor);
            glDrawElements(GL_POINTS, indexes.size(), GL_UNSIGNED_INT, 0);

            glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
            glDisableClientState(GL_NORMAL_ARRAY);  // disable normal arrays
            //END draw
            
            // it is good idea to release VBOs with ID 0 after use.
            // Once bound with 0, all pointers in gl*Pointer() behave as real
            // pointer, so, normal vertex array operations are re-activated
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
        }
        else if (Surface *surf = dynamic_cast<Surface*>(item)) 
        {
            //NOTE
            //los indices empeizan en cero solo se puede tener un solo array bufer cuando se usa vbo
            //por eso en el array buffer se ponen los datos de lops vertices y de las normales
            
            glBindBuffer(GL_ARRAY_BUFFER, m_itemGeometries.value(surf).second);
            glVertexPointer(3, GL_DOUBLE, 0, 0);
            glNormalPointer(GL_DOUBLE, sizeof(double)*3, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_itemGeometries.value(surf).first);

            //BEGIN draw
            // start to render polygons
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);

            glDrawElements(GL_TRIANGLES, surf->indexes().size(), GL_UNSIGNED_INT, 0);

            fcolor[0] = 0; fcolor[1] = 0; fcolor[2] = 0; fcolor[3] = 1;
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
            glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, fcolor);
            glDrawElements(GL_POINTS, surf->indexes().size(), GL_UNSIGNED_INT, 0);

            glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
            glDisableClientState(GL_NORMAL_ARRAY);  // disable normal arrays
            //END draw
            
            // it is good idea to release VBOs with ID 0 after use.
            // Once bound with 0, all pointers in gl*Pointer() behave as real
            // pointer, so, normal vertex array operations are re-activated
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    glPopMatrix();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
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
                addPlots(item);
            }
        }

        renderGL();

        return ;
    }

    for(int i=s; i<=e; i++) {
        PlotItem *item = itemAt(i);

        if (!item)
            return;

        deleteVBO(m_itemGeometries.value(item).first);
        deleteVBO(m_itemGeometries.value(item).second);
        
        if (item->isVisible()) {
            //         addFuncs(QModelIndex(), s.row(), s.row());
            //igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data
            addPlots(item);
        }
    }

    if (m_model->rowCount() != m_itemGeometries.size())
    {
        //if the user delete a item from the model
//         if (m_model->rowCount() > m_itemGeometries.size())
        {
            for (int i = 0; i < m_itemGeometries.size(); ++i)
            {
                deleteVBO(m_itemGeometries.value(itemAt(i)).first);
                deleteVBO(m_itemGeometries.take(itemAt(i)).second);
            }
        }
    }

    renderGL();
}

void Plotter3D::setModel(QAbstractItemModel* f)
{
    m_model=f;

    modelChanged();
}

void Plotter3D::setPlottingFocusPolicy(PlottingFocusPolicy fp)
{
    m_plottingFocusPolicy = fp;
    
    for (int i = 0; i < m_itemGeometries.size(); ++i)
    {
        deleteVBO(m_itemGeometries.value(itemAt(i)).first);
        deleteVBO(m_itemGeometries.value(itemAt(i)).second);
    }

    updatePlots(QModelIndex(), 0, m_model->rowCount()-1);
}

void Plotter3D::scale(GLdouble factor)
{
    m_scale = qBound(1., factor*m_scale, 140.);
    renderGL();
}

void Plotter3D::rotate(int dx, int dy)
{
    GLdouble ax = -dy;
    GLdouble ay = -dx;
    double angle = sqrt(ax*ax + ay*ay)/(m_viewport.width() + 1)*360.0;
    
    if (m_simpleRotation) {
        m_rot.setToIdentity();
        resetViewPrivate(m_simpleRotationVector + QVector3D(ax, 0, ay));
        renderGL();
    } else if (!m_rotFixed.isNull()) {
        m_rot.rotate(angle, m_rotFixed.normalized());
        renderGL();
    } else {
        GLdouble matrix[16] = {0}; // model view matrix from current OpenGL state

        glGetDoublev(GL_MODELVIEW_MATRIX, matrix);

        QMatrix4x4 matrix4(matrix);
        bool couldInvert;
        matrix4 = matrix4.inverted(&couldInvert);

        if (couldInvert) {
            QVector3D rot(matrix4.row(0).x()*ax + matrix4.row(1).x()*ay,
                          matrix4.row(0).y()*ax + matrix4.row(1).y()*ay,
                          matrix4.row(0).z()*ax + matrix4.row(1).z()*ay);
            
            m_rot.rotate(rot.length(), rot.normalized());
            
            renderGL();
        }
    }
}

CartesianAxis Plotter3D::selectAxisArrow(int x, int y)
{
    GLint viewport[4];
    GLubyte pixel[3];

    glGetIntegerv(GL_VIEWPORT,viewport);

    glReadPixels(x, viewport[3]-y, 1, 1, GL_RGB,GL_UNSIGNED_BYTE,(void *)pixel);

    if (memcmp(pixel, XAxisArrowColor, sizeof(pixel)) == 0) return XAxis;
    if (memcmp(pixel, YAxisArrowColor, sizeof(pixel)) == 0) return YAxis;
    if (memcmp(pixel, ZAxisArrowColor, sizeof(pixel)) == 0) return ZAxis;

    return InvalidAxis;
}

void Plotter3D::fixRotation(const QVector3D& vec)
{
    m_rotFixed = vec;
}

void Plotter3D::showAxisArrowHint(CartesianAxis axis)
{
    if (axis == InvalidAxis)
        return ;

    m_currentAxisIndicator = axis;
    
    renderGL();
}

void Plotter3D::hideAxisHint()
{
    m_currentAxisIndicator = InvalidAxis;
    
    renderGL();
}

void Plotter3D::addPlots(PlotItem* item)
{
    Q_ASSERT(item);

    if (Surface* surf = dynamic_cast<Surface*>(item))
    {
        if (surf->indexes().isEmpty())
            surf->update(QVector3D(), QVector3D());
    }
    else if (SpaceCurve* c = dynamic_cast<SpaceCurve*>(item))
    {
        if (c->points().isEmpty())
            c->update(QVector3D(), QVector3D());
    }

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

    //BEGIN display list

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3fv(diffuseColor);
    glColor3i(item->color().red(), item->color().green(), item->color().blue());

    if (SpaceCurve *curve = dynamic_cast<SpaceCurve*>(item))
    {
//         glEnable(GL_LINE_SMOOTH);

//         glLineWidth(2.5);

//         if (m_plottingFocusPolicy == All)
//             switch (m_plotStyle)
//             {
//                 case Solid: 
//                 case Wired: glBegin(GL_LINES); break;
//                 case Dots: glBegin(GL_POINTS); break;
//             }
//         else
//             if (m_plottingFocusPolicy == Current)
//             {
//                 QModelIndex pi = m_model->index(currentPlot(), 0);
// 
//                 PlotItem* plot = 0;
//                 if (!pi.isValid())
//                    plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();
// 
//                 if (plot == curve)
//                 {
//                     switch (m_plotStyle)
//                     {
//                         case Solid: 
//                         case Wired: glBegin(GL_LINES); break;
//                         case Dots: glBegin(GL_POINTS); break;
//                     }
//                 }
//                 else
//                     glBegin(GL_LINES);
//             }
// 
        const QVector<QVector3D> points = curve->points();
// 
//         //TODO copy approach from plotter 2d and use jumps optimization
//         for (int i = 0; i < points.size() -1 ; ++i)
//         {
//             glVertex3d(points[i].x(), points[i].y(), points[i].z());
//             glVertex3d(points[i+1].x(), points[i+1].y(), points[i+1].z());
//         }
//         
//         glEnd();
//         glDisable(GL_LINE_SMOOTH);
        
        QVector<double> vertices;
        QVector<double> normals;
        indexes.clear();

        for (int i = 0; i < points.size(); ++i)
        {
            vertices << points[i].x() << points[i].y() << points[i].z();
            
            if (i%2 != 0)
            {
                QVector3D n = QVector3D::crossProduct(points[i], points[i-1]).normalized();

                normals << n.x() << n.y() << n.z();
                indexes.append(indexes.size());
            }
        }

        //vertices & normals vbo just allows 1 buffferdata of type array_buffer
        glGenBuffers(1, &m_itemGeometries[item].second);
        glBindBuffer(GL_ARRAY_BUFFER, m_itemGeometries[item].second);
        glBufferData(GL_ARRAY_BUFFER, sizeof(double)*vertices.size() + sizeof(double)*normals.size(), 0, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(double)*vertices.size(), vertices.data());
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(double)*vertices.size(), sizeof(double)*normals.size(), normals.data());
        //TODO ifdef debug_Graph
        //glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        //qDebug() << "Vertex Array in VBO: " << bufferSize << " bytes";

        //indices
        glGenBuffers(1, &m_itemGeometries[item].first);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_itemGeometries[item].first);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexes.size(), indexes.data(), GL_STATIC_DRAW);
        //glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        //qDebug() << "Index Array in VBO: " << bufferSize << " bytes";    

    }
    else
    {
        Surface* surf = dynamic_cast<Surface*>(item);
    
        int bufferSize;

        //vertices & normals vbo just allows 1 buffferdata of type array_buffer
        glGenBuffers(1, &m_itemGeometries[item].second);
        glBindBuffer(GL_ARRAY_BUFFER, m_itemGeometries[item].second);
        glBufferData(GL_ARRAY_BUFFER, sizeof(double)*surf->vertices().size() + sizeof(double)*surf->normals().size(), 0, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(double)*surf->vertices().size(), surf->vertices().data());
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(double)*surf->vertices().size(), sizeof(double)*surf->normals().size(), surf->normals().data());
        //TODO ifdef debug_Graph
        //glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        //qDebug() << "Vertex Array in VBO: " << bufferSize << " bytes";

        //indices
        glGenBuffers(1, &m_itemGeometries[item].first);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_itemGeometries[item].first);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*surf->indexes().size(), surf->indexes().data(), GL_STATIC_DRAW);
        //glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        //qDebug() << "Index Array in VBO: " << bufferSize << " bytes";        

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

void Plotter3D::initAxes()
{
    const GLubyte XAxisColor[] = {250, 0, 0};
    const GLubyte YAxisColor[] = {0, 255, 0};
    const GLubyte ZAxisColor[] = {0, 0, 255};    
    
    if (m_sceneObjects.contains(Axes))
        glDeleteLists(m_sceneObjects.value(Axes), 1);

    m_sceneObjects[Axes] = glGenLists(1);
    glNewList(m_sceneObjects.value(Axes), GL_COMPILE );

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(1.5f);

    glBegin( GL_LINES );
    glColor3ubv(XAxisColor);
    glVertex3f( 400.0, 0.0, 0.0);
    glVertex3f(   0.0, 0.0, 0.0);
    glColor3ubv(YAxisColor);
    glVertex3f(0.0, 400.0, 0.0);
    glVertex3f(0.0,   0.0, 0.0);
    glColor3ubv(ZAxisColor);
    glVertex3f(0.0, 0.0, 400.0);
    glVertex3f(0.0, 0.0,   0.0);
    glEnd();

    glLineWidth(1.0f);

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    glBegin(GL_TRIANGLE_FAN);
    glColor3ubv(XAxisArrowColor);
    glVertex3f(400.0, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glVertex3f(380.0, 0.0, 10.0);
    glVertex3f(380.0, -10.0, 0.0);
    glVertex3f(380.0, 0.0, -10.0);
    glVertex3f(380.0, 10.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3ubv(YAxisArrowColor);
    glVertex3f(0.0, 400.0, 0.0);
    glVertex3f(10.0, 380.0, 0.0);
    glVertex3f(0.0, 380.0, 10.0);
    glVertex3f(-10.0, 380.0, 0.0);
    glVertex3f(.0, 380.0, -10.0);
    glVertex3f(10.0, 380.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3ubv(ZAxisArrowColor);
    glVertex3f(0.0, 0.0, 400.0);
    glVertex3f(10.0, 0.0, 380.0);
    glVertex3f(0.0, 10.0, 380.0);
    glVertex3f(-10.0, 0.0, 380.0);
    glVertex3f(0.0, -10.0, 380.0);
    glVertex3f(10.0, 0.0, 380.0);
    glEnd();

    glEndList();
    
    GLUquadricObj *sphereObj;
    sphereObj = gluNewQuadric();
    
    // XArrowAxisHint
    if (m_sceneObjects.contains(XArrowAxisHint))
        glDeleteLists(m_sceneObjects.value(XArrowAxisHint), 1);

    m_sceneObjects[XArrowAxisHint] = glGenLists(1);
    glNewList(m_sceneObjects.value(XArrowAxisHint), GL_COMPILE );

    glColor3ub(XAxisArrowColor[0]/2, XAxisArrowColor[1]/2, XAxisArrowColor[2]/2);
    glTranslatef(408.0, 0.0, 0.0);
    gluSphere(sphereObj, 8.0, 32, 32);
    glTranslatef(-408.0, 0.0, 0.0);
    
    glEndList();
    
    // YArrowAxisHint

    if (m_sceneObjects.contains(YArrowAxisHint))
        glDeleteLists(m_sceneObjects.value(YArrowAxisHint), 1);

    m_sceneObjects[YArrowAxisHint] = glGenLists(1);
    glNewList(m_sceneObjects.value(YArrowAxisHint), GL_COMPILE );

    glColor3ub(YAxisArrowColor[0]/2, YAxisArrowColor[1]/2, YAxisArrowColor[2]/2);
    glTranslatef(0.0, 408.0, 0.0);
    gluSphere(sphereObj, 8.0, 32, 32);
    glTranslatef(0.0, -408.0, 0.0);

    glEndList();

    // YArrowAxisHint

    if (m_sceneObjects.contains(ZArrowAxisHint))
        glDeleteLists(m_sceneObjects.value(ZArrowAxisHint), 1);

    m_sceneObjects[ZArrowAxisHint] = glGenLists(1);
    glNewList(m_sceneObjects.value(ZArrowAxisHint), GL_COMPILE );

    glColor3ub(ZAxisArrowColor[0]/2, ZAxisArrowColor[1]/2, ZAxisArrowColor[2]/2);
    glTranslatef(0.0, 0.0, 408.0);
    gluSphere(sphereObj, 8.0, 32, 32);
    glTranslatef(0.0, 0.0, -408.0);

    glEndList();
}

void Plotter3D::initRefPlanes()
{
    if (m_sceneObjects.contains(RefPlaneXY))
        glDeleteLists(m_sceneObjects.value(RefPlaneXY), 1);

    m_sceneObjects[RefPlaneXY] = glGenLists(1);

    glNewList(m_sceneObjects.value(RefPlaneXY), GL_COMPILE );
    glLineWidth(1);
    glColor3ub(48, 48, 48);
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


