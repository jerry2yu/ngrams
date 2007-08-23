/*******************************************************************
C++ Package of  Ternary Search Tree
Copyright (C) 2006  Zheyuan Yu

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Read full GPL at http://www.gnu.org/copyleft/gpl.html

Email me at jerryy@gmail.com if you have any question or comment
WebSite: http://www.cs.dal.ca/~zyu

*************************************************************************/

#include "mystring.h"

#define MAX(a,b) a>b ? a : b;
string::string( const char * cstring )
{
	assert ( cstring );
	size_t newlen = strlen ( cstring ) + 1;
	buffer = ( char * )malloc( newlen );
	if ( buffer )
	{
		memcpy ( buffer, cstring, newlen);
		bufferLength = newlen;
		strLength = newlen - 1;
	}
}

string::string( const char ch ) : strLength(1), bufferLength(2)
{
	if ( ( buffer = ( char * ) calloc( 2, 1 ) ) )
	{
		buffer[0]= ch;
	}
}

string::string ( int value ) 
{
	char valueBuffer[32];
	sprintf( valueBuffer, "%d", value );
	strLength = strlen( valueBuffer );
	this->resize( strLength+1 );
	memcpy( buffer, valueBuffer, bufferLength ); 
}

string::string( size_t len, const char chr )
{
	resize( len + 1);
	memset( buffer, chr, len);
	strLength = len;
}

string::string( const string & copy ) 
{
	//	if ( &copy != this )
	//	{
	if ( copy.bufferLength )
	{
		size_t newlen = copy.strLength + 1;
		buffer = (char*) malloc( newlen );
		if ( buffer )
		{
			memcpy ( buffer, copy.buffer, newlen );
			bufferLength = newlen;
			strLength = newlen - 1;
		}
	}
	else
	{
		init(); // initialize the string object
	}

	//	}
}

string::string( const string &str, size_t start, size_t len )
{
	/*if( start > str.length() || start < 0 || len < 0 )
	{
		throw StringIndexOutOfBounds();
		return;
	}
	*/
	assert ( start <= str.length() );
	// if given length is overpass the size of string, use the longest substring available.
	size_t count = str.length() - start;
	if ( len <= count )
	{
		count = len;
	}
	bufferLength = count + 1;
	buffer = ( char* ) malloc( bufferLength );
	memcpy ( buffer, str.c_str() + start, count );
	buffer[ count ] =0;
	strLength = count;
}


// string = operator. Safe when assign own content
const string & string::operator = ( const char * content )
{
	if ( content )
	{
		strLength = strlen (content);
		if ( !bufferLength )
		{
			bufferLength = strLength + 1;
			buffer = ( char * ) malloc( bufferLength );
		}
		else if ( bufferLength <= strLength )
		{
			bufferLength = strLength + 1;
			buffer = ( char * ) realloc( buffer, bufferLength );
		}
		memcpy ( buffer, content, strLength + 1 );
	}
	else
	{
		emptyIt ();
	}
	return *this;
}

const string & string::operator=( const string & copy )
{
	// Prevent copy to self! if copy itself, do nothing.
	if ( &copy != this ) 
	{
		strLength = copy.strLength;
		if ( !bufferLength )
		{
			bufferLength = strLength + 1;
			buffer = ( char * ) malloc( bufferLength );
		}
		else if ( bufferLength <= strLength )
		{
			bufferLength = strLength + 1;
			buffer = ( char * ) realloc( buffer, bufferLength );
		}
		memcpy ( buffer, copy.buffer, strLength + 1 );

/*
		if ( copy.bufferLength > 0 )
		{
			size_t newlen = copy.strLength + 1;
			//char * newstring = (char*)malloc( newlen );
			if ( newstring )
			{
				memcpy ( newstring, copy.buffer, newlen );
				bufferLength = newlen;
				buffer = newstring;
				strLength = copy.strLength;
			}
		}
		*/
	}
	return *this;
}


const string & string::operator=( const char ch )
{
	emptyIt();
	char * newstring = (char*) calloc( 2, 1 );
	newstring[0] = ch;
	if ( newstring )
	{
		bufferLength = 2;
		buffer = newstring;
		strLength = 1;
	}
	return *this;
}

