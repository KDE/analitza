// #ifndef SPACECURVE_H
// #define SPACECURVE_H
// 
// #include "private/curve.h"
// 
// class ANALITZAPLOT_EXPORT SpaceCurve : public Curve 
// {
//     //3D
// //torsion,   
//     //curvature, length of arc, curvature
//     
// public:
//     SpaceCurve(const Analitza::Expression &functionExpression, const QString &name, const QColor& col);
//     SpaceCurve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
//     virtual ~SpaceCurve();
// 
//     static bool canDraw(const Analitza::Expression &functionExpression);
//     //with stringlist is used in model for add a item ... de otra manera se crearia una instancia solo para verrificar que sea valido
//     static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);
// 
//     bool reset(const Analitza::Expression &functionExpression);
//     void setVariables(Analitza::Variables *variables);
// 
//     //MappingGraph
//     const QString typeName() const;
//     const Analitza::Expression &expression() const;
//     QString iconName() const;
//     QStringList examples() const;
//     int spaceDimension() const;
//     CoordinateSystem coordinateSystem() const;
//     QStringList errors() const;
//     bool isCorrect() const;
// 
//     //FunctionGraph
//     
//     QPair<Analitza::Expression, Analitza::Expression> interval(const QString &argname, bool evaluate) const;
//     bool setInterval(const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max);
//     
//     QPair<double, double> interval(const QString &argname) const;
//     bool setInterval(const QString &argname, double min, double max);
//     
//     QStringList parameters() const;
// 
//     //Curve
//     QVector<int> jumps() const;
// 
//     //Own 
//     const QVector<QVector3D> & points() const;
//     void update(const Box& viewport);
// 
// protected:
//     SpaceCurve() {}
//     SpaceCurve(const SpaceCurve &other) {}
// 
// private:
//     Analitza::Analyzer *analyzer;
//     Analitza::Variables *m_varsModule;
// 
//     QStringList m_errors;
// };
// 
// #endif // SPACECURVE_H
