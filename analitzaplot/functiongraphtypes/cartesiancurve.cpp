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


namespace Keomath
{
class ANALITZAPLOT_EXPORT CartesianCurveY : public Keomath::FunctionGraph
{
public:
    CartesianCurveY();
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

    static QStringList argumentNames() { return QStringList() << "x"; }

    static CoordinateSystem coordinateSystem() { return Cartesian; }

    static bool isImplicit() { return false;}

    static QStringList examples()
    {
        return QStringList() << "x->root(x, 2)-5";
    }

    static QString iconName() { return QString(""); }

        virtual void updateGraphData (Keomath::Function *function);

        virtual FunctionGraph *copy();

//         virtual void setFixedGradient (const VectorXd &funcvalargs);
        virtual void clearFixedGradients();
        virtual FunctionGraphData *data() const;


private:


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

void optimizeJump(Keomath::Function *function)
{
	QPointF before = m_data->points.at(m_data->points.count()-2), after=m_data->points.last();
	qreal x1=before.x(), x2=after.x();
	qreal y1=before.y(), y2=after.y();
	int iterations=5;

// 	qDebug() << "+++++++++" << before << after;
	for(; iterations>0; --iterations) {
		qreal dist = x2-x1;
		qreal x=x1+dist/2;

		double y = function->evaluateRealValue(x);

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

	Keomath::FunctionGraphData2D *m_data;

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

CartesianCurveY::CartesianCurveY()
{
	m_data = new Keomath::FunctionGraphData2D;
}


FunctionGraph *CartesianCurveY::copy()
{
	return 0;
}

void CartesianCurveY::updateGraphData (Keomath::Function *function)
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
		double y = function->evaluateRealValue(x);
		QPointF p(x, y);
		bool ch=addValue(p);

		bool jj=jumping;
		jumping=false;
		if(ch && !jj) {
// 			if(!m_jumps.isEmpty()) qDebug() << "popopo" << m_jumps.last() << points.count();
			double prevY=m_data->points[m_data->points.count()-2].y();
			if(function->argumentValue("x")->format()!=Analitza::Cn::Real && prevY!=y) {
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

REGISTER_FUNCTIONGRAPH(CartesianCurveY)
}

class ANALITZAPLOT_EXPORT CartesianCurveY : public FunctionImpl2D
{
public:
    explicit CartesianCurveY(const Analitza::Expression &expression, Analitza::Variables *variables);
    CartesianCurveY(const CartesianCurveY &cartesianCurveY);
    virtual ~CartesianCurveY();

    static QStringList supportedBVars()
    {
        return QStringList("x");
    }
    static Analitza::ExpressionType expectedType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }
    static QStringList examples()
    {
        return QStringList("x->root(x, 2)-5");
    }

    QStringList arguments() const
    {
        return supportedBVars();
    }
    Function::Axe axeType() const
    {
        return Function::Cartesian;
    }
    void solve(const RealInterval::List &spaceBounds = RealInterval::List());
    FunctionImpl * copy()
    {
        return new CartesianCurveY(*this);
    }

    QPair<QPointF, QString> calc(const QPointF &dp);
    QLineF derivative(const QPointF &point);

    bool allDisconnected() const
    {
        return false;
    }


protected:
    Analitza::Cn* m_arg;
    Analitza::Expression m_deriv;
};

CartesianCurveY::CartesianCurveY(const Analitza::Expression &expression, Analitza::Variables *variables)
    : FunctionImpl2D(expression, variables)
    , m_arg(new Analitza::Cn)
{
    m_runStack.append(m_arg);
    m_evaluator.setStack(m_runStack);
    m_type2D = 1;



    {


        if (m_evaluator.isCorrect())
        {

            m_deriv = m_evaluator.derivative(arguments().first());

            m_evaluator.flushErrors();

        }
    }
}

CartesianCurveY::CartesianCurveY(const CartesianCurveY &cartesianCurveY)
    : FunctionImpl2D(cartesianCurveY)
    , m_arg(new Analitza::Cn)

{
    m_runStack.append(m_arg);
    m_evaluator.setStack(m_runStack);
    m_type2D = 1;


    m_deriv = cartesianCurveY.m_deriv;
    m_evaluator.flushErrors();

}

CartesianCurveY::~CartesianCurveY()
{
    delete m_arg;
}

void CartesianCurveY::solve(const RealInterval::List &spaceBounds)
{

























    double l_lim=m_domain.at(0).lower();
    double r_lim=m_domain.at(0).upper();



    QRectF viewport;
    viewport.setLeft(spaceBounds[0].lower());
    viewport.setRight(spaceBounds[0].upper());
    viewport.setBottom(spaceBounds[1].lower());
    viewport.setTop(spaceBounds[1].upper());


    QList<QPointF> points;

    qreal image = 0.;


    for (qreal domain = l_lim; domain < r_lim; domain += 0.01)
    {

        m_arg->setValue(domain);

        image = m_evaluator.calculateLambda().toReal().value();


        points.append(QPointF(domain, image));
    }


    buildPaths(viewport, points);

}

QPair<QPointF, QString> CartesianCurveY::calc(const QPointF& p)
{
    QPointF dp=p;
    m_arg->setValue(dp.x());
    Analitza::Expression r=m_evaluator.calculateLambda();

    if(!r.isReal())
        m_errors += i18n("We can only draw Real results.");

    dp.setY(r.toReal().value());

    return QPair<QPointF, QString>(dp, QString());



}

QLineF CartesianCurveY::derivative(const QPointF& p)
{
    Analitza::Analyzer a(m_evaluator.variables());
    double ret;


    qDebug() << m_deriv.isCorrect() << m_deriv.error();
    qDebug() << m_deriv.toString();


    {
        QVector<Analitza::Object*> vars;
        vars.append(new Analitza::Cn(p.x()));
        a.setExpression(m_evaluator.expression());
        ret=a.derivative(vars);
        qDeleteAll(vars);
    }

    return slopeToLine(ret);

}

REGISTER_FUNCTION_2D(CartesianCurveY)






class ANALITZAPLOT_EXPORT CartesianCurveX : public CartesianCurveY
{
public:
    explicit CartesianCurveX(const Analitza::Expression &expression, Analitza::Variables *variables);
    CartesianCurveX(const CartesianCurveX &cartesianCurveX);
    virtual ~CartesianCurveX();

    static QStringList supportedBVars()
    {
        return QStringList("y");
    }
    static Analitza::ExpressionType expectedType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }
    static QStringList examples()
    {
        return QStringList("y->y*sin(y)");
    }

    QStringList arguments() const
    {
        return supportedBVars();
    }
    void solve(const RealInterval::List &spaceBounds = RealInterval::List());
    FunctionImpl * copy()
    {
        return new CartesianCurveX(*this);
    }

    QPair<QPointF, QString> calc(const QPointF &dp);
    QLineF derivative(const QPointF &point);
};

CartesianCurveX::CartesianCurveX(const Analitza::Expression &expression, Analitza::Variables *variables)
    : CartesianCurveY(expression, variables)
{
}

CartesianCurveX::CartesianCurveX(const CartesianCurveX &cartesianCurveX)
    : CartesianCurveY(cartesianCurveX)
{
}

CartesianCurveX::~CartesianCurveX()
{
}

void CartesianCurveX::solve(const RealInterval::List &spaceBounds)
{

    double l_lim=m_domain.at(0).lower();
    double r_lim=m_domain.at(0).upper();


    QRectF viewport;
    viewport.setLeft(spaceBounds[0].lower());
    viewport.setRight(spaceBounds[0].upper());
    viewport.setBottom(spaceBounds[1].lower());
    viewport.setTop(spaceBounds[1].upper());


    QList<QPointF> points;

    qreal image = 0.;

    for (qreal domain = l_lim; domain < r_lim; domain += 0.01)
    {
        m_arg->setValue(domain);

        image = m_evaluator.calculateLambda().toReal().value();

        points.append(QPointF(image, domain));
    }


    buildPaths(viewport, points);


}

QPair<QPointF, QString> CartesianCurveX::calc(const QPointF& p)
{
    QPointF dp=p;
    m_arg->setValue(dp.y());
    Analitza::Expression r=m_evaluator.calculateLambda();

    if(!r.isReal())
        m_errors += i18n("We can only draw Real results.");

    dp.setX(r.toReal().value());

    return QPair<QPointF, QString>(dp, QString());

}

QLineF CartesianCurveX::derivative(const QPointF& p)
{
    QPointF p1(p.y(), p.x());
    QLineF ret=CartesianCurveY::derivative(p1);
    return mirrorXY(ret);



}

REGISTER_FUNCTION_2D(CartesianCurveX)


