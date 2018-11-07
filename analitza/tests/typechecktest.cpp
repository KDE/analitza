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

#include "typechecktest.h"

#include <QTest>
#include <cmath>
#include "expression.h"
#include <expressiontypechecker.h>
#include <analyzer.h>

QTEST_MAIN( TypeCheckTest )

using namespace Analitza;

TypeCheckTest::TypeCheckTest(QObject* parent)
    : QObject(parent)
    , v(new Analitza::Variables)
{
    v->modify(QStringLiteral("fnum"), Expression(QStringLiteral("x->3")));
    v->modify(QStringLiteral("fplus"), Expression(QStringLiteral("x->x+x")));
    v->modify(QStringLiteral("tovector"), Expression(QStringLiteral("x->vector{x,x}")));
    v->modify(QStringLiteral("fnull"), Expression(QStringLiteral("x->x")));
    v->modify(QStringLiteral("number"), Expression(QStringLiteral("3")));
    v->modify(QStringLiteral("fwrong"), Expression(QStringLiteral("x->piecewise { 1>2 ? fwrong(x), ?fwrong(x+1) }")));
//     v->modify("frec", Expression("x->piecewise { 3>3? frec(x-1), ? 1}"));
    v->modify(QStringLiteral("frec"), Expression(QStringLiteral("x->piecewise { frec(cos(x))>3? 2, ? 1}")));
    v->modify(QStringLiteral("fact"), Expression(QStringLiteral("n->piecewise { n=1?1, ? n*fact(n-1) }")));
    v->modify(QStringLiteral("golambda"), Expression(QStringLiteral("(func, param)->func(param)")));
    v->modify(QStringLiteral("gonum"), Expression(QStringLiteral("func->func(1, 2)"))); // (num -> num -> a) -> a
    v->modify(QStringLiteral("valid"), Expression(QStringLiteral("a->true"))); // (num -> num -> a) -> a
    
    v->modify(QStringLiteral("fib"), Expression(QStringLiteral("n->piecewise { eq(n,0)?0, eq(n,1)?1, ?fib(n-1)+fib(n-2) }")));
    v->modify(QStringLiteral("fv"), Expression(QStringLiteral("vector{x->sin(x), x->cos(x)}")));
    v->modify(QStringLiteral("rtail"), Expression(QStringLiteral("(elems,i)->piecewise { card(elems)>=i ? union(list{elems[i]}, rtail(elems, i+1)), ? list{} }")));
    v->modify(QStringLiteral("tail"), Expression(QStringLiteral("elems->rtail(elems,1)")));
    v->modify(QStringLiteral("foldr"), Expression(QStringLiteral("(f,z,elems)->piecewise {card(elems)=0 ? z, ? f(elems[1], foldr(f, z, tail(elems))) }")));
    v->modify(QStringLiteral("foldl"), Expression(QStringLiteral("(f,z,xs)->piecewise{ card(xs)<=0? z, ? foldl(f, f(z, xs[1]), tail(xs)) }")));
    v->modify(QStringLiteral("twoargs"), Expression(QStringLiteral("(x,y)->sin(x)+y")));
}

TypeCheckTest::~TypeCheckTest()
{
    delete v;
}

