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
#include <QCheckBox>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/planecurve.h"
#include "analitzaplot/plotsview2d.h"
#include <analitzaplot/plotsmodel.h>
#include <surface.h>
#include <plotsproxymodel.h>
#include <KSelectionProxyModel>

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
    
    QCheckBox *checkvisible = new QCheckBox(central);
    checkvisible->setText("Allow to the model can change the visibility of items");
    checkvisible->setCheckState(Qt::Checked);
    checkvisible->setTristate(false);
    
    QSplitter *tabs = new QSplitter(Qt::Horizontal, central);

    layout->addWidget(checkvisible);
    layout->addWidget(tabs);

    //BEGIN test calls
    
    PlotsModel *model = new PlotsModel(tabs);
    
    checkvisible->connect(checkvisible, SIGNAL(toggled(bool)), model, SLOT(setCheckable(bool)));

    PlotsProxyModel *proxy = new PlotsProxyModel(tabs);
    proxy->setFilterSpaceDimension(2);
    proxy->setSourceModel(model);
    
    QItemSelectionModel *selection = new QItemSelectionModel(proxy);

    PlotsView2D *view2d = new PlotsView2D(tabs);
    view2d->setSquares(false);
    view2d->setModel(proxy);
    view2d->setSelectionModel(selection);

    model->addPlaneCurve(Analitza::Expression("y->y*y"), "y->", Qt::magenta);
    model->addPlaneCurve(Analitza::Expression("x->x*x/12"), "para", Qt::cyan);
    model->addPlaneCurve(Analitza::Expression("p->p+2"), "polar simple", Qt::green);
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
    
    //if you want to obey viewport changes always, then set to true AutoUpdate flag
    //WARNING some functions have to compute complex algorithms in order to generate its geometry, that is why you may use intervals instead
    //AutoUpdate ignores intervals
    
    PlaneCurve *curve = 0;
    for(int i=0; i<model->rowCount(); ++i)
        static_cast<FunctionGraph*>(model->item(i))->setAutoUpdate(true);
    
    //END test calls

    QTreeView *viewsource = new QTreeView(tabs);
    viewsource->setRootIsDecorated(false);
    viewsource->setMouseTracking(true);
    viewsource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewsource->setModel(model);
    
    QTreeView *viewproxy = new QTreeView(tabs);
    viewproxy->setRootIsDecorated(false);
    viewproxy->setMouseTracking(true);
    viewproxy->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewproxy->setModel(proxy);
    viewproxy->setSelectionModel(selection);
    
    tabs->addWidget(viewsource);
    tabs->addWidget(viewproxy);
    tabs->addWidget(view2d);

    mainWindow->setCentralWidget(central);

    mainWindow->show();

    return app.exec();
}

