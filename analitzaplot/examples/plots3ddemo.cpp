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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

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


#include <functional>



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
    
        QTabWidget *tabs = new QTabWidget(mainWindow);
    QTreeView *viewsource = new QTreeView(tabs);
    QTreeView *viewproxy = new QTreeView(tabs);
    
    //BEGIN test calls

    
    PlotsModel *model = new PlotsModel(mainWindow);
    QItemSelectionModel *selection = new QItemSelectionModel(model);
    
    PlotsProxyModel *proxy = new PlotsProxyModel(mainWindow);
    proxy->setFilterSpaceDimension(3);
    proxy->setSourceModel(model);
    
    PlotsView3D *view3d = new PlotsView3D(mainWindow);
//     view3d->setGridIsDrawn(false);
    view3d->setModel(proxy);
    view3d->setSelectionModel(selection);
    
//     qDebug() << model->addSurface(Analitza::Expression("(r,p)->2"), "Hola", Qt::cyan);
//     qDebug() << model->addSurface(Analitza::Expression("(a,p)->2"), "Hola", Qt::cyan);
//     qDebug() << model->addSurface(Analitza::Expression("(u,v)->vector{u,v,u*u+v*v}"), "Hola", Qt::cyan);
//     qDebug() << model->addSurface(Analitza::Expression("x*x+y*y+z*z=5"), "Hola", Qt::cyan);
//    qDebug() << model->addSurface(Analitza::Expression("cos(x) + cos(y) + cos(z)=0"), "Hola", Qt::cyan);

    model->addSurface(Analitza::Expression("(x^2 + y^2 - 1) * ( x^2 + z^2 - 1) = 1"), "Hola", Qt::cyan)->spaceDimension();
    model->addPlaneCurve(Analitza::Expression("x+3-y=7"), "asdasd, ", Qt::red)->spaceDimension();
    model->addSurface(Analitza::Expression("x*x+y*y-z*z= 1/2"), "Hola", Qt::yellow)->spaceDimension();
    qDebug() << model->addSurface(Analitza::Expression("(x,y)->-e^(-x^2-y^2)"), "aaa", Qt::lightGray);
    model->addPlaneCurve(Analitza::Expression("t->vector{t,t*t}"), "asdasd, ", Qt::blue)->spaceDimension();

    
//     qDebug() << item << item->spaceDimension();
    
//     qDebug() << item->expression().toString() << item->interval("x");
    
    
//     PlotsDictionaryModel *dict = new PlotsDictionaryModel(mainWindow);
    
//     qDebug() << dict->isLoaded() << dict->errors();

    
//     model->addSurface(Analitza::Expression("(x,y)->y*y"), "Hola", Qt::yellow);
//     model->addSurface(Analitza::Expression("(x,y)->x*x+y*y"), "Hola", Qt::gray);
//     
//     model->removeItem(1);
//     model->removeItem(1);
//     model->removeItem(1);
//     model->removeItem(1);
    

//     qDebug() << model->item(0)->spaceDimension() << static_cast<const Surface*>(model->item(0))->faces().size();

//     Analitza::Expression exp(QString("x*x-4*y+6"));
// //     Analitza::Expression exp(QString("x*x+3*x=-y*y+5"));
// 
//     Analitza::Analyzer a;
//     a.setExpression(exp);
//     a.setExpression(a.dependenciesToLambda());
// 
//     qDebug() << a.type().toString() << a.type().parameters().last().toString() << a.type().parameters().size();
//     
    /*
    Analitza::Container *eq = static_cast<Analitza::Container*>(exp.tree()->copy());
    Analitza::Apply *apply = new Analitza::Apply(   );*/

    
    


    
    /*
    
    Analitza::Expression exp(QString("z*x+x+y=z+5"));
/*
    Analitza::Cn *x = new Analitza::Cn;
    Analitza::Cn *y = new Analitza::Cn;
    Analitza::Cn *z = new Analitza::Cn;

    QVector<Analitza::Object*> stack;
    stack << x << y << z;
    x->setValue(3);
    y->setValue(0);
    z->setValue(1);
    
    Analitza::Analyzer a;
//     a.setStack(stack);
    a.setExpression(exp);
    a.setExpression(a.dependenciesToLambda());

    qDebug() << exp.tree()->type() << a.expression().lambdaBody().tree()->type();

    
    Analitza::Container *eq1 = static_cast<Analitza::Container*>(exp.tree()->copy());

    Analitza::Apply *eq = static_cast<Analitza::Apply *>(a.expression().lambdaBody().tree()->copy());

    Analitza::Apply *product = new Analitza::Apply;
    product->appendBranch(new Analitza::Operator(Analitza::Operator::product));
    product->appendBranch(new Analitza::Cn(-1.0));
    product->appendBranch(eq->at(1)->copy());

    Analitza::Apply *apply = new Analitza::Apply;
    apply->appendBranch(new Analitza::Operator(Analitza::Operator::plus));
    apply->appendBranch(eq->at(0)->copy());
    apply->appendBranch(product);

    */
/*
    Analitza::Operator *product = new Analitza::Operator(Analitza::Operator::product);
    Analitza::Apply *apply3 = new Analitza::Apply;
    apply3->appendBranch(product);
    apply3->appendBranch(new Analitza::Cn(-1.0));
    apply3->appendBranch(apply->at(1)->copy());

    apply2->appendBranch(apply3);*/
    
    
//     apply2.appendBranch(new Analitza::Operator(Analitza::Operator::product));

//     qDebug() << Analitza::Expression(apply).toString();
    
    
    
/*
    Analitza::Analyzer a;
    a.setStack(stack);
    a.setExpression(exp);
    a.setExpression(a.dependenciesToLambda());
    
    Analitza::Expression implpart(a.expression().lambdaBody());
    const Analitza::Apply *apply = static_cast<const Analitza::Apply *>(implpart.tree());

    Analitza::Expression eq(apply->at(0)->toString()+"-"+apply->at(1)->toString());
    
    qDebug() << apply->toString() << eq.toString();
    */
//     model->addItem(Analitza::Expression("t->vector{t*t, t, t*t}"),3, "Hola", Qt::yellow);

    if (model->rowCount()>0)
    {
        selection->setCurrentIndex(model->index(model->rowCount()-1), QItemSelectionModel::Select);
    }

    //END test calls
    
    viewsource->setModel(model);
    viewproxy->setModel(proxy);
    
    tabs->addTab(viewsource, "source");
    tabs->addTab(viewproxy, "proxy3d");
    tabs->addTab(view3d, "view3d");

    mainWindow->setCentralWidget(tabs);
    mainWindow->show();

    return app.exec();
}