void TypeCheckTest::testConstruction_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    
    QTest::newRow("addition") << "2+4" << "num";
    QTest::newRow("tovector") << "tovector" << "a -> <a,2>";
    QTest::newRow("vectorcall") << "tovector(3)" << "<num,2>";
    QTest::newRow("list+vector") << "list{tovector(3)}" << "[<num,2>]";
    QTest::newRow("additionvec") << "vector{2,2}+vector{2,2}" << "<num,2>";
    QTest::newRow("sum") << "sum(x : x=1..3)" << "num";
    QTest::newRow("sumvec") << "sum(vector{x,x} : x=1..3)" << "<num,2>";
    QTest::newRow("sumvec2") << "sum(x : x=vector{1,1}..vector{3,3})" << "<num,2>";
    QTest::newRow("vector") << "vector {2,2}" << "<num,2>";
    QTest::newRow("list") << "list {2,2}" << "[num]";
    QTest::newRow("direct") << "(x->x)(3)" << "num";
    QTest::newRow("lambda") << "fnum" << "a -> num";
    QTest::newRow("call") << "fnum(3)" << "num";
    QTest::newRow("plus") << "fplus" << "(num -> num) | (<num,-1> -> <num,-1>) | ({num,-1x-2} -> {num,-1x-2})";
    QTest::newRow("call plus") << "fplus(3)" << "num";
    QTest::newRow("call plus vect") << "fplus(vector{3})" << "<num,1>";
    QTest::newRow("vec to vec") << "tovector(vector{3})" << "<<num,1>,2>";
    QTest::newRow("piecewise") << "piecewise { number=3? 3, ?2 }" << "num";
    QTest::newRow("selector") << "selector(2, vector{3, 3})" << "num";
    QTest::newRow("selector1") << "selector(2, vector{vector{3}, vector{3}})" << "<num,1>";
    QTest::newRow("selectorlist") << "selector(2, list{3, 3})" << "num";
    
    QTest::newRow("rec") << "frec" << "num -> num";
    QTest::newRow("infinite") << "piecewise { 2=3? frec(3), ? 3}" << "num";
    QTest::newRow("infinite_1") << "piecewise { 2=3? 3, ? frec(3)}" << "num";
    QTest::newRow("fact") << "fact(4)" << "num";
    QTest::newRow("factdef") << "fact" << "num -> num";
    QTest::newRow("deriv") << "(diff(x:x))(2)" << "num";
    QTest::newRow("derivdiff") << "diff(x:x)" << "num -> num";
    
    QTest::newRow("infer_unary") << "n->cos(n)" << "num -> num";
    QTest::newRow("infer") << "n->n=1" << "num -> bool";
    QTest::newRow("siplambda") << "func->func(2)" << "(num -> a) -> a";
    QTest::newRow("numlambda") << "(func, x)->cos(func(x))" << "(a -> num) -> a -> num";
    QTest::newRow("golambda")  << "golambda" << "(a -> b) -> a -> b";
    QTest::newRow("golambda1") << "golambda(x->x, 1)" << "num";
    QTest::newRow("golambda2") << "golambda(x->x, vector{1})" << "<num,1>";
    QTest::newRow("golambda3") << "golambda(x->list{x}, 1)" << "[num]";
    QTest::newRow("golambda4") << "golambda(x->list{x}, vector{1})" << "[<num,1>]";
    
    QTest::newRow("fib") << "fib" << "num -> num";
    QTest::newRow("equation") << "x+cos(x)" << "num";
