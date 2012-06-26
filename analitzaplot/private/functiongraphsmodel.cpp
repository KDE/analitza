#include "functiongraphsmodel.h"


#include "analitza/analyzer.h"
#include "analitza/variables.h"
#include "analitza/localize.h"

FunctionGraphModel::FunctionGraphModel(Analitza::Variables *v, QObject * parent)
    : QAbstractListModel(parent)
{
    Q_ASSERT(v);

    variablesModule = v;
}

FunctionGraphModel::~FunctionGraphModel()
{

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