string operator + ( const string & s1, const string & s2 )
{
	return s1 + s2.c_str();
}
string operator + ( const string & s1, const char* s2 )
{
	return string( s1 ) += s2;
}

string operator + (const char * s1, const string & s2)
{
	return string( s1 ) += s2;
}

string operator + ( const string &s1, const char c2 )
{
	return string( s1 ) += c2;
}

inline string operator + ( const char c1, const string &s2)
{

	return string ( c1 ) += s2;
}

// append a const char * to an existing string
string & string::append( const char* str, size_t len )
{
	char * new_string;
	size_t new_alloc, new_size, size_suffix=0;

	// don't use strlen - it can overrun the len passed in!
	// following codes get the char* length
	const char* p = str;

	while ( *p && size_suffix < (unsigned)len )
	{
		++p;
		++size_suffix;
	}
	if ( !size_suffix)
		return *this;

	new_size = length () + size_suffix + 1;
	// check if we need to expand
	if ( new_size > bufferLength )
	{
		// compute new size
		new_alloc = assign_new_size ( new_size );

		// allocate new buffer
		new_string = (char*) malloc( new_alloc );        
		new_string [ 0 ] = 0;

		// copy the previous allocated buffer into this one
		if ( bufferLength && buffer )
			memcpy ( new_string, buffer, length () );

		// append the suffix. It does exist, otherwize we wouldn't be expanding 
		memcpy ( new_string + length (), str, size_suffix );

		// return previsously allocated buffer if any
		if ( bufferLength && buffer )
			free ( buffer );

		// update member variables
		buffer = new_string;
		bufferLength = new_alloc;
	}
	else
	{
		// we know we can safely append the new string
		memcpy ( buffer + length (), str, size_suffix );
	}
	strLength = new_size - 1;
	buffer [ strLength ] = 0;
	return *this;

}

inline string & string::append(char c)
{
	size_t len = length();
	if( len + 1 >= getSize() )
		resize(len + 2);

	buffer[len++] = c;
	strLength =len;
	buffer[len] = 0;
	return *this;
}



char & string::operator[ ]( unsigned k )
{
	assert( k < strLength );
	//if( k < 0 || k >= strLength )
	//	throw StringIndexOutOfBounds( );
	return buffer[ k ];
}

char string::operator[ ]( unsigned k ) const
{
	assert ( k < strLength );
	//if( k < 0 || k >= strLength )
	//	throw StringIndexOutOfBounds( );
	return buffer[ k ];
}

ostream & operator<<( ostream & out, const string & str )
{
	return out << str.c_str();
}

istream & operator>>( istream & in, string & str )
{
	char buf[ string::MAX_LENGTH + 1 ];
	in >> buf;
	if( !in.fail( ) )
		str = buf;
	return in;
}

istream & getline( istream & in, string & str )
{
	char buf[ string::MAX_LENGTH + 1 ];
	in.getline( buf, string::MAX_LENGTH );
	if( !in.fail( ) )
		str = buf;
	return in;
}

void string::reserve ( size_t size )
{
	emptyIt ();
	if (size)
	{
		buffer = (char*)malloc( size );
		if ( buffer )
		{
			bufferLength = size;
			buffer [0] = 0;
			strLength = 0;
		}
	}
}

void string::resize(size_t newSize )
{
	newSize = newSize > 0 ? newSize : 0;
	buffer = ( char* )realloc( buffer, newSize );
	if ( buffer )
	{
	/*	if ( strLength > newSize )
		{
			strLength = newSize > 0 ? newSize - 1 : 0;
		}
		*/
		buffer[ strLength ] = 0;
		bufferLength = newSize;
	}
	else
	{
		init();
	}
}

void string::squeeze ()
{
	this->resize( strLength +1 );
}

inline void string::init()
{
	buffer = NULL;
	bufferLength = strLength = 0;
}

inline void string::insert( size_t start, const char *str )
{
	insert( start, str, strlen( str ) );
}

inline void string::insert( size_t start, const string &str )
{
	insert( start, str.c_str(), str.length() );
}

void string::insert( size_t start, const char *str, size_t len )
{


	if( !str || len<=0 || start < 0 || start > strLength  )
		return;

	if( strLength + len +1 >= bufferLength )
	{
		resize( strLength + len + 1);
	}

	memmove( buffer + start + len, buffer + start, strLength - start );
	memmove( buffer + start, str, len );
	strLength += len;
	return;
}	

