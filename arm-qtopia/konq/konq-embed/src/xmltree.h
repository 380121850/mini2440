/*  This file is part of the KDE project
    Copyright (C) 2000,2001 Simon Hausmann <hausmann@kde.org>

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

*/


#ifndef __bookmarks_h__
#define __bookmarks_h__

#include <qstring.h>
#include <qmap.h>
#include <qtextstream.h>

class XMLElement
{
public:
    typedef QMap<QString, QString> AttributeMap;

    XMLElement();
    ~XMLElement();

    // transfers ownership
    void appendChild( XMLElement *child );

    // transfers ownership
    void insertAfter( XMLElement *newChild, XMLElement *refChild );

    // transfers ownership
    void insertBefore( XMLElement *newChild, XMLElement *refChild );

    // does not delete child, just free refs
    void removeChild( XMLElement *child );

    XMLElement *parent() const { return m_parent; }
    XMLElement *firstChild() const { return m_first; }
    XMLElement *nextChild() const { return m_next; }
    XMLElement *prevChild() const { return m_prev; }
    XMLElement *lastChild() const { return m_last; }

    void setTagName( const QString &tag ) { m_tag = tag; }
    QString tagName() const { return m_tag; }

    void setValue( const QString &val ) { m_value = val; }
    QString value() const { return m_value; }

    void setAttributes( const AttributeMap &attrs ) { m_attributes = attrs; }
    AttributeMap attributes() const { return m_attributes; }
    AttributeMap &attributes() { return m_attributes; }

    void save( QTextStream &stream, uint indent = 0 );

    XMLElement *namedItem( const QString &name );

    XMLElement *clone() const;

    static XMLElement *load( const QString &fileName );

private:
    static QString encodeAttr( const QString &attr );

    QString m_tag;
    QString m_value;
    AttributeMap m_attributes;

    XMLElement *m_parent;
    XMLElement *m_next;
    XMLElement *m_prev;
    XMLElement *m_first;
    XMLElement *m_last;
};

#endif
