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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/planecurve.h"
#include "analitzaplot/planecurvesmodel.h"
#include "analitzaplot/plotview2d.h"
#include <analitzaplot/private/functiongraphsmodel.h>

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

    QTabWidget *tabs = new QTabWidget(mainWindow);
    
    //BEGIN test calls

    VisualItemsModel *model = new VisualItemsModel(tabs);
    QItemSelectionModel *selection = new QItemSelectionModel(model);
    
    Graph2D *view2d = new Graph2D(tabs);
//     view2d->setReadOnly(true);
    view2d->setSquares(false);
    view2d->setModel(model);
    view2d->setSelectionModel(selection);

    PlaneCurve *item = model->addPlaneCurve(Analitza::Expression("x->x*x"), "para", Qt::cyan);
    model->addPlaneCurve(Analitza::Expression("q->q+2"), "polar simple", Qt::green);
    model->addPlaneCurve(Analitza::Expression("t->vector{t*t+1, t+2}"), "vec", Qt::yellow);
    PlaneCurve *item2 = model->addPlaneCurve(Analitza::Expression("5*(x**2+y**2)**3=15*(x*y*72)**2"), "impl", Qt::red);
    model->addPlaneCurve(Analitza::Expression("x->2+x*x"), "otra simple", Qt::blue);
    

//     qDebug() << item2->expression().toString();
    
    if (model->rowCount()>0)
    {
        selection->setCurrentIndex(model->index(model->rowCount()-1), QItemSelectionModel::Select);
    }

    //END test calls

    mainWindow->setCentralWidget(view2d);

    mainWindow->show();

    
//     item2->setVisible(false);
//     item2->setColor(Qt::red);

//     qDebug() << item << static_cast<PlaneCurve*>(model->item(0)) << model->item(0);
    item->setInterval("x", 0, 4);

    item2->setInterval("x", 0, 4);
    item2->setInterval("y", 0, 4);
    
    
    return app.exec();
}

