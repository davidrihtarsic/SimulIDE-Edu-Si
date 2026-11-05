/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>

#include "rail.h"
#include "simulator.h"
#include "iopin.h"
#include "itemlibrary.h"
#include "pin.h"

#include "doubleprop.h"

#define tr(str) simulideTr("Rail",str)

Component* Rail::construct( QString type, QString id )
{ return new Rail( type, id ); }

LibraryItem* Rail::libraryItem()
{
    return new LibraryItem(
        tr("Rail"),
        "Sources",
        "rail.png",
        "Rail",
        Rail::construct );
}

Rail::Rail( QString type, QString id )
    : Component( type, id )
    , eElement( id )
{
    setLabelPos(-64,-24 );

    m_area = QRect(-4, -8, 15, 16 );
    m_changed = false;

    m_pin.resize(1);
    m_pin[0] = m_out = new IoPin( 0, QPoint(16,0), id+"-outnod", 0, this, source );

    setRotation( 90 );
    setValLabelPos(-12, 6,-90 ); // x, y, rot
    setLabelPos(-5,-10,-90);

    addPropGroup( { tr("Main"), {
        new DoubProp<Rail>("Voltage", tr("Voltage"), "V"
                          , this, &Rail::volt, &Rail::setVolt )
    }, 0} );

    setShowProp("Voltage");
    setPropStr( "Voltage", "5" );
}
Rail::~Rail() { delete m_out; }

void Rail::setVolt( double v )
{
    Simulator::self()->pauseSim();
    m_volt = v;
    updateVoltageLabel();
    stamp();
    Simulator::self()->resumeSim();
}

bool Rail::setPropStr( QString prop, QString val )
{
    bool ok = Component::setPropStr( prop, val );
    if( prop == "Voltage" ) updateVoltageLabel();
    return ok;
}

void Rail::setup()
{
    Component::setup();
    updateVoltageLabel();
}

void Rail::updateVoltageLabel()
{
    QString voltage = QString::number( m_volt, 'g', 12 );
    if( m_volt > 0 ) voltage.prepend( '+' );
    setValLabelText( voltage.append( 'V' ) );
}

void Rail::stamp()
{
    m_out->setOutHighV( m_volt );
    m_out->setOutState( true );
    update();
}

QPainterPath Rail::shape() const
{
    QPainterPath path;
    path.addRect( QRectF( -4, -8, 15, 16 ) );
    return path;
}

void Rail::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );
    p->setBrush( Qt::NoBrush );

    // Rail is rotated 90 degrees by default. In local coordinates the
    // vertical line therefore becomes the horizontal IEEE potential bar.
    p->drawLine( QPointF( 0, -6 ), QPointF( 0, 6 ) );
    p->drawLine( QPointF( 0,  0 ), QPointF( 9, 0 ) );

    Component::paintSelected( p );
}
