/*************************************************************************************
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

#ifndef SURFACETEST_H
#define SURFACETEST_H

#include <QObject>

/**
    @author Percy Camilo T. Aucahuasi
*/
namespace Analitza { class Variables; }

class SurfaceTest : public QObject
{
Q_OBJECT
    public:
        SurfaceTest(QObject *parent = nullptr);
        ~SurfaceTest();

    private Q_SLOTS:
        void initTestCase();

        void testCorrect(); // correct surfaces
        void testCorrect_data();

        void testIncorrect(); // cases that can not be drawn
        void testIncorrect_data();

        //TODO
//         void testParamIntervals();
//         void testParamIntervals_data();

        void cleanupTestCase();
    private:
        Analitza::Variables* m_vars;
};

#endif
