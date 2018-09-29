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

#ifndef COMMANDSTEST_H
#define COMMANDSTEST_H

#include <QObject>

namespace Analitza { class Analyzer; }

/**
    @author Percy Camilo T. Aucahuasi
*/
class CommandsTest : public QObject
{
Q_OBJECT
    public:
        CommandsTest(QObject *parent = nullptr);
        ~CommandsTest();
    
    private Q_SLOTS:
        void initTestCase();
        
        // check correct calculation of matrix commands
        void testCorrect_data();
        void testCorrect();
        
        // check correct generation of empty matrix by using commands
        void testIncorrect_data();
        void testIncorrect();
        
        void cleanupTestCase();
    private:
        
        Analitza::Analyzer *a;
};

#endif // COMMANDSTEST_H
