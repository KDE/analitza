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


class ANALITZAPLOT_EXPORT CartesianCurveY : public FunctionImpl2D
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


        virtual void updateGraphData (Function *function);

        virtual FunctionImpl *copy();

//         virtual void setFixedGradient (const VectorXd &funcvalargs);
        virtual void clearFixedGradients();
        virtual FunctionGraphData *data() const;


private:

    Analitza::Cn *m_x;

bool traverse(double p1, double p2, double next)
{
	static const double delta=3;
	double diff=p2-p1, diff2=next-p2;
	bool ret = (diff>0 && diff2<-delta) || (diff<0 && diff2>delta);

	return ret;
}


bool addValue(const QPointF& p)
{
	int count=m_data->points.count();
	if(count<2) {
		m_data->points.append(p);
		return false;
	}

	double angle1=std::atan2(m_data->points[count-1].y()-m_data->points[count-2].y(), m_data->points[count-1].x()-m_data->points[count-2].x());
	double angle2=std::atan2(p.y()-m_data->points[count-1].y(), p.x()-m_data->points[count-1].x());

	bool append=!isSimilar(angle1, angle2);
	if(append)
		m_data->points.append(p);
	else
		m_data->points.last()=p;

	return append;
}

void optimizeJump(Function *function)
{
	QPointF before = m_data->points.at(m_data->points.count()-2), after=m_data->points.last();
	qreal x1=before.x(), x2=after.x();
	qreal y1=before.y(), y2=after.y();
	int iterations=5;

// 	qDebug() << "+++++++++" << before << after;
	for(; iterations>0; --iterations) {
		qreal dist = x2-x1;
		qreal x=x1+dist/2;

        m_x->setValue(x);
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
}

	FunctionGraphData2D *m_data;

};

// void CartesianCurveY::setFixedGradient (const VectorXd &funcvalargs)
// {
//
// }

void CartesianCurveY::clearFixedGradients()
{

}

FunctionGraphData * CartesianCurveY::data() const
{
	return m_data;
}

CartesianCurveY::CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables)
:FunctionImpl2D(functionExpression, variables)
{
	m_data = new FunctionGraphData2D;

    //TODO gsoc
//     m_x = dynamic_cast<Analitza::Cn*>(m_runStack.first());
}


FunctionImpl *CartesianCurveY::copy()
{
	return 0;
}

void CartesianCurveY::updateGraphData (Function *function)
{
	int resolution = function->graphPrecision()*10;

	int l_lim = function->argumentInterval("x").first;
	int r_lim = function->argumentInterval("x").second;
	m_data->jumps.clear();
	m_data->points.clear();
	m_data->points.reserve(resolution);

	double step= (-l_lim+r_lim)/(double)(resolution);

	bool jumping=true;
	for(double x=l_lim; x<r_lim-step; x+=step) {
        m_x->setValue(x);
		double y = analyzer.calculateLambda().toReal().value();
		QPointF p(x, y);
		bool ch=addValue(p);

		bool jj=jumping;
		jumping=false;
		if(ch && !jj) {
// 			if(!m_jumps.isEmpty()) qDebug() << "popopo" << m_jumps.last() << points.count();
			double prevY=m_data->points[m_data->points.count()-2].y();
			if(m_x->format()!=Analitza::Cn::Real && prevY!=y) {
				m_data->jumps.append(m_data->points.count()-1);
				jumping=true;
			} else if(m_data->points.count()>3 && traverse(m_data->points[m_data->points.count()-3].y(), prevY, y)) {
				optimizeJump(function);
				m_data->jumps.append(m_data->points.count()-1);
				jumping=true;
			}
		}
	}
// 	qDebug() << "juuuumps" << m_jumps << resolution();
}

REGISTER_FUNCTION2D(CartesianCurveY)

