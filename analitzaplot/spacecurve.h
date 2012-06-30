#ifndef SPACECURVE_H
#define SPACECURVE_H

#include "private/functiongraph.h"

class ANALITZAPLOT_EXPORT SpaceCurve : public FunctionGraph
{
    //3D
//torsion,   
    //curvature, length of arc, curvature
    
public:
    SpaceCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
    SpaceCurve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~SpaceCurve();

    static bool canDraw(const Analitza::Expression &functionExpression);
    static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);

    bool reset(const Analitza::Expression& functionExpression);

    //Curve
    QVector<int> jumps() const;

    //Own 
    const QVector<QVector3D> & points() const;
    void update(const Box& viewport);

protected:
    SpaceCurve() {}
    SpaceCurve(const SpaceCurve &other) {}
};

#endif // SPACECURVE_H
