/*************************************************************************************
 *  Copyright (C) 2019 Aleix Pol Gonzalez <aleixpol@kde.org>                         *
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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sndfile.h>
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QColor>
#include <plotsmodel.h>
#include <plotsfactory.h>
#include <planecurve.h>

#define SAMPLE_RATE 44100.0

using namespace Analitza;

void recordPlot(PlaneCurve* curve)
{
    curve->setResolution(3 * SAMPLE_RATE);
    curve->update(QRect(-10, 10, 20, -20));

    const auto points = curve->points();
    QVector<qreal> buffer;
    buffer.reserve(points.size());

//     for(const auto &p: points) {
//         buffer.append(p.y());
//     }
    const double frequency = 40;
    for(const auto &p: points) {
        for (int i = 0; i<3; ++i)
            buffer.append(sin( (2.0 * M_PI * frequency * p.y()) * (points.count() / SAMPLE_RATE)));
    }

    {
        SF_INFO sndInfo;

        sndInfo.samplerate = SAMPLE_RATE;
        sndInfo.format = SF_FORMAT_PCM_16 | SF_FORMAT_WAV;
        sndInfo.channels = 1;
        sndInfo.frames = buffer.size();

        SNDFILE *output = sf_open("plot.wav", SFM_WRITE, &sndInfo);
        sf_writef_double(output, buffer.data(), buffer.size());
        sf_close(output);
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("AudioPlots"));
    parser.addHelpOption();
    parser.process(app);

    if (parser.positionalArguments().isEmpty()) {
        parser.showHelp(1);
        return 1;
    }

    const QString expression = parser.positionalArguments().constLast();
    PlotsFactory* s = PlotsFactory::self();
    QScopedPointer<FunctionGraph> p(s->requestPlot(Analitza::Expression(expression), Dim2D).create(Qt::green, QStringLiteral("curve")));

    PlaneCurve* curve = dynamic_cast<PlaneCurve*>(p.data());
    if (!curve) {
        QTextStream(stderr) << "Cannot produce a sound curve from " << expression << '\n';
        return 1;
    }
    recordPlot(curve);
    return 0;
}
