/*************************************************************************************
 *  Copyright (C) 2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      * 
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

#ifndef PLOTTER3D_H
#define PLOTTER3D_H

#include "shapeinterface.h"
#include "curve.h"

#include <QModelIndex>
#include <QRect>
#include <QVector3D>
#include <QMatrix4x4>
// #include <QGLFunctions> //NOTE we use glew until QTBUG-5729 get fixed (issue with opengl and threads -contex buffers

#define GL_GLEXT_PROTOTYPES
#include <GL/glu.h>

#include <QGLWidget>

class QAbstractItemModel;
class QPainter;
class QPaintDevice;
class QModelIndex;

namespace Analitza
{

/**
 * \class Plotter
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief This class manage the OpenGL scene where the plots will be rendered.
 *
 * Plotter provides an agnostic way to manage a 3d scene for draw math plots, 
 * Contains just OpenGL calls, so is uncoupled with QWidget nor QtQuick. This 
 * class needs the PlotsModel (to create the geometry for 3D plots) and also 
 * exposes some methods to change the scene (like hide/show the axis or 
 * reference planes for example)
 */

class ANALITZAPLOT_EXPORT Plotter //NOTE we use glew until QTBUG-5729 get fixed (issue with opengl and threads -contex buffers) : protected QGLFunctions
{
public:
    Plotter(QAbstractItemModel* model = 0);
    virtual ~Plotter();

    virtual void initialize();
    QAbstractItemModel *model() const;
    virtual void plot(/*const QGLContext* context*/);
//     void setModel(QAbstractItemModel* model); not yet
    virtual void setViewport(GLint x, GLint y, GLsizei width, GLsizei height);
    
private:
    QAbstractItemModel *m_model;
    
    
    
    
///
    
private:
    //WARNING experimental
        GLhandleARB shader_programme;
        GLuint vbo;
    Curve c;

        

//         /** Force OpenGL to render the scene. QGLWidget should call updateGL in this method. */
//         virtual void renderGL() = 0;
//         
//         /** Force the plots from @p start to @p end to be recalculated. */
//         void updatePlots(const QModelIndex & parent, int start, int end);
// 
//         void setModel(QAbstractItemModel* f);
//         QAbstractItemModel* model() const { return m_model; }
// 
//         PlotStyle plotStyle() const { return m_plotStyle; }
//         void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
// //         void setPlottingAttributes(PlotStyle st) { m_plotStyle = ps; }
//         PlottingFocusPolicy plottingFocusPolicy() const { return m_plottingFocusPolicy; }
//         void setPlottingFocusPolicy(PlottingFocusPolicy fp);
// 
//         /** Set the scale of all the scene by @p factor */
//         void scale(GLdouble factor);
// 
//         // Advanced rotation features
// 
//         /** Rotates by @p dx and @p dy in screen coordinates. */
//         void rotate(int dx, int dy);
// 
//         /** Query if there is a valid axis arrow for @p x and @p y screen coordinates. */
//         CartesianAxis selectAxisArrow(int x, int y);
// 
//         /** Fix the rotation arround @p direction */
//         void fixRotation(const QVector3D &direction);
//         
//         /** Query if the rotation is fixed by a specific direction. */
//         bool isRotationFixed() const { return !m_rotFixed.isNull(); }
//         
//         /** Show a little indicator (as a hint) next to the arrow of @p axis */
//         void showAxisArrowHint(CartesianAxis axis);
// 
//         /** Hide the current indicator of the axis */
//         void hideAxisHint();
// 
//         /**  If the flag @p simplerot is true the rotation ignores any fixed or free direction */
//         void setUseSimpleRotation(bool simplerot) { m_simpleRotation = simplerot; }
// 
//         /**  Get information about the current rotarion approach: if return true then rotation is simple. */
//         bool isUsingSimpleRotation() const { return m_simpleRotation; }
//         
//         /** sets the view to the initial perspective */
//         void resetView();
// 
//     protected:
//         void addPlots(PlotItem* item);
//         
//     private:
//         void resetViewPrivate(const QVector3D& rot);
//         
//         enum SceneObjectType {Axes, RefPlaneXY, XArrowAxisHint, YArrowAxisHint, ZArrowAxisHint};
//         
//         PlotItem *itemAt(int row) const;
// 
//         void initAxes();
//         void initRefPlanes();
// 
//         QAbstractItemModel* m_model;
// 
//         QMap<PlotItem*, QPair<GLuint, GLuint> > m_itemGeometries; // pair:=<indexdata_id, vertex-normals_id>
// 
//         PlotStyle m_plotStyle;
//         PlottingFocusPolicy m_plottingFocusPolicy;
// 
//         //scene properties
//         QMap<SceneObjectType, GLuint > m_sceneObjects;
//         QRectF m_viewport;
//         const GLfloat m_depth;
//         GLdouble m_scale;
//         QMatrix4x4 m_rot;
//         QVector3D m_rotFixed;
//         CartesianAxis m_currentAxisIndicator;
//         bool m_simpleRotation;
//         QVector3D m_simpleRotationVector;
//         QVector3D m_lightpos;
};


///

class ANALITZAPLOT_EXPORT MyGLDrawer : public QGLWidget, public Plotter
{
    Q_OBJECT
    
public:
    MyGLDrawer(QWidget *parent = 0)
        : QGLWidget(parent) {
            setMinimumSize(640,640);
        }
        
    virtual ~MyGLDrawer() {}
    
    virtual void keyPressEvent(QKeyEvent* )
    {
        close();
    }

protected:

    void initializeGL()
    {
        initialize();
    }

    void resizeGL(int w, int h)
    {
        setViewport(0,0,w,h);
    }

    void paintGL()
    {
        plot(/*context()*/);
        

    }
};


}

#endif // PLOTTER3D_H
