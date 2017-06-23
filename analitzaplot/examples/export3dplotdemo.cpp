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
#include <QFileInfo>
#include <QImage>
#include <analitzaplot/plotter3d_es.h>
#include <analitzaplot/plotsmodel.h>
#include <analitzaplot/plotsfactory.h>
#include <functiongraph.h>
#include <iostream>

using namespace Analitza;

class ExportPlotter3D : public Plotter3DES
{
public:
    ExportPlotter3D(PlotsModel* m) : Plotter3DES(m) {}

    virtual int currentPlot() const override { return -1; }
    virtual void renderGL() override {}
    virtual void modelChanged() override {}
    QImage grabImage() override { return {}; }
};

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("expression"), QGuiApplication::translate("option description", "Expression to plot"), QStringLiteral("expression..."));
    parser.addPositionalArgument(QStringLiteral("output"), QGuiApplication::translate("option description", "Created filename"), QStringLiteral("output.x3d"));
    parser.addOption(QCommandLineOption(QStringLiteral("interval"), QGuiApplication::translate("option description", "Specifies an interval"), QStringLiteral("var=num..num")));
    parser.addHelpOption();

    parser.process(app);
    PlotsModel model;

    QMap<QString, QPair<double,double> > intervals;
    foreach(const QString& interval, parser.values(QLatin1String("interval"))) {
        int equalIdx = interval.indexOf('=');
        int dotdotIdx = interval.indexOf(QStringLiteral(".."));

        if(equalIdx<0 || dotdotIdx<0) {
            qDebug() << "Intervals should be specified as x=b..c";
        }
        bool ok;
        double from = interval.midRef(equalIdx+1, dotdotIdx-equalIdx).toDouble(&ok);
        Q_ASSERT(ok);
        double to = interval.midRef(dotdotIdx+2).toDouble(&ok);
        Q_ASSERT(ok);
        intervals[interval.left(equalIdx)] = qMakePair<double, double>(from, to);
    }

    QStringList args = parser.positionalArguments();
    const QString output = args.takeLast();

    foreach(const QString& input, args) {
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
        foreach(const QString& bvar, plot.expression().bvarList()) {
            QMap< QString, QPair< double, double > >::const_iterator itF = intervals.constFind(bvar);
            if(itF != intervals.constEnd()) {
                it->setInterval(itF.key(), itF->first, itF->second);
            }
        }
        model.addPlot(it);
    }
    ExportPlotter3D plotter(&model);
    plotter.updatePlots(QModelIndex(), 0, model.rowCount()-1);
    plotter.exportSurfaces(output);

    return 0;
}
