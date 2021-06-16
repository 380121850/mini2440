/*
    SHP - A Shell+HTML Preprocessor

    Copyright (C) 2002 Paul Chitescu <paulc-devel@null.ro>

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

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

const char description[] =
    "SHP: A Shell+HTML Preprocessor (C) 2002 Paul Chitescu <paulc-devel@null.ro>\n\n";
const char usagestr[] =
    "Usage: shp [-c|-e] [-s shell] [-f file]\n"
    "   -c  Compile mode (only generates output)\n"
    "   -e  Execute mode (for CGI, requires -s)\n"
    "   -s  Change shell (default: /bin/sh)\n"
    "   -f  Change input (default: stdin)\n";

const char param[] = "PARAM_";
char eofmark[16] = "EOF-";

const char *shell = 0;
int toshell = 2;
int child = 0;
int putnl = 0;
int compile = 0;
int execute = 0;

void spawnShell()
{
    int filedes[2];

    if ( pipe( filedes ) )
	exit( errno );

    child = fork();
    if ( child == -1 )
    {
	close( filedes[0] );
	close( filedes[1] );
	exit( errno );
    }

    if ( child == 0 ) /* in child */
    {
	dup2( filedes[0], 0 );
	close( filedes[0] );
	close( filedes[1] );
	execl( shell, shell, 0 );
	exit( errno );
    }

    dup2( filedes[1], 1 );
    close( filedes[0] );
    close( filedes[1] );
    /* Erase this here for a bit more security.
       The shell should do the same as soon as possible. */
    unsetenv( "PROTOCOL_USER_PASS" );
}

void output( int fd, const char *data )
{
    if ( data && *data )
    {
	int len = strlen( data );
	putnl = ( '\n' != data[len-1] );
	write( fd, data, len );
    }
}


void toRaw()
{
    if ( toshell )
    {
	if ( putnl )
	    output( 1, "\n" );
	output( 1, "cat <<" );
	output( 1, eofmark );
    }
    toshell = 0;
}

void toShell()
{
    if ( !toshell )
    {
	if ( putnl )
	    output( 1, "\n" );
	output( 1, eofmark );
    }
    toshell = 1;
}

void processLine( char *line )
{
    char *pc;
    while ( line && *line )
    {
	switch ( toshell )
	{
	    case 0:
		pc = strstr( line, "<?sh" );
		break;
	    case 1:
		pc = strstr( line, "?>" );
		break;
	    default:
		if ( '\n' == *line || '#' == *line )
		    return;
		else if ( strncmp( line, "<?sh", 4 ) == 0 )
		    pc = line;
		else
		{
		    pc = strstr( line, "<?sh" );
		    toRaw();
		}
	}

        if ( pc )
	{
	    *pc = 0;
	    output( 1, line );
	    if ( 1 == toshell )
	    {
		toRaw();
		pc += 2;
	    }
	    else
	    {
		toShell();
		pc += 4;
		while ( isspace( *pc ) )
		    pc++;
	    }
	    if ( '\n' == *pc )
		pc++;
	    line = pc;
	}
	else
	{
	    output( 1, line );
	    line = 0;
	}
    }
}

void init()
{
    unsigned int i;
    srandom( getpid() );
    for ( i = strlen( eofmark ); i < sizeof( eofmark )-2; i++ )
	eofmark[i] = '0' + random() % 10;
    eofmark[i++] = '\n';
    eofmark[i] = 0;
}

void readLines()
{
    char buf[16384], *pc, c;
    int rd = 0;
    for ( ; ; )
    {
	int i = read( 0, buf+rd, sizeof( buf ) - rd - 1 );

	if ( i < 0 )
	{
	    output( 2, "Input read error\n" );
	    exit( errno );
	}

	rd += i;
	buf[rd] = 0;
	do {
	    pc = strchr( buf, '\n' );
	    if ( pc )
	    {
		pc++;
		c = *pc;
		*pc = 0;
		processLine( buf );
		*pc = c;
		rd -= pc - buf;
		memmove( buf, pc, rd+1 );
	    }
	    else if ( !i )
	    {
		processLine( buf );
		return;
	    }
	} while ( pc );
    }
}

int nibble( char c )
{
    if ( '0' <= c && c <= '9' )
	return c - '0';
    if ( 'A' <= c && c <= 'F' )
	return c + 10 - 'A';
    if ( 'a' <= c && c <= 'f' )
	return c + 10 - 'a';
    return -1;
}

void urlConvert( char *dest, const char *source )
{
    for ( ; *source; source++ )
    {
	switch ( *source )
	{
	    case '+':
		*dest++ = ' ';
		break;
	    case '%':
		if ( source[1] )
		{
		    int hi, lo;
		    hi = nibble( *++source );
		    if ( source[1] )
		    {
			lo = nibble( *++source );
			*dest++ = ( -1 != hi && -1 != lo ) ? ( 16 * hi + lo ) : '?';
		    }
		}
		break;
	    default:
		*dest++ = *source;
	}
    }
    *dest = 0;
}

void parseParam( char *query )
{
    if ( !query )
	return;

    for ( ; ; )
    {
	char c, *pc = query;
	while ( 0 != ( c = *pc ) && '=' != c && '?' != c && '&' != c )
	    pc++;

	if ( !c )
	    break;

	if ( '=' == c )
	{
	    char *p1, *p2;
	    char *pp = pc;
	    *pc++ = 0;
	    while ( 0 != ( c = *pc ) && '?' != c && '&' != c )
		pc++;
	    *pc = 0;
	    p1 = malloc( strlen( param ) + strlen( query ) + 1 );
	    p2 = malloc( strlen( pp+1 ) + 1 );

	    strcpy( p1, param );
	    urlConvert( p1 + strlen( param ), query );
	    urlConvert( p2, pp+1 );

	    setenv( p1, p2, 1 );

	    free( p1 );
	    free( p2 );
	    *pp = '=';
	    if ( !c )
		break;
	    *pc = c;
	}
	query = pc+1;
    }
}

void usage()
{
    output( 2, usagestr );
    exit( 1 );
}

int main( int argc, char **argv )
{
    int i;
    for ( i = 1; i < argc; i++ )
    {
	if ( !strcmp( argv[i], "-h" ) || !strcmp( argv[i], "--help" ))
	{
	    output( 1, description );
	    output( 1, usagestr );
	    exit( 0 );
	}
	else if ( '-' == argv[i][0] )
	{
	    char *pc = argv[i];
	    while ( pc && *++pc ) switch ( *pc )
	    {
		case 'c':
		    compile = 1;
		    break;
		case 'e':
		    execute = 1;
		    break;
		case 's':
		    i++;
		    if ( i >= argc )
			usage();
		    shell = argv[i];
		    pc = 0;
		    break;
		case 'f':
		    i++;
		    if ( i >= argc )
			usage();
		    close( 0 );
		    if ( open( argv[i], O_RDONLY ) )
		    {
			output( 2, "File open error\n" );
			exit( errno );
		    }
		    pc = 0;
		    break;
		default:
		    usage();
	    }
	}
	else
	    usage();
    }

    init();

    parseParam( getenv( "QUERY_STRING" ) );
    if ( execute )
    {
	if ( compile || !shell )
	    usage();
	execl( shell, shell, 0 );
	exit( errno );
    }

    if ( !shell )
	shell = "/bin/sh";

    if ( compile )
    {
	output( 1, "#!" );
	output( 1, shell );
	output( 1, "\n" );
    }
    else
	spawnShell();

    readLines();

    toShell();
    close( 1 );

    return child ? waitpid( child, 0, 0 ) : 0;
}
