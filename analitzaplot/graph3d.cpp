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


#include "graph3d.h"

#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include "analitza/analyzer.h"
#include "analitza/value.h"
#include "analitza/variables.h"
#include "analitza/vector.h"
#include <QDebug>


#include "functionsmodel.h"
#include "functionimpl.h"

// #include "solvers/solvers3d/MarchingCubes/ctab.h"
// #include "solvers/solvers3d/MarchingCubes/glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif


Graph3D::Graph3D(FunctionsModel *fm, QWidget *parent)
    : QGLViewer(parent)
    , m_drawingType(Function::Solid)
    , m_color(Qt::white)
    , m_functionsModel(fm)
{


    //WARNING
    //TODO

//     logo = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;


    camara_x = camara_y = 0;
    camara_z = -5;

        //WARNING
    //TODO
//     logo = NULL;


    num=1000;
    dlnum=1000;
    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    setGridIsDrawn(true);
}

void Graph3D::clearDisplayLists()
{


    glDeleteLists(1, m_displayList.values().size());



    m_displayList.clear();
}


void Graph3D::drawGrid(float size,int nbSubdivisions)
{

    GLboolean lighting;
    glGetBooleanv(GL_LIGHTING, &lighting);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for (int i=0; i<=nbSubdivisions; ++i)
    {
        const float pos = size*(2.0*i/nbSubdivisions-1.0);
        glVertex2f(pos, -size);
        glVertex2f(pos, +size);
        glVertex2f(-size, pos);
        glVertex2f( size, pos);
    }
    glEnd();
    if (lighting)
        glEnable(GL_LIGHTING);


}

void Graph3D::generateDisplayLists()
{
    for (int i = 0; i < m_functionsModel->rowCount(); i+=1)
    {
        FunctionImpl *tempsol = m_functionsModel->funclist.at(i).solver();

        if (tempsol->dimension() == 2) continue;

        FunctionImpl3D *solver = static_cast<FunctionImpl3D*>(m_functionsModel->funclist.at(i).solver());

        if (!m_functionsModel->funclist.at(i).isShown()) continue;

        updateSurface(m_functionsModel->funclist.at(i));
    }
}

void Graph3D::setFunctionsModel(FunctionsModel *fm)
{
    m_functionsModel = fm;

    connect(m_functionsModel, SIGNAL(functionModified(Function)), SLOT(updateSurface(Function)));
    connect(m_functionsModel, SIGNAL(functionImplicitCall(QUuid,QColor,int,QList<double>,int,int,bool,bool,bool,double)), SLOT(updateSurfaceImplicit(QUuid,QColor,int,QList<double>,int,int,bool,bool,bool,double)));
    connect(m_functionsModel, SIGNAL(functionRemoved(QUuid,QString)), SLOT(removeSurface(QUuid,QString)));
}

void Graph3D::updateSurface(const Function &function)
{


    if (function.dimension() == 2)
        return ;
    if (function.name()=="Esfera")
        return;

    m_currentSolver = static_cast<FunctionImpl3D*>(function.solver());



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






















}

void Graph3D::graficar_curvas(QUuid funcId,int tipo, QList<double> cons, bool plano, double pres)
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
}

static int funcID = 0;

void Graph3D::updateSurfaceImplicit(QUuid funId,QColor col,int index,QList<double> cons,int oct,int axi,bool solid,bool curva,bool xy,double pres)
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
    //WARNING
    //TODO

//         switch(index)
//         {
//         case 0:
//             cambiar_funcion(funId,col,esfera,cons,oct,axi,solid);
//             break;
//         case 1:
//             cambiar_funcion(funId,col,elipsoide,cons,oct,axi,solid);
//             break;
//         case 2:
//             cambiar_funcion(funId,col,cilindro,cons,oct,axi,solid);
//             break;
//         case 3:
//             cambiar_funcion(funId,col,hiperboloide_1hoja,cons,oct,axi,solid);
//             break;
//         case 4:
//             cambiar_funcion(funId,col,hiperboloide_2hojas,cons,oct,axi,solid);
//             break;
//         case 5:
//             cambiar_funcion(funId,col,paraboloide_hiperbolico,cons,oct,axi,solid);
//             break;
//         case 6:
//             cambiar_funcion(funId,col,paraboloide_eliptico,cons,oct,axi,solid);
//             break;
//         case 7:
//             cambiar_funcion(funId,col,cono_eliptico,cons,oct,axi,solid);
//             break;
//         }
    }
    updateGL();
}

