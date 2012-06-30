#ifndef FUNCTIONGRAPH2_H
#define FUNCTIONGRAPH2_H

#include "mappinggraph.h"

class AbstractFunctionGraph;

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph(const Analitza::Expression &functionExpression, int spaceDimension, const QString &name, const QColor& col);
    FunctionGraph(const Analitza::Expression &functionExpression, Analitza::Variables *variables, int spaceDimension, const QString &name, const QColor& col);
    virtual ~FunctionGraph();

    static bool canDraw(const Analitza::Expression &functionExpression, int spaceDimension);
    //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
    static bool canDraw(const Analitza::Expression &functionExpression, int spaceDimension, QStringList &errors);

    bool reset(const Analitza::Expression &functionExpression, int spaceDimension);
    
    Analitza::Variables *variables() const;
    void setVariables(Analitza::Variables *variables);

    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    QStringList errors() const;
    bool isCorrect() const;
    
    //if evaluate true then result of expressiones will be strings of the value
    //if evaluate is false then the expressions will not evaluate
    QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
    bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);

    //2 convenience methods to work with doubles instead of Expression->Cn->value ... see above
    QPair<double, double> interval(const QString &argname) const;
    bool setInterval(const QString &argname, double min, double max);

    QStringList parameters() const;
    
protected:
    FunctionGraph() {}
    FunctionGraph(const FunctionGraph &other) {}
    
    AbstractFunctionGraph *backend() const { return m_functionGraph; }

private:
    AbstractFunctionGraph *m_functionGraph;

    QStringList m_errors;
};



#endif // FUNCTIONGRAPH2_H
