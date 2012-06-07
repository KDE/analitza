// /*************************************************************************************
//  *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
//  *                                                                                   *
//  *  This program is free software; you can redistribute it and/or                    *
//  *  modify it under the terms of the GNU General Public License                      *
//  *  as published by the Free Software Foundation; either version 2                   *
//  *  of the License, or (at your option) any later version.                           *
//  *                                                                                   *
//  *  This program is distributed in the hope that it will be useful,                  *
//  *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
//  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
//  *  GNU General Public License for more details.                                     *
//  *                                                                                   *
//  *  You should have received a copy of the GNU General Public License                *
//  *  along with this program; if not, write to the Free Software                      *
//  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
//  *************************************************************************************/
//
// #ifndef KEOMATH_VIEW3D_H
// #define KEOMATH_VIEW3D_H
//
// #include "QGLViewer/qglviewer.h"
// #include "function.h"
// #include <QStackedWidget>
// #include <QMouseEvent>
// // #include "solvers/solvers3d/MarchingCubes/qtlogo.h"
// // #include "solvers/solvers3d/MarchingCubes/formulas.h"
//
// #define MAXAROUND 64
// #define MAXSTRIP 64
// #define MAXALONG  64
//
// class FunctionsModel;
//
// typedef GLfloat point3[3];
//
// class FunctionImpl3D;
// class ANALITZAPLOT_EXPORT Graph3D : public QGLViewer
// {
//     Q_OBJECT
//
// private:
//
//     QMap<QUuid, int> m_displayList;
//     unsigned int num;
//     unsigned int dlnum;
//     QUuid if_quuid;
//
//     //percy gsoc2012 was private
// public:
//     void clearDisplayLists();
//     void generateDisplayLists();
//
// public:
//     Graph3D(FunctionsModel *fm, QWidget *parent = 0);
//     void setFunctionsModel(FunctionsModel *fm);
//
// public slots:
//     void updateSurface(const Function &function);
//     void updateSurfaceImplicit(QUuid funId,QColor col,int index,QList<double> cons,int oct,int axi,bool solid,bool curva,bool xy,double pres);
//     void removeSurface(const QUuid &funid, const QString &funlambda);
//     void pintar_ejes(unsigned int modo);
//     void setXRotation(int angle);
//     void setYRotation(int angle);
//     void setZRotation(int angle);
//
// signals:
//     void viewClicked();
//     void xRotationChanged(int angle);
//     void yRotationChanged(int angle);
//     void zRotationChanged(int angle);
//
// public:
//
//     void mousePressEvent(QMouseEvent * event);
//         //WARNING
//     //TODO
//
// //     void cambiar_funcion(QUuid funcId,QColor col,Figuras tipo, QList<double> constantes,int oct,int axi,bool solid);
//     void graficar_curvas(QUuid funcId,int tipo, QList<double> cons, bool plano, double pres);
//     void drawGrid(float size,int nbSubdivisions);
//
//
//
//
// private:
//     class surfpoint
//     {
//     public:
//         qreal x, y, z;
//     };
//
//     surfpoint surface[MAXALONG][MAXAROUND];
//     surfpoint theStrip[MAXSTRIP][2];
//
//     void draw();
//     void init();
//
//     GLfloat UU(int i);
//     GLfloat VV(int j);
//     QVector3D shape(float u,float v);
//
//         //WARNING
//     //TODO
//
// //     QtLogo *logo;
//
//
//
//
//
//     int xRot;
//     int yRot;
//     int zRot;
//     QPoint lastPos;
//     QColor qtGreen;
//     QColor qtPurple;
//     QSize minimumSizeHint() const;
//     QSize sizeHint() const;
//
//     float camara_x, camara_y, camara_z;
//     unsigned int ejes_modo;
//     QVector3D evalCurve(int tipo,double dZ,qreal t,QList<double> lst);
//
//
//
//     void doSurface();
//     void doQuad(int, int, surfpoint, surfpoint, surfpoint, surfpoint);
//     void doStrip(int, surfpoint, surfpoint, surfpoint, surfpoint);
//     void _emit( surfpoint *buffer );
//
//     qreal umin;
//     qreal umax;
//     qreal vmin;
//     qreal vmax;
//     int usteps;
//     int vsteps;
//
//     GLuint m_indexDisplayList;
//     FunctionImpl3D *m_currentSolver;
//     Function::DrawingType m_drawingType;
//     QColor m_color;
//
// private:
//     FunctionsModel *m_functionsModel;
//     QUuid m_spaceId;
// };
//
// #endif
//
//