//     QTest::newRow("long") << "(c, c1, c2, t1, t2)->(t2-t1)/(c2-c1)a(c-c1)+t1" << "num -> num -> num -> num -> num -> num";
    QTest::newRow("selec_call") << "(selector(1, fv))(1)" << "num";
    QTest::newRow("selec") << "selector(1, fv)" << "num -> num";
    
    QTest::newRow("piece") << "x->piecewise { gt(x,0) ? x, ? x+1 }" << "num -> num";
    QTest::newRow("parametric") << "t->vector{t,t^2}" << "(num -> <num,2>) | ({num,-1x-1} -> <{num,-1x-1},2>)";
    QTest::newRow("somelist") << "t->list{t,t^2}" << "(num -> [num]) | ({num,-1x-1} -> [{num,-1x-1}])";
    QTest::newRow("x->piece") << "x->piecewise { gt(x,0) ? selector(1, vector{x, 1/x}),"
                                    "? selector(2, vector{x, 1/x} ) }" << "num -> num";
    QTest::newRow("div") << "v->selector(1, v)/selector(2, v)" << "(<num,-1> -> num) | (<<num,-1>,-1> -> <num,-1>) | ([num] -> num) | ([<num,-1>] -> <num,-1>) | ({num,-2x-1} -> <num,-1>)";
    
    QTest::newRow("selec_cos") << "v->cos(selector(1, v))" << "(<num,-1> -> num) | ([num] -> num)";
    QTest::newRow("shadowed_param") << "fv->cos(fv)" << "num -> num";
    QTest::newRow("eq") << "x->eq(1,x)" << "num -> bool";
    QTest::newRow("list@sum") << "v->sum(i^2 : i@v)" << "([num] -> num) | (<num,-1> -> num) | ({num,-3x-2} -> num) | ([{num,-1x-1}] -> {num,-1x-1}) | (<{num,-1x-1},-1> -> {num,-1x-1}) | ({{num,-1x-1},-3x-2} -> {num,-1x-1})";
    
    QTest::newRow("bounded sum_up") << "n->sum(x : x=n..0)" << "num -> num";
    QTest::newRow("bounded sum_down") << "n->sum(x : x=0..n)" << "num -> num";
    QTest::newRow("exists") << "exists(l : l@list{true,false,false})" << "bool";
    QTest::newRow("existslambda") << "x->sum(l : l@list{true,x,false})" << "bool -> bool";
    
    QTest::newRow("unknown") << "w" << "a";
    
    QTest::newRow("gonum") << "gonum((x,y)->x*y)" << "num";
    
    QTest::newRow("selec_cond") << "xs->piecewise {card(xs)=0? xs}" << "([a] -> [a]) | (<a,-1> -> <a,-1>)";
    QTest::newRow("justlambda") << "(f,e)->f(f(e))" << "(a -> a) -> a -> a";
    
    QTest::newRow("tail") << "ptail:=(elems,i)->piecewise { card(elems)>=i ? union(list{elems[i]}, ptail(elems, i+1)), ? list{} }"
                        << "(<a,-1> -> num -> [a]) | ([a] -> num -> [a])";
    QTest::newRow("tail1") << "(elems,i)->list{elems[i]}" << "(<a,-1> -> num -> [a]) | ([a] -> num -> [a]) | ({a,-2x-1} -> num -> [<a,-1>])";
    QTest::newRow("tailp") << "(elems,i)->piecewise{ card(elems)>=i ? list{elems[i]}, ? list{}}" << "(<a,-1> -> num -> [a]) | ([a] -> num -> [a])";
    QTest::newRow("tail3") << "(elems,i)->union(list{elems[i]}, list{})" << "(<a,-1> -> num -> [a]) | ([a] -> num -> [a]) | ({b,-2x-1} -> num -> [<b,-1>])";
    QTest::newRow("tail4") << "ptail:=(elems,i)->union(list{elems[i]}, ptail(elems, i))" << "(<a,-1> -> num -> [a]) | ([a] -> num -> [a]) | ({b,-2x-1} -> num -> [<b,-1>])";
//     QTest::newRow("tail5") << "(elems,i)->list{list{elems[i]}, ptail(elems, i)}" << "(<a,-1> -> num -> [a]) | ([a] -> num -> [a])";
    QTest::newRow("tail6") << "tail" << "(<a,-1> -> [a]) | ([a] -> [a])";
    
    QTest::newRow("pe") << "vector{x->x, x->x+2}" << "<(num -> num),2>";
    QTest::newRow("ana") << "f:=o->vector { x->x+o, x->x*o }" << "(num -> <(num -> num),2>) | (<num,-1> -> <(<num,-1> -> <num,-1>),2>) | ({num,-1x-2} -> <({num,-1x-2} -> {num,-1x-2}),2>)";
    QTest::newRow("pp") << "(x,f)->list{ f(x), cos(x) }" << "num -> (num -> num) -> [num]";
    QTest::newRow("pp1") << "(x,f)->list{ cos(x), f(x) }" << "num -> (num -> num) -> [num]";
    
    QTest::newRow("foldl") << "foldl"
                        << "((a -> b -> a) -> a -> <b,-1> -> a) | ((a -> b -> a) -> a -> [b] -> a)";
