/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> * 
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

#include <QMainWindow>
#include <qtreeview.h>
#include <qsplitter.h>
#include <QVBoxLayout>
#include <QStatusBar>
#include <qapplication.h>

#include "analitzaplot/surface.h"
#include "analitzaplot/spacecurve.h"
#include "analitzaplot/plotsmodel.h"
#include "plotsview3d.h"
#include <plotsfactory.h>
#include <analitza/expression.h>
#include <QCommandLineParser>

using namespace Analitza;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("PlotView3DTest");
    parser.addOption(QCommandLineOption("all-disabled", app.tr("marks all the plots as not visible")));
    parser.addOption(QCommandLineOption("simple-rotation", app.tr("doesn't let you rotate the Z axis")));
    parser.addHelpOption();
    parser.process(app);

    QMainWindow *mainWindow = new QMainWindow();
    mainWindow->setMinimumSize(640, 480);
    mainWindow->statusBar()->show();

    QSplitter *central = new QSplitter(Qt::Horizontal, mainWindow);
    central->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    PlotsModel *model = new PlotsModel(central);

    QTreeView *viewsource = new QTreeView(central);
    viewsource->setModel(model);
    
    PlotsView3D *view3d = new PlotsView3D(central);
    view3d->setSelectionModel(viewsource->selectionModel());

    //BEGIN test calls
    PlotsFactory* s = PlotsFactory::self();
    model->addPlot(s->requestPlot(Analitza::Expression("t->vector{cos(t), sin(t), t}"), Dim3D).create(Qt::green, "curve"));
    model->addPlot(s->requestPlot(Analitza::Expression("t->vector {t,t**2,t}"), Dim3D).create(Qt::green, "parametric"));
    model->addPlot(s->requestPlot(Analitza::Expression("(t,p)->2"), Dim3D).create(Qt::darkGreen, "sphere-sphcoords"));
    model->addPlot(s->requestPlot(Analitza::Expression("(r,p)->p"), Dim3D).create(Qt::magenta, "cyl"));
    model->addPlot(s->requestPlot(Analitza::Expression("piecewise{(x^2 + y^2 +z^2 < 35)?2 - (cos(x + (1+power(5,0.5))/2*y) + cos(x - (1+power(5,0.5))/2*y) + cos(y + (1+power(5,0.5))/2*z) + cos(y - (1+power(5,0.5))/2*z) + cos(z - (1+power(5,0.5))/2*x) + cos(z + (1+power(5,0.5))/2*x)),?1}=0"), Dim3D).create(QColor(40, 87, 159), "complex"));
    model->addPlot(s->requestPlot(Analitza::Expression("(x^2 + y^2 - 1) * ( x^2 + z^2 - 1) = 1"), Dim3D).create(Qt::cyan, "implicit 0"));
    model->addPlot(s->requestPlot(Analitza::Expression("(x,y)->(x*x-y*y)/8"), Dim3D).create(Qt::yellow, "z-map"));
    model->addPlot(s->requestPlot(Analitza::Expression("x*x+y*y-z*z= 1/2"), Dim3D).create(Qt::darkBlue, "implicit 2"));
    //END test calls

    if(parser.isSet("all-disabled"))
        for(int i=0; i<model->rowCount(); i++)
            model->setData(model->index(i), false, Qt::CheckStateRole);

    view3d->setUseSimpleRotation(parser.isSet("simple-rotation"));

    central->addWidget(viewsource);
    central->addWidget(view3d);
    central->setStretchFactor(1, 2);

    mainWindow->setCentralWidget(central);
    mainWindow->show();
    view3d->setModel(model);
    view3d->setFocus();

    return app.exec();
}
