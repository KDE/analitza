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
    LocalScene.ScalCoeff =1.0;
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
void PlotsView3D::addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(PlotItem* item)
{
    Q_ASSERT(item);
    makeCurrent(); //NOTE: Remember to call makeCurrent before any OpenGL operation. We might have multiple clients in the same window

    if (Surface* surf = static_cast<Surface*>(item))
    {
        if (surf->faces().isEmpty())
            surf->update(Box3D());
    }
    else if (SpaceCurve* c = static_cast<SpaceCurve*>(item))
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
    PlotItem *item = itemAt(start);

    if (!item || item->spaceDimension() != Dim3D)
        return;

    makeCurrent();

    if (Surface* surf = static_cast<Surface*>(item))
        surf->update(Box3D());
    else if (SpaceCurve* c = static_cast<SpaceCurve*>(item))
        c->update(Box3D());

    GLuint dlid = glGenLists(1);
    m_displayLists[item] = dlid;

    float shininess = 15.0f;
    float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
    float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};
    Q_UNUSED(diffuseColor); //TODO: Percy? ... give more time :)

    //BEGIN display list
    glNewList(dlid, GL_COMPILE);

    // set specular and shiniess using glMaterial (gold-yellow)
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

    // set ambient and diffuse color using glColorMaterial (gold-yellow)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//     glColor3fv(diffuseColor);
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
    else // en caso q el elemento 3d sea superfici ... dibujarla generando la malla
    {
        Surface* surf = static_cast<Surface*>(item);

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

    glDeleteLists(m_displayLists[item], 1);

    if (item->isVisible()) {
//         addFuncs(QModelIndex(), s.row(), s.row());
//igual no usar addFuncs sino la funcion interna pues no actualiza los items si tienen data
        addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(item);
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

    int i, j, startpl, polysize, actualpointindice;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Blend Effect activation:
    if(scene->transparency == 1)  {
        glDepthMask(GL_FALSE);
        //glEnable(GL_CULL_FACE);
    }
    /// Ratation (Animation):
    if(scene->anim == 1) glRotatef(scene->RotStrength,scene->axe_x,scene->axe_y,scene->axe_z);
    /// Axe Drawing :
    if(scene->axe == 1)  glCallList(scene->axeliste);
    /// Plan drawing :
    if(scene->plan == 1) glCallList(scene->planliste);
    /// Box Drawing:
    if(scene->box == 1) glCallList(scene->boxliste);

// Object Drawing :
// If No condition :
    if(scene->condition != 1) {
        if(scene->mesh == 1) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(scene->polyfactor, scene->polyunits);







            ///

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

                    if (!item) continue;

                    if (item->isVisible())
                        addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(item);
                }
            }

            for (int i = 0; i < m_model->rowCount(); ++i)
            {
                PlotItem *item = itemAt(i);
                Surface* surf = static_cast<Surface*>(item);

                if (!item || item->spaceDimension() != Dim3D)
                    continue;
                //TODO: make it possible to plot things that aren't surfaces

                    
                    if (!item->isVisible()) continue;
                    
                    ///
                    //TODO el color ya no se configura en la dlista, en la dlist solo va la geometria
                    GLfloat  fcolor[4] = {item->color().redF(), item->color().greenF(), item->color().blueF(), 1.0f};
                    GLfloat  bcolor[4] = {item->color().redF(), item->color().greenF(), item->color().blueF(), 1.0f};
            
                    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
                    glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, bcolor);
                    
                    ///
                    
                glCallList(m_displayLists[item]);
            }

            glPopMatrix();
            //restauro conf de luces

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LocalScene.frontcol);
    glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, LocalScene.backcol);            
            
            ///

















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
//     qglClearColor(Qt::black);
//
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
//     glEnable(GL_NORMALIZE);
    glEnable(GL_POLYGON_SMOOTH);
//
    glShadeModel(GL_SMOOTH);