void Graph3D::removeSurface(const QUuid &funid, const QString &funlambda)
{
    glDeleteLists(m_displayList.value(funid), 1);
    m_displayList.remove(funid);

    updateGL();
}


void Graph3D::draw()
{

    foreach (QUuid fid, m_displayList.keys())
    {
        glCallList(m_displayList.value(fid));
    }


}

void Graph3D::pintar_ejes(unsigned int modo)
{
    GLfloat color[4];
    switch(modo)
    {
    case 0:
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
        color[3] = 1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        glBegin(GL_LINES);

        glVertex3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.2f, 0.0f);
        glVertex3f(0.9f, 0.1f, 0.0f);
        glVertex3f(0.9f, 0.2f, 0.0f);
        glVertex3f(1.0f, 0.1f, 0.0f);

        glVertex3f(0.0f, -1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.1f, 1.0f, 0.0f);
        glVertex3f(0.12f, 0.97f, 0.0f);
        glVertex3f(0.14f, 1.0f, 0.0f);
        glVertex3f(0.12f, 0.97f, 0.0f);
        glVertex3f(0.12f, 0.97f, 0.0f);
        glVertex3f(0.12f, 0.9f, 0.0f);

        glVertex3f(0.0f, 0.0f, -1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.2f, 1.0f);
        glVertex3f(0.0f, 0.2f, 0.9f);
        glVertex3f(0.0f, 0.2f, 0.9f);
        glVertex3f(0.0f, 0.1f, 1.0f);
        glVertex3f(0.0f, 0.1f, 1.0f);
        glVertex3f(0.0f, 0.1f, 0.9f);
        glEnd();
        break;
    case 1:
        glEnable(GL_CULL_FACE);

        color[0] = 1;
        color[1] = 0;
        color[2] = 0;
        color[3] = 1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        glBegin(GL_QUADS);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);

        color[0] = 0;
        color[1] = 1;
        color[2] = 0;
        color[3] = 1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        color[0] = 0;
        color[1] = 0;
        color[2] = 1;
        color[3] = 1;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glEnd();
        glDisable(GL_CULL_FACE);
        break;
    default:
        break;
    }
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void Graph3D::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot)
    {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void Graph3D::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot)
    {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void Graph3D::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot)
    {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

QVector3D Graph3D::evalCurve(int tipo,double dZ,qreal t,QList<double> lst)
{

    QVector3D ans;
    double tmp1,tmp2,tmp3;
    switch(tipo)
    {

    case 0:
        tmp1=pow(lst.at(3),2)-pow(dZ-lst.at(2),2);
        ans.setX(lst.at(0)+sqrt(tmp1)*cos(t));
        ans.setY(lst.at(1)+sqrt(tmp1)*sin(t));
        ans.setZ(dZ);
        break;

    case 1:
        tmp1=1-pow((dZ-lst.at(2))/lst.at(5),2);
        tmp2=sqrt(pow(lst.at(3),2)*tmp1);
        tmp3=sqrt(pow(lst.at(4),2)*tmp1);
        ans.setX(tmp2*cos(t)+lst.at(0));
        ans.setY(tmp3*sin(t)+lst.at(1));
        ans.setZ(dZ);
        break;

    case 2:
        ans.setX(lst.at(0)+lst.at(3)*cos(t));
        ans.setY(lst.at(1)+lst.at(3)*sin(t));
        ans.setZ(dZ);
        break;

    case 3:
        tmp1=1+pow((dZ-lst.at(2))/lst.at(5),2);
        tmp2=sqrt(pow(lst.at(3),2)*tmp1);
        tmp3=sqrt(pow(lst.at(4),2)*tmp1);
        ans.setX(tmp2*cos(t)+lst.at(0));
        ans.setY(tmp3*sin(t)+lst.at(1));
        ans.setZ(dZ);
        break;

    case 4:
        tmp1=pow((dZ-lst.at(2))/lst.at(5),2)-1;
        tmp2=sqrt(pow(lst.at(3),2)*tmp1);
        tmp3=sqrt(pow(lst.at(4),2)*tmp1);
        ans.setX(tmp2*cos(t)+lst.at(0));
        ans.setY(tmp3*sin(t)+lst.at(1));
        ans.setZ(dZ);
        break;

    case 5:
        tmp1=(dZ-lst.at(2));
        tmp2=sqrt(pow(lst.at(3),2)*tmp1);
        tmp3=sqrt(pow(lst.at(4),2)*tmp1);
        ans.setX(tmp2*(1/cos(t))+lst.at(0));
        ans.setY(tmp3*tan(t)+lst.at(1));
        ans.setZ(dZ);
        break;

    case 6:
        tmp1=(dZ-lst.at(2))/lst.at(5);
        tmp2=sqrt(pow(lst.at(3),2)*tmp1);
        tmp3=sqrt(pow(lst.at(4),2)*tmp1);
        ans.setX(tmp2*cos(t)+lst.at(0));
        ans.setY(tmp3*sin(t)+lst.at(1));
        ans.setZ(dZ);
        break;

    case 7:
        tmp1=pow((dZ-lst.at(2))/lst.at(5),2);
        tmp2=sqrt(pow(lst.at(3),2)*tmp1);
        tmp3=sqrt(pow(lst.at(4),2)*tmp1);
        ans.setX(tmp2*cos(t)+lst.at(0));
        ans.setY(tmp3*sin(t)+lst.at(1));
        ans.setZ(dZ);
        break;
    }

    return ans;
}


void Graph3D::init()
{








    usteps=MAXALONG;
    vsteps=MAXAROUND;

        //WARNING
    //TODO

//     QtLogo::constantes.clear();
//     QtLogo::constantes.append(0.5);
//     QtLogo::constantes.append(0.5);
//     QtLogo::constantes.append(0.25);
//     QtLogo::constantes.append(0.75);
//     QtLogo::constantes.append(0.25);

        //WARNING
    //TODO

//     logo = new QtLogo(this, &func_cilindro);
//     logo->setColor(qtGreen.dark());

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


GLfloat Graph3D::UU(int i)
{
    return (umin+((umax-umin)/(float)(usteps-1))*(float)(i));
}


GLfloat Graph3D::VV(int j)
{
    return (vmin+((vmax-vmin)/(float)(vsteps-1))*(float)(j));
}

QVector3D Graph3D::shape(float u,float v)
{

    QVector3D ret;

    ret = m_currentSolver->evalSurface(u,v);

    return ret;
}

void Graph3D::doSurface()
{

    int  i, j;
    float u, v;

    qDebug() << "view3 colo surff" << m_color.red() << m_color.green() << m_color.blue();


    GLfloat yellow[]= {m_color.redF(), m_color.greenF(), m_color.blueF(), 1.0};
    GLfloat mat_shininess[]= { 8.0 };


    for ( i=0; i<usteps; i++ )
        for ( j=0; j<vsteps; j++ )
        {
            u = UU(i);
            v = VV(j);

            QVector3D point = shape(u,v);

            surface[i][j].x = point.x();
            surface[i][j].y = point.y();
            surface[i][j].z = point.z();
        }


    for (i = 0; i < usteps -1; i++ )
        for ( j=0; j<vsteps-1; j++ )
        {
            glColor3ub(m_color.red(), m_color.green(), m_color.blue());



            doQuad(1, 1, surface[i][j], surface[i+1][j], surface[i][j+1], surface[i+1][j+1]);
        }

}

void Graph3D::doQuad(int n, int m, surfpoint p0, surfpoint p1, surfpoint p2, surfpoint p3)
{
    int i;

    surfpoint A, B, C, D;


    for (i=0; i<m; i++)
    {
        A.x = (p0.x*(float)(m-i)   + p1.x*(float)i)/(float)m;
        A.y = (p0.y*(float)(m-i)   + p1.y*(float)i)/(float)m;
        A.z = (p0.z*(float)(m-i)   + p1.z*(float)i)/(float)m;
        B.x = (p0.x*(float)(m-i-1) + p1.x*(float)(i+1))/(float)m;
        B.y = (p0.y*(float)(m-i-1) + p1.y*(float)(i+1))/(float)m;
        B.z = (p0.z*(float)(m-i-1) + p1.z*(float)(i+1))/(float)m;

        C.x = (p2.x*(float)(m-i)   + p3.x*(float)i)/(float)m;
        C.y = (p2.y*(float)(m-i)   + p3.y*(float)i)/(float)m;
        C.z = (p2.z*(float)(m-i)   + p3.z*(float)i)/(float)m;
        D.x = (p2.x*(float)(m-i-1) + p3.x*(float)(i+1))/(float)m;
        D.y = (p2.y*(float)(m-i-1) + p3.y*(float)(i+1))/(float)m;
        D.z = (p2.z*(float)(m-i-1) + p3.z*(float)(i+1))/(float)m;
        doStrip(n, A, B, C, D);
    }
}



void Graph3D::doStrip(int n, surfpoint p0, surfpoint p1, surfpoint p2, surfpoint p3)
{
    int i, j;
    surfpoint A, B, buffer[3];

    for (i=0; i<=n; i++)
    {
        A.x = (p0.x*(float)(n-i) + p2.x*(float)i)/(float)n;
        A.y = (p0.y*(float)(n-i) + p2.y*(float)i)/(float)n;
        A.z = (p0.z*(float)(n-i) + p2.z*(float)i)/(float)n;
        B.x = (p1.x*(float)(n-i) + p3.x*(float)i)/(float)n;
        B.y = (p1.y*(float)(n-i) + p3.y*(float)i)/(float)n;
        B.z = (p1.z*(float)(n-i) + p3.z*(float)i)/(float)n;
        theStrip[i][0] = A;
        theStrip[i][1] = B;
    }



    buffer[0] = theStrip[0][0];
    buffer[1] = theStrip[0][1];
    for (i=1; i<=n; i++)
        for (j=0; j<2; j++)
        {
            buffer[2] = theStrip[i][j];
            _emit(buffer);
            buffer[0] = buffer[1];
            buffer[1] = buffer[2];
        }
}

void Graph3D::_emit( surfpoint *buffer )
{
    surfpoint Normal, diff1, diff2;

    diff1.x = buffer[1].x - buffer[0].x;
    diff1.y = buffer[1].y - buffer[0].y;
    diff1.z = buffer[1].z - buffer[0].z;
    diff2.x = buffer[2].x - buffer[1].x;
    diff2.y = buffer[2].y - buffer[1].y;
    diff2.z = buffer[2].z - buffer[1].z;
    Normal.x = diff1.y*diff2.z - diff2.y*diff1.z;
    Normal.y = diff1.z*diff2.x - diff1.x*diff2.z;
    Normal.z = diff1.x*diff2.y - diff1.y*diff2.x;

    switch (m_drawingType)
    {
    case Function::Solid:
        glBegin(GL_POLYGON);
        break;
    case Function::Wired:
        glBegin(GL_LINES);
        break;
    case Function::Dots:
        glBegin(GL_POINTS);
        break;

    }


    glNormal3f(Normal.x,Normal.y,Normal.z);
    glVertex3f(buffer[0].x,buffer[0].y,buffer[0].z);
    glVertex3f(buffer[1].x,buffer[1].y,buffer[1].z);
    glVertex3f(buffer[2].x,buffer[2].y,buffer[2].z);
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

}



void Graph3D::mousePressEvent(QMouseEvent * event)
{

    emit viewClicked();
    QGLViewer::mousePressEvent(event);
}

QSize Graph3D::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize Graph3D::sizeHint() const
{
    return QSize(400, 400);
}
    //WARNING
    //TODO

/*
void Graph3D::cambiar_funcion(QUuid funcId,QColor col,Figuras tipo, QList<double> constantes,int oct,int axi,bool solid)
{
    //WARNING
    //TODO
//     delete logo;
//     QtLogo::constantes = constantes;
//     switch(tipo)
//     {
//     case esfera:
//         logo = new QtLogo(this,&func_esfera);
//         break;
//     case elipsoide:
//         logo = new QtLogo(this,&func_elipsoide);
//         break;
//     case cilindro:
//         logo = new QtLogo(this,&func_cilindro);
//         break;
//     case hiperboloide_1hoja:
//         logo = new QtLogo(this,&func_hiperboloide_1hoja);
//         break;
//     case hiperboloide_2hojas:
//         logo = new QtLogo(this,&func_hiperboloide_2hojas);
//         break;
//     case paraboloide_hiperbolico:
//         logo = new QtLogo(this,&func_paraboloide_hiperbolico);
//         break;
//     case paraboloide_eliptico:
//         logo = new QtLogo(this,&func_paraboloide_eliptico);
//         break;
//     case cono_eliptico:
//         logo = new QtLogo(this,&func_cono_eliptico);
//         break;
//     default:
//         logo = new QtLogo(this,&func_esfera);
//     }
//     logo->ejecentral=axi;
//     logo->octante=oct;
//     logo->esSolido=solid;


    ++dlnum;
    glNewList(dlnum, GL_COMPILE);

    glColor3ub(col.red(),col.green(),col.blue());

    //WARNING
    //TODO

//     if(logo->esSolido)
//     {
//         logo->buildGeometry();
//         logo->draw();
//         logo->esSolido=false;
//     }
//     logo->buildGeometry();
//     logo->draw();

    glEndList();


    m_displayList.insert(funcId, dlnum);



    updateGL();

    setXRotation(4900);
    setYRotation(0);
    setZRotation(3600);
}*/



