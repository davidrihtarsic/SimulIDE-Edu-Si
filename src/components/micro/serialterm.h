/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include "component.h"
#include "e-element.h"
#include "usartmodule.h"
#include "e-node.h"

class LibraryItem;
class CustomButton;
class Terminal;
class QGraphicsProxyWidget;

class SerialTerm : public Component, public UsartModule, public eElement
{
    public:
        SerialTerm( QString type, QString id );
        ~SerialTerm();
        
 static Component* construct( QString type, QString id );
 static LibraryItem* libraryItem();

        void initialize() override;
        virtual void stamp() override;
        virtual void updateStep() override;
        virtual void runEvent() override;

        void setSerialMon( bool s );

        bool showDataBuses() { return m_showDataBuses; }
        void setShowDataBuses( bool show );
        void setHidden( bool hide, bool hidArea=false, bool hidLabel=false ) override;

        void sendByteArray( QByteArray data );
        virtual void sendByte( uint8_t data ) override;

        virtual void setIdLabel( QString id ) override;

        virtual void byteReceived( uint8_t byte ) override;
        virtual void frameSent( uint8_t data ) override;

        virtual void monitorClosed() override;
        void TerminalClosed();

        virtual void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void onbuttonclicked();
        void slotOpenTerm();

    protected:
        virtual void setflip() override;
        virtual void contextMenu( QGraphicsSceneContextMenuEvent* event, QMenu* menu ) override;

    private:
        void setBusByte( int bus, uint8_t byte );

        bool m_showDataBuses;
        Pin* m_dataBus[2];
        ePin* m_busDriver[2][8];
        eNode m_busGround;

        bool m_receiving;
        bool m_sending;

        Terminal* m_terminal;

        CustomButton* m_button;
        QGraphicsProxyWidget* m_proxy;

        QByteArray m_uartData;
};
