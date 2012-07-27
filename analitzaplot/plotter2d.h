/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      * 
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


#ifndef FUNCTIONSPAINTER_H
#define FUNCTIONSPAINTER_H

#include "planecurve.h"

class PlotsProxyModel;
class QPainter;
class QPaintDevice;

class QModelIndex;

class ANALITZAPLOT_EXPORT Plotter2D
{
    public:
        Plotter2D(const QSizeF& size, PlotsProxyModel* model = 0);
        virtual ~Plotter2D();
        
        virtual void drawFunctions(QPaintDevice *qpd);
        virtual void forceRepaint() = 0;
        virtual void viewportChanged() = 0;
        virtual int currentFunction() const = 0;
        virtual void modelChanged() = 0;
        
        /** Sets whether we will see a grid or only the axes. */
        void setSquares(bool newSquare) { !newSquare?m_useCoordSys=0:m_useCoordSys=m_useCoordSys; forceRepaint(); }
        
        /** Returns whether we have chosen to see the grid. */
        bool squares() const {return m_useCoordSys!=0;}
        
        /** Sets whether it has to keep the aspect ratio (1:1 grid). */
        void setKeepAspectRatio(bool ar);
        
        /** Sets whether it is keeping the aspect ratio (1:1 grid). */
        bool keepAspectRatio() const { return m_keepRatio; }
        
        /** Force the functions from @p start to @p end to be recalculated. */
        //los index son de PROXY , esto para evitar que el widget escuche signal inecesarias
        void updateFunctions( const QModelIndex & parent, int start, int end );
        
        void setModel(PlotsProxyModel* f);
        PlotsProxyModel* model() const { return m_model; }
        
        int width() const { return m_size.width(); }
        int height() const { return m_size.height(); }
        
        /** Sets the graph's viewport to @p v. */
        void setViewport(const QRectF& vp, bool repaint=true);
        
        /** Moves the viewport @p delta */
        void moveViewport(const QPoint& delta);
        
    protected:
        QRectF lastViewport() const { return viewport; }
        QRectF lastUserViewport() const { return userViewport; }
        void updateScale(bool repaint);
        
        QPointF toWidget(const QPointF &) const;
        QPointF fromWidget(const QPoint& p) const;
        QPointF toViewport(const QPoint& mv) const;
        QPointF calcImage(const QPointF& ndp) const;
        QLineF slope(const QPointF& dp) const;
        
        QLineF toWidget(const QLineF &) const;
        void setPaintedSize(const QSize& size);
        void scaleViewport(qreal s, const QPoint& center);

    private:
        void drawAxes(QPainter *f, CoordinateSystem a);
        void drawPolarAxes(QPainter *f);
        void drawCartesianAxes(QPainter *f);
        PlotItem *fromProxy(int proxy_row) const; // get the real item from proxy
        PlotItem *fromSource(int realmodel_row) const; // get the item filtered by the proxy

        
        double rang_x, rang_y;
        bool m_squares;
        bool m_keepRatio;
        bool m_dirty; // or m_updated; como ahora contamos con setmodel, es necesario que se actualicen los datos antes de pintar, es necesario que no sea dirty
        QRectF viewport;
        QRectF userViewport;
        QSizeF m_size;
        PlotsProxyModel* m_model;
        
        static const QColor m_axeColor;
        static const QColor m_axe2Color;
        static const QColor m_derivativeColor;
        
protected: /// TODO improve this ... it should be private
        //GSOC scale ticks & axis
    QString m_tickScaleSymbol;
    qreal m_tickScaleSymbolValue;
    bool m_tickScaleUseSymbols; // cuando son numeros o cuando el usuario no kiere mostrar los simboles
    int m_tickScaleNumerator;
    int m_tickScaleDenominator;
    bool m_showHTicks;
    bool m_showVTicks;
    bool m_showHAxes;
    bool m_showVAxes;
    QString m_axisXLabel;
    QString m_axisYLabel;
    int m_useCoordSys; // polar 2 cart 1 none 0 y case 3 es automatic (es decir el plot acutal elije su coordenada)
    
    QColor m_gridColor;
};

#endif // FUNCTIONSPAINTER_H
