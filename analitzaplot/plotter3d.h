/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      * 
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

#ifndef PLOTTER3D_H
#define PLOTTER3D_H

#include "plotitem.h"


#include <QModelIndex>
#include <QRect>
#include <QVector3D>
#include <QMatrix4x4>

#define GL_GLEXT_PROTOTYPES
#ifdef Q_OS_WIN
#include <windows.h>
#include <GL/glew.h>
#include <GL/glu.h>
#endif

class QAbstractItemModel;
class QPainter;
class QPaintDevice;
class QModelIndex;

namespace Analitza
{

/**
 * @class Plotter3D
 * @author Percy Camilo T. Aucahuasi
 * @short This class manage the OpenGL scene where the plots will be rendered.
 * 
 * Plotter3D provides an agnostic way to manage a 3d scene for draw math plots, 
 * Contains just OpenGL calls, so is uncoupled with QWidget nor QtQuick. This 
 * class needs the PlotsModel (to create the geometry for 3D plots) and also 
 * exposes some methods to change the scene (like hide/show the axis or 
 * reference planes for example)
 * 
 * @internal current implementation is using display lists as geometries store.
 * 
 */

class ANALITZAPLOT_EXPORT Plotter3D
{
    public:
        static const GLubyte XAxisArrowColor[];
        static const GLubyte YAxisArrowColor[];
        static const GLubyte ZAxisArrowColor[];

        //TODO transparency effect when select current item 
//         enum FocusEffect {};

        Plotter3D(QAbstractItemModel* model = 0);
        virtual ~Plotter3D();

        virtual void initGL();
        virtual void setViewport(const QRectF &vp);
        virtual void drawPlots();
        virtual int currentPlot() const = 0;
        virtual void modelChanged() = 0;

        /** Force OpenGL to render the scene. QGLWidget should call updateGL in this method. */
        virtual void renderGL() = 0;
        
        /** Force the plots from @p start to @p end to be recalculated. */
        void updatePlots(const QModelIndex & parent, int start, int end);

        void setModel(QAbstractItemModel* f);
        QAbstractItemModel* model() const { return m_model; }

        PlotStyle plotStyle() const { return m_plotStyle; }
        void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
//         void setPlottingAttributes(PlotStyle st) { m_plotStyle = ps; }
        PlottingFocusPolicy plottingFocusPolicy() const { return m_plottingFocusPolicy; }
        void setPlottingFocusPolicy(PlottingFocusPolicy fp);

        /** Set the scale of all the scene by @p factor */
        void scale(GLdouble factor);

        // Advanced rotation features

        /** Rotates by @p dx and @p dy in screen coordinates. */
        void rotate(int dx, int dy);

        /** Query if there is a valid axis arrow for @p x and @p y screen coordinates. */
        CartesianAxis selectAxisArrow(int x, int y);

        /** Fix the rotation arround @p direction */
        void fixRotation(const QVector3D &direction);
        
        /** Query if the rotation is fixed by a specific direction. */
        bool isRotationFixed() const { return !m_rotFixed.isNull(); }
        
        /** Show a little indicator (as a hint) next to the arrow of @p axis */
        void showAxisArrowHint(CartesianAxis axis);

        /** Hide the current indicator of the axis */
        void hideAxisHint();

        /**  If the flag @p simplerot is true the rotation ignores any fixed or free direction */
        void setUseSimpleRotation(bool simplerot) { m_simpleRotation = simplerot; }

        /**  Get information about the current rotarion approach: if return true then rotation is simple. */
        bool isUsingSimpleRotation() const { return m_simpleRotation; }
        
        /** sets the view to the initial perspective */
        void resetView();

    protected:
        void addPlots(PlotItem* item);
        
    private:
        void resetViewPrivate(const QVector3D& rot);
        
        enum SceneObjectType {Axes, RefPlaneXY, XArrowAxisHint, YArrowAxisHint, ZArrowAxisHint};
        
        PlotItem *itemAt(int row) const;

        void initAxes();
        void initRefPlanes();

        QAbstractItemModel* m_model;

        QMap<PlotItem*, QPair<GLuint, GLuint> > m_itemGeometries; // pair:=<indexdata_id, vertex-normals_id>

        PlotStyle m_plotStyle;
        PlottingFocusPolicy m_plottingFocusPolicy;

        //scene properties
        QMap<SceneObjectType, GLuint > m_sceneObjects;
        QRectF m_viewport;
        const GLfloat m_depth;
        GLdouble m_scale;
        QMatrix4x4 m_rot;
        QVector3D m_rotFixed;
        CartesianAxis m_currentAxisIndicator;
        bool m_simpleRotation;
        QVector3D m_simpleRotationVector;
        QVector3D m_lightpos;
};

}

#endif // PLOTTER3D_H
