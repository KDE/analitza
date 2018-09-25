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

#ifndef PLOTSMODELTEST_H
#define PLOTSMODELTEST_H

#include <QObject>

namespace Analitza { class Variables; class PlotsModel; }

class PlotsModelTest : public QObject
{
Q_OBJECT
    public:
        PlotsModelTest(QObject *parent = nullptr);
        ~PlotsModelTest();

    private Q_SLOTS:
        void testAppend();
        void testAppend_data();
        void testDelete();
        void testExamples2D();

    private:
        Analitza::PlotsModel *m_model;
};

#endif
