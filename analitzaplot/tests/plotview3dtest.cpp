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
#include <analitza/variables.h>
#include <analitza/apply.h>
#include <analitza/variable.h>
#include <analitza/container.h>
#include <analitza/polynomial.h>


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
    
//     model->addItem(Analitza::Expression("x*x"), "Hola", Qt::cyan);

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

//     if (model->rowCount()>0)
//     {
//         selection->setCurrentIndex(model->index(model->rowCount()-1), QItemSelectionModel::Select);
//     }

    //END test calls
    
    mainWindow->setCentralWidget(view3d);

    mainWindow->show();

    return app.exec();
}
