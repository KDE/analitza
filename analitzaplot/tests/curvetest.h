/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#ifndef ANALITZATEST_H
#define ANALITZATEST_H

#include <QObject>

/**
    @author Aleix Pol
*/
namespace Analitza { class Variables; }

class PlaneCurveTest : public QObject
{
Q_OBJECT
    public:
        PlaneCurveTest(QObject *parent = 0);
        ~PlaneCurveTest();

    private slots:
        void initTestCase();
        
        void testCopyCompare_data();
        void testCopyCompare(); // use of =, == and != operators
        
        void testCorrectExpressions_data();
        void testCorrectExpressions(); // correct curves in native analitza sintax
        
        void testIncorrect_data();
        void testIncorrect(); // cases that can not be drawn
// ctor de curve con qlist y en el model pon solo addcurve pro con tipos nativos de qt
// el plottype ctor con typos de datos como exprresion en el model como string ...

        void cleanupTestCase();
    private:
        Analitza::Variables* m_vars;
};

#endif