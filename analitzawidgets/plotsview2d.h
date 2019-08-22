/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef PLOTSVIEW2D_H
#define PLOTSVIEW2D_H

#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QPixmap>
#include <QModelIndex>

#include "analitzawidgets_export.h"
#include <analitzaplot/plotter2d.h>

class QItemSelectionModel;

namespace Analitza
{

/**
 * \class PlotsView2D
 * 
 * \ingroup AnalitzaGUIModule
 *
 * \brief Widget that allows visualization of 2D plots.
 *
 * This class lets you create a widget that can draw multiple 2D graphs. This widget 
 * use Plotter2D as a backend.
 */

class ANALITZAWIDGETS_EXPORT PlotsView2D : public QWidget, public Plotter2D
{
Q_OBJECT

Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid)

/** The default grid color is a soft mix between KColorScheme::NormalBackground and KColorScheme::NormalText (foreground) of QPalette::Active. **/
Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor)

/** The default background color is KColorScheme::NormalBackground of QPalette::Active. **/
Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

Q_PROPERTY(bool autoGridStyle READ autoGridStyle WRITE setAutoGridStyle)

public:
    enum Format { PNG, SVG };
    
    /** Constructor. Constructs a new Graph2D. */
    explicit PlotsView2D(QWidget* parent = nullptr);
    
    ~PlotsView2D();
    
    QSize sizeHint() const override { return QSize(100, 100); }
    
    /** Saves the graphs to a file located at @p path. */
    bool toImage(const QString& path, Format f);
    
    /** Returns the viewing port */
    QRectF definedViewport() const;
    
    void setSelectionModel(QItemSelectionModel* selection);

public Q_SLOTS:
    /** Marks the image as dirty and repaints everything. */
    void forceRepaint() override { valid=false; repaint(); }

    /** Sets the viewport to a default viewport. */
    void resetViewport() { setViewport(defViewport); }
    
    /** Returns whether it has a little border frame. */
    bool isFramed() const { return m_framed; }
    
    /** Sets whether it has a little border frame. */
    void setFramed(bool fr) { m_framed=fr; }
    
    /** Returns whether it is a read-only widget. */
    bool isReadOnly() const { return m_readonly; }
    
    /** Sets whether it is a read-only widget. */
    void setReadOnly(bool ro);
    
    void snapshotToClipboard();
    
    //exposed from plotter2d as slots...
    void setXAxisLabel(const QString &label) { Plotter2D::setXAxisLabel(label); }
    void setYAxisLabel(const QString &label) { Plotter2D::setYAxisLabel(label); }
    void setGridColor(const QColor &color) { Plotter2D::setGridColor(color); }
    void setTicksShown(QFlags<Qt::Orientation> o) { Plotter2D::setShowTickLabels(o); }
    void setAxesShown(QFlags<Qt::Orientation> o) { Plotter2D::setShowAxes(o); }
    //TODO  set bgcolor, setbgcolormode auto means that colo is chosses based in lumninosisty onf current bgprofiles
    
    /** Zooms in to the Viewport center */
    virtual void zoomIn() { Plotter2D::zoomIn(true); }
        
    /** Zooms out */
    virtual void zoomOut() { Plotter2D::zoomOut(true); }

    void modelChanged() override;
    
private Q_SLOTS:
    void changeViewport(const QRectF& vp) { setViewport(vp); }
    
Q_SIGNALS:
    /** Emits a status when it changes. */
    void status(const QString &msg);
    
    void viewportChanged(const QRectF&);

    void showGridChanged() override;
    
private:
    //TODO setviewmodemosusemovemode, pan
    /** The graph mode will especify the selection mode we are using at the moment */
    enum GraphMode {
        None=0,     /**< Normal behaviour */
        Pan,        /**< Panning, translates the viewport. */
        Selection   /**< There is a rectangle delimiting a region, for zooming. */
    };
    
private:
    virtual void viewportChanged() override;
    virtual int currentFunction() const override;
    
    //painting
    QPixmap buffer;
    bool valid;
    QPointF mark;
    QPoint cursorPos;
    
    //events
    void paintEvent( QPaintEvent * ) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent * e ) override;
    void wheelEvent(QWheelEvent *e) override;
    void resizeEvent(QResizeEvent *) override;

    GraphMode mode;
    QPoint press; QPoint last;
    
    //presentation
    QPointF ant;
    QRectF defViewport;
        
    void sendStatus(const QString& msg) { emit status(msg); }
    bool m_framed;
    bool m_readonly;
    QString m_posText;
    QItemSelectionModel* m_selection;
};

}

#endif