//     QTest::newRow("foldl1") << "foldl1:=(f,z,xs)->piecewise{ card(xs)>0? foldl1(f, f(z, xs[1]), tail(xs)), ? z }"
//                         << "((a -> b -> a) -> a -> <b,-1> -> a) | ((a -> b -> a) -> a -> [b] -> a)";
    QTest::newRow("foldl2") << "foldl2:=(f,z,xs)->piecewise{ card(xs)>0? foldl2(f, f(z, xs[1]), tail(xs)), ? cos(z) }"
                        << "((num -> a -> num) -> num -> <a,-1> -> num) | ((num -> a -> num) -> num -> [a] -> num)";
    QTest::newRow("foldl3") << "foldl3:=(f,z,xs)->foldl3(f, f(z, xs[1]), tail(xs))" << "((a -> b -> a) -> a -> <b,-1> -> c) | ((a -> b -> a) -> a -> [b] -> c)";
    
    QTest::newRow("foldr") << "(f,z,elems)->piecewise {card(elems)=0 ? z, ? f(elems[1], foldr(f, z, tail(elems))) }"
                                << "((a -> b -> b) -> b -> <a,-1> -> b) | ((a -> b -> b) -> b -> [a] -> b)";
    QTest::newRow("foldr_call") << "foldr"
                                << "((a -> b -> b) -> b -> <a,-1> -> b) | ((a -> b -> b) -> b -> [a] -> b)";
    QTest::newRow("foldr1") << "foldr1:=(f,elems)->piecewise {card(elems)=1 ? elems[1], ? f(elems[1], foldr1(f, elems)) }"
                                << "((a -> a -> a) -> <a,-1> -> a) | ((a -> a -> a) -> [a] -> a)";
    QTest::newRow("foldr11") << "(f,elems)->f(elems[1], f(elems[2], elems[3]))" << "((a -> a -> a) -> <a,-1> -> a) | ((a -> a -> a) -> [a] -> a) | ((<b,-1> -> <b,-1> -> <b,-1>) -> {b,-2x-1} -> a)";
    QTest::newRow("foldr2") << "(f,elems)->f(elems[1], elems[2])" << "((a -> a -> b) -> <a,-1> -> b) | ((a -> a -> b) -> [a] -> b) | ((<a,-1> -> <a,-1> -> b) -> {a,-2x-1} -> b)";
    QTest::newRow("foldr3") << "(f,elems)->f(elems[1])" << "((a -> b) -> <a,-1> -> b) | ((a -> b) -> [a] -> b) | ((<a,-1> -> b) -> {a,-2x-1} -> b)";
    QTest::newRow("foldr4") << "(f,elems)->f(cos(elems[1]))" << "((num -> a) -> <num,-1> -> a) | ((num -> a) -> [num] -> a)";
    QTest::newRow("foldr5") << "(f,elems)->f(f(elems[1]))" << "((a -> a) -> <a,-1> -> a) | ((a -> a) -> [a] -> a) | ((<b,-1> -> <b,-1>) -> {b,-2x-1} -> a)";
    QTest::newRow("foldr6") << "(f,elems)->piecewise {card(elems)=1 ? cos(elems[1]), ? f(elems) }"
                                << "((<num,-1> -> num) -> <num,-1> -> num) | (([num] -> num) -> [num] -> num)";
    
    QTest::newRow("filter")  << "(condition,elems)->foldr((v,pred)->piecewise{ condition(v) ? union(list{v}, pred), ? pred }, list{}, elems)"
                                << "((a -> bool) -> <a,-1> -> [a]) | ((a -> bool) -> [a] -> [a])";
    
    QTest::newRow("arg") << "c->arctan(c[2]/c[1])" << "(<num,-1> -> num) | ([num] -> num)";
    QTest::newRow("rect") << "v->v[1]*cos(v[2])" << "(<num,-1> -> num) | ([num] -> num)";
    QTest::newRow("crash") << "u->(v->vector { v[2] })((v->vector { v[2] })(u))" << "(<a,1> -> <a,1>) | ([a] -> <a,1>) | ({a,-2x1} -> <<a,1>,1>)";
    
    QTest::newRow("scopes") << "or((x->list{}=x)(list{}), (x->x=0)(0))" << "bool";
    
    QTest::newRow("unify") << "t->vector { 1, times(2, 3, t) }" << "num -> <num,2>";
    
    QTest::newRow("nary") << "l->and(l=3, l=3, l=3)" << "num -> bool";
    QTest::newRow("nary1") << "l->and(valid(l), l=3, l=3)" << "num -> bool";
    QTest::newRow("nary2") << "l->and(l=3, l=3, valid(l))" << "num -> bool";
    
    QTest::newRow("matrix") << "matrix { matrixrow { 1 } }" << "{num,1x1}";
    QTest::newRow("matrix2r") << "matrix { matrixrow { 1 }, matrixrow { 2 } }" << "{num,2x1}";
    QTest::newRow("matrix2c") << "matrix { matrixrow { 1, 2 } }" << "{num,1x2}";
    QTest::newRow("matrix+") << "matrix { matrixrow { 1 }, matrixrow { 2 } }+matrix { matrixrow { 1 }, matrixrow { 2 } }" << "{num,2x1}";
    QTest::newRow("matrix_selector0") << "selector(1, matrix { matrixrow { 1 }, matrixrow { 2 } })" << "<num,1>";
    QTest::newRow("matrix_selector1") << "selector(1, selector(1, matrix { matrixrow { 1 }, matrixrow { 2 } }))" << "num";
    QTest::newRow("matrix_transpose") << "transpose( matrix { matrixrow { 1, 2 } })" << "{num,2x1}";
}

