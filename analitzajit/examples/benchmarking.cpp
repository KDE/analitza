/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include <QTime>

#include "analitza/value.h"
#include "analitza/vector.h"
#include "analitza/container.h"
#include "analitzajit/jitanalyzer.h"

// #define PRINT_BENCHMARKING_DATA_FLAG 1
#define PRINT_BENCHMARKING_DATA() qDebug() << result;

Q_DECLARE_METATYPE(QVarLengthArray<double>);

QDebug operator<<(QDebug dbg, const QVarLengthArray<double> &a)
{
	dbg.nospace() << "(";
	for (int i = 0; i < a.size()-1; ++i) {
		dbg.nospace() << a.at(i) << ",";
	}
	dbg.nospace() << a.last() << ")";
	return dbg.space();
}

using namespace std;
using namespace Analitza;

double xmin = 0;
double xmax = 0;
double ymin = 0;
double ymax = 0;
double zmin = 0;
double zmax = 0;
double xstep = 0;
double ystep = 0;
double zstep = 0;

Cn* arg1 = new Analitza::Cn;
Cn* arg2 = new Analitza::Cn;
Cn* arg3 = new Analitza::Cn;

Analyzer a;
JITAnalyzer b;

void useDomain1() // standard [-1,1] 
{
	xmin = -1.0;
	xmax =  1.0;
	ymin = -1.0;
	ymax =  1.0;
	zmin = -1.0;
	zmax =  1.0;
	xstep = 0.01;
	ystep = 0.01;
	zstep = 0.01;
}

void useDomain2() // dense near zero
{
	xmin = -0.24;
	xmax =  0.32;
	ymin = -0.24;
	ymax =  0.16;
	zmin = -0.3;
	zmax =  0.9;
	xstep = 0.0037;
	ystep = 0.0025;
	zstep = 0.009;
}

void useDomain3() // in the interval [-100,100]
{
	xmin = -89.24;
	xmax =  65.32;
	ymin = -74.24;
	ymax =  35.16;
	zmin = -98.3;
	zmax =  36.9;
	xstep = 0.37;
	ystep = 0.25;
	zstep = 0.9;
}

//BEGIN profile 1

const Expression profile1 = Expression("(x,y)->(x^2+y^2-4)*((x-1)^2+y^2-0.3)*((x+1)^2+y^2-0.3)*(x^2+(y-2)^2-0.4)*(x^2+(-y+2)^2-0.4)");

double f_profile1(double x, double y)
{
	return (pow(x,2)+pow(y,2)-4)*(pow(x-1,2)+pow(y,2)-0.3)*(pow(x+1,2)+pow(y,2)-0.3)*(pow(x,2)+pow(y-2,2)-0.4)*(pow(x,2)+pow(y+2,2)-0.4);
}

void ceval_profile1()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		for (double y = ymin; y < ymax; y+=ystep) {
			double result = f_profile1(x,y);
			#ifdef PRINT_BENCHMARKING_DATA_FLAG
			PRINT_BENCHMARKING_DATA()
			#endif
		}
	}
	
	qDebug("Native evaluation:\t\t%20d\tms", t.elapsed());
}

void jiteval_profile1()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		arg1->setValue(x);
		for (double y = ymin; y < ymax; y+=ystep) {
			arg2->setValue(y);
			double result = 0;
			b.calculateLambda(result);
			#ifdef PRINT_BENCHMARKING_DATA_FLAG
			PRINT_BENCHMARKING_DATA()
			#endif
		}
	}
	
	qDebug("AnalitzaJIT evaluation:\t%20d\tms", t.elapsed());
}

void eval_profile1()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		arg1->setValue(x);
		for (double y = ymin; y < ymax; y+=ystep) {
			arg2->setValue(y);
			double result = a.calculateLambda().toReal().value();
			#ifdef PRINT_BENCHMARKING_DATA_FLAG
			PRINT_BENCHMARKING_DATA()
			#endif
		}
	}
	
	qDebug("Analitza evaluation:\t\t%20d\tms", t.elapsed());
}

//END profile 1

//BEGIN profile 2

const Expression profile2 = Expression("(u,v)->vector{(3+1.5*cos(u))*cos(v),(3+1.5*cos(u))*sin(v),abs(1.5*sin(u))}");

QVarLengthArray<double> f_profile2(double u, double v)
{
	double x = (3+1.5*cos(u))*cos(v);
	double y = (3+1.5*cos(u))*sin(v);
	double z = 1.5*sin(u);
	
	QVarLengthArray<double> ret(3);
	ret[0] = x;
	ret[1] = y;
	ret[2] = z;
	
	return ret;
}

void ceval_profile2()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		for (double y = ymin; y < ymax; y+=ystep) {
			QVarLengthArray<double> result = f_profile2(x,y);
			#ifdef PRINT_BENCHMARKING_DATA_FLAG
			PRINT_BENCHMARKING_DATA()
			#endif
		}
	}
	
	qDebug("Native evaluation:\t\t%20d\tms", t.elapsed());
}

void jiteval_profile2()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		arg1->setValue(x);
		for (double y = ymin; y < ymax; y+=ystep) {
			arg2->setValue(y);
			QVarLengthArray<double> result(3);
			b.calculateLambda(result);
			#ifdef PRINT_BENCHMARKING_DATA_FLAG
			PRINT_BENCHMARKING_DATA()
			#endif
		}
	}
	
	qDebug("AnalitzaJIT evaluation:\t%20d\tms", t.elapsed());
}

