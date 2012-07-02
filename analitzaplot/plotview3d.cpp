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
#include <QVector3D>
#include <QVector2D>
#include <qitemselectionmodel.h>
#include <QVector>
#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variables.h"
#include "analitza/vector.h"
#include <QDebug>


View3D::View3D(QWidget *parent)
    : QGLViewer(parent)
    ,m_model(0), m_selection(0)
{
    setGridIsDrawn(true);
    setAxisIsDrawn(true);
}

View3D::View3D(FunctionGraphsModel* m, QWidget* parent): QGLViewer(parent)
    ,m_model(m), m_selection(0)
{
    setGridIsDrawn(true);
    setAxisIsDrawn(true);
}

void View3D::setModel(FunctionGraphsModel *model)
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

    const Surface* surf = static_cast<const Surface*>(m_model->item(start));
    m_model->updateItem(start, Box());
        
    GLuint dlid = glGenLists(1);
    m_displayLists[m_model->item(start)->id()] = dlid;
    
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
    glColor3fv(diffuseColor);

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
    
    glDeleteLists(m_displayLists[m_model->item(start)->id()], 1);
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

void View3D::draw()
{
    foreach (const QString &itemid, m_displayLists.keys())
    {
        glCallList(m_displayLists[itemid]);
//         qDebug() << itemid;
    }
}

void View3D::init()
{
    glPushMatrix();
    glTranslatef(6,5,5);
    glRotatef(90.,0.,1.,0.);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

}

