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

#include <QMainWindow>
#include <qtreeview.h>
#include <qsplitter.h>
#include <QStatusBar>
#include <QVBoxLayout>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/planecurve.h"
#include "analitzaplot/surface.h"
#include "analitzaplot/plotsview2d.h"
#include "analitzaplot/plotsmodel.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("PlotView2DTest",
                         0,
                         ki18n("PlotView2DTest"),
                         "1.0",
                         ki18n("PlotView2DTest"),
                         KAboutData::License_LGPL_V3,
                         ki18n("(c) 2012 Percy Camilo T. Aucahuasi"),
                         ki18n("PlotView2DTest"),
                         "http://www.kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    QMainWindow *mainWindow = new QMainWindow();
    mainWindow->setMinimumSize(640, 480);
    mainWindow->statusBar()->show();

    QWidget *central = new QWidget(mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    QSplitter *tabs = new QSplitter(Qt::Horizontal, central);

    layout->addWidget(tabs);

    //BEGIN test calls
    
    PlotsModel *model = new PlotsModel(tabs);
    
    PlotsView2D *view2d = new PlotsView2D(tabs);
//     view2d->setSquares(false);
    view2d->setModel(model);

    model->addPlot(new PlaneCurve(Analitza::Expression("t->4*sin(2*t)"), "polar curv", Qt::cyan));
    model->addPlot(new PlaneCurve(Analitza::Expression("t**2=cos(r)*(3*pi/4)**2"), "implicit polar curv", Qt::yellow));
    model->addPlot(new PlaneCurve(Analitza::Expression("x->x*x"), "f(x)", Qt::magenta));
    model->addPlot(new PlaneCurve(Analitza::Expression("(2*x+y)*(x^2+y^2)^4+2*y*(5*x^4+10*x^2*y^2-3*y^4)+y=2*x"), "khipu", Qt::green));
    model->addPlot(new PlaneCurve(Analitza::Expression("t->vector{t*t+1, t+2}"), "param2d1", Qt::blue));
    model->addPlot(new Surface(Analitza::Expression("(x,y)->x*x-y*y"), "3D", Qt::red));

    //END test calls

    QTreeView *viewsource = new QTreeView(tabs);
    viewsource->setModel(model);
    
    view2d->setSelectionModel(viewsource->selectionModel());
    
    tabs->addWidget(viewsource);
    tabs->addWidget(view2d);

    PlotsModel *model2 = new PlotsModel(tabs);
    view2d->setModel(model2);
    
    view2d->setModel(model);

    mainWindow->setCentralWidget(central);

    mainWindow->show();

    return app.exec();
}

