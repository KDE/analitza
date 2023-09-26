// SPDX-FileCopyrightText: 2010 Aleix Pol <aleixpol@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef ANALITZAWRAPPER_H
#define ANALITZAWRAPPER_H

#include <QScopedPointer>
#include <QObject>
#include <QVariant>
#include <QtQml/qqmlregistration.h>
#include <QtQml/qqmlengine.h>

#include <analitza/expression.h>
#include <analitza/variables.h>
#include <analitzaplot/plotsmodel.h>
#include <analitzagui/variablesmodel.h>
#include <analitzagui/operatorsmodel.h>
#include <analitza/variables.h>

namespace Analitza {
    class Analyzer;
}

class ExpressionWrapper : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Expression)
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
    QML_NAMED_ELEMENT(Analitza)
    Q_PROPERTY(bool calculate READ isCalculate WRITE setCalculate NOTIFY isCalculateChanged)
    Q_PROPERTY(bool isCorrect READ isCorrect)
    Q_PROPERTY(QStringList errors READ errors)
    Q_PROPERTY(QSharedPointer<Analitza::Variables> variables READ variables WRITE setVariables)
    public:
        explicit AnalitzaWrapper(QObject* parent = nullptr);
        ~AnalitzaWrapper() override;
        
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

class VariablesModelForeign
{
    Q_GADGET
    QML_FOREIGN(Analitza::VariablesModel)
    QML_NAMED_ELEMENT(VariablesModel)
};

class OperatorsModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(OperatorsModel)
    QML_FOREIGN(OperatorsModel)
};

class PlotsModelForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PlotsModel)
    QML_FOREIGN(Analitza::PlotsModel)
};

#endif // ANALITZAWRAPPER_H
