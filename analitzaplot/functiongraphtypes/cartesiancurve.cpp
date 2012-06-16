/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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


#include "functiongraph.h"
#include "functiongraphfactory.h"

#include <QRectF>
#include "analitza/value.h"
#include "analitza/variable.h"


class ANALITZAPLOT_EXPORT CartesianCurveY : public AbstractCurve
{
public:


    

    CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables);
	~CartesianCurveY()
	{
		delete m_data;
	}

    static Analitza::ExpressionType expressionType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }

    static QStringList arguments() { return QStringList() << "x"; }

    static CoordinateSystem coordinateSystem() { return Cartesian; }
    static QString iconName() { return QString(""); }

    static QStringList examples()
    {
        return QStringList() << "x->root(x, 2)-5";
    }

    
    QStringList errors() const
    {
            return QStringList();
    }
    
    
    bool isCorrect() const { return m_errors.isEmpty() && analyzer.isCorrect(); }

    void updateGraph(const QRect& viewport);
    QPair<QPointF, QString> calc(const QPointF& dp);
    QLineF derivative(const QPointF& p);
        virtual AbstractMappingGraph *copy();


private:
QStringList m_errors;

bool traverse(double p1, double p2, double next)
{
	static const double delta=3;
	double diff=p2-p1, diff2=next-p2;
	bool ret = (diff>0 && diff2<-delta) || (diff<0 && diff2>delta);

	return ret;
}



/*
void optimizeJump(FunctionGraph *function)
{
	QPointF before = m_data->points.at(m_data->points.count()-2), after=m_data->points.last();
	qreal x1=before.x(), x2=after.x();
	qreal y1=before.y(), y2=after.y();
	int iterations=5;

// 	qDebug() << "+++++++++" << before << after;
	for(; iterations>0; --iterations) {
		qreal dist = x2-x1;
		qreal x=x1+dist/2;

        arg("x")->setValue(x);
		double y = analyzer.calculateLambda().toReal().value();

		if(fabs(y1-y)<fabs(y2-y)) {
			before.setX(x);
			before.setY(y);
			x1=x;
			y1=y;
		} else {
			after.setX(x);
			after.setY(y);
			x2=x;
			y2=y;
		}
	}
// 	qDebug() << "---------" << before << after;
	m_data->points[m_data->points.count()-2]=before;
	m_data->points.last()=after;
}*/

	FunctionGraphData2D *m_data;

};

// void CartesianCurveY::setFixedGradient (const VectorXd &funcvalargs)
// {
//
// }

CartesianCurveY::CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables)
:AbstractCurve(functionExpression, variables)
{
	m_data = new FunctionGraphData2D;

    //TODO gsoc
//     arg("x") = dynamic_cast<Analitza::Cn*>(m_runStack.first());
}


AbstractMappingGraph *CartesianCurveY::copy()
{
	return 0;
}

void CartesianCurveY::updateGraph(const QRect& viewport)
{
	
}


QPair< QPointF, QString > CartesianCurveY::calc(const QPointF& dp)
{
return qMakePair<QPointF, QString>(QPointF(), QString());
}

QLineF CartesianCurveY::derivative(const QPointF& p)
{
return QLineF();
}












REGISTER_FUNCTION2D(CartesianCurveY)

