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
#include <QStringListModel>
#include <QAction>
#include <qtreeview.h>
#include <qsplitter.h>
#include <QStatusBar>
#include <qboxlayout.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/planecurve.h"
#include "analitzaplot/plotsview2d.h"
#include <analitzaplot/plotsmodel.h>
#include <surface.h>
#include <plotsproxymodel.h>

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

    QSplitter *tabs = new QSplitter(Qt::Horizontal, mainWindow);
    
    //BEGIN test calls
    
    QTreeView *viewsource = new QTreeView(tabs);
    viewsource->setMouseTracking(true);
    viewsource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QTreeView *viewproxy = new QTreeView(tabs);
    viewproxy->setMouseTracking(true);
    viewproxy->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    PlotsModel *model = new PlotsModel(tabs);
    viewsource->setModel(model);

    PlotsProxyModel *proxy = new PlotsProxyModel(tabs);
    proxy->setFilterSpaceDimension(2);
    proxy->setSourceModel(model);
    
    viewproxy->setModel(proxy);

    PlotsView2D *view2d = new PlotsView2D(tabs);
    view2d->setSquares(false);
    view2d->setModel(proxy);
    view2d->setSelectionModel(viewproxy->selectionModel());
    view2d->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);

    model->addPlaneCurve(Analitza::Expression("x->x*x"), "para", Qt::cyan);
    model->addPlaneCurve(Analitza::Expression("q->q+2"), "polar simple", Qt::green);
    model->addPlaneCurve(Analitza::Expression("t->vector{t*t+1, t+2}"), "vec", Qt::yellow);
    model->addPlaneCurve(Analitza::Expression("5*(x**2+y**2)**3=15*(x*y*72)**2"), "impl", Qt::red);
    model->addSurface(Analitza::Expression("x=y-z"), "fsdfs", Qt::yellow);
    model->addPlaneCurve(Analitza::Expression("x->2+x*x"), "otra simple", Qt::blue);
    model->addPlaneCurve(Analitza::Expression("(x**2+y**2)**3=4*(x**2)*(y**2)"), "otra simple", Qt::lightGray);
    model->addSurface(Analitza::Expression("(x,y)->x*x+y*y"), "fsdfs", Qt::yellow);
    model->addPlaneCurve(Analitza::Expression("(y-x**2)**2=x*y**3"), "otra simple", Qt::lightGray);
    model->addPlaneCurve(Analitza::Expression("sin(x)*sin(y)=1/2"), "otra simple", Qt::yellow);
    model->addPlaneCurve(Analitza::Expression("(2*x+y)*(x^2+y^2)^4+2*y*(5*x^4+10*x^2*y^2-3*y^4)+y=2*x"), "ESTO NO ES SIMPLE", Qt::black);
    model->addPlaneCurve(Analitza::Expression("(x^4)-5*x^3+25*y^2=0"), "simple", Qt::darkBlue);
    model->addSurface(Analitza::Expression("(x,y)->x*x+y*y"), "fsdfssss", Qt::yellow);

    //END test calls

    tabs->addWidget(viewsource);
    tabs->addWidget(viewproxy);
    tabs->addWidget(view2d);

    mainWindow->setCentralWidget(tabs);

    mainWindow->show();

    return app.exec();
}

