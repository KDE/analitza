/*************************************************************************************
 *  Copyright (C) 2012 by Aleix Pol Gonzalez <aleixpol@kde.org>                      *
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
#include <QVBoxLayout>
#include <QListView>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <analitzagui/plotsview2d.h>
#include <plotsdictionarymodel.h>
#include <plotsmodel.h>

using namespace Analitza;

int main(int argc, char *argv[])
{
    KAboutData aboutData("PlotsDictionaryDemo",
                         0,
                         ki18n("PlotsDictionaryDemo"),
                         "1.0",
                         ki18n("PlotsDictionaryDemo"),
                         KAboutData::License_LGPL_V3,
                         ki18n("(c) 2012 Aleix Pol Gonzalez"),
                         ki18n("PlotsDictionaryDemo"),
                         "http://www.kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    QWidget central;
    central.resize(800,600);
    QHBoxLayout *layout = new QHBoxLayout(&central);
    QListView* tree = new QListView(&central);
    PlotsDictionaryModel m;
    tree->setModel(&m);
    layout->addWidget(tree);
    QObject::connect(tree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), &m, SLOT(setCurrentIndex(QModelIndex)));
    
    QListView* plots = new QListView(&central);
    plots->setModel(m.plotModel());
    layout->addWidget(plots);
    
    PlotsView2D* view2d = new PlotsView2D(&central);
    view2d->setModel(m.plotModel());
    layout->addWidget(view2d);
    
    central.show();
    return app.exec();
}

