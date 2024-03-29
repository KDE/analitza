/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef PLANECURVETEST_H
#define PLANECURVETEST_H

#include <QObject>
#include <QSharedPointer>

/**
    @author Aleix Pol
*/
namespace Analitza { class Variables; }

class PlaneCurveTest : public QObject
{
Q_OBJECT
    public:
        PlaneCurveTest(QObject *parent = nullptr);
        ~PlaneCurveTest() override;

    private Q_SLOTS:
        void initTestCase();

        void testCorrect(); // correct curves
        void testCorrect_data();

        void testIncorrect(); // cases that can not be drawn
        void testIncorrect_data();

        void testJumps();
        void testJumps_data();

        void testParamIntervals();
        void testParamIntervals_data();

        void cleanupTestCase();
    private:
        QSharedPointer<Analitza::Variables> m_vars;
};

#endif
