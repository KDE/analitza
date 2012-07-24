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
//     
    setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));
    setSceneRadius(6); // TODO no magic number 5 es el size de las coords (alrededor )
    
    camera()->setPosition(qglviewer::Vec(0,0,15));
}

PlotsView3D::~PlotsView3D()
{
    PlotsProxyModel* model = m_model;

    if (model)
        if (model->rowCount( ) > 0)
        {
            for (int i = 0; i < model->rowCount(); ++i)
                if (fromProxy(i))
                {
                    glDeleteLists(m_displayLists[fromProxy(i)], 1);
            
                    addFuncs(QModelIndex(), 0, model->rowCount()-1);
                }
        }
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
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
    
    //visible off on
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(testvisible(QModelIndex,QModelIndex)));

    
    updateGL();
}

void PlotsView3D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection);
//     Q_ASSERT(selection->model() == m_model);
    
    //El item selection es del proxy
    Q_ASSERT(qobject_cast< const PlotsProxyModel* >(selection->model()));
    
    
    m_selection = selection;
//     connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(forceRepaint()));
}


void PlotsView3D::addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(int modelindex) // modelindex del proxy
{
    //NOTE 
    //IMPORTANT SIEMPRE HACER UN MAKECURRENT ANTES DE OPERACIONES OPENGL : EL CLIENTE PUEDE TERNER MAS DE UN GLWIDGET A LA VEZ
    makeCurrent();
    
    
//     Q_ASSERT(!parent.isValid());
//     Q_ASSERT(start == end); // siempre se agrega un solo item al model

    PlotItem *item = fromProxy(modelindex);
//         qDebug() << start << end<< item->spaceDimension();
    
//     if (item)
//     qDebug() << item->expression().toString() << item->spaceDimension();

    if (!item) return ;// no agregar nada que no cumpla las politicas/filtros del proxy

//NOTE si el item no es visible salir ... no generar listas de acceso 
    if (!item->isVisible()) return ;

    if (item->spaceDimension() != 3) return ;
    
    Surface* surf = static_cast<Surface*>(item);
    
    if (!surf) return ;

//     qDebug() << surf->faces().isEmpty();
    
    if (surf->faces().isEmpty()) // si no esta vacio no es necesario generar nada 
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

//     qDebug() << "FACES -> " << surf->faces().size();
    
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
    //NOTE 
    //IMPORTANT SIEMPRE HACER UN MAKECURRENT ANTES DE OPERACIONES OPENGL : EL CLIENTE PUEDE TERNER MAS DE UN GLWIDGET A LA VEZ
    makeCurrent();
    
    
//     Q_ASSERT(!parent.isValid());
//     Q_ASSERT(start == end); // siempre se agrega un solo item al model


    
    PlotItem *item = fromProxy(start);
//         qDebug() << start << end<< item->spaceDimension();
    
//     if (item)
//     qDebug() << item->expression().toString() << item->spaceDimension();

    if (!item) return ;// no agregar nada que no cumpla las politicas/filtros del proxy

    if (item->spaceDimension() != 3) return ;
    
    Surface* surf = static_cast<Surface*>(item);
    
    if (!surf) return ;
    
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

//     qDebug() << "FACES -> " << surf->faces().size();
    
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
    
    updateGL();
}

void PlotsView3D::removeFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
//     Q_ASSERT(start == end); // siempre se agrega un solo item al model
    
    PlotItem *item = fromProxy(start);

    //pese a que el item no es valido igual ejecuto el updategl pues seguro alguien emitio la signal removerows y el widget necetia ser actualizado
    if (!item) {updateGL(); return ;}// no eliminar  nada que no cumpla las politicas/filtros del proxy

    glDeleteLists(m_displayLists[item], 1);

    updateGL();
}

//NOTE
//si hay un cambio aki es desetvisible (que no es necesario configurar en el plotitem) o es del 
//setinterval (que si es necesario configurarlo en el plotitem)
//el enfoque es: si hay un cambio borro el displaylist y lo genero de nuevo (no lo genero si el item no es visible)
void PlotsView3D::testvisible(const QModelIndex& s, const QModelIndex& e)
{
    PlotItem *item = fromProxy(s.row());
        
    if (!item) return;

    Surface* surf = static_cast<Surface*>(item);

    //primero borro el displaylist
    glDeleteLists(m_displayLists[surf], 1);

    //si es visible lo quenero de nuevo .. pues seuro cambio el setinterval
    if (surf->isVisible())
    {
//         addFuncs(QModelIndex(), s.row(), s.row());
//igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data 
        addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(s.row());

    }
    //caso contrario .. no hago nada 
    
    updateGL();
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
    PlotsProxyModel* model = m_model;
    
    if (!model) return ; // si no hay modelo retornar ... //NOTE guard
    
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
        for (int i = 0; i < model->rowCount(); ++i)
            addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(i);
    }
    
    
    ///

    for (int i = 0; i < model->rowCount(); ++i)
    {
        PlotItem *item = fromProxy(i);

        if (!item) continue;

        if (item->spaceDimension() != 3) continue;
        
        Surface* surf = static_cast<Surface*>(item);
        
        //TODO GSOC
        // por el momento solo se dibujan superficies
        if (!surf) continue;
        
        glCallList(m_displayLists[item]);
    }
    
    //TODO 
    //WARNING eliminar next iter
    //no dibujar para cada display list ... puede que ya halla sido borrado .. en vez dibujar para cada item en el proxy
    /// algo similar al updatefunctions
//     foreach (PlotItem *item, m_displayLists.keys())
//     {
//         glCallList(m_displayLists[item]);
// //         qDebug() << itemid;
//     }
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

    if (!pi.isValid())
        return 0;
    
    if (qobject_cast<PlotsModel *>(m_model->sourceModel())->item(pi.row())->spaceDimension() != 3)
        return 0; // evitamos que los proxies de los usuario causen un bug

    
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


