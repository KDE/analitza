#include "functiongraphsmodel.h"
#include "functiongraph.h"


#include "analitza/analyzer.h"
#include "analitza/variables.h"
#include "analitza/localize.h"

#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QIcon>

FunctionGraphModel::FunctionGraphModel(Analitza::Variables *v, QObject * parent)
    : QAbstractListModel(parent)
{
//     Q_ASSERT(v);

//     variablesModule = v;
}

FunctionGraphModel::~FunctionGraphModel()
{
    qDeleteAll(items);
    items.clear();
}

int FunctionGraphModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return 2;
}

Qt::ItemFlags FunctionGraphModel::flags(const QModelIndex & index) const
{
    if(index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsTristate;
    else
        return 0;
}

bool FunctionGraphModel::hasChildren(const QModelIndex & parent) const
{
    Q_UNUSED(parent);

    return false;
}

QVariant FunctionGraphModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;

    if(role==Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        switch(section) {
        case 0:
            ret=i18nc("@title:column", "Name");
            break;
        case 1:
            ret=i18nc("@title:column", "Function");
            break;
        }
    }
    return ret;
}


Qt::DropActions FunctionGraphModel::supportedDropActions() const
{
    return Qt::IgnoreAction;
}

QVariant FunctionGraphModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()>=items.count())
        return QVariant();

    int var=index.row();

    FunctionGraph *tmpcurve = items.at(var);

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            return tmpcurve->name();
            break;
        case 1:
            return tmpcurve->expression().toString();
            break;
        }
        break;
    case Qt::DecorationRole:
        if(index.column()==0) {
            QPixmap ico(15, 15);
            ico.fill(tmpcurve->color());
            return  QIcon(ico);
        } else {
            return QIcon::fromTheme(tmpcurve->iconName());
        }
        break;
    }

    return QVariant();
}

int FunctionGraphModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return items.size();
}

//agrego item al model y no como un puntero ... esto para manejar que el model maneje el scope del planecurve internamente
bool FunctionGraphModel::addItem(const Analitza::Expression& functionExpression, int spaceDimension, const QString& name, const QColor& col)
{

    //no se permiten items invalidos
    if (FunctionGraph::canDraw(functionExpression, spaceDimension))
    {
        beginInsertRows (QModelIndex(), items.count(), items.count());

        //TODO
        items.append(new FunctionGraph(functionExpression, /*variablesModule, */ spaceDimension,name, col));

        endInsertRows();
        
        return true;
    }
    
    return false;
    
}

bool FunctionGraphModel::addItem(const Analitza::Expression& functionExpression,int spaceDimension, const QString& name, const QColor& col, QStringList &errors)
{
    //no se permiten items invalidos
    if (FunctionGraph::canDraw(functionExpression, spaceDimension,errors))
    {
        beginInsertRows (QModelIndex(), items.count(), items.count());

        //TODO
        items.append(new FunctionGraph(functionExpression, /*variablesModule, */ spaceDimension, name, col));

        endInsertRows();
        
        return true;
    }
    
    return false;
}

void FunctionGraphModel::removeItem(int row)
{
    Q_ASSERT(row<items.size());

    beginRemoveRows(QModelIndex(), row, row);

    FunctionGraph *tmpcurve = items[row];
    delete tmpcurve;
        
    items.removeAt(row);

    endRemoveRows();
}

const FunctionGraph* FunctionGraphModel::item(int curveIndex) const
{
    Q_ASSERT(curveIndex<items.count());

    return items[curveIndex];
}

bool FunctionGraphModel::setItem(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col)
{
//                 if (FunctionGraph::canDraw(fexp))
//                 {
//                     items[index.row()]->reset(fexp);
// 
//                     emit dataChanged(index, index);
// 
//                     return true;
//                 }
return false;
}

void FunctionGraphModel::setItemParameterInterval(int curveIndex, const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    
}


void FunctionGraphModel::setItemParameterInterval(int curveIndex, const QString &argname, double min, double max)
{
    
}