string string::toUpper()
{
	string newString( *this );
	if ( !newString.isNull() && !newString.isEmpty() )
	{
		char* ptr = newString.buffer;
		do 
		{
			*ptr = (char) toupper( *ptr );
		}
		while ( *ptr++ );
	}
	return newString;
}

string string::toLower()
{
	string newString( *this );
	if ( !newString.isNull() && !newString.isEmpty() )
	{
		char* ptr = newString.buffer;
		do 
		{
			*ptr = (char) tolower( *ptr );
		}
		while ( *ptr++ );
	}
	return newString;
}

string & string::trimEnd( const char * trimChars )
{
	if ( !trimChars )
		trimChars = " ";

	size_t len = strlen( trimChars );

	if ( !isNull() && !isEmpty() )
	{
		if ( len <= strLength )
		{
			if ( 0 == memcmp( buffer + strLength - len, trimChars, len ) )
			{
				strLength -= len;
				buffer[ strLength ] =0;

			}
		}
	}
	return *this;
}

string & string::trimStart( const char *trimChars )
{

	if ( !trimChars )
		trimChars = " ";

	size_t len = strlen( trimChars );

	if ( !isNull() && !isEmpty() )
	{
		if ( len <= strLength )
		{
			if ( 0 == memcmp( buffer, trimChars, len ) )
			{
				memmove( buffer, buffer + len, strLength - len );
				strLength -= len;
				buffer[ strLength ] = 0;
			}
		}
	}
	return *this;
}

void string::remove( size_t startIndex, size_t count )
{
	if ( startIndex >= strLength )
		return;

	if ( startIndex + count >= strLength || ! count )
	{
		strLength = startIndex;
		buffer [ strLength ] = 0;
		return;
	}		

	memmove( buffer + startIndex, buffer + startIndex + count, strLength - startIndex - count );
	strLength -= count;
	buffer[ strLength ] = 0;
}

string & string::replace ( size_t startIndex, size_t len, const char *replChars, size_t count )
{
	remove( startIndex, len);
	insert( startIndex, replChars, count );
	return *this;
}

string & string::replace(size_t startIndex, size_t len, const string &replStr)
{
	remove( startIndex, len );
	insert( startIndex, replStr );
	return *this;
}

/* Brute Force implementation of string pattern matching*/
/*
int string::indexOf( const char * chars, size_t startIndex, size_t len ) const
{
int ret = -1;
int index = 0;
if( chars && startIndex < strLength )
{
while( len + index <= strLength )
{
if( compare( chars + startIndex, len, index ) == 0)
return index;
++index;
}
}
return ret;
}
*/
int string::getOccurrence( const char * chars ) const
{
	int count = 0, index = 0;
	while ( ( index = indexOf( chars, (size_t)index )  ) != -1 )
	{
		index++;
		count ++;
	}
	return count;
}

int string::indexOf( const char * chars, size_t startIndex, size_t len ) const
{
	int index = -1;
	if( chars && startIndex < strLength && len <= strLength )
	{
		index = this->BoyerMooreSearch( buffer + startIndex, chars, len );
	}
	return index == -1 ? -1 :  index + (int)startIndex;
}

int string::BoyerMooreSearch( const char * str, const char* pattern, size_t len ) const
{
	const int max_chars = 256; // max number of unique chars
	int skip[ max_chars ];

	for( int i=0; i<max_chars; ++i )  skip[i] = (int)len;
	for( int i=0; i<(int)len; ++i ) skip[ (unsigned char)pattern[i] ] = (int)len - i - 1;

	int textIndex;
	int patternIndex;
	int intLen = (int)strlen( str );
	for( textIndex = patternIndex = (int)len-1; patternIndex >= 0; --textIndex, --patternIndex )
	{
		while( str[textIndex] != pattern[patternIndex] ) 
		{
			textIndex += MAX( (int)len - patternIndex, skip[ (int)str[textIndex] ] ); // reposition the text index
			if( textIndex >= intLen ) 
			{
				return -1; // pattern string not found in text string.
			}
			patternIndex = (int)len-1; // repoint to last char of pattern string
		}
	}
	return textIndex + 1;
}
