/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> * 
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

#ifndef PLOTTER2D_H
#define PLOTTER2D_H

#include <QRectF>
#include <QLineF>
#include <QString>
#include <QPair>
#include <QColor>

#include "analitzaplotexport.h"
#include <analitzaplot/plottingenums.h>

class QAbstractItemModel;
class QPainter;
class QPaintDevice;
class QModelIndex;

namespace Analitza
{
class PlotItem;
class Plotter2DPrivate;
/**
 * \class Plotter2D
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Render 2D plots.
 *
 * This class uses QPainter as backend for drawing plots.
 * The default value of showGrid is true.
 * The default grid color is Qt::lightGray.
 * The default background color is Qt::white.
 * The default value of autoGridStyle is true.
 */

class ANALITZAPLOT_EXPORT Plotter2D
{
    private: // private structs
        struct GridInfo; // interval structure for carry current grid state information across interval methods

    public:
        explicit Plotter2D(const QSizeF& size);
        virtual ~Plotter2D();

        /** Sets whether we will draw the grid. */
        void setShowGrid(bool show);
        //only works if showgrid is true. for polar grid it affects to subdivision of angles/rays
        void setShowMinorGrid(bool mt);

        /** Returns whether we have chosen to draw the grid. */
        bool showGrid() const {return m_showGrid; }

        /** Returns whether we have chosen to draw the minor grid. */
        bool showMinorGrid() const {return m_showMinorGrid; }
        
        void setGridColor(const QColor &color) { m_gridColor = color;  forceRepaint(); }
        
        //default Qt::lightGray
        QColor gridColor() const { return m_gridColor; }
        
        void setBackgroundColor(const QColor &color) { m_backgroundColor = color;  forceRepaint(); }
        
        // default Qt::white
        QColor backgroundColor() const { return m_backgroundColor; }
        
        /** If true then we ignore the grid style suggested by setGridStyleHint, if false then we use as grid style the hint. */
        void setAutoGridStyle(bool autogs) { m_autoGridStyle = autogs; forceRepaint(); }
        
        /** Returns whether we will change automatically the grid style based on the curent plot. */
        bool autoGridStyle() const { return m_autoGridStyle; }
        
        /** Sets the suggested grid style. Only works if autoGridStyle is false. Note that we only accept CoordinateSystem::Cartesian or CoordinateSystem::Polar. */
        void setGridStyleHint(GridStyle suggestedgs);
        
        /** Sets whether it has to keep the aspect ratio (1:1 grid). */
        void setKeepAspectRatio(bool ar);
        
        /** Sets whether it is keeping the aspect ratio (1:1 grid). */
        bool keepAspectRatio() const { return m_keepRatio; }
        
        /** Force the functions from @p start to @p end to be recalculated. */
        void updateFunctions(const QModelIndex & parent, int start, int end);
        
        void setModel(QAbstractItemModel* f);
        QAbstractItemModel* model() const;
        
        /** Sets the graph's viewport to @p v. */
        void setViewport(const QRectF& vp, bool repaint=true);
        
        //TODO doc
        //normlized current viewport, that includes scale information
        QRectF currentViewport() const { return viewport; }
        
        //DEPRECATED
        QRectF lastViewport() const { return currentViewport(); }
        
        /** Moves the viewport @p delta */
        void moveViewport(const QPoint& delta);
        
        void setXAxisLabel(const QString &label);
        void setYAxisLabel(const QString &label);
        
        //by default linear
        void setScaleMode(ScaleMode sm) { m_scaleMode = sm; forceRepaint(); }
        ScaleMode scaleMode() const { return m_scaleMode; }
        
        //default radiasn, this will afecto when scalemode is trig and for the angles in polargridmode
        void setAngleMode(AngleMode am) { m_angleMode = am; forceRepaint(); }
        AngleMode angleMode() const { return m_angleMode; }
        
        void setShowTicks(Qt::Orientations o) { m_showTicks = o; forceRepaint(); }
        void setShowTickLabels(Qt::Orientations o) { m_showTickLabels = o; forceRepaint(); }
        //only works if showticks is true
        void setShowMinorTicks(bool mt) { m_showMinorTicks=mt; forceRepaint(); }
        bool minorTicksShown() const { return m_showMinorTicks; }
        
        //these 2 only work when gridmode is polar, showpolar axis aumenta cuando esta lejos de origin
        void setShowPolarAxis(bool pt) { m_showPolarAxis = pt;  forceRepaint(); }
        void setShowPolarAngles(bool pt) { m_showPolarAngles = pt;  forceRepaint(); }
        