//     glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
//     static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
//     glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//     glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    /*
        //Texture initialisation :
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, 3, tex1.width(), tex1.height(), 0,
        GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits() );
     */
    /// For drawing Filled Polygones :
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    glFrontFace (GL_CCW);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, LocalScene.frontcol);
    glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, LocalScene.backcol);
    glMaterialf (GL_FRONT, GL_SHININESS, 35.0);
    glMaterialf (GL_BACK, GL_SHININESS, 35.0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(LocalScene.groundcol[0], LocalScene.groundcol[1],LocalScene.groundcol[2], LocalScene.groundcol[3]);

    //pasar a draw para efecto de transparente a current surface plot
    /// For drawing Lines :
    if(LocalScene.smoothline == 1) {
        glEnable (GL_LINE_SMOOTH);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    }
    else {
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
    }

//     GLUquadricObj *sphereObj;
//     sphereObj = gluNewQuadric();
    LocalScene.axeliste = glGenLists(1);
    glNewList(LocalScene.axeliste, GL_COMPILE );
    glLineWidth(1);
    glBegin( GL_LINES );
    glColor3f (1., 0., 0.);
    glVertex3f( 400.0, 0.0, 0.0);
    glVertex3f(   0.0, 0.0, 0.0);
    glColor3f (0., 1., 0.);
    glVertex3f(0.0, 400.0, 0.0);
    glVertex3f(0.0,   0.0, 0.0);
    glColor3f (0., 0., 1.);
    glVertex3f(0.0, 0.0, 400.0);
    glVertex3f(0.0, 0.0,   0.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f (0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 400.0);
    glVertex3f(10.0, 0.0, 380.0);
    glVertex3f(0.0, 10.0, 380.0);
    glColor3f (0.0, 0.0, 0.3);
    glVertex3f(-10.0, 0.0, 380.0);
    glColor3f (0.0, 0.0, 1.0);
    glVertex3f(0.0, -10.0, 380.0);
    glColor3f (0.0, 0.0, 0.3);
    glVertex3f(10.0, 0.0, 380.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f (0.0, 1., 0.);
    glVertex3f(0.0, 400.0, 0.0);
    glVertex3f(10.0, 380.0, 0.0);
    glVertex3f(0.0, 380.0, 10.0);
    glColor3f (0.0, 0.3, 0.0);
    glVertex3f(-10.0, 380.0, 0.0);
    glColor3f (0.0, 1.0, 0.0);
    glVertex3f(.0, 380.0, -10.0);
    glColor3f (0.0, 0.3, 0.0);
    glVertex3f(10.0, 380.0, 0.0);
    glEnd();

/// Axe X :
    glBegin(GL_TRIANGLE_FAN);
    glColor3f (1.0, 0.0, 0.0);
    glVertex3f(400.0, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glVertex3f(380.0, 0.0, 10.0);
    glColor3f (0.3, 0.0, 0.0);
    glVertex3f(380.0, -10.0, 0.0);
    glColor3f (1.0, 0.0, 0.0);
    glVertex3f(380.0, 0.0, -10.0);
    glColor3f (0.3, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glEnd();

    glColor3f (1.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(400.0, 0.0, 0.0);
    glVertex3f(380.0, 10.0, 0.0);
    glVertex3f(380.0, 0.0, 10.0);
    glVertex3f(380.0, -10.0, 0.0);
    glVertex3f(380.0, 0.0, -10.0);
    glVertex3f(380.0, 10.0, 0.0);
    glEnd();

    glColor3f (1., 0., 0.);
    glRasterPos3i(410, 10, 10);
    glCallLists(strlen("X"),GL_UNSIGNED_BYTE,"X");

    glColor3f (0.7, 0.7, 0.7);
    glTranslatef(410.0, 4.0, 4.0);
    //gluSphere(sphereObj, .0, 16, 16);
    glTranslatef(-410.0, -4.0, -4.0);

    glColor3f (0., 1., 0.);
    glRasterPos3i(10, 410, 10);
    //printString("Y");
    glCallLists(strlen("Y"),
                GL_UNSIGNED_BYTE,
                "Y");

    glColor3f (1., 1., 0.);
    glTranslatef(4.0, 410.0, 4.0);
    //gluSphere(sphereObj, 8.0, 32, 32);
    glTranslatef(-4.0, -410.0, -4.0);

    glColor3f (0., 0., 1.);
    glRasterPos3i(10, 10, 410);
    //printString("Z");
    glCallLists(strlen("Z"),
                GL_UNSIGNED_BYTE,
                "Z");

    glColor3f (0., 0.7, 0.7);
    glTranslatef(4.0, 4.0, 410.0);
    //gluSphere(sphereObj, 10.0, 16, 16);
    glTranslatef(-4.0, -4.0, -410.0);
    glLineWidth(0.9);
    glEndList();

    LocalScene.planliste = glGenLists(1);
    glNewList(LocalScene.planliste, GL_COMPILE );
    glLineWidth(1);
    glColor3f (0.8, 0., 0.7);
    glBegin( GL_LINES );
    glVertex3f(-150.0, 600.0, LocalScene.zminim);
    glVertex3f(-150.0,-600.0, LocalScene.zminim);
    glVertex3f(0.0, 600.0, LocalScene.zminim);
    glVertex3f(0.0,-600.0, LocalScene.zminim);

    glVertex3f(150.0, 600.0, LocalScene.zminim);
    glVertex3f(150.0,-600.0, LocalScene.zminim);
    glVertex3f(600.0, -150.0, LocalScene.zminim);
    glVertex3f(-600.0,-150.0, LocalScene.zminim);

    glVertex3f(600.0, 0.0, LocalScene.zminim);
    glVertex3f(-600.0, 0.0, LocalScene.zminim);
    glVertex3f(600.0, 150.0, LocalScene.zminim);
    glVertex3f(-600.0, 150.0, LocalScene.zminim);

    glVertex3f(-75.0, 600.0, LocalScene.zminim);
    glVertex3f(-75.0,-600.0, LocalScene.zminim);
    glVertex3f(-225.0, 600.0, LocalScene.zminim);
    glVertex3f(-225.0,-600.0, LocalScene.zminim);
    glVertex3f(-300.0, 600.0, LocalScene.zminim);
    glVertex3f(-300.0,-600.0, LocalScene.zminim);
    glVertex3f(-375.0, 600.0, LocalScene.zminim);
    glVertex3f(-375.0,-600.0, LocalScene.zminim);
    glVertex3f(-450.0, 600.0, LocalScene.zminim);
    glVertex3f(-450.0,-600.0, LocalScene.zminim);
    glVertex3f(-525.0, 600.0, LocalScene.zminim);
    glVertex3f(-525.0,-600.0, LocalScene.zminim);

    glVertex3f(75.0, 600.0, LocalScene.zminim);
    glVertex3f(75.0,-600.0, LocalScene.zminim);
    glVertex3f(225.0, 600.0, LocalScene.zminim);
    glVertex3f(225.0,-600.0, LocalScene.zminim);
    glVertex3f(300.0, 600.0, LocalScene.zminim);
    glVertex3f(300.0,-600.0, LocalScene.zminim);
    glVertex3f(375.0, 600.0, LocalScene.zminim);
    glVertex3f(375.0,-600.0, LocalScene.zminim);
    glVertex3f(450.0, 600.0, LocalScene.zminim);
    glVertex3f(450.0,-600.0, LocalScene.zminim);
    glVertex3f(525.0, 600.0, LocalScene.zminim);
    glVertex3f(525.0,-600.0, LocalScene.zminim);

    glVertex3f(600.0,-75.0, LocalScene.zminim);
    glVertex3f(-600.0,-75.0, LocalScene.zminim);
    glVertex3f(600.0,-225.0, LocalScene.zminim);
    glVertex3f(-600.0,-225.0, LocalScene.zminim);
    glVertex3f(600.0,-300.0, LocalScene.zminim);
    glVertex3f(-600.0,-300.0, LocalScene.zminim);
    glVertex3f(600.0,-375.0, LocalScene.zminim);
    glVertex3f(-600.0,-375.0, LocalScene.zminim);
    glVertex3f(600.0,-450.0, LocalScene.zminim);
    glVertex3f(-600.0,-450.0, LocalScene.zminim);
    glVertex3f(600.0,-525.0, LocalScene.zminim);
    glVertex3f(-600.0,-525.0, LocalScene.zminim);

    glVertex3f(600.0,75.0, LocalScene.zminim);
    glVertex3f(-600.0,75.0, LocalScene.zminim);
    glVertex3f(600.0,225.0, LocalScene.zminim);
    glVertex3f(-600.0,225.0, LocalScene.zminim);
    glVertex3f(600.0,300.0, LocalScene.zminim);
    glVertex3f(-600.0,300.0, LocalScene.zminim);
    glVertex3f(600.0,375.0, LocalScene.zminim);
    glVertex3f(-600.0,375.0, LocalScene.zminim);
    glVertex3f(600.0,450.0, LocalScene.zminim);
    glVertex3f(-600.0,450.0, LocalScene.zminim);
    glVertex3f(600.0,525.0, LocalScene.zminim);
    glVertex3f(-600.0,525.0, LocalScene.zminim);
    glEnd();
    glLineWidth(0.9);
    glEndList();

}

void PlotsView3D::resizeGL(int newwidth, int newheight)
{
    static int CornerH, CornerW;
    static int heightresult, widthresult;

    int tmp, starth, startw;
    //GLdouble mm[16];
    //glGetDoublev(GL_MODELVIEW_MATRIX,mm);
    CornerH = (int)(newheight/2);
    CornerW = (int)(newwidth/2);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(newwidth > newheight) {
        tmp = newwidth;
        starth=(newwidth-newheight)/4;
        startw=0;
    }
    else {
        tmp = newheight;
        startw = (newheight-newwidth)/4;
        starth=0;
    }
    glViewport(0, 0, tmp, tmp);
    glFrustum(-250+startw, 250+startw, -250+starth, 250+starth, 350.0, 3000.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -800.0 );

    //glMultMatrixd(mm);

    heightresult = tmp/2;
    widthresult = 250+starth;
}

void PlotsView3D::mousePressEvent(QMouseEvent *e)
{
    if ( e->button() == Qt::LeftButton ) btgauche = 1;
    else btgauche = 0;
    if ( e->button() == Qt::RightButton ) btdroit = 1;
    else btdroit = 0;
    if ( e->button() == Qt::MidButton ) btmilieu = 1;
    else btmilieu = 0;

    old_y = LocalScene.oldRoty = e->y();
    old_x = LocalScene.oldRotx = e->x();
}



/* NB. OpenGL Matrices are COLUMN major. */
#define MAT(m,r,c) (m)[(c)*4+(r)]

/* Here's some shorthand converting standard (row,column) to index. */
#define m11 MAT(m,0,0)
#define m12 MAT(m,0,1)
#define m13 MAT(m,0,2)
#define m14 MAT(m,0,3)
#define m21 MAT(m,1,0)
#define m22 MAT(m,1,1)
#define m23 MAT(m,1,2)
#define m24 MAT(m,1,3)
#define m31 MAT(m,2,0)
#define m32 MAT(m,2,1)
#define m33 MAT(m,2,2)
#define m34 MAT(m,2,3)
#define m41 MAT(m,3,0)
#define m42 MAT(m,3,1)
#define m43 MAT(m,3,2)
#define m44 MAT(m,3,3)
//    static double m[16];

void PlotsView3D::mouseMoveEvent(QMouseEvent *e)
{
    static double m[16];
    static double anglefinal=0;

    LocalScene.RotStrength = sqrt((LocalScene.oldRotx-e->x())*(LocalScene.oldRotx-e->x()) +
                                  (LocalScene.oldRoty-e->y())*(LocalScene.oldRoty-e->y()))/2.;
    LocalScene.oldRoty = e->y();
    LocalScene.oldRotx = e->x();

// Scale function :
    if(btdroit ==1)  {
        if(old_y - e->y() > 0 ) LocalScene.ScalCoeff = 1.02f;
        else if( LocalScene.ScalCoeff > 0.1f ) LocalScene.ScalCoeff = 0.98f;
        glScalef(LocalScene.ScalCoeff, LocalScene.ScalCoeff, LocalScene.ScalCoeff);
        LocalScene.view_rotx = LocalScene.view_roty = 0.0;
    }
// Rotational function :
    if(btgauche ==1) {
        LocalScene.view_roty = -(old_y - e->y());
        LocalScene.view_rotx = -(old_x - e->x());
        LocalScene.ScalCoeff = 1.0;

        glGetIntegerv(GL_VIEWPORT,LocalScene.viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX,LocalScene.matrix);
        memcpy(m, LocalScene.matrix, 16*sizeof(GLdouble));

        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++///
        GLdouble det;
        GLdouble d12, d13, d23, d24, d34, d41;
        GLdouble tmp[16]; /* Allow out == in. */

        /* Inverse = adjoint / det. (See linear algebra texts.)*/
        /* pre-compute 2x2 dets for last two rows when computing */
        /* cofactors of first two rows. */
        d12 = (m31*m42-m41*m32);
        d13 = (m31*m43-m41*m33);
        d23 = (m32*m43-m42*m33);
        d24 = (m32*m44-m42*m34);
        d34 = (m33*m44-m43*m34);
        d41 = (m34*m41-m44*m31);

        tmp[0] =  (m22 * d34 - m23 * d24 + m24 * d23);
        tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
        tmp[2] =  (m21 * d24 + m22 * d41 + m24 * d12);
        tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

        /* Compute determinant as early as possible using these cofactors. */
        det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

        /* Run singularity test. */
        if (det == 0.0) {
            printf("invert_matrix: Warning: Singular matrix.\n");
            /*    memcpy(out,_identity,16*sizeof(double)); */
        }
        else {
            GLdouble invDet = 1.0 / det;
            /* Compute rest of inverse. */
            tmp[0] *= invDet;
            tmp[1] *= invDet;
            tmp[2] *= invDet;
            tmp[3] *= invDet;

            tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
            tmp[5] =  (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
            tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
            tmp[7] =  (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

            /* Pre-compute 2x2 dets for first two rows when computing */
            /* cofactors of last two rows. */
            d12 = m11*m22-m21*m12;
            d13 = m11*m23-m21*m13;
            d23 = m12*m23-m22*m13;
            d24 = m12*m24-m22*m14;
            d34 = m13*m24-m23*m14;
            d41 = m14*m21-m24*m11;

            tmp[8] =  (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
            tmp[9] = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
            tmp[10] =  (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
            tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
            tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
            tmp[13] =  (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
            tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
            tmp[15] =  (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

            memcpy(LocalScene.matrixInverse, tmp, 16*sizeof(GLdouble));
        }
        double ax,ay,az;
        ax = LocalScene.view_roty;
        ay = LocalScene.view_rotx;
        az = 0.0;
        anglefinal += (angle = sqrt(ax*ax + ay*ay)/(double)(LocalScene.viewport[2]+1)*360.0);
        /* Use inverse matrix to determine local axis of rotation */
        LocalScene.axe_x = Axe_x = LocalScene.matrixInverse[0]*ax + LocalScene.matrixInverse[4]*ay;
        LocalScene.axe_y = Axe_y = LocalScene.matrixInverse[1]*ax + LocalScene.matrixInverse[5]*ay;
        LocalScene.axe_z = Axe_z = LocalScene.matrixInverse[2]*ax + LocalScene.matrixInverse[6]*ay;
        ///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++///
        glRotatef(angle,Axe_x,Axe_y,Axe_z);
    }
    old_y = e->y();
    old_x = e->x();
    updateGL();
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
