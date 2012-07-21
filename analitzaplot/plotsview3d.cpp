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
#include <QVector3D>
#include <QVector2D>
#include <qitemselectionmodel.h>
#include <QVector>
#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variables.h"
#include "analitza/vector.h"
#include <QDebug>


PlotsView3D::PlotsView3D(QWidget *parent, PlotsProxyModel* m)
    : QGLViewer(parent), m_model(m),m_selection(0)
{
    setGridIsDrawn(true);
    setAxisIsDrawn(true);
    
        
    setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));
    setSceneRadius(6); // TODO no magic number 5 es el size de las coords (alrededor )
    
    camera()->setPosition(qglviewer::Vec(0,0,15));
}

PlotsView3D::~PlotsView3D()
{

}

void PlotsView3D::setModel(PlotsProxyModel* f)
{
    //esto proxy debe ser del plotsmodel ... no de otro modelo
    Q_ASSERT(qobject_cast< PlotsModel* >(f->sourceModel()));
//     Q_ASSERT(f->filterSpaceDimension() == 3); // este widget renderiza solo plots que esten en un spacio 3D
    

    m_model=f;
    PlotsProxyModel* model = m_model;

    if (model->rowCount( ) > 0)
    {
    for (int i = 0; i < model->rowCount(); ++i)
        if (fromProxy(i))
        {
            glDeleteLists(m_displayLists[fromProxy(i)], 1);
    
            addFuncs(QModelIndex(), 0, model->rowCount()-1);
        }
    }
    
    //TODO disconnect prev model
    //NOTE Estas signal son del PROXY ... para evitar que este widget reciba signals que no le interesan 
//     connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
    connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
//     connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
    
    updateGL();
}

void PlotsView3D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection);
//     Q_ASSERT(selection->model() == m_model);
    
    //El item selection es del proxy
    Q_ASSERT(qobject_cast< const PlotsProxyModel* >(selection->model()));
    
    
    m_selection = selection;
    connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(forceRepaint()));
}
//los index son del modelo original es decir del PlotsModel ... NO DEL PROXY
void PlotsView3D::addFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
//     Q_ASSERT(start == end); // siempre se agrega un solo item al model

        qDebug() << start << end;

    
    PlotItem *item = fromProxy(start);
    
//     if (item)
//     qDebug() << item->expression().toString() << item->spaceDimension();

    if (!item) return ;// no agregar nada que no cumpla las politicas/filtros del proxy
    
    Surface* surf = static_cast<Surface*>(item);
    surf->update(Box3D());
        
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

void PlotsView3D::removeFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
    Q_ASSERT(start == end); // siempre se agrega un solo item al model
    
        PlotItem *item = fromSource(start);

    if (!item) return ;// no eliminar  nada que no cumpla las politicas/filtros del proxy
    
    glDeleteLists(m_displayLists[item], 1);
}

void PlotsView3D::updateFuncs(const QModelIndex& start, const QModelIndex& end)
{
//     updateFunctions(start, end);
}

int PlotsView3D::currentFunction() const
{
    if (!m_model) return -1; // guard
    
    int ret=-1;
    if(m_selection) {
        ret=m_selection->currentIndex().row();
    }
    
    return ret;
}

void PlotsView3D::draw()
{
    foreach (PlotItem *item, m_displayLists.keys())
    {
        glCallList(m_displayLists[item]);
//         qDebug() << itemid;
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


PlotItem* PlotsView3D::fromProxy(int proxy_row) const
{
    QModelIndex pi = m_model->mapToSource(m_model->index(proxy_row, 0));
    
//     qDebug() << "de" << m_model->rowCount();
    
    if (pi.isValid())
        return qobject_cast<PlotsModel *>(m_model->sourceModel())->item(pi.row());
    
    return 0;
}

PlotItem* PlotsView3D::fromSource(int realmodel_row) const
{
    
    QModelIndex si = m_model->mapFromSource(m_model->sourceModel()->index(realmodel_row,0));

    if (si.isValid())
        return qobject_cast<PlotsModel *>(m_model->sourceModel())->item(realmodel_row);
    
    return 0;
//     .row());
}


