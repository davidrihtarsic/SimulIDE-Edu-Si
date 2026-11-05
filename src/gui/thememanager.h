/***************************************************************************
 *   Copyright (C) 2025 by Santiago González                               *
 *                                                                         *
 ***( see copyright.txt file at root folder )*******************************/

#pragma once

#include <QIcon>
#include <QPalette>
#include <QColor>

enum class Theme { Light, Dark };

class ThemeManager
{
    public:
        ThemeManager();
        ~ThemeManager();

 static ThemeManager* self() { return m_pSelf; }

        void setTheme( Theme theme );
        Theme theme() { return m_currentTheme; }
        bool isDark() { return m_isDark; }

        QColor lightCircuitBackground() const;
        bool hasCustomCircuitBackground() const { return m_hasCustomCircuitBackground; }
        void setCircuitBackground( const QColor& color );
        void resetCircuitBackground();

        uint32_t* getColorPtr( uint index );

        QIcon icon( QString iconFile );
        QPixmap invertPixmap( QPixmap pixmap );

    private:

        Theme m_currentTheme = Theme::Light;
        bool  m_isDark = false;
        bool  m_hasCustomCircuitBackground = false;

        uint32_t m_customCircuitBackground = 0;

        std::vector<uint32_t> m_darkColors;
        std::vector<uint32_t> m_lightColors;
        std::vector<uint32_t> m_colors;

        QPalette m_darkPalette;
        QPalette m_lightPalette;

 static ThemeManager* m_pSelf;
};
