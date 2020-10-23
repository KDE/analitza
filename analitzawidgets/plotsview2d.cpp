/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#include "plotsview2d.h"

#include <QSvgGenerator>
#include <QList>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <QItemSelectionModel>
#include <QCoreApplication>
#include <QApplication>
#include <QClipboard>

#include <analitza/analyzer.h>
#include <analitzaplot/plotsmodel.h>
#include <cmath>

using namespace Analitza;

PlotsView2D::PlotsView2D(QWidget *parent)
    : QWidget(parent)
    , Plotter2D(size())
    , valid(false)
    , mode(None)
    , m_framed(false)
    , m_readonly(false)
    , m_selection(nullptr)
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->setCursor(Qt::CrossCursor);
    
    this->setMouseTracking(!m_readonly);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumSize(128, 128);
    
    //BEGIN setup plotter2d grid
    defViewport = QRectF(QPointF(-10.0, 10.0), QSizeF(20.0, -20.0));
    resetViewport();
    
    //colors
    QColor bgcolor = palette().color(QPalette::Base);
    setBackgroundColor(bgcolor);
    
    QColor gridcolor = palette().color(QPalette::Midlight);
    setGridColor(gridcolor);
    //END
    
    this->setAutoFillBackground(false);
}

PlotsView2D::~PlotsView2D() {}

void PlotsView2D::paintEvent(QPaintEvent * )
{
    if (!valid)
    {
        if (buffer.isNull() || buffer.size()!=size())
            buffer = QPixmap(size());
        buffer.fill(backgroundColor());
    
        drawFunctions(&buffer);

        valid=true;
    }
    QPainter p(this);
    p.drawPixmap(QRect(QPoint(0,0), size()), buffer);
    
    QPen ccursor;
    
//  finestra.setRenderHint(QPainter::Antialiasing, true);
    
    //NOTE GSOC 2012 better guards : model()->rowCount() > 0 && !mark.isNull() ... si no se cumplen que no dibuje las lineas rojas
    if(!m_readonly && mode==None) {
        //QPointF ultim = toWidget(mark);
        QPointF ultim(cursorPos);
        
        //Draw derivative
        if (!mark.isNull()) 
        {
            ultim = toWidget(mark); // si no es nulo apunto al lugar del punto de pendiente
            
            ccursor.setColor(palette().color(QPalette::Active, QPalette::Link));
            ccursor.setStyle(Qt::SolidLine);
            QLineF sl=slope(mark);
            sl.translate(mark);
            
            p.setPen(ccursor);
            p.setRenderHint(QPainter::Antialiasing, true);
    #ifndef Q_CC_MSVC
            if(!sl.isNull() && !std::isnan(sl.length()))
    #else
            if(!sl.isNull() && !_isnan(sl.length()))
    #endif
                p.drawLine(toWidget(sl));
            p.setRenderHint(QPainter::Antialiasing, false);
        }
        //EOderivative
        
        ccursor.setColor(QColor(0xc0,0,0));
        ccursor.setStyle(Qt::SolidLine);
        
        p.setPen(ccursor);
        p.drawLine(QPointF(0.,ultim.y()), QPointF(size().width(), ultim.y()));
        p.drawLine(QPointF(ultim.x(),0.), QPointF(ultim.x(), size().height()));
        
        int w=p.fontMetrics().boundingRect(m_posText).width()+15, h=p.fontMetrics().height();
        
        if(ultim.x()+w > size().width())
            ultim.setX(size().width()-w);
        if(ultim.y()+h > size().height())
            ultim.setY(size().height()-h);
        if(ultim.y() < 0.)
            ultim.setY(0.);
        
        p.setPen(QPen(Qt::black));
        p.drawText(QPointF(ultim.x()+15., ultim.y()+15.), m_posText);
    } else if(!m_readonly && mode==Selection) {
           //NOTE GSOC 2012 accessibility code, the selection follow system rules :)

//         ccursor.setColor(QColor(0xc0,0,0));
        QColor selcol = QPalette().highlight().color();
        ccursor.setColor(QPalette().highlightedText().color());
        ccursor.setStyle(Qt::SolidLine);
        p.setPen(ccursor);
//         p.setBrush(QColor(0xff,0xff, 0,0x90));
        selcol.setAlpha(0x90);
        p.setBrush(selcol);
        p.drawRect(QRect(press,last));
    }
    
    if(m_framed) {
        QPen bord(Qt::black);
        p.setPen(bord);
        p.drawRect(QRect(QPoint(0,0), size()-QSize(2,2)));
    }
    p.end();
    
//  qDebug() << "xxx2 " << viewport;
}

void PlotsView2D::wheelEvent(QWheelEvent *e)
{
    QRectF viewport=currentViewport();
    int steps = e->angleDelta().y()/(8*15);
    qreal d = (-0.03*steps) + 1;
    
    if(d>0 || (viewport.width()+d > 2 && viewport.height()+d < 2)) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        scaleViewport(d, e->pos());
#else
        scaleViewport(d, e->position().toPoint());
#endif
    }
}

