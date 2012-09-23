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

#include "plotter3d.h"

#include "surface.h"
#include "spacecurve.h"

#include "plotsmodel.h"
#include "private/utils/box3d.h"
#include "private/utils/triangle3d.h"

#include <QApplication>
#include <cmath>
#include <QDebug>
#include <KLocalizedString>
#include <KColorUtils>
#include <GL/gl.h>

#if defined(HAVE_IEEEFP_H)
#include <ieeefp.h>
// bool isinf(double x) { return !finite(x) && x==x; }
#endif

Q_DECLARE_METATYPE(PlotItem*);

using namespace std;

// #define DEBUG_GRAPH

Plotter3D::Plotter3D(const QSizeF& size, QAbstractItemModel* model)
    : m_model(model)
{}

Plotter3D::~Plotter3D()
{}

PlotItem* Plotter3D::itemAt(int row) const
{
    QModelIndex pi = m_model->index(row, 0);

    if (!pi.isValid())
        return 0;

    PlotItem* plot = pi.data(PlotsModel::PlotRole).value<PlotItem*>();

    if (plot->spaceDimension() != Dim3D)
        return 0;

    return plot;
}

void Plotter3D::drawPlots()
{
    if (!m_model)
        return;

    //if we have more registered items in our m_displayLists then the model has changed, so we reset
    if (m_model->rowCount() != m_displayLists.count())
        foreach (PlotItem *plot, m_displayLists.keys())
        {
            glDeleteLists(m_displayLists.value(plot), 1);
            
            m_displayLists.remove(plot);
        }

    //if our m_displayLists is empty then we need to rebuild the plots
    if (m_displayLists.isEmpty())
        for (int i = 0; i < m_model->rowCount(); ++i)
        {
            PlotItem* plot = itemAt(i);
            
            if (!plot || !plot->isVisible())
                continue;

            Surface* surf = static_cast<Surface*>(plot);
            
            if (surf->faces().isEmpty())
                surf->update(Box3D());

            GLuint dlid = glGenLists(1);
            m_displayLists[surf] = dlid;

            float shininess = 15.0f;
            float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
            float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};
            Q_UNUSED(diffuseColor); //TODO: Percy?

            //BEGIN display list
            glNewList(dlid, GL_COMPILE);

            // set specular and shiniess using glMaterial (gold-yellow)
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

            // set ambient and diffuse color using glColorMaterial (gold-yellow)
            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        //     glColor3fv(diffuseColor);
            glColor3ub(surf->color().red(), surf->color().green(), surf->color().blue());

        //     qDebug() << "FACES -> " << surf->faces().size();
            
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

            glEndList();
            //END display list
        }
    
    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        PlotItem* plot = itemAt(i);
        Surface* surf = dynamic_cast<Surface*>(plot);
        
        if (!surf && !surf->isVisible())
            continue;

        glCallList(m_displayLists[plot]);
    }
}

void Plotter3D::updatePlots(const QModelIndex & parent, int start, int end)
{
//     if (!m_model || parent.isValid())
//         return;
// 
//     for(int i=start; i<=end; i++) {
//         PlaneCurve* curve = dynamic_cast<PlaneCurve *>(itemAt(i));
// 
//         if (!curve || !curve->isVisible())
//             continue;
// 
//         QRectF viewport_fixed = viewport;
//         viewport_fixed.setTopLeft(viewport.bottomLeft());
//         viewport_fixed.setHeight(fabs(viewport.height()));
//         curve->update(viewport_fixed);
//     }
// 
//     m_dirty = false;
// 
//     forceRepaint();
}

void Plotter3D::setModel(QAbstractItemModel* f)
{
    m_model=f;
    modelChanged();
}

PlotItem* Plotter3D::currentPlotItem() const
{
    return itemAt(currentPlot());
}
