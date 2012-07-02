/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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
#include <QStringListModel>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/surface.h"
#include "analitzaplot/spacecurve.h"
#include "analitzaplot/private/functiongraphsmodel.h"
#include "analitzaplot/plotview3d.h"


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
    
    //BEGIN test calls

    FunctionGraphsModel *model = new FunctionGraphsModel(mainWindow);
    QItemSelectionModel *selection = new QItemSelectionModel(model);
    
    View3D *view3d = new View3D(mainWindow);
    view3d->setModel(model);
    view3d->setSelectionModel(selection);
    
    model->addItem(Analitza::Expression("(x,y)->(x*x+y*y)"),3, "Hola", Qt::cyan);
//     model->addItem(Analitza::Expression("t->vector{t*t, t, t*t}"),3, "Hola", Qt::yellow);

//     if (model->rowCount()>0)
//     {
//         selection->setCurrentIndex(model->index(model->rowCount()-1), QItemSelectionModel::Select);
//     }

    //END test calls
    
    mainWindow->setCentralWidget(view3d);

    mainWindow->show();

    return app.exec();
}
