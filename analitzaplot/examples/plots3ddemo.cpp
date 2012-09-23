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
#include <QLabel>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/surface.h"
#include "analitzaplot/spacecurve.h"
#include "analitzaplot/plotsmodel.h"
#include "analitzaplot/plotsview3d.h"
#include <analitza/expression.h>

int main(int argc, char *argv[])
{
    KAboutData aboutData("PlotView3DTest",
                         0,
                         ki18n("PlotView3DTest"),
                         "1.0",
                         ki18n("PlotView3DTest"),
                         KAboutData::License_LGPL_V3,
                         ki18n("(c) 2012 Percy Camilo T. Aucahuasi"),
                         ki18n("PlotView3DTest"),
                         "http://www.kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    QMainWindow *mainWindow = new QMainWindow();
    mainWindow->setMinimumSize(640, 480);
    mainWindow->statusBar()->show();

    QWidget *central = new QWidget(mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    QSplitter *tabs = new QSplitter(Qt::Horizontal, central);
    tabs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel *info = new QLabel("Rotate: right click + move \n Scale: left click + move ", central);
    
    layout->addWidget(info);
    layout->addWidget(tabs);
    
    //BEGIN test calls

    PlotsModel *model = new PlotsModel(tabs);

    QTreeView *viewsource = new QTreeView(tabs);
    viewsource->setModel(model);
    
    PlotsView3D *view3d = new PlotsView3D(tabs);
    view3d->setModel(model);
    view3d->setSelectionModel(viewsource->selectionModel());

    model->addPlot(new Surface(Analitza::Expression("(r,p)->2"), "cyl", Qt::magenta));
    model->addPlot(new Surface(Analitza::Expression("(x,y)->-e^(-x^2-y^2)"), "z-map", Qt::lightGray));
    model->addPlot(new Surface(Analitza::Expression("(x^2 + y^2 - 1) * ( x^2 + z^2 - 1) = 1"), "implicit 0", Qt::cyan));
    model->addPlot(new Surface(Analitza::Expression("x+3-y=7"), "implicit 1, ", Qt::red));
    model->addPlot(new Surface(Analitza::Expression("x*x+y*y-z*z= 1/2"), "implicit 2", Qt::darkBlue));
    model->addPlot(new SpaceCurve(Analitza::Expression("t->vector{cos(t), sin(t), t}"), "curve, ", Qt::green));
    model->addPlot(new Surface(Analitza::Expression("(x,y)->x*x"), "z-map", Qt::yellow));
    
    //END test calls

    tabs->addWidget(viewsource);
    tabs->addWidget(view3d);

    mainWindow->setCentralWidget(central);
    mainWindow->show();

    return app.exec();
}
