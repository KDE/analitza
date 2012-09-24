/*************************************************************************************
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

#ifndef FUNCTIONSPAINTER3D_H
#define FUNCTIONSPAINTER3D_H

#include "plotitem.h"

#include <GL/gl.h>

#include <QModelIndex>

class QAbstractItemModel;
class QPainter;
class QPaintDevice;

class QModelIndex;

/**
 * @class Plotter3D
 * @author Percy Camilo T. Aucahuasi
 * @short This class manage the OpenGL scene where the plots will be rendered.
 * 
 * Plotter3D provides an agnostic way to manage a 3d scene for draw math plots, 
 * Contains just OpenGL calls, so is uncoupled with QWidget nor QtQuick. This 
 * class needs the PlotsModel (to create the geometry of 3D plots) and also 
 * exposes some methods to change the scene (like hide/show the axis or 
 * reference planes for example)
 * 
 * @internal current implementation is using display lists as geometries store.
 * 
 */

class ANALITZAPLOT_EXPORT Plotter3D
{
    public:
        Plotter3D(QAbstractItemModel* model = 0);
        virtual ~Plotter3D();

        virtual void initGL();
        virtual void setViewport(const QRect &vp);
        virtual void drawPlots();
        virtual int currentPlot() const = 0;
        virtual void modelChanged() = 0;

        /** Force the plots from @p start to @p end to be recalculated. */
        void updatePlots(const QModelIndex & parent, int start, int end);

        void setModel(QAbstractItemModel* f);
        QAbstractItemModel* model() const { return m_model; }

    private:
        enum SceneObjectType {Axes, RefPlaneXY};
        
        PlotItem *itemAt(int row) const;

        QAbstractItemModel* m_model;
        
        QMap<PlotItem*, GLuint> m_itemGeometries;

        //scene properties
        GLfloat m_depth;
        QMap<SceneObjectType, QPair<GLuint, bool> > m_sceneObjects; // pair:=<displaylist, visible>
};

#endif // FUNCTIONSPAINTER3D_H