void eval_profile2()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		arg1->setValue(x);
		for (double y = ymin; y < ymax; y+=ystep) {
			arg2->setValue(y);
			const Vector* vec = static_cast<Vector*>(a.calculateLambda().tree());
			QVarLengthArray<double> result(3);
			result[0] = static_cast<Cn*>(vec->at(0))->value();
			result[1] = static_cast<Cn*>(vec->at(1))->value();
			result[2] = static_cast<Cn*>(vec->at(2))->value();
			#ifdef PRINT_BENCHMARKING_DATA_FLAG
			PRINT_BENCHMARKING_DATA()
			#endif
		}
	}
	
	qDebug("Analitza evaluation:\t\t%20d\tms", t.elapsed());
}

//END profile 2

//BEGIN profile 3

const Expression profile3 = Expression("(x,y,z)->piecewise{(x^2 + y^2 +z^2 < 35)?2 - (cos(x + (1+power(5,0.5))/2*y) + cos(x - (1+power(5,0.5))/2*y) + cos(y + (1+power(5,0.5))/2*z) + cos(y - (1+power(5,0.5))/2*z) + cos(z - (1+power(5,0.5))/2*x) + cos(z + (1+power(5,0.5))/2*x)),?z}");

double f_profile3(double x, double y, double z)
{
	double ret = 0;
	bool cond1 = (pow(x,2) + pow(y,2) +pow(z,2)) < 35;
	if (cond1)
		ret = 2 - (cos(x + (1+pow(5,0.5))/2*y) + cos(x - (1+pow(5,0.5))/2*y) + cos(y + (1+pow(5,0.5))/2*z) + cos(y - (1+pow(5,0.5))/2*z) + cos(z - (1+pow(5,0.5))/2*x) + cos(z + (1+pow(5,0.5))/2*x));
	else 
		ret = z;
	return ret;
}

void ceval_profile3()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		for (double y = ymin; y < ymax; y+=ystep) {
			for (double z = xmin; z < zmax; z+=zstep) {
				double result = f_profile3(x,y,z);
				#ifdef PRINT_BENCHMARKING_DATA_FLAG
				PRINT_BENCHMARKING_DATA()
				#endif
			}
		}
	}
	
	qDebug("Native evaluation:\t\t%20d\tms", t.elapsed());
}

void jiteval_profile3()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		arg1->setValue(x);
		for (double y = ymin; y < ymax; y+=ystep) {
			arg2->setValue(y);
			for (double z = xmin; z < zmax; z+=zstep) {
				arg3->setValue(z);
				double result = 0;
				b.calculateLambda(result);
				#ifdef PRINT_BENCHMARKING_DATA_FLAG
				PRINT_BENCHMARKING_DATA()
				#endif
			}
		}
	}
	
	qDebug("AnalitzaJIT evaluation:\t%20d\tms", t.elapsed());
}

void eval_profile3()
{
	QTime t;
	t.start();
	
	for (double x = xmin; x < xmax; x+=xstep) {
		arg1->setValue(x);
		for (double y = ymin; y < ymax; y+=ystep) {
			arg2->setValue(y);
			for (double z = xmin; z < zmax; z+=zstep) {
				arg3->setValue(z);
				double result = a.calculateLambda().toReal().value();
				b.calculateLambda(result);
				#ifdef PRINT_BENCHMARKING_DATA_FLAG
				PRINT_BENCHMARKING_DATA()
				#endif
			}
		}
	}
	
	qDebug("Analitza evaluation:\t\t%20d\tms", t.elapsed());
}

//END profile 3

void setupBenchmarking(const Expression &profile)
{
	a.setExpression(profile);
	a.setStack(QVector<Analitza::Object*>() << arg1 << arg2 << arg3);
	
	b.setExpression(profile);
	b.setStack(QVector<Analitza::Object*>() << arg1 << arg2 << arg3);
}

#define BENCHMARK_PROFILE(prfl, title) \
	setupBenchmarking(profile##prfl); \
	 \
	useDomain1(); \
	qDebug() << "==== PROFILE "+QString::number(prfl)+": "+title+" (USING DOMAIN 1) ===="; \
	ceval_profile##prfl(); \
	jiteval_profile##prfl(); \
	eval_profile##prfl(); \
	qDebug() << ""; \
	useDomain2(); \
	qDebug() << "==== PROFILE "+QString::number(prfl)+": "+title+" (USING DOMAIN 2) ===="; \
	ceval_profile##prfl(); \
	jiteval_profile##prfl(); \
	eval_profile##prfl(); \
	qDebug() << ""; \
	useDomain3(); \
	qDebug() << "==== PROFILE "+QString::number(prfl)+": "+title+" (USING DOMAIN 3) ===="; \
	ceval_profile##prfl(); \
	jiteval_profile##prfl(); \
	eval_profile##prfl(); \
	qDebug() << "\n\n\n"; \

void benchmarking()
{
	BENCHMARK_PROFILE(1, "REAL-VALUED FUNCTION")
	BENCHMARK_PROFILE(2, "VECTOR-VALUED FUNCTION")
	BENCHMARK_PROFILE(3, "REAL-VALUED PIECEWISE FUNCTION")
}

void finishBenchmarking()
{
	delete arg1;
	delete arg2;
	delete arg3;
}

int main(int argc, char *argv[])
{
	benchmarking();
	finishBenchmarking();
	
	return 0;
}
