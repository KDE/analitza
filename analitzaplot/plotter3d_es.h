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

#ifndef PLOTTER3DES_H
#define PLOTTER3DES_H

#include "plotitem.h"

#include <QModelIndex>
#include <QRect>
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "analitzaplotexport.h"

class QAbstractItemModel;

namespace Analitza
{

/**
 * \class Plotter3DES
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief This class manage the OpenGL scene where the plots will be rendered.
 *
 * Plotter3DES provides an agnostic way to manage a 3d scene for draw math plots,
 * Contains just OpenGL calls, so is uncoupled with QWidget nor QtQuick. This 
 * class needs the PlotsModel (to create the geometry for 3D plots) and also 
 * exposes some methods to change the scene (like hide/show the axis or 
 * reference planes for example)
 */

class ANALITZAPLOT_EXPORT Plotter3DES : private QOpenGLFunctions
{
    public:
        //TODO transparency effect when select current item
//         enum FocusEffect {};

        explicit Plotter3DES(QAbstractItemModel* model = nullptr);
        virtual ~Plotter3DES();

        virtual void initGL();
        virtual void setViewport(const QRectF &vp);
        void drawPlots();
        virtual int currentPlot() const = 0;
        virtual void modelChanged() = 0;

        /** Force OpenGL to render the scene. QGLWidget should call updateGL in this method. */
        virtual void renderGL() = 0;

        void exportSurfaces(const QString& path) const;

        /** Force the plots from @p start to @p end to be recalculated. */
        void updatePlots(const QModelIndex & parent, int start, int end);

        void setModel(QAbstractItemModel* f);
        QAbstractItemModel* model() const { return m_model; }

        PlotStyle plotStyle() const { return m_plotStyle; }
        void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; renderGL(); }
//         void setPlottingAttributes(PlotStyle st) { m_plotStyle = ps; }
        PlottingFocusPolicy plottingFocusPolicy() const { return m_plottingFocusPolicy; }
        void setPlottingFocusPolicy(PlottingFocusPolicy fp);

        /** Set the scale of all the scene by @p factor */
        void scale(qreal factor);

        // Advanced rotation features

        /** Rotates by @p dx and @p dy in screen coordinates. */
        void rotate(int dx, int dy);

        /** Query if there is a valid axis arrow for @p x and @p y screen coordinates. */
        CartesianAxis selectAxisArrow(int x, int y);

        /** Fix the rotation around @p direction */
        void fixRotation(const QVector3D &direction);

        /** Query if the rotation is fixed by a specific direction. */
        bool isRotationFixed() const { return !m_rotFixed.isNull(); }

        /** Show a little indicator (as a hint) next to the arrow of @p axis */
        void showAxisArrowHint(CartesianAxis axis);

        /** Hide the current indicator of the axis */
        void hideAxisHint();

        /**  If the flag @p simplerot is true the rotation ignores any fixed or free direction */
        ANALITZAPLOT_DEPRECATED void setUseSimpleRotation(bool simplerot);

        /**  Get information about the current rotarion approach: if return true then rotation is simple. */
        bool isUsingSimpleRotation() const { return m_simpleRotation; }

        /** sets the view to the initial perspective */
        void resetViewport();

        /**
         * saves the currently displayed plot in @p url
         *
         * @returns whether it was saved successfully
         */
        bool save(const QUrl &url);

        /**
         * @returns the filters supported by save
         *
         * @see save()
         */
        QStringList filters() const;

        void setReferencePlaneColor(const QColor &color) { m_referencePlaneColor = color; }

    protected:
        void addPlots(PlotItem* item);

    private:
        static const quint8 XAxisArrowColor[];
        static const quint8 YAxisArrowColor[];
        static const quint8 ZAxisArrowColor[];

        virtual QImage grabImage() = 0;

        void resetViewPrivate(const QVector3D& rot);

        enum SceneObjectType {Axes, RefPlaneXY, XArrowAxisHint, YArrowAxisHint, ZArrowAxisHint};

        PlotItem *itemAt(int row) const;

        void drawAxes();
        void drawRefPlane();

        QAbstractItemModel* m_model;

        QMap<PlotItem*, QOpenGLBuffer> m_itemGeometries; // pair:=<indexdata_id, vertex-normals_id>

        PlotStyle m_plotStyle;
        PlottingFocusPolicy m_plottingFocusPolicy;

        //scene properties
        QMap<SceneObjectType, uint > m_sceneObjects;
        QRectF m_viewport;
        const GLfloat m_depth;
        qreal m_scale;
        QMatrix4x4 m_rot;
        QVector3D m_rotFixed;
        CartesianAxis m_currentAxisIndicator;
        bool m_simpleRotation;
        QVector3D m_simpleRotationVector;
        QVector3D m_lightpos;
        QColor m_referencePlaneColor;

        QOpenGLShaderProgram program;
        QMatrix4x4 m_projection;
};

}

#endif // PLOTTER3D_H
