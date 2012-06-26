#ifndef FUNCTIONGRAPHMODEL_H
#define FUNCTIONGRAPHMODEL_H


#include <QAbstractListModel>

#include "analitzaplot/analitzaplotexport.h"


namespace Analitza
{
class Variables;
}

class ANALITZAPLOT_EXPORT FunctionGraphModel : public QAbstractListModel
{
Q_OBJECT
    
public:
    FunctionGraphModel(Analitza::Variables *v, QObject * parent = 0);
    virtual ~FunctionGraphModel();
    
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;

protected:
    Analitza::Variables *variablesModule;
};

#endif // FUNCTIONGRAPHMODEL_H
