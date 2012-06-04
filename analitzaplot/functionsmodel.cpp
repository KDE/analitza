/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#include "functionsmodel.h"

#include <KLocale>
#include <KApplication>
#include <KDebug>
#include <QPixmap>
#include <QFont>
#include <cmath>

#include "functiongraph.h"

#include "analitza/expression.h"


FunctionsModel::FunctionsModel(QObject *parent)
    : QAbstractTableModel(parent), m_selectedRow(-1), m_resolution(500), m_fcount(1), animActiva(3), modAnActivo(false), dirAnim(1),
      mArea(false), mFuncion1("-"), mFuncion2("-"), pFuncion1(-1), pFuncion2(-1), mAreaE(false), limiteI(-1000),
      limiteS(1000)
{
}

QVariant FunctionsModel::data(const QModelIndex & index, int role) const
{
    int var=index.row();

    if (!index.isValid() || index.row() >= funclist.count())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case 0:
            return funclist.at(index.row()).name();
            break;

        case 1:
            return funclist.at(index.row()).expression().toString();
            break;

        case 2:
            if (funclist.at(index.row()).dimension() == 2)
                return i18n("2D");
            else if (funclist.at(index.row()).dimension() == 3)
                return i18n("3D");
            break;

        case 3:
            return funclist.at(index.row()).dateTime().toString();
            break;
        }
    }
    break;

    case Qt::DecorationRole:
    {
        if( index.column() ==0 )
        {
            QPixmap icon(15, 15);
            icon.fill(funclist.at(index.row()).color());

            return icon;
        }
    }
    break;

    case Qt::FontRole:
        if (var==m_selectedRow)
        {
            QFont f(qApp->font());
            f.setBold(true);
            return f;
        }
        break;

    case Selection:
        return m_selectedRow;
        break;

    case Shown:
        return funclist.at(index.row()).isShown();
        break;

    }

    return QVariant();
}

QVariant FunctionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    if(role==Qt::DisplayRole && orientation==Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return i18nc("@title:column", "Nombre");
            break;

        case 1:
            return i18nc("@title:column", "Ecuación");
            break;

        case 2:
            return i18nc("@title:column", "Dimensión");
            break;

        case 3:
            return i18nc("@title:column", "Fecha");
            break;
        }
    }
    return ret;
}

int FunctionsModel::rowCount(const QModelIndex &idx) const
{
    if(idx.isValid())
        return 0;
    else
        return funclist.count();
}

bool FunctionsModel::addFunction(Function& func,int index,QList<double> cons,int oct,int axi,bool solid,bool curva,bool xy,double pres)
{
    beginInsertRows (QModelIndex(), rowCount(), rowCount());
    funclist.append(func);
    m_selectedRow=funclist.count()-1;
    endInsertRows();
    sendStatus(i18n("%1 function added", func.name()));

    emit functionImplicitCall(func.id(),func.color(),index,cons,oct,axi,solid,curva,xy,pres);

    return true;
}

bool FunctionsModel::addFunction(const Function& func)
{
    {

        beginInsertRows (QModelIndex(), rowCount(), rowCount());
        funclist.append(func);

        m_selectedRow=funclist.count()-1;
        endInsertRows();
        sendStatus(i18n("%1 function added", func.name()));


        emit functionModified(funclist[m_selectedRow]);


    }

    return true;
}

double FunctionsModel::getLimiteS()
{
    return limiteS;
}

double FunctionsModel::getLimiteI()
{
    return limiteI;
}

void FunctionsModel::setLimiteS(double ls)
{
    limiteS = ls;
}

void FunctionsModel::setLimiteI(double li)
{
    limiteI = li;
}

bool FunctionsModel::removeRows(int row, int count, const QModelIndex & parent)
{

    if(parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count-1);

    if(m_selectedRow>=row)
        m_selectedRow-=count;

    FunctionList::iterator it=funclist.begin()+row;
    for(int i=count-1; i>=0; i--)
    {
        QString name=it->name();
        QUuid id(it->id());
        it=funclist.erase(it);
        emit functionRemoved(id, name);
    }
    endRemoveRows();

    return true;
}

void FunctionsModel::setSelected(const QModelIndex & indexSel)
{
    int previous=m_selectedRow;
    m_selectedRow=indexSel.row();
    if(previous!=m_selectedRow)
    {
        QModelIndex idx=index(m_selectedRow, 0), idxEnd=index(m_selectedRow, columnCount()-1);
        emit dataChanged(idx, idxEnd);

        idx=index(previous, 0), idxEnd=index(previous, columnCount()-1);
        emit dataChanged(idx, idxEnd);
    }
}

