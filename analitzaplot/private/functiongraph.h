#ifndef FUNCTIONGRAPH2_H
#define FUNCTIONGRAPH2_H

#include "mappinggraph.h"

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph(const QString &name, const QColor& col) : MappingGraph(name, col) {}

    //if evaluate true then result of expressiones will be strings of the value
    //if evaluate is false then the expressions will not evaluate
    virtual QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const = 0;
    virtual void setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max) = 0;

    //2 convenience methods to work with doubles instead of Expression->Cn->value ... see above
    virtual QPair<double, double> interval(const QString &argname) const = 0;
    virtual void setInterval(const QString &argname, double min, double max) = 0;

    virtual QStringList parameters() const = 0;
    
protected:
    FunctionGraph() {}
    FunctionGraph(const FunctionGraph &other) {}
};



#endif // FUNCTIONGRAPH2_H
