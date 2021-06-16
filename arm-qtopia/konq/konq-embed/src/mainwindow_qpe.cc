/*  This file is part of the KDE project
    Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception this program may be linked with Qt non-commercial 
    edition, the resulting executable be distributed, without including the 
    source code for the Qt non-commercial edition in the source distribution.

*/


// Implementation of the QPE GUI

#include "mainwindow_qpe.h"

#if defined(KONQ_GUI_QPE)

#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qlineedit.h>
#include <qstatusbar.h>
#include <qcombobox.h>
#include <qimage.h>

#include <klocale.h>

// QPE
#include <qpe/applnk.h>

MainWindowQPE::MainWindowQPE()
    : MainWindowX11()
{
    s_winStyle = STYLE_MAXIMIZED;

    QPixmap brokenIcon = KIconLoader::self()->loadIcon( "file_broken" );
    if ( brokenIcon.size() != QSize( 14, 14 ) )
    {
        QImage img = brokenIcon.convertToImage().smoothScale( 14, 14 );
        QPixmap pix; pix.convertFromImage( img );
        KIconLoader::self()->registerIcon( "file_broken", pix );
    }
}

void MainWindowQPE::setDocument( const QString &document )
{
    DocLnk lnk( document );
    createNewView( lnk.file().prepend( "file:" ) );
}

QIconSet MainWindowQPE::loadPixmap( const char * const xpm[] )
{
    QImage image( ( const char ** )xpm );
    // ### 14x14 hardcoded toolbar icon size in Qtopia
    QPixmap pix;
    pix.convertFromImage( image.smoothScale( 14, 14 ) );
    QIconSet iconSet( pix );

#ifndef QT_NO_DEPTH_32
    QImage disabledImage( image.width(), image.height(), 32 );
    disabledImage.setAlphaBuffer( true );
    for ( int y = 0; y < disabledImage.height(); ++y )
        for ( int x = 0; x < disabledImage.width(); ++x )
        {
            QRgb pix = image.pixel( x, y );
            int r = qRed( pix );
            int g = qGreen( pix );
            int b = qBlue( pix );
            int a = qAlpha( pix ) / 3;
            disabledImage.setPixel( x, y, qRgba( r, g, b, a ) );
        }
    QPixmap disabledPixmap; disabledPixmap.convertFromImage( disabledImage );
    if ( !disabledPixmap.isNull() )
        iconSet.setPixmap( disabledPixmap, QIconSet::Small, QIconSet::Disabled );
#endif

    return iconSet;
}

#include "mainwindow_qpe.moc"

#endif

/*
 * vim:ts=4:et:sw=4
 */
