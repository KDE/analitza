/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef ANALITZAWRAPPER_H
#define ANALITZAWRAPPER_H

#include <QScopedPointer>
#include <QObject>
#include <QVariant>
#include <analitza/expression.h>

namespace Analitza {
    class Analyzer;
    class Variables;
}

class ExpressionWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString expression READ toString CONSTANT)
    Q_PROPERTY(bool isCorrect READ isCorrect CONSTANT)
    public:
        explicit ExpressionWrapper(QObject* parent=nullptr);
        explicit ExpressionWrapper(const Analitza::Expression & e, QObject* parent = nullptr);
        
        bool isCorrect() const;
        QString toString() const;

    public Q_SLOTS:
        QVariant value() const;
        QStringList errors() const;
        
    private:
        Analitza::Expression m_exp;
};

class AnalitzaWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool calculate READ isCalculate WRITE setCalculate NOTIFY isCalculateChanged)
    Q_PROPERTY(bool isCorrect READ isCorrect)
    Q_PROPERTY(QStringList errors READ errors)
    Q_PROPERTY(QSharedPointer<Analitza::Variables> variables READ variables WRITE setVariables)
    public:
        explicit AnalitzaWrapper(QObject* parent = nullptr);
        ~AnalitzaWrapper();
        
        void setCalculate(bool calc);
        bool isCalculate() const { return m_calc; } 
        
        Analitza::Analyzer* wrapped() const { return m_wrapped.data(); }
        
        Q_SCRIPTABLE QVariant simplify(const QString& expression);

        Q_SCRIPTABLE QVariant execute(const QString& expression);
        Q_SCRIPTABLE QVariant executeFunc(const QString& name, const QVariantList& args);
        Q_SCRIPTABLE QString unusedVariableName() const;
        Q_SCRIPTABLE QString dependenciesToLambda(const QString& expression) const;
        Q_SCRIPTABLE void insertVariable(const QString& name, const QString& expression) const;
        Q_SCRIPTABLE void removeVariable(const QString& name);
        
        QStringList errors() const;
        bool isCorrect() const;
        
        QSharedPointer<Analitza::Variables> variables() const { return m_vars; }
        void setVariables(const QSharedPointer<Analitza::Variables> &v);

    Q_SIGNALS:
        void isCalculateChanged(bool isCalculate);
        
    private:
        void initWrapped();
        
        QScopedPointer<Analitza::Analyzer> m_wrapped;
        QSharedPointer<Analitza::Variables> m_vars;
        bool m_calc;
};

#endif // ANALITZAWRAPPER_H
