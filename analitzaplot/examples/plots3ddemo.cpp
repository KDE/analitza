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
#include <QStringListModel>
#include <qtreeview.h>
#include <qsplitter.h>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QStatusBar>
#include <QItemEditorFactory>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcolorcombo.h>

#include "analitzaplot/surface.h"
#include "analitzaplot/spacecurve.h"
#include "analitzaplot/plotsmodel.h"
#include "analitzaplot/plotsview3d.h"
#include <analitzaplot/plotsdictionarymodel.h>
#include <planecurve.h>
#include <plotsproxymodel.h>
#include <analitza/variables.h>
#include <analitza/apply.h>
#include <analitza/variable.h>
#include <analitza/container.h>
#include <analitza/polynomial.h>
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

//     QItemEditorFactory *factory = new QItemEditorFactory;
//     QItemEditorCreatorBase *colorListCreator = new QStandardItemEditorCreator<KColorCombo>();
//     factory->registerEditor(QVariant::Color, colorListCreator);
//     QItemEditorFactory::setDefaultFactory(factory);

    QWidget *central = new QWidget(mainWindow);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
//     QCheckBox *checkvisible = new QCheckBox(central);
//     checkvisible->setText("Editable view");
//     checkvisible->setCheckState(Qt::Checked);
//     checkvisible->setTristate(false);

    QSplitter *tabs = new QSplitter(Qt::Horizontal, central);

//     layout->addWidget(checkvisible);
    layout->addWidget(tabs);
    
    //BEGIN test calls
    
    PlotsModel *model = new PlotsModel(tabs);
    
//     checkvisible->connect(checkvisible, SIGNAL(toggled(bool)), model, SLOT(setCheckable(bool)));

    PlotsProxyModel *proxy = new PlotsProxyModel(tabs);
    proxy->setFilterSpaceDimension(Dim3D);
    proxy->setSourceModel(model);
    
    QItemSelectionModel *selection = new QItemSelectionModel(proxy);
    
    PlotsView3D *view3d = new PlotsView3D(tabs);
    view3d->setModel(proxy);
    view3d->setSelectionModel(selection);

    model->addPlot(new Surface(Analitza::Expression("(r,p)->2"), "Hola", Qt::magenta));
    model->addPlot(new Surface(Analitza::Expression("(x^2 + y^2 - 1) * ( x^2 + z^2 - 1) = 1"), "Hola", Qt::cyan));
    model->addPlot(new PlaneCurve(Analitza::Expression("x+3-y=7"), "asdasd, ", Qt::red));
    model->addPlot(new Surface(Analitza::Expression("x*x+y*y-z*z= 1/2"), "Hola", Qt::yellow));
    model->addPlot(new Surface(Analitza::Expression("(x,y)->-e^(-x^2-y^2)"), "aaa", Qt::lightGray));
    model->addPlot(new PlaneCurve(Analitza::Expression("t->vector{t,t*t}"), "asdasd, ", Qt::blue));

    if (model->rowCount()>0)
    {
        selection->setCurrentIndex(model->index(model->rowCount()-1), QItemSelectionModel::Select);
    }

    //END test calls
    
    QTreeView *viewsource = new QTreeView(tabs);
    viewsource->setRootIsDecorated(false);
    viewsource->setMouseTracking(true);
    viewsource->setEditTriggers(QAbstractItemView::AllEditTriggers);
    viewsource->setModel(model);

    QTreeView *viewproxy = new QTreeView(tabs);
    viewproxy->setRootIsDecorated(false);
    viewproxy->setMouseTracking(true);
    viewproxy->setEditTriggers(QAbstractItemView::AllEditTriggers);
    viewproxy->setModel(proxy);
    viewproxy->setSelectionModel(selection);
    
    tabs->addWidget(viewsource);
    tabs->addWidget(viewproxy);
    tabs->addWidget(view3d);

    mainWindow->setCentralWidget(central);
    mainWindow->show();

    return app.exec();
}