bool FunctionsModel::setSelected(const QUuid& fid)
{
    int i=0;
    int previous=m_selectedRow;
    bool found=false;
    foreach(const Function& f, funclist)
    {
        if(f.id() == fid)
        {
            m_selectedRow=i;
            found=true;
        }
        i++;
    }

    if(found && previous!=m_selectedRow)
    {
        QModelIndex idx=index(m_selectedRow, 0), idxEnd=index(m_selectedRow, columnCount()-1);
        emit dataChanged(idx, idxEnd);

        idx=index(previous, 0);
        idxEnd=index(previous, columnCount()-1);
        emit dataChanged(idx, idxEnd);
    }
    return found;
}

void FunctionsModel::clear()
{
    if(!funclist.isEmpty())
    {
        beginRemoveRows (QModelIndex(), 0, rowCount());
        funclist.clear();
        endRemoveRows ();
        reset();
    }
}

bool FunctionsModel::setShown(const QString& f, bool shown)
{
    for (FunctionList::iterator it = funclist.begin(); it != funclist.end(); ++it )
    {
        if(it->name() == f)
        {
            it->setShown(shown);
            return true;
        }
    }
    return false;
}

Function* FunctionsModel::editFunction(int num)
{
    return &funclist[num];
}

void FunctionsModel::editFunction(int num, const Function& func)
{

    funclist[num]=func;

    QModelIndex idx=index(num, 0), idxEnd=index(num, columnCount()-1);
    emit dataChanged(idx, idxEnd);

    emit functionModified(funclist[num]);
}

void FunctionsModel::setAnimActiva(int flag)
{
    animActiva = flag;
}

int FunctionsModel::getAnimActiva()
{
    return animActiva;
}

void FunctionsModel::setModAnActivo(bool flag)
{
    modAnActivo = flag;
}

bool FunctionsModel::getModAnActivo()
{
    return modAnActivo;
}

void FunctionsModel::setDirAnim(int flag)
{
    dirAnim = flag;
}

int FunctionsModel::getDirAnim()
{
    return dirAnim;
}

void FunctionsModel::setMArea(bool flag)
{
    mArea = flag;
}

bool FunctionsModel::getMArea()
{
    return mArea;
}

void FunctionsModel::setMAreaE(bool flag)
{
    mAreaE = flag;
}

bool FunctionsModel::getMAreaE()
{
    return mAreaE;
}

void FunctionsModel::setMFuncion1(QString func1)
{
    mFuncion1 = func1;
}

void FunctionsModel::setMFuncion2(QString func2)
{
    mFuncion2 = func2;
}

QString FunctionsModel::getMFuncion1()
{
    return mFuncion1;
}

QString FunctionsModel::getMFuncion2()
{
    return mFuncion2;
}

void FunctionsModel::setPFuncion1(int pf1)
{
    pFuncion1 = pf1;
}

void FunctionsModel::setPFuncion2(int pf2)
{
    pFuncion2 = pf2;
}

int FunctionsModel::getPFuncion1()
{
    return pFuncion1;
}

int FunctionsModel::getPFuncion2()
{
    return pFuncion2;
}

QVector<QLineF> FunctionsModel::getPathsArea()
{
    return m_pathsArea;
}

void FunctionsModel::setPathsArea(QVector<QLineF> &areaLines)
{
    m_pathsArea = areaLines;
}

void FunctionsModel::setPathsCre(QVector<QLineF> &linesCre)
{
    m_pathsCre = linesCre;
}

QVector<QLineF> FunctionsModel::getPathsCre()
{
    return m_pathsCre;
}

void FunctionsModel::setPathsDec(QVector<QLineF> &linesDec)
{
    m_pathsDec = linesDec;
}

QVector<QLineF> FunctionsModel::getPathsDec()
{
    return m_pathsDec;
}

void FunctionsModel::setPathsEnt(QVector<QLineF> &linesEnt)
{
    m_pathsEnt = linesEnt;
}

QVector<QLineF> FunctionsModel::getPathsEnt()
{
    return m_pathsEnt;
}

void FunctionsModel::setPathsEnt2(QVector<QLineF> &linesEnt)
{
    m_pathsEnt2 = linesEnt;
}

QVector<QLineF> FunctionsModel::getPathsEnt2()
{
    return m_pathsEnt2;
}

