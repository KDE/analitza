/*************************************************************************************
 *  Copyright (C) 2012 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "analitzadeclarativeplugin.h"
#include "analitzawrapper.h"
#include <analitzaplot/plotsmodel.h>
#include <analitzagui/variablesmodel.h>
#include <analitzagui/operatorsmodel.h>
#include <analitza/variables.h>
#include <graph2dmobile.h>
#include <graph3ditem.h>
#include <QQuickItem>

void AnalitzaDeclarativePlugin::registerTypes(const char* uri)
{
    qmlRegisterType<AnalitzaWrapper>(uri, 1, 0, "Analitza");
    qmlRegisterType<ExpressionWrapper>(uri, 1, 0, "Expression");
    qmlRegisterType<Graph2DMobile>(uri, 1, 0, "Graph2DView");
    qmlRegisterType<Graph3DItem>(uri, 1, 1, "Graph3DView");
    qmlRegisterType<Analitza::PlotsModel>(uri, 1, 0, "PlotsModel");
    qmlRegisterType<Analitza::VariablesModel>(uri, 1, 0, "VariablesModel");
    qmlRegisterType<OperatorsModel>(uri, 1, 0, "OperatorsModel");

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    qmlRegisterInterface<Analitza::Variables*>("Analitza::Variables");
#else
    qmlRegisterInterface<Analitza::Variables>(uri, 1);
#endif
}
