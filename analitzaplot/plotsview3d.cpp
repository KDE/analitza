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

#include "plotsview3d.h"
#include "plotsmodel.h"
#include "plotsproxymodel.h"
#include "surface.h"
#include "private/utils/box3d.h"
#include "private/utils/triangle3d.h"
#include <QVector3D>
#include <QVector2D>
#include <qitemselectionmodel.h>
#include <QVector>
#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variables.h"
#include "analitza/vector.h"
#include <QDebug>

Q_DECLARE_METATYPE(PlotItem*);

PlotsView3D::PlotsView3D(QWidget *parent, PlotsProxyModel* m)
    : QGLViewer(parent), m_model(m),m_selection(0)
{
    setGridIsDrawn(true);
    setAxisIsDrawn(true);

    setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));
    setSceneRadius(6); //TODO no magic number, 5 is the size of the ¿coords?
    
    camera()->setPosition(qglviewer::Vec(0,0,15));
}

PlotsView3D::~PlotsView3D()
{
    if (m_model && m_model->rowCount() > 0) {
        for (int i = 0; i < m_model->rowCount(); ++i) {
            glDeleteLists(m_displayLists[itemAt(i)], 1);
            addFuncs(QModelIndex(), 0, m_model->rowCount()-1);
        }
    }
}

void PlotsView3D::setModel(QAbstractItemModel* f)
{
    if(m_model) {
        disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
        disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
        disconnect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
        disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(testvisible(QModelIndex,QModelIndex)));
    }
    
    m_model=f;

    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (itemAt(i)) {
            glDeleteLists(m_displayLists[itemAt(i)], 1);
            addFuncs(QModelIndex(), 0, m_model->rowCount()-1);
        }
    }
     
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
    connect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(testvisible(QModelIndex,QModelIndex)));

    updateGL();
}

void PlotsView3D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection);
    
    m_selection = selection;
//     connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(forceRepaint()));
}

void PlotsView3D::resizeScene(int v)
{
    qreal si = (qreal)v;
    setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));
    setSceneRadius(si);

    updateGL();
}

void PlotsView3D::addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(Surface* surf)
{
    Q_ASSERT(surf);
    makeCurrent(); //NOTE: Remember to call makeCurrent before any OpenGL operation. We might have multiple clients in the same window
    
    if (surf->faces().isEmpty()) // si no esta vacio no es necesario generar nada 
        surf->update(Box3D());

    GLuint dlid = glGenLists(1);
    m_displayLists[surf] = dlid;

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};
    Q_UNUSED(diffuseColor); //TODO: Percy?

    //BEGIN display list
    glNewList(dlid, GL_COMPILE);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//     glColor3fv(diffuseColor);
    glColor3ub(surf->color().red(), surf->color().green(), surf->color().blue());

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

    glEndList();
    //END display list
}


void PlotsView3D::addFuncs(const QModelIndex & parent, int start, int end)
{
    PlotItem *item = itemAt(start);

    if (!item || item->spaceDimension() != Dim3D)
        return;
    
    Surface* surf = static_cast<Surface*>(item);
    
    makeCurrent();
    
    surf->update(Box3D());
        
    GLuint dlid = glGenLists(1);
    m_displayLists[surf] = dlid;

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};
    Q_UNUSED(diffuseColor); //TODO: Percy?

    //BEGIN display list
    glNewList(dlid, GL_COMPILE);

    // set specular and shiniess using glMaterial (gold-yellow)
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//     glColor3fv(diffuseColor);
    glColor3ub(surf->color().red(), surf->color().green(), surf->color().blue());

    foreach (const Triangle3D &face, surf->faces())
    {
        glBegin(GL_TRIANGLES);
        QVector3D n;
        
        //TODO no magic numbers
        if (!face.faceNormal().isNull())
            n = face.faceNormal().normalized();
        else
            n = QVector3D(0.5, 0.1, 0.2).normalized();
        
        glNormal3d(n.x(), n.y(), n.z());
        glVertex3d(face.p().x(), face.p().y(), face.p().z());
        glVertex3d(face.q().x(), face.q().y(), face.q().z());
        glVertex3d(face.r().x(), face.r().y(), face.r().z());
        glEnd();
    }

    glEndList();
    //END display list
    
    updateGL();
}

void PlotsView3D::removeFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
    for(int i=start; i<=end; i++) {
        PlotItem *item = itemAt(i);

        if (item) {
            glDeleteLists(m_displayLists[item], 1);
            updateGL();
        }
    }
}

//NOTE
//si hay un cambio aki es desetvisible (que no es necesario configurar en el plotitem) o es del 
//setinterval (que si es necesario configurarlo en el plotitem)
//el enfoque es: si hay un cambio borro el displaylist y lo genero de nuevo (no lo genero si el item no es visible)
//TODO cache para exp e interval ... pues del resto es solo cuestion de update
void PlotsView3D::testvisible(const QModelIndex& s, const QModelIndex& e)
{
    PlotItem *item = itemAt(s.row());
        
    if (!item) return;

    Surface* surf = static_cast<Surface*>(item);

    glDeleteLists(m_displayLists[surf], 1);

    if (surf->isVisible()) {
//         addFuncs(QModelIndex(), s.row(), s.row());
//igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data 
        addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(surf);

    }
    updateGL();
}

void PlotsView3D::updateFuncs(const QModelIndex& start, const QModelIndex& end)
{
//     updateFunctions(start, end);
//     updateGL();
}

int PlotsView3D::currentFunction() const
{
    int ret=-1;
    if(m_selection && m_model) {
        ret=m_selection->currentIndex().row();
    }
    
    return ret;
}

void PlotsView3D::draw()
{
    if(!m_model)
        return;
    
    //NOTE si esto pasa entonces quiere decir que el proxy empezado a filtrar otros items
    // y si es asi borro todo lo que esta agregado al la memoria de la tarjeta
    //esto se hace pues m_displayLists es una copia del estado actual de model
    if (m_model->rowCount() != m_displayLists.count())
    {
        foreach (PlotItem *item, m_displayLists.keys())
        {
            glDeleteLists(m_displayLists[item], 1);
            
            m_displayLists.remove(item);
        }
    }
    
    /// luego paso a verificar el map de display list no este vacio ... si lo esta lo reconstruyo 
    
    if (m_displayLists.isEmpty())
    {
        //NOTE no llamar a ninguna funcion que ejucute un updategl, esto para evitar una recursividad
        for (int i = 0; i < m_model->rowCount(); ++i) {
            Surface* s = dynamic_cast<Surface*>(itemAt(i));
            addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(s);
        }
    }

    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        PlotItem *item = itemAt(i);
        Surface* surf = static_cast<Surface*>(item);
        
        if (!item || item->spaceDimension() != Dim3D)
            continue;
        //TODO: make it possible to plot things that aren't surfaces
        
        glCallList(m_displayLists[item]);
    }
}

void PlotsView3D::init()
{
    glPushMatrix();
//     glTranslatef(6,5,5);
    glRotatef(90.,0.,1.,0.);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
//     glEnable(GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH);
//     glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

}

PlotItem* PlotsView3D::itemAt(int row) const
{
    if(!m_model)
        return 0;
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim3D)
        return 0;

    return plot;
}
