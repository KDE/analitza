/*************************************************************************************
 *  Copyright (C) 2014 Aleix Pol Gonzalez <aleixpol@kde.org>                         *
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

#include <QGuiApplication>
#include <QCommandLineParser>
#include <qfileinfo.h>
#include <analitzaplot/plotter3d.h>
#include <analitzaplot/plotsmodel.h>
#include <analitzaplot/plotsfactory.h>
#include <functiongraph.h>
#include <iostream>

using namespace Analitza;

class ExportPlotter3D : public Plotter3D
{
public:
    ExportPlotter3D(PlotsModel* m) : Plotter3D(m) {}

    virtual int currentPlot() const { return -1; }
    virtual void renderGL() {}
    virtual void modelChanged() {}
};

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addPositionalArgument("expression", QGuiApplication::translate("option description", "Expression to plot"), "expression...");
    parser.addOption(QCommandLineOption("output", QGuiApplication::translate("option description", "Created filename"), "output.x3d"));

    parser.process(app);
    PlotsModel model;

    foreach(const QString& input, parser.positionalArguments()) {
        Expression exp(input);
        if(!exp.isCorrect()) {
            std::cerr << "Incorrect expression: " << qPrintable(input) << std::endl;
        }
        PlotBuilder plot = PlotsFactory::self()->requestPlot(exp, Analitza::Dim3D);
        if(!plot.canDraw()) {
            std::cerr << "Cannot draw " << qPrintable(input) << " in 3D" << std::endl;
            return 1;
        }

        FunctionGraph* it = plot.create(Qt::blue, QStringLiteral("hola"));
        model.addPlot(it);
    }
    ExportPlotter3D plotter(&model);
    plotter.updatePlots(QModelIndex(), 0, model.rowCount()-1);
    plotter.exportSurfaces(parser.value("output"));

    return 0;
}
