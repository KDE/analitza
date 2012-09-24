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

#include "plotsfactory.h"
#include "kglobal.h"
#include <KLocalizedString>
#include <analitzaplot/functiongraph.h>
#include "private/functiongraphfactory.h"
#include <analitza/analyzer.h>
#include <QStringList>

K_GLOBAL_STATIC(PlotsFactory, factoryInstance)

PlotsFactory::PlotsFactory()
{}

PlotsFactory* PlotsFactory::self()
{
	return factoryInstance;
}

PlotBuilder PlotsFactory::requestPlot(const Analitza::Expression& testexp, Dimension dim) const
{
	QStringList errs;
	
	if(!testexp.isCorrect() || testexp.toString().isEmpty()) {
		errs << i18n("The expression is not correct");
		PlotBuilder b;
		b.m_errors = errs;
		return b;
	}
	
	Analitza::Expression exp(testexp);
	if(exp.isDeclaration())
		exp = exp.declarationValue();
	
	if(exp.isEquation())
		exp = exp.equationToFunction();
	
	Analitza::Analyzer a;
	a.setExpression(exp);
	a.setExpression(a.dependenciesToLambda());
	
	QString id;
	if(a.isCorrect()) {
		QString expectedid = FunctionGraphFactory::self()->trait(a.expression(), a.type(), dim);
		if(FunctionGraphFactory::self()->contains(expectedid)) {
			id = expectedid;
		} else
			errs << i18n("Function type not recognized");
	} else {
		errs << a.errors();
	}
	
	Q_ASSERT(!errs.isEmpty() || !id.isEmpty());
	
	PlotBuilder b;
	b.m_errors = errs;
	b.m_id = id;
	b.m_expression = a.expression();
	return b;
}

QStringList PlotsFactory::examples(Dimensions s) const
{
	QStringList examples;
	if(s & Dim1D) examples += FunctionGraphFactory::self()->examples(Dim1D);
	if(s & Dim2D) examples += FunctionGraphFactory::self()->examples(Dim2D);
	if(s & Dim3D) examples += FunctionGraphFactory::self()->examples(Dim3D);
	return examples;
}

//------------------------------------------ PlotBuilder

bool PlotBuilder::canDraw() const
{
	return m_errors.isEmpty() && !m_id.isEmpty();
}

FunctionGraph* PlotBuilder::create(const QColor& color, const QString& name, Analitza::Variables* v) const
{
	FunctionGraph* it = FunctionGraphFactory::self()->buildItem(m_id, m_expression, v);
	it->setColor(color);
	it->setName(name);
	return it;
}

Analitza::Expression PlotBuilder::expression() const
{
    return m_expression;
}