        void setShowAxes(Qt::Orientations o) { m_showAxes = o; forceRepaint(); }
        
        Qt::Orientations ticksShown() const { return m_showTickLabels; }
        
        /** Zooms in to the Viewport center */
        void zoomIn(bool repaint=true);
        
        /** Zooms out taken ref center too*/
        void zoomOut(bool repaint=true);
        
    protected:
        virtual void drawGrid(QPaintDevice *qpd);
        virtual void drawFunctions(QPaintDevice *qpd);
        virtual void forceRepaint() = 0;
        virtual void viewportChanged() = 0;
        virtual int currentFunction() const = 0;
        virtual void modelChanged() = 0;
        virtual void showGridChanged() = 0;
        
    protected: // utils
        QRectF lastUserViewport() const { return userViewport; }
        QRectF normalizeUserViewport(const QRectF &uvp); // from userViewport to viewport, this one uses current scale information
        void updateScale(bool repaint);
        
        QPointF toWidget(const QPointF &) const;
        QPointF fromWidget(const QPoint& p) const;
        QPointF toViewport(const QPoint& mv) const;
        QPair<QPointF, QString> calcImage(const QPointF& ndp) const;
        QLineF slope(const QPointF& dp) const;
        
        QLineF toWidget(const QLineF &) const;
        void setPaintedSize(const QSize& size);
        void setDevicePixelRatio(qreal dpr);
        void scaleViewport(qreal scale, const QPoint& center, bool repaint=true);

    private:
        void drawAxes(QPainter* painter, GridStyle a) const;
        void drawCircles(QPainter* painter, const GridInfo& gridinfo, GridStyle gridStyle) const;
        void drawSquares(QPainter* painter, const GridInfo& gridinfo, GridStyle gridStyle) const;
        void drawMainAxes(QPainter* painter) const;
        void drawCartesianTickLabels(QPainter* painter, const GridInfo& gridinfo, CartesianAxis axis) const;
        void drawPolarTickLabels(QPainter* painter, const GridInfo& gridinfo) const;
        void drawGridTickLabels(QPainter* painter, const GridInfo& gridinfo, GridStyle gridStyle) const;
        PlotItem *itemAt(int row) const;
        int width() const { return m_size.width(); }
        int height() const { return m_size.height(); }

        const GridInfo getGridInfo() const; // calculate correct grid params
        const QColor computeSubGridColor() const;
        const QString computeAngleLabelByFrac(unsigned int n, unsigned int d) const; // input npi/d return angle in m_angleMode
        const QString computeAngleLabelByStep(unsigned int k, unsigned int step) const; // input n*step*pi return angle in m_angleMode
        
        bool m_showGrid;
        bool m_showMinorGrid;
        QColor m_gridColor;
        QColor m_backgroundColor;
        bool m_autoGridStyle;
        GridStyle m_gridStyleHint;
        //TODO set move auto tick labels
        
        double rang_x, rang_y;
        bool m_keepRatio;
        bool m_dirty; // or m_updated; como ahora contamos con setmodel, es necesario que se actualicen los datos antes de pintar, es necesario que no sea dirty
        QRectF viewport; // normalized viewport (with scale information), this one is the current viewport (used in currentViewport)
        QRectF userViewport; // raw viewport that user sets by setViewport, so we need to normalized userViewport into viewport to include scale and aspect radio information 
        QSizeF m_size;

        friend class Plotter2DPrivate;
        Plotter2DPrivate* const d;
        
        AngleMode m_angleMode;
        ScaleMode m_scaleMode;
        Qt::Orientations m_showTicks;
        Qt::Orientations m_showTickLabels;
        bool m_showMinorTicks;
        Qt::Orientations m_showAxes;
        bool m_showPolarAxis;
        bool m_showPolarAngles;
        QString m_axisXLabel;
        QString m_axisYLabel;
        
    private: // constants
        static const QColor m_axeColor;
        static const QColor m_derivativeColor;
        static const QString PiSymbol;
        static const QString DegreeSymbol;
        static const QString GradianSymbol;       
        static const double Pi6; // pi/6
        static const double Pi12; // pi/12
        static const double Pi36; // pi/36
        static const double ZoomInFactor;
        static const double ZoomOutFactor;
};

}

#endif // PLOTTER2D_H
