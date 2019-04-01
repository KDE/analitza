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
#include <QTreeView>
#include <QSplitter>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QApplication>

#include "analitzaplot/surface.h"
#include "analitzaplot/spacecurve.h"
#include "analitzaplot/plotsmodel.h"
#include "plotsview3d_es.h"
#include <plotsfactory.h>
#include <analitza/expression.h>
#include <QCommandLineParser>

using namespace Analitza;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("PlotView3DTest"));
    parser.addOption(QCommandLineOption(QStringLiteral("all-disabled"), app.tr("marks all the plots as not visible")));
    parser.addOption(QCommandLineOption(QStringLiteral("simple-rotation"), app.tr("doesn't let you rotate the Z axis")));
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

    //BEGIN test calls
    PlotsFactory* s = PlotsFactory::self();
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("t->vector{cos(t), sin(t), t}")), Dim3D).create(Qt::green, QStringLiteral("curve")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("t->vector {t,t**2,t}")), Dim3D).create(Qt::green, QStringLiteral("parametric")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("(t,p)->2")), Dim3D).create(Qt::darkGreen, QStringLiteral("sphere-sphcoords")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("(r,p)->p")), Dim3D).create(Qt::magenta, QStringLiteral("cyl")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("piecewise{(x^2 + y^2 +z^2 < 35)?2 - (cos(x + (1+power(5,0.5))/2*y) + cos(x - (1+power(5,0.5))/2*y) + cos(y + (1+power(5,0.5))/2*z) + cos(y - (1+power(5,0.5))/2*z) + cos(z - (1+power(5,0.5))/2*x) + cos(z + (1+power(5,0.5))/2*x)),?1}=0")), Dim3D).create(QColor(40, 87, 159), QStringLiteral("complex")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("(x^2 + y^2 - 1) * ( x^2 + z^2 - 1) = 1")), Dim3D).create(Qt::cyan, QStringLiteral("implicit 0")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("(x,y)->(x*x-y*y)/8")), Dim3D).create(Qt::yellow, QStringLiteral("z-map")));
    model->addPlot(s->requestPlot(Analitza::Expression(QStringLiteral("x*x+y*y-z*z= 1/2")), Dim3D).create(Qt::darkBlue, QStringLiteral("implicit 2")));
    //END test calls

    if(parser.isSet(QStringLiteral("all-disabled")))
        for(int i=0; i<model->rowCount(); i++)
            model->setData(model->index(i), false, Qt::CheckStateRole);


    central->addWidget(viewsource);
    PlotsView3DES *view3des = new PlotsView3DES(central);
    view3des->setSelectionModel(viewsource->selectionModel());
    view3des->setUseSimpleRotation(parser.isSet(QStringLiteral("simple-rotation")));
    view3des->setModel(model);
    central->addWidget(view3des);
    view3des->setFocus();

    central->setStretchFactor(1, 2);
    mainWindow->setCentralWidget(central);
    mainWindow->show();

    return app.exec();
}
