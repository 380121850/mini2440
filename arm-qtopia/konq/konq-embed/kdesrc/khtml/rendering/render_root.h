/**
 * This file is part of the HTML widget for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id: render_root.h,v 1.34 2001/07/28 13:16:43 koivisto Exp $
 */
#ifndef render_root_h
#define render_root_h

#include "render_flow.h"

#include <qdatetime.h>

class KHTMLView;
class QScrollView;

namespace khtml {

class RenderRoot : public RenderFlow
{
public:
    RenderRoot(KHTMLView *view);
    virtual ~RenderRoot();

    virtual const char *renderName() const { return "RenderRoot"; }

    virtual bool isRendered() const { return true; }
    virtual bool isRoot() const { return true; }

    virtual void layout();
    virtual void calcWidth();
    virtual void calcMinMaxWidth();
    virtual bool absolutePosition(int &xPos, int&yPos, bool f = false);
    virtual void updateSize();
    virtual void updateHeight();
    virtual void close();

    int docHeight() const;
    int docWidth() const;

    KHTMLView *view() const { return m_view; }

    virtual void repaint();
    virtual void repaintRectangle(int x, int y, int w, int h, bool f=false);
    virtual void print( QPainter *, int x, int y, int w, int h, int tx, int ty);
    void printObject(QPainter *p, int _x, int _y,
                     int _w, int _h, int _tx, int _ty);

    virtual void setSelection(RenderObject *s, int sp, RenderObject *e, int ep);
    virtual void clearSelection();

    void setPrintingMode(bool print) { m_printingMode = print; }
    bool printingMode() const { return m_printingMode; }
    
    virtual void setWidth( int width ) { m_rootWidth = m_width = width; }
    virtual void setHeight( int height ) { m_rootHeight = m_height = height; }

protected:

    virtual void selectionStartEnd(int& spos, int& epos);

    virtual QRect viewRect() const;

    KHTMLView *m_view;

    QTime updateTimer;

    RenderObject* selectionStart;
    RenderObject* selectionEnd;
    int selectionStartPos;
    int selectionEndPos;

    int m_rootWidth;
    int m_rootHeight;

    int oldLayoutTime;
    int timeout;

    // used to ignore viewport width when printing to the printer
    bool m_printingMode;
};

};
#endif