void FunctionsModel::setPathsACre(QVector<QLineF> &linesACre)
{
    m_pathsACre = linesACre;
}

QVector<QLineF> FunctionsModel::getPathsACre()
{
    return m_pathsACre;
}

void FunctionsModel::setPathsADec(QVector<QLineF> &linesADec)
{
    m_pathsADec = linesADec;
}

QVector<QLineF> FunctionsModel::getPathsADec()
{
    return m_pathsADec;
}

void FunctionsModel::setPathsAEnt(QVector<QLineF> &linesAEnt)
{
    m_pathsAEnt = linesAEnt;
}

QVector<QLineF> FunctionsModel::getPathsAEnt()
{
    return m_pathsAEnt;
}

void FunctionsModel::setIntPoints(QVector<QPointF> &puntosInt)
{
    m_pointsInt = puntosInt;
}


void FunctionsModel::setAreaEjemplo(int ej)
{
    areaEjemplo = ej;
}



int FunctionsModel::getAreaEjemplo()
{
    return areaEjemplo;
}

void FunctionsModel::setCantDiv(int div)
{
    cantDiv = div;
}

int FunctionsModel::getCantDiv()
{
    return cantDiv;
}

QVector<QPointF> FunctionsModel::getIntPoints()
{
    return m_pointsInt;
}




bool FunctionsModel::editFunction(const QUuid& toChange, const Function& func)
{
    bool exist=false;

    int i=0;
    for (FunctionList::iterator it = funclist.begin(); !exist && it != funclist.end(); ++it, ++i )
    {
        if(it->id() == toChange)
        {
            exist=true;
            *it = func;






            it->setName(func.name());


            it->setSpaceId(func.spaceId());
            it->setDrawingType(func.drawingType());
            it->setColor(func.color());
            it->setResolution(func.resolution());


            QModelIndex idx=index(i, 0), idxEnd=index(i, columnCount()-1);
            emit dataChanged(idx, idxEnd);

            emit functionModified(funclist[i]);

            break;
        }
    }

    return exist;
}

void FunctionsModel::updateSpaceId(const QUuid& functionId, const QUuid& spaceId)
{
    int i=0;
    for (FunctionList::iterator it = funclist.begin(); it != funclist.end(); ++it, ++i )
    {


        if(it->id() == functionId)
        {
            it->setSpaceId(spaceId);

            QModelIndex idx=index(i, 0), idxEnd=index(i, columnCount()-1);
            emit dataChanged(idx, idxEnd);
        }
    }
}

bool FunctionsModel::setData(const QModelIndex & idx, const QVariant &value, int role)
{
    if(role==Selection) setSelected(idx);
    else if(role==Shown)
    {
        bool isshown=value.toBool();
        funclist[idx.row()].setShown(isshown);

        QModelIndex idx1=index(idx.row(), 0), idxEnd=index(idx.row(), columnCount()-1);

        emit dataChanged(idx1, idxEnd);

        return true;
    }
    return false;
}

void FunctionsModel::solve(int i, const QList<RealInterval> &spaceBounds)
{

    funclist[i].solver()->solve(spaceBounds);
}

const Function & FunctionsModel::currentFunction() const
{





    return funclist[m_selectedRow];
}

Function & FunctionsModel::currentFunction()
{





    return funclist[m_selectedRow];
}

Qt::ItemFlags FunctionsModel::flags(const QModelIndex &idx) const
{
    if(idx.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsTristate;
    else
        return 0;
}

void FunctionsModel::unselect()
{
    m_selectedRow=-1;
}

void FunctionsModel::setResolution(uint res)
{
    m_resolution=res;
    if(!funclist.isEmpty())
    {


        QModelIndex idx=index(0, 0), idxEnd=index(rowCount()-1, 0);
        emit dataChanged(idx, idxEnd);
    }
}

QString FunctionsModel::freeId()
{
    return QString("f%1").arg(m_fcount++);
}

void FunctionsModel::removeFunctionsBySpaceId(const QUuid &sid)
{



    for(int i=0; i<funclist.size(); i+=1)
    {

        if (funclist.at(i).spaceId() == sid)
        {
            if(m_selectedRow>=i)
                m_selectedRow-=1;

            beginRemoveRows(QModelIndex(), i, i+1);
            emit functionRemoved(funclist.at(i).id(), funclist.at(i).name());

            funclist.removeAt(i);
            endRemoveRows();
        }
    }
}