void PlotsView2D::mousePressEvent(QMouseEvent *e)
{
    if(!m_readonly && (e->button()==Qt::LeftButton || e->button()==Qt::MiddleButton)) {
        last = press = e->pos();
        ant = toViewport(e->pos());
        this->setCursor(QCursor(Qt::PointingHandCursor));
        if(e->button()==Qt::MiddleButton || (e->button()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier))
            mode=Pan;
        else if(e->button()==Qt::LeftButton)
            mode=Selection;
    }
}

void PlotsView2D::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_readonly)
        this->setCursor(Qt::ArrowCursor);
    else
        this->setCursor(Qt::CrossCursor);
    
    if(!m_readonly && mode==Selection) {
        QPointF pd = toViewport(e->pos())-toViewport(press);
        
        QPoint pd2 = e->pos()-press;
        QRect rr(press, QSize(pd2.x(), pd2.y()));
        
        if(rr.width()>20 && rr.height()>20) { //if selection is not very small
            QRectF r(fromWidget(press), QSizeF(pd.x(), pd.y()));
            
            if(r.top()<r.bottom()) {
                double aux = r.bottom();
                r.setBottom(r.top());
                r.setTop(aux);
            }
            
            if(r.right()<r.left()) {
                double aux = r.left();
                r.setLeft(r.right());
                r.setRight(aux);
            }
            
            setViewport(r);
        } else
            sendStatus(QCoreApplication::tr("Selected viewport too small"));
    }
    
    mode = None;
    this->repaint();
}

void PlotsView2D::mouseMoveEvent(QMouseEvent *e)
{
    cursorPos = e->pos();
    QPair<QPointF, QString> img=calcImage(fromWidget(e->pos()));
    mark=img.first;
    
    if(!m_readonly && mode==Pan && ant != toViewport(e->pos())){
        moveViewport(e->pos() - press);
        
        press = e->pos();
    } else if(e->buttons()&Qt::LeftButton) {
        last = e->pos();
    } else if(e->buttons()==0) {
        if(img.second.isEmpty()) {
            mark = fromWidget(e->pos());
            sendStatus(QCoreApplication::tr("x=%1 y=%2").arg(mark.x()).arg(mark.y()));
        } else
            sendStatus(img.second);
    }
    
    this->repaint();
}

void PlotsView2D::keyPressEvent(QKeyEvent * e)
{
    //TODO use grid info (inc) here, made a public method to return current scale increment
    const double xstep=currentViewport().width()/12., ystep=currentViewport().height()/10.;
    
    switch(e->key()) {
        case Qt::Key_Right:
            setViewport(lastUserViewport().translated(xstep, 0));
            break;
        case Qt::Key_Left:
            setViewport(lastUserViewport().translated(-xstep, 0));
            break;
        case Qt::Key_Down:
            setViewport(lastUserViewport().translated(0, -ystep));
            break;
        case Qt::Key_Up:
            setViewport(lastUserViewport().translated(0, ystep));
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_Plus:
            zoomIn();
            break;
        default:
            return;
    }
}

void PlotsView2D::resizeEvent(QResizeEvent * ev)
{
    if (ev->size() != buffer.size()) {
        buffer = QPixmap(ev->size());
        setPaintedSize(ev->size());
    }
}

bool PlotsView2D::toImage(const QString &path, Format f)
{
    Q_ASSERT(!path.isEmpty());
    bool b=false;
    
    switch(f) {
        case SVG: {
            QFile f(path);
            QSvgGenerator gen;
            gen.setOutputDevice(&f);
            gen.setSize(this->size());
            drawFunctions(&gen);
            b=true;
            forceRepaint();
        }   break;
        case PNG:
            this->repaint();
            b=buffer.save(path, "PNG");
            break;
    }
    
    return b;
}

void PlotsView2D::snapshotToClipboard()
{
    QClipboard *cb = QApplication::clipboard();

    cb->setImage(buffer.toImage());
}

void Analitza::PlotsView2D::modelChanged()
{
}

void PlotsView2D::setReadOnly(bool ro)
{
    m_readonly=ro;
    setCursor(ro ? Qt::ArrowCursor : Qt::CrossCursor);
    setMouseTracking(!ro);
}

QRectF PlotsView2D::definedViewport() const
{
    return lastUserViewport();
}

void PlotsView2D::viewportChanged()
{
    QRectF userViewport=lastUserViewport(), viewport=currentViewport();
    
    sendStatus(QStringLiteral("(%1, %2)-(%3, %4)")
            .arg(viewport.left()).arg(viewport.top()).arg(viewport.right()).arg(viewport.bottom()));
    emit viewportChanged(userViewport);
}

int PlotsView2D::currentFunction() const
{
    if (!model())
        return -1;
    
    int ret=-1;
    if(m_selection) {
        ret=m_selection->currentIndex().row();
    }
    
    return ret;
}

void PlotsView2D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection->model() == model());

    if (m_selection)
        disconnect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(forceRepaint()));

    m_selection = selection;

    connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(forceRepaint()));
}
