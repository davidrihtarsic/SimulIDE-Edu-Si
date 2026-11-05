/***************************************************************************
 *   Copyright (C) 2020 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#include <QPainter>
#include <math.h>
#include <QMenu>
#include <QGraphicsProxyWidget>

#include "dcmotor.h"
#include "itemlibrary.h"
#include "simulator.h"
#include "circuit.h"
#include "pin.h"
#include "label.h"
#include "watcher.h"

#include "doubleprop.h"
#include "intprop.h"

#define tr(str) simulideTr("DcMotor",str)

Component* DcMotor::construct( QString type, QString id )
{ return new DcMotor( type, id ); }

LibraryItem* DcMotor::libraryItem()
{
    return new LibraryItem(
        tr("Dc Motor"),
        "Motors",
        "dcmotor.png",
        "DcMotor",
        DcMotor::construct );
}

DcMotor::DcMotor( QString type, QString id )
       : LinkerComponent( type, id )
       , eResistor( id )
       , Watched()
{
    m_graphical = true;
    
    m_area = QRectF( -36, -42, 72, 84 );
    m_color = QColor( 50, 50, 70 );
    m_ang  = 0;
    m_speed = 0;
    m_voltNom = 5;
    setRpm( 60 );

    m_pin.resize( 2 );
    m_ePin[0] = m_pin[0] = new Pin( 180, QPoint(-40,0), m_id+"-lPin", 0, this);
    m_pin[0]->setLength( 4 );

    m_ePin[1] =m_pin[1] = new Pin( 0, QPoint(40,0), m_id+"-rPin", 1, this);
    m_pin[1]->setLength( 4 );

    setShowId( true );
    setLabelPos(-22,-48, 0);
    setValLabelPos(-14, 36, 0);

    Simulator::self()->addToUpdateList( this );

    addPropGroup( { tr("Main"), {
        new IntProp <DcMotor>("RPM_Nominal", tr("Nominal Speed"), "_RPM"
                             , this, &DcMotor::rpm, &DcMotor::setRpm ),

        new DoubProp<DcMotor>("Volt_Nominal", tr("Nominal Voltage"), "V"
                             , this, &DcMotor::volt, &DcMotor::setVolt ),

        new DoubProp<DcMotor>("Resistance", tr("Resistance"), "Ω"
                             , this, &DcMotor::resistance, &DcMotor::setResSafe )
    },0} );
}
DcMotor::~DcMotor(){}

QString DcMotor::getPropStr( QString prop )
{
    if( prop.toLower() == "speed" ) // To be called from scripts
    {
        return QString::number( -m_speed );  // Speed: 1 = 100%
    }
    else return LinkerComponent::getPropStr( prop );
}

void DcMotor::initialize()
{
    m_ang = 0;
    m_speed = 0;
    m_delta = 0;
    m_lastTime = 0;
    m_updtTime = 0;
    m_LastVolt = 0;
}

void DcMotor::stamp()
{
     if( m_ePin[0]->isConnected() && m_ePin[1]->isConnected() )
     {
         m_ePin[0]->changeCallBack( this );
         m_ePin[1]->changeCallBack( this );
     }
     eResistor::stamp();
}

void DcMotor::updateStep()
{
    updatePos();

    if( m_updtTime ) m_speed = m_delta/(m_updtTime/1e12);

    m_ang += m_motStPs*m_delta;
    m_ang = remainder( m_ang, (16.0*360.0) );

    if(  m_linkedComp.size() )
    {
        double val = m_ang*1000/(16.0*360.0);
        if( val > 0 ) val = 1000-val;
        else          val = -val;

        emitLinkedValue( val ); // angle 0-1000
    }
    if( m_watcher ) m_watcher->updateValues();

    m_delta = 0;
    m_updtTime = 0;
    update();
}

void DcMotor::voltChanged() { updatePos(); }

void DcMotor::updatePos()
{
    uint64_t timePS = Simulator::self()->circTime();
    uint64_t duration = timePS-m_lastTime;
    m_updtTime += duration;
    m_lastTime = timePS;

    m_delta += (m_LastVolt/m_voltNom)*(duration/1e12);
    m_LastVolt = m_ePin[1]->getVoltage() - m_ePin[0]->getVoltage();
}

void DcMotor::setRpm( int rpm )
{
    if( rpm < 1 ) rpm = 1;
    m_rpm = rpm;
    m_motStPs = 16*360*rpm/60;

    update();
}

void DcMotor::openWatcher()
{
    if( !m_watcher )
    {
        createWatcher( true );

        QGraphicsProxyWidget* proxy = Circuit::self()->addWidget( m_watcher );
        proxy->setParentItem( this );
        proxy->setPos(-65,40 );
        //proxy->setTransformOriginPoint( proxy->boundingRect().center() );
        m_watcher->setProxy( proxy );

        m_watcher->addRegister( "Voltage" , "double", "V" );
        m_watcher->addRegister( "Current" , "double", "A" );
        m_watcher->addRegister( "Speed"   , "double", "RPM" );
        m_watcher->setWindowTitle( this->idLabel() );
    }
    m_watcher->show();
}

double DcMotor::getDblReg( QString reg )
{
    if( reg == "Voltage" ) return -m_LastVolt;
    if( reg == "Current" ) return eResistor::current();
    if( reg == "Speed"   ) return -m_speed*m_rpm;

    return 0;
}

void DcMotor::contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu )
{
    QAction* watcherAction = menu->addAction( m_theme->icon(":/terminal.svg"),tr("Open Monitor") );
    QObject::connect( watcherAction, &QAction::triggered, [=](){ openWatcher(); } );

    menu->addSeparator();

    LinkerComponent::contextMenu( event, menu );
}

void DcMotor::paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w )
{
    Component::paint( p, o, w );

    p->setBrush( Qt::NoBrush );
    p->drawLine( QPointF(-36, 0), QPointF(-22, 0) );
    p->drawLine( QPointF( 22, 0), QPointF( 36, 0) );
    p->drawEllipse( QRectF(-22, -22, 44, 44) );

    QPen symbolPen = p->pen();
    QPen polarityPen( Qt::black, 1.5, Qt::SolidLine
                    , Qt::RoundCap, Qt::RoundJoin );
    p->setPen( polarityPen );
    p->drawLine( QPointF(-17.5, 0), QPointF(-12.5, 0) );
    p->drawLine( QPointF(-15, -2.5), QPointF(-15, 2.5) );
    p->drawLine( QPointF( 12.5, 0), QPointF( 17.5, 0) );
    p->setPen( symbolPen );

    QFont motorFont = p->font();
    motorFont.setBold( true );
    motorFont.setPixelSize( 15 );

    double componentAngle = fmod( rotation(), 360.0 );
    if( componentAngle < 0 ) componentAngle += 360.0;

    int rpm = 0;
    bool running = Simulator::self()->isRunning();
    if( running ) rpm = qRound( -m_speed*m_rpm );

    // Keep the static M upright on the page. While the motor is running,
    // rotate it by the simulated shaft angle, independently of component
    // orientation.
    double motorAngle = 0;
    if( running && rpm != 0 ) motorAngle = m_ang/16.0;

    p->save();
    p->rotate( motorAngle-componentAngle );
    p->setFont( motorFont );
    p->drawText( QRectF(-14, -11, 28, 22), Qt::AlignCenter, "M" );
    p->restore();

    if( running )
    {
        QFont rpmFont = p->font();
        rpmFont.setBold( false );
        rpmFont.setPixelSize( 9 );

        // At 180 and 270 degrees turn the label locally by 180 degrees.
        // It consequently remains readable from the bottom/right side of
        // the page and is placed below horizontal motors or left of vertical
        // motors, as used in technical documentation.
        int quadrant = qRound( componentAngle/90.0 ) % 4;
        p->save();
        if( quadrant >= 2 ) p->rotate( 180 );
        p->setFont( rpmFont );
        p->drawText( QRectF(-32, 26, 64, 14), Qt::AlignCenter
                   , QString::number(rpm)+" RPM" );
        p->restore();
    }
    Component::paintSelected( p );
}
