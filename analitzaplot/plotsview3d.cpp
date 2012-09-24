/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#include "plotsview3d.h"
#include "plotsmodel.h"
#include "plotsproxymodel.h"
#include "surface.h"
#include "private/utils/box3d.h"
#include "private/utils/triangle3d.h"
#include "spacecurve.h"
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

Q_DECLARE_METATYPE(PlotItem*);

static GLfloat staticcolor[20]= {
//Green(Back face)
    0.0,0.7,0.0,1.0,
//Gold (Front face)
    249./255.,170./255.,0.0,1.0,
// grid:
    0.1,0.4,0.1,1.0,
//Background:
    0.0,0.0,0.0,1.0,
//Condition:
    1.0,0.2,0.0,1.0
}; // greencol, goldcol,...

#warning should respect PlotItem::plotStyle (or move the style to the whole view, which I think it would make sense)

PlotsView3D::PlotsView3D(QWidget *parent, PlotsProxyModel* m)
    : QGLWidget(parent), m_model(m),m_selection(0)
{
    //TODO remove qglviewer
//     setGridIsDrawn(true);
//     setAxisIsDrawn(true);

//     setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));
//     setSceneRadius(5); //TODO no magic number, 5 is the size of the ¿coords?

    static int NBGlWindow = 0;

//     MaximumNumberPoints    = 1000000;
//     MaximumNumberTriangles = 2000000;
//
//     LocalScene.ArrayNorVer_localPt    = new GLfloat[6*MaximumNumberPoints];
//     PolyIndices_local                 = new GLuint [3*MaximumNumberTriangles];
//     LocalScene.PolyIndices_localPtMin = new GLuint [3*MaximumNumberTriangles];
//     TypeIsoSurfaceTriangleListeCND    = new int    [MaximumNumberTriangles];
//     //NbPolygnNbVertexMin = new GLuint[1];
//     latence = 30;
//     colortype = 0;
    LocalScene.RotStrength=0;
    LocalScene.zminim = -400.0;
    LocalScene.polyfactor = 1.0;
    LocalScene.polyunits = 1.0;
//     LocalScene.ScalCoeff =1.0;
    LocalScene.view_rotx = LocalScene.view_roty = 90.0;
    LocalScene.view_rotz = 1.0;
    val1 = val2 = val3 = 0.0;
//     nb_colone = nb_ligne = 25;
//     coupure_col = coupure_ligne = 0;
//     isoline = isocolumn = isodepth = 26;
//     cpisoline = cpisocolumn = cpisodepth = 0;
//     width = 620; height = 620;
    resize(620, 620);
//     NbMorphFrame = 30; /// The maximum of morph sequences...
//     timer = new QTimer( this );
//     connect( timer, SIGNAL(timeout()), this, SLOT(updateGL()) );

//     pixmap = new QPixmap(620,620);
//     png_ok = 1; jpg_ok = bmp_ok = -1; quality_image = 50;

    LocalScene.gridcol[0]   = staticcolor[8];
    LocalScene.gridcol[1]   = staticcolor[9];
    LocalScene.gridcol[2]   = staticcolor[10];
    LocalScene.gridcol[3]   = 0.39;//staticcolor[11];

    LocalScene.groundcol[0] = staticcolor[12];
    LocalScene.groundcol[1] = staticcolor[13];
    LocalScene.groundcol[2] = staticcolor[14];
    LocalScene.groundcol[3] = 0.39;//staticcolor[15];

    LocalScene.backcol[0]   = staticcolor[0];
    LocalScene.backcol[1]   = staticcolor[1];
    LocalScene.backcol[2]   = staticcolor[2];
    LocalScene.backcol[3]   = 0.39;//staticcolor[3];

    LocalScene.frontcol[0]  = staticcolor[4];
    LocalScene.frontcol[1]  = staticcolor[5];
    LocalScene.frontcol[2]  = staticcolor[6];
    LocalScene.frontcol[3]  = 0.39;//staticcolor[7];

    LocalScene.condcol[0]   = staticcolor[16];
    LocalScene.condcol[1]   = staticcolor[17];
    LocalScene.condcol[2]   = staticcolor[18];
    LocalScene.condcol[3]   = 0.39;//staticcolor[19];

    LocalScene.border  = 1;
    LocalScene.cndmesh =-1;
    LocalScene.drawcnd = 1;

    LocalScene.mesh  = 1;
    LocalScene.line  = 1;
    LocalScene.infos = 1;
    LocalScene.infosdetails[0] = LocalScene.infosdetails[1] = LocalScene.infosdetails[2] = 1;
    LocalScene.axe   = 1;
    LocalScene.box   = 1;
    LocalScene.isobox= 1;
    LocalScene.typedrawing =-1;
    LocalScene.smoothpoly  = 1;
    LocalScene.smoothline  =-1;
    LocalScene.anim  =-1;
    LocalScene.morph =-1;
    LocalScene.norm  =-1;
    LocalScene.plan  = 1;
    LocalScene.front = 1;
    LocalScene.back  = 1;
    LocalScene.condition  = -1;
    LocalScene.transparency = -1;
    //LocalScene.ArrayNorVer_localPt = ArrayNorVer_local;
//     LocalScene.PolyIndices_localPt = PolyIndices_local;
    //LocalScene.PolyIndices_localPtMin = PolyIndices_localMin;
//     LocalScene.morelistept[9] = NbPolygnNbVertex;
//     IDGlWindow = NBGlWindow;
//     PolyNumber        = 0;
//     VertexNumber      = 0;

    NBGlWindow++;
    if(NBGlWindow == 1) {
        GLfloat ambient[] = { .0, .0, .0, 1.0 };
        GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
        GLfloat local_view[] = { 1.0 };
        static GLfloat position[4] = {0.0, 0.0,1000.0, 1.0};

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
        glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    }
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

void PlotsView3D::resetView()
{
    //TODO remove qglviewer
//     setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));

//     camera()->setUpVector(qglviewer::Vec(-1,-1,1), false);
//     camera()->setPosition(qglviewer::Vec(7,7,7));
//     camera()->lookAt(qglviewer::Vec(0,0,0));
//     camera()->setRevolveAroundPoint(qglviewer::Vec(0,0,0));
}

// void PlotsView3D::resizeScene(int v)
// {
//     qreal si = (qreal)v;

//TODO remove qglviewer
//     setSceneCenter(qglviewer::Vec(0.f,0.f,0.f));
//     setSceneRadius(si);

//     updateGL();
// }

//se llama cuando se oculta o se visualiza el plot por medio de la propiedad plot::visible
void PlotsView3D::addFuncsInternal(PlotItem* item)
{
    Q_ASSERT(item);
    makeCurrent(); //NOTE: Remember to call makeCurrent before any OpenGL operation. We might have multiple clients in the same window

    if (Surface* surf = dynamic_cast<Surface*>(item))
    {
        if (surf->faces().isEmpty())
            surf->update(Box3D());
    }
    else if (SpaceCurve* c = dynamic_cast<SpaceCurve*>(item))
    {
        if (c->points().isEmpty())
            c->update(Box3D());
    }

    GLuint dlid = glGenLists(1);
    m_displayLists[item] = dlid;

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

    //BEGIN display list
    glNewList(dlid, GL_COMPILE);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3fv(diffuseColor);
    glColor3ub(item->color().red(), item->color().green(), item->color().blue());

    if (SpaceCurve *curve = dynamic_cast<SpaceCurve*>(item))
    {
        glEnable(GL_LINE_SMOOTH);

        glLineWidth(2.5);

        glBegin(GL_LINES);
        {
            const QVector<QVector3D> points = curve->points();

            //TODO copy approach from plotter 2d and use jumps optimization
            for (int i = 0; i < points.size() -1 ; ++i)
            {
                glVertex3d(points[i].x(), points[i].y(), points[i].z());
                glVertex3d(points[i+1].x(), points[i+1].y(), points[i+1].z());
            }
        }
        glEnd();
        glDisable(GL_LINE_SMOOTH);
    }
    else
    {
        Surface* surf = static_cast<Surface*>(item);

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
    }

    glEndList();
    //END display list
}

void PlotsView3D::addFuncs(const QModelIndex & parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
    Q_UNUSED(parent);

    for(int i=start; i<=end; i++) {
        PlotItem *item = itemAt(i);
        if (item && item->spaceDimension() & Dim3D && item->isVisible()) {
            addFuncsInternal(item);
        }
    }

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
    for(int i=s.row(); i<=e.row(); i++) {
        PlotItem *item = itemAt(i);

        if (!item)
            return;

        glDeleteLists(m_displayLists[item], 1);

        if (item->isVisible()) {
    //         addFuncs(QModelIndex(), s.row(), s.row());
    //igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data
            addFuncsInternal(item);
        }
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

void PlotsView3D::paintGL()
{
    
    Scene *scene = &LocalScene;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawPlots();

    /// Blend Effect activation:
    if(scene->transparency == 1)  {
        glDepthMask(GL_FALSE);
        //glEnable(GL_CULL_FACE);
    }
    /// Ratation (Animation):
//     if(scene->anim == 1) 
//         glRotatef(scene->RotStrength,scene->axe_x,scene->axe_y,scene->axe_z);
//     /// Box Drawing:
//     if(scene->box == 1) glCallList(scene->boxliste);

// Object Drawing :
// If No condition :
    if(scene->condition != 1) {
        if(scene->mesh == 1) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(scene->polyfactor, scene->polyunits);

            glPushMatrix();
            static const double ss = 40; // space size : with scale factor 0 - 80 -> esto hace que cada eje este visible en [0,+/-10]
            glScalef(ss, ss, ss);

            if(!m_model)
                return;

            //NOTE si esto pasa entonces quiere decir que el proxy empezado a filtrar otros items
            // y si es asi borro todo lo que esta agregado al la memoria de la tarjeta
            //esto se hace pues m_displayLists es una copia del estado actual de model
            if (m_model->rowCount() != m_displayLists.count())
            {
                foreach (PlotItem *item, m_displayLists.keys())
                {
                    glDeleteLists(m_displayLists.take(item), 1);
                }
            }

            /// luego paso a verificar el map de display list no este vacio ... si lo esta lo reconstruyo

            if (m_displayLists.isEmpty())
            {
                //NOTE no llamar a ninguna funcion que ejucute un updategl, esto para evitar una recursividad
                for (int i = 0; i < m_model->rowCount(); ++i) {
                    PlotItem* item = dynamic_cast<Surface*>(itemAt(i));

                    if (item && item->isVisible())
                        addFuncsInternal(item);
                }
            }

            for (int i = 0; i < m_model->rowCount(); ++i)
            {
                PlotItem *item = itemAt(i);

                if (!item || item->spaceDimension() != Dim3D || !item->isVisible())
                    continue;
                
                //TODO el color ya no se configura en la dlista, en la dlist solo va la geometria
                GLfloat  fcolor[4] = {float(item->color().redF()), float(item->color().greenF()), float(item->color().blueF()), 1.0f};
        
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
                glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, fcolor);
                
                glCallList(m_displayLists[item]);
            }

            glPopMatrix();
            //restauro conf de luces

            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LocalScene.frontcol);
            glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, LocalScene.backcol);            
            
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
        }

        if(scene->line == 1) {
            glColor4f (scene->gridcol[0], scene->gridcol[1], scene->gridcol[2], scene->gridcol[3]);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            (scene->typedrawing == 1) ?
            glDrawElements(GL_TRIANGLES, scene->PolyNumber, GL_UNSIGNED_INT, scene->PolyIndices_localPt)
            :
            glDrawElements(GL_QUADS, scene->PolyNumber, GL_UNSIGNED_INT, scene->PolyIndices_localPt);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
// End If No condition

    if(scene->transparency == 1)  {
        glDepthMask(GL_TRUE);
//glDisable(GL_CULL_FACE);
    }

}

void PlotsView3D::initializeGL()
{
    initGL();
}

void PlotsView3D::resizeGL(int newwidth, int newheight)
{
    setViewport(QRect(0,0,newwidth,newheight));
}

void PlotsView3D::mousePressEvent(QMouseEvent *e)
{
    buttons = e->buttons();

    old_y = LocalScene.oldRoty = e->y();
    old_x = LocalScene.oldRotx = e->x();
}

void PlotsView3D::modelChanged()
{
//     if(m_model) {
//         disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
//         disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
//         disconnect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
//         disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(testvisible(QModelIndex,QModelIndex)));
//     }
// 
//     m_model=f;
// 
//     for (int i = 0; i < m_model->rowCount(); ++i) {
//         if (itemAt(i)) {
//             glDeleteLists(m_displayLists[itemAt(i)], 1);
//             addFuncs(QModelIndex(), 0, m_model->rowCount()-1);
//         }
//     }
// 
//     connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
//     connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
//     connect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
//     connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(testvisible(QModelIndex,QModelIndex)));
// 
//     updateGL();
}

void PlotsView3D::renderGL()
{
    updateGL();
}

void PlotsView3D::wheelEvent(QWheelEvent* ev)
{
    scale(1.f+ev->delta()/1000.f);
}

void PlotsView3D::mouseMoveEvent(QMouseEvent *e)
{
    LocalScene.RotStrength = sqrt((LocalScene.oldRotx-e->x())*(LocalScene.oldRotx-e->x()) +
                                  (LocalScene.oldRoty-e->y())*(LocalScene.oldRoty-e->y()))/2.;
    LocalScene.oldRoty = e->y();
    LocalScene.oldRotx = e->x();
//TODO translate with middle btn
// Rotational function :
    if(buttons & Qt::LeftButton)
        rotate(old_x - e->x(), old_y - e->y());

    old_y = e->y();
    old_x = e->x();
}

PlotItem* PlotsView3D::itemAt(int row) const
{
    if(!m_model)
        return 0;
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

//     qDebug() << plot->expression().toString() << plot->spaceDimension();

    if (plot->spaceDimension() != Dim3D)
        return 0;

    return plot;
}
