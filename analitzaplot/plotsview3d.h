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

#ifndef PLOTSVIEW3D_H
#define PLOTSVIEW3D_H

#include <QGLWidget>
#include <QMouseEvent>

#include <QModelIndex>
#include "analitzaplotexport.h"

class Surface;
class PlotItem;
class PlotsProxyModel;
class QItemSelectionModel;

//TODO refactor this code: is from K3DSurf but we need more clean API (move some of this to plotter3d)
//NOTE Aleix: all of this is work in progress, wait until tomorrow ;) (the best part: 
// now we don't need libQGLViewer :)) ... you can see the result by running plots3ddemo (better rendering)

struct  Scene {
    GLfloat * ArrayNorVer_localPt;
    GLuint  * PolyIndices_localPt;
    GLuint  * NbPolygnNbVertexPt ;
    GLuint  * PolyIndices_localPtMin;
    int       NbPolygnNbVertexPtMin ;
    GLfloat * ArrayNorVer_localPtCND;
    GLuint   *PolyIndices_localVerifyCND;
    GLuint   *PolyIndices_localNotVerifyCND;
    GLuint   *PolyIndices_localBorderCND;

    GLint    PolyNumber;
    GLint    PolyNumberBorderCND;
    GLint    PolyNumberVerifyCND;
    GLint    PolyNumberNotVerifyCND;

    GLint    border;
    GLint    cndmesh;
    GLint    drawcnd;
    GLint    anim;
    GLint    morph;
    GLint    typedrawing;
    GLint    mesh;
    GLint    front;
    GLint    back;
    GLint    smoothline;
    GLint    transparency;
    GLint    smoothpoly;
    GLint    box;
    GLint    isobox;
    GLint    interior;
    GLint    exterior;
    GLint    infos;
    GLint    infosdetails[10];
    GLint    line; ///Draw line on top of filled polygons
    GLint    axe;
    GLint    clip;
    GLint    norm;
    GLint    plan;
    GLint    condition;
    GLuint   axeliste;
    GLuint   boxliste;
    GLuint   planliste;
    GLfloat  boxlimits[18];
    GLuint   texture[100];
    GLint    ColorOrTexture[10];
    GLuint   moreliste[10];
    GLuint  *morelistept[10];
    GLfloat *morefloatpt[10];
    GLfloat  morefloat[10];
    GLfloat  frontcol[4];
    GLfloat  backcol[4];
    GLfloat  gridcol[4];
    GLfloat  groundcol[4];
    GLfloat  condcol[4];

    GLfloat RotStrength, oldRotx, oldRoty;
    GLfloat zminim, BoxLimits[3][6];
    GLint viewport[4];
    GLfloat polyfactor;
    GLfloat polyunits;
    double matrix[16];
    double matrixInverse[16];
    double axe_x, axe_y, axe_z, ScalCoeff, view_rotx, view_roty, view_rotz;
};
              
class ANALITZAPLOT_EXPORT PlotsView3D : public  QGLWidget
{
    Q_OBJECT

public:
    PlotsView3D(QWidget *parent = 0, PlotsProxyModel *m = 0);
    virtual ~PlotsView3D();

    void setModel(QAbstractItemModel* f);
    void setSelectionModel(QItemSelectionModel* selection);
    PlotItem *itemAt(int row) const;

public slots:
    void resetView();
    
private slots:
    void addFuncsInternalVersionWithOutUpdateGLEstaSellamadesdeElDraw(PlotItem* item);
    void updateFuncs(const QModelIndex &indexf,const QModelIndex &indext);
    void addFuncs(const QModelIndex &index,int,int);
    void removeFuncs(const QModelIndex &index,int,int);

    void testvisible(const QModelIndex &, const QModelIndex &);
    
private:
    int currentFunction() const;
    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
    QAbstractItemModel *m_model;
    QItemSelectionModel* m_selection;
    
//     <graphid, displaylistid>
    QMap<PlotItem*, GLuint> m_displayLists;

    int btgauche, btdroit, btmilieu;
    double Axe_x, Axe_y, Axe_z, angle, val1, val2, val3, old_x, old_y, old_z;
    Scene  LocalScene;
};

#endif


