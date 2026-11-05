/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <QMenu>

#include "led.h"
#include "pin.h"
#include "itemlibrary.h"

#include "stringprop.h"

#define tr(str) simulideTr("Led",str)

Component* Led::construct( QString type, QString id )
{ return new Led( type, id ); }

LibraryItem* Led::libraryItem()
{
    return new LibraryItem(
        tr("Led"),
        "Leds",
        "led.png",
        "Led",
        Led::construct);
}

Led::Led( QString type, QString id )
   : LedBase( type, id )
{
    m_area = QRect(-8, -16, 20, 26 );

    m_isLinker = true;

    m_pin.resize( 2 );
    m_pin[0] = new Pin( 180, QPoint(-16, 0 ), m_id+"-lPin", 0, this);
    m_pin[1] = new Pin(   0, QPoint( 16, 0 ), m_id+"-rPin", 1, this);
    m_pin[0]->setLength( 10 );

    setEpin( 0, m_pin[0] );
    setEpin( 1, m_pin[1] );

    addPropGroup( { "Hidden", {
        new StrProp<Led>("Links", "Links",""
                        , this, &Led::getLinks , &Led::setLinks )
    }, groupHidden} );
}
Led::~Led(){}

void Led::voltChanged()
{
    eLed::voltChanged();
    if( !m_converged ) return;

    emitLinkedValue( m_current );
}

void Led::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if( !acceptedMouseButtons() ) { event->ignore(); return; }

    event->accept();
    QMenu* menu = new QMenu();

    if( !parentItem() )
    {
        QAction* linkCompAction = menu->addAction( m_theme->icon(":/subcl.png"),tr("Link to Component") );
        QObject::connect( linkCompAction, &QAction::triggered, [=](){ slotLinkComp(); } );

        menu->addSeparator();
    }
    Component::contextMenu( event, menu );
    menu->deleteLater();
}

void Led::drawBackground( QPainter* p )
{
    Q_UNUSED( p );
}

void Led::drawForeground( QPainter* p )
{
    QColor lightColor = p->pen().color();
    QColor symbolColor( *m_foreColor );

    QPen pen( symbolColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    p->setPen( pen );

    if( m_intensity > 0 ) p->setBrush( lightColor );
    else                  p->setBrush( Qt::NoBrush );

    static const QPointF points[3] = {
        QPointF( 8, 0 ),
        QPointF(-3,-6 ),
        QPointF(-3, 6 ) };

    p->drawPolygon( points, 3 );
    p->drawLine( 8,-6, 8, 6 );
    p->drawLine(-6, 0,-3, 0 );
    p->drawLine( 8, 0,10, 0 );

    p->setBrush( Qt::NoBrush );

    // Light emission arrows.
    p->drawLine( QPointF(-2,-8), QPointF( 1,-11) );
    p->drawLine( QPointF( 1,-11), QPointF(-1,-10) );
    p->drawLine( QPointF( 1,-11), QPointF( 0, -9) );

    p->drawLine( QPointF( 4,-8), QPointF( 7,-11) );
    p->drawLine( QPointF( 7,-11), QPointF( 5,-10) );
    p->drawLine( QPointF( 7,-11), QPointF( 6, -9) );
}
