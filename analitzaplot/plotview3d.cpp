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
}

View3D::View3D(FunctionGraphsModel* m, QWidget* parent): QGLViewer(parent)
    ,m_model(m), m_selection(0)
{
    setGridIsDrawn(true);
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
    
//     updateFunctions(model()->index(start,0), model()->index(end,0));
}

void View3D::removeFuncs(const QModelIndex &, int, int)
{
//     forceRepaint();
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


//TODO
/*
void View3D::updateSurface(const Keomath::Function &function)
{
    

    if (function.dimension() == 2)
        return ;
    if (function.name()=="Esfera")
        return;

    m_currentSolver = static_cast<Solver3D*>(function.solver());

    

    m_color = function.color();

    
    

    static unsigned int dlnum = 0;
    ++dlnum;

    
    
    
    
    if (m_displayList.contains(function.id()))
    {
        glDeleteLists(m_displayList.value(function.id()), 1);
    }

    
    glNewList(dlnum, GL_COMPILE);

    if (m_currentSolver->isCurve())
    {

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(function.lineWidth()*0.5); 

        glColor3ub(function.color().red(), function.color().green(), function.color().blue());




        qreal maxCurveSpaceRadius = m_currentSolver->evalCurve(m_currentSolver->domain().at(0).upper()
                                    -m_currentSolver->domain().at(0).lower()).length();


        maxCurveSpaceRadius = qMax(maxCurveSpaceRadius, (qreal)sceneRadius());

        glBegin(GL_LINES);
        {
            for (qreal t = 0.0;
                    t < 90.0; t+=0.001)
            {
                QVector3D v = m_currentSolver->evalCurve(t);
                QVector3D v2 = m_currentSolver->evalCurve(t+0.001);



                if (maxCurveSpaceRadius > v.length()) 
                {
                    glVertex3d(v.x(), v.y(), v.z());
                    glVertex3d(v2.x(), v2.y(), v2.z());
                }
                else
                {
                }
            }
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);

    }
    else 
    {
        m_drawingType = function.drawingType();

        umin = m_currentSolver->domain().at(0).lower();
        umax = m_currentSolver->domain().at(0).upper();
        vmin = m_currentSolver->domain().at(1).lower();
        vmax = m_currentSolver->domain().at(1).upper();

        doSurface();
    }
    glEndList();

    m_displayList.insert(function.id(), dlnum);

    

    updateGL();






















}*/
/*
void View3D::graficar_curvas(QUuid funcId,int tipo, QList<double> cons, bool plano, double pres)
{
    ++dlnum;
    glNewList(dlnum, GL_COMPILE);

    glLineWidth(2.0);
    glColor3ub(255, 255, 0);
    glBegin(GL_LINES);
    {
        for(double tt=0.0; tt<5.0; tt+=pres)
            for (qreal t = 0.0; t<8.0; t+=0.005)
            {
                QVector3D v = evalCurve(tipo,tt,t,cons);
                QVector3D v2 = evalCurve(tipo,tt,t+0.005,cons);
                if (10.0 > v.length()) 
                {
                    if(plano)
                    {
                        glVertex3d(v.x(), v.y(), 0);
                        glVertex3d(v2.x(), v2.y(), 0);
                    }
                    else
                    {
                        glVertex3d(v.x(), v.y(), v.z());
                        glVertex3d(v2.x(), v2.y(), v2.z());
                    }
                }
            }
    }
    glEnd();

    glEndList();
    num+=1;
    m_displayList.insert(funcId, dlnum);
}*/

static int funcID = 0;
/*
void View3D::updateSurfaceImplicit(QUuid funId,QColor col,int index,QList<double> cons,int oct,int axi,bool solid,bool curva,bool xy,double pres)
{

    if(curva)
    {
        graficar_curvas(funId,index,cons,xy,pres);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(camara_x, camara_y, camara_z);
        glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
        glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
        glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

        switch(index)
        {
        case 0:
            cambiar_funcion(funId,col,esfera,cons,oct,axi,solid);
            break;
        case 1:
            cambiar_funcion(funId,col,elipsoide,cons,oct,axi,solid);
            break;
        case 2:
            cambiar_funcion(funId,col,cilindro,cons,oct,axi,solid);
            break;
        case 3:
            cambiar_funcion(funId,col,hiperboloide_1hoja,cons,oct,axi,solid);
            break;
        case 4:
            cambiar_funcion(funId,col,hiperboloide_2hojas,cons,oct,axi,solid);
            break;
        case 5:
            cambiar_funcion(funId,col,paraboloide_hiperbolico,cons,oct,axi,solid);
            break;
        case 6:
            cambiar_funcion(funId,col,paraboloide_eliptico,cons,oct,axi,solid);
            break;
        case 7:
            cambiar_funcion(funId,col,cono_eliptico,cons,oct,axi,solid);
            break;
        }
    }
    updateGL();
}*/

void View3D::draw()
{

//TODO
//     foreach (QUuid fid, m_displayList.keys())
//     {
//         glCallList(m_displayList.value(fid));
//     }
    
    
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