void TypeCheckTest::testConstruction()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    
    Expression e(input);
    if(!e.isCorrect()) qDebug() << "errors: " << e.error();
    QVERIFY(e.isCorrect());
    ExpressionTypeChecker t(v);
    
    ExpressionType type=t.check(e);
    if(!t.isCorrect()) qDebug() << "errors: " << t.errors();
    
    QVERIFY(t.isCorrect());
    QCOMPARE(type.simplifyStars().toString(), output);
}

void TypeCheckTest::testUncorrection()
{
    QFETCH(QString, input);
    
    Expression e(input);
    if(!e.isCorrect())
        qDebug() << "wrong exp:" << e.error();
    QVERIFY(e.isCorrect());
    ExpressionTypeChecker t(v);
    
    ExpressionType result=t.check(e);
    
    if(t.isCorrect())
        qDebug() << "wrong type:" << result.toString();
    QVERIFY(!t.isCorrect());
//     QVERIFY(!t.errors().isEmpty());
}

void TypeCheckTest::testUncorrection_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    
    QTest::newRow("consistency vector") << "vector{2, list{2}}";
    QTest::newRow("consistency list")   << "list{2, list{2}}";
    
    QTest::newRow("piecewise result") << "piecewise { 2=3? 3, ?vector{2} }";
    QTest::newRow("piecewise condit") << "piecewise { vector{3}? 3, ?2 }";
    
    QTest::newRow("no operator") << "list { 2,2 }+list{2}";
    QTest::newRow("diff vectors") << "vector { 2,2 }+vector { 2 }";
    QTest::newRow("diff types") << "list { 2,2 }+vector { 2 }";
    QTest::newRow("wrong call") << "(x->x+x)(list{3})";
    QTest::newRow("wrong call2") << "fplus(list{3})";
    QTest::newRow("wrong call3") << "(x(x^2))^2";
    QTest::newRow("wrong bounds") << "sum(x : x=1..vector{3,3})";
    QTest::newRow("bounds2") << "sum(x:x@2)";
    QTest::newRow("number call") << "number(3)";
    QTest::newRow("wrong param count") << "golambda(2)";
    QTest::newRow("unresolved operation") << "selector(2,2)";
    
    QTest::newRow("charvsreal") << "union(\"lalala\", list{1,2,3})";
    QTest::newRow("boolvsreal") << "or(true, false)+2";
    QTest::newRow("argscount") << "f:=(x,y)->f(x)";
    
    QTest::newRow("twoargs") << "(x->x(3))((x,y)->x+y)";
    QTest::newRow("times") << "x(x+1)";
    QTest::newRow("unify") << "min(2, 3, list{})";
    
    //TODO: Add invalid recursive call
}

