#ifndef FUNCTIONGRAPHMODEL_H
#define FUNCTIONGRAPHMODEL_H


#include <QAbstractListModel>

#include "analitzaplot/analitzaplotexport.h"


class FunctionGraph;

namespace Analitza
{
class Variables;
class Expression;
}

class ANALITZAPLOT_EXPORT FunctionGraphModel : public QAbstractListModel
{
Q_OBJECT
    
public:
    FunctionGraphModel(QObject * parent = 0);
    FunctionGraphModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~FunctionGraphModel();
    
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    bool addItem(const Analitza::Expression &functionExpression,int spaceDimension, const QString &name, const QColor& col);
    bool addItem(const Analitza::Expression &functionExpression,int spaceDimension, const QString &name, const QColor& col, QStringList &errors);
    void removeItem(int curveIndex);

    //planecurve getters
    const FunctionGraph * item(int curveIndex) const; //read only pointer the data CAN NOT be changed (is a good thing :) )... use this instead of roles ... razon: el uso de roles hace que el cliente deba hacer casts largos

    //planecurve setters and calculation/evaluation methods  .. don't forget to emit setdata signal' ... ninguno de estos metodos tiene cont al final
    bool setItem(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    void setItemParameterInterval(int curveIndex, const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
    void setItemParameterInterval(int curveIndex, const QString &argname, double min, double max);

protected:    
    QList<FunctionGraph*> items;
    
private:
    Analitza::Variables *m_variables;
};

#endif // FUNCTIONGRAPHMODEL_H
