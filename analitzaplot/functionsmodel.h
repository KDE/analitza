/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#ifndef FUNCTIONSMODEL_H
#define FUNCTIONSMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QPointF>
#include <QtGui/QVector3D>
#include <QVector>
#include <QLineF>

#include <KDE/KLocalizedString>

#include "function.h"

class ANALITZAPLOT_EXPORT FunctionsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum FunctionsModelRoles { Selection=Qt::UserRole+1, Shown=Qt::UserRole+2 };
    typedef QList<Function>::const_iterator const_iterator;
    friend class Graph2D;
    friend class Graph3D;
    friend class FunctionsView;


    explicit FunctionsModel(QObject *parent=0);

    Qt::ItemFlags flags ( const QModelIndex & index ) const;

    QVariant data( const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex & =QModelIndex()) const
    {
        return 4;
    }


    bool addFunction(const Function& f);

    bool addFunction(Function& func,int index,QList<double> cons,int oct,int axi,bool solid,bool curva,bool xy,double pres);


    bool setSelected(const QUuid& fid);

    inline bool isSelected(int i) const
    {
        return i==m_selectedRow;
    }


    bool setShown(const QString& exp, bool shown);


    void editFunction(int num, const Function& func);


    bool editFunction(const QUuid& id, const Function& func);


    void updateSpaceId(const QUuid& functionId, const QUuid& spaceId);


    Function* editFunction(int num);

    void setResolution(uint res);

    void unselect();

    void clear();

    void sendStatus(const QString& msg)
    {
        emit status(msg);
    }

    void solve(int i, const QList<RealInterval> &spaceBounds);

    const Function& currentFunction() const;

    bool hasSelection() const
    {
        return m_selectedRow>=0 && !funclist.isEmpty();
    }

    const_iterator constBegin() const
    {
        return funclist.constBegin();
    }
    const_iterator constEnd() const
    {
        return funclist.constEnd();
    }

    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );


    QString freeId();

    void removeFunctionsBySpaceId(const QUuid &sid);

    void setAnimActiva(int flag);
    int getAnimActiva();
    void setModAnActivo(bool flag);
    bool getModAnActivo();
    void setDirAnim(int flag);
    int getDirAnim();
    bool getMArea();
    void setMArea(bool flag);
    bool getMAreaE();
    void setMAreaE(bool flag);
    void setMFuncion1(QString func1);
    void setMFuncion2(QString func2);
    QString getMFuncion1();
    QString getMFuncion2();
    void setPFuncion1(int pf1);
    void setPFuncion2(int pf2);
    int getPFuncion1();
    int getPFuncion2();
    void setAreaEjemplo(int ej);
    int getAreaEjemplo();
    void setCantDiv(int div);
    int getCantDiv();
    QVector<QLineF> getPathsArea();
    void setPathsArea(QVector<QLineF> &areaLines);
    void setIntPoints(QVector<QPointF> &puntosInt);
    QVector<QPointF> getIntPoints();
    void setPathsCre(QVector<QLineF> &linesCre);
    QVector<QLineF> getPathsCre();
    void setPathsDec(QVector<QLineF> &linesDec);
    QVector<QLineF> getPathsDec();
    void setPathsEnt(QVector<QLineF> &linesEnt);
    QVector<QLineF> getPathsEnt();
    void setPathsEnt2(QVector<QLineF> &linesEnt);
    QVector<QLineF> getPathsEnt2();
    void setPathsACre(QVector<QLineF> &linesACre);
    QVector<QLineF> getPathsACre();
    void setPathsADec(QVector<QLineF> &linesADec);
    QVector<QLineF> getPathsADec();
    void setPathsAEnt(QVector<QLineF> &linesAEnt);
    QVector<QLineF> getPathsAEnt();
    double getLimiteS();
    double getLimiteI();
    void setLimiteS(double ls);
    void setLimiteI(double li);

    /// ???? TODO gsoc
    QModelIndex indexForId(const QString& id)
    {
        int i=0;
        for (QList<Function>::iterator it = funclist.begin(); it!=funclist.end(); ++it, ++i)
            if(it->name() == id)
                return index(i,0);
        return QModelIndex();
    }


public slots:
    void setSelected(const QModelIndex& idx);

protected:

    int selectedRow() const
    {
        return m_selectedRow;
    }

signals:

    void status(const QString &msg);
    void functionModified(const Function &function);
    void functionImplicitCall(QUuid functionId,QColor col,int index,QList<double> cons,int oct,int axi,bool solid,bool curva,bool xy,double pres);
    void functionRemoved(const QUuid &functionId, const QString &functionName);

private:
    Function& currentFunction();

    FunctionList funclist;
    int m_selectedRow;

    //WARNING ??????????????
    int animActiva;
    bool modAnActivo;
    int dirAnim;
    bool mArea;
    bool mAreaE;
    QString mFuncion1;
    QString mFuncion2;
    int pFuncion1;
    int pFuncion2;
    QVector<QLineF> m_pathsArea;
    QVector<QPointF> m_pointsInt;
    QVector<QLineF> m_pathsCre;
    QVector<QLineF> m_pathsDec;
    QVector<QLineF> m_pathsEnt;
    QVector<QLineF> m_pathsEnt2;
    QVector<QLineF> m_pathsACre;
    QVector<QLineF> m_pathsADec;
    QVector<QLineF> m_pathsAEnt;
    int areaEjemplo;
    int cantDiv;
    double limiteS;
    double limiteI;

    uint m_resolution;

    uint m_fcount;
};

#endif