Q_DECLARE_METATYPE(ExpressionType)

void TypeCheckTest::testReduction()
{
    QFETCH(ExpressionType, type);
    QFETCH(ExpressionType, reduced);
    QFETCH(bool, correct);
    
    QCOMPARE(type.canReduceTo(reduced), correct);
}

ExpressionType s(int stars) { return ExpressionType(ExpressionType::Any, stars); }
ExpressionType vec(const ExpressionType& c, int size) { return ExpressionType(ExpressionType::Vector, c, size); }
ExpressionType list(const ExpressionType& c) { return ExpressionType(ExpressionType::List, c); }
ExpressionType val=ExpressionType(ExpressionType::Value);

void TypeCheckTest::testReduction_data()
{
    QTest::addColumn<ExpressionType>("type");
    QTest::addColumn<ExpressionType>("reduced");
    QTest::addColumn<bool>("correct");
    
    ExpressionType lambdaStarStar(ExpressionType::Lambda); // a -> a
    lambdaStarStar.addParameter(s(1)).addParameter(s(1));
    
    ExpressionType lambdaNumVector2(ExpressionType::Lambda); // num -> <num, 2>
    lambdaNumVector2.addParameter(val).addParameter(vec(val, 2));
    
    QTest::newRow("sss") << lambdaStarStar << lambdaNumVector2 << false;
}

class FooCommand : public Analitza::FunctionDefinition
{
    virtual Expression operator()(const QList< Expression >&) override
    {
        return Expression(QStringLiteral("matrix { matrixrow { 8, 9, 0},matrixrow { 7, 5, 3},matrixrow { 1, 2, 10}}"));
    }
};

void TypeCheckTest::testVariadic()
{
    QFETCH(QString, expression);
    QFETCH(ExpressionType, type);
    
    Analyzer a(v);
    a.builtinMethods()->insertFunction(QStringLiteral("foocommand"), type, new FooCommand);
    
    a.setExpression(Expression(expression));
    
    if (a.type() != type.returnValue())
        qDebug() << a.type().toString() << type.toString();
    QCOMPARE(a.type(), type.returnValue());
    
    if(!a.isCorrect())
        qDebug() << "wrong exp:" << a.errors();
    QVERIFY(a.isCorrect());
    
    Expression result = a.calculate();
    
    if(!a.isCorrect() || !result.isCorrect())
        qDebug() << "errors in calc:" << a.errors() << result.error();
    QVERIFY(result.isCorrect());
}

void TypeCheckTest::testVariadic_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<ExpressionType>("type");
    
    ExpressionType vtype = ExpressionType(ExpressionType::Lambda)
    .addParameter(ExpressionType(ExpressionType::Any))
    .addParameter(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
    
    QTest::newRow("f1type_alt_1param_ok") << "foocommand(3)" << vtype;
    QTest::newRow("f2type_alt_2params_ok") << "foocommand(3,44)" << vtype;
    QTest::newRow("f3type_alt_3params_ok") << "foocommand(list{5,6,2},44,vector{3,5})" << vtype;
}
