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

#ifndef JITANALITZATEST_H
#define JITANALITZATEST_H

#include <QObject>

namespace Analitza { 
	class JITAnalyzer; 
	class Cn;
}

/**
	@author Percy Camilo T. Aucahuasi
*/
class AnalitzaJitTest : public QObject
{
Q_OBJECT
	public:
		AnalitzaJitTest(QObject *parent = 0);
		~AnalitzaJitTest();
	
	private Q_SLOTS:
		void initTestCase();
		
		void testCalculateUnaryBooleanLambda_data();
		void testCalculateUnaryBooleanLambda();
		
		void testCalculateUnaryRealLambda_data();
		void testCalculateUnaryRealLambda();
		
		void testCalculateBinaryRealLambda_data();
		void testCalculateBinaryRealLambda();
		
		void testCalculateUnaryRealVectorLambda_data();
		void testCalculateUnaryRealVectorLambda();
		
		void testCalculateBinaryRealVectorLambda_data();
		void testCalculateBinaryRealVectorLambda();
		
		void testCalculateUnaryMatrixVectorLambda_data();
		void testCalculateUnaryMatrixVectorLambda();
		
		void cleanupTestCase();
	private:
		Analitza::JITAnalyzer *a;
		Analitza::Cn* arg1;
		Analitza::Cn* arg2;
};

#endif
