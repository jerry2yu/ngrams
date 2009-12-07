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

#include "WordNgrams.h"

WordNgrams::WordNgrams( int newNgramN, const char * newInFileName, const char * newOutFileName, const char * newDelimiters, const char * newStopChars ) :
Ngrams( newNgramN, newInFileName, newOutFileName, newDelimiters, newStopChars )
{
	this->addTokens();

}

WordNgrams::~WordNgrams()
{
}

void WordNgrams::addTokens()
{
	// get token string from input file
	string & inFileName = getInFileName();
	FILE * fp = inFileName.length() > 0 ? fopen( inFileName.c_str(), "r" ) : stdin;
	if ( !fp )
	{
		printf("Can not find file %s, use stdio as input.\n", inFileName.c_str() );
		fp = stdin;
	}

	int count = 0;
	int c;
	bool isSpecialChar = false;
	string token;
	token.reserve(256);
	while ( ( c = fgetc( fp ) ) != EOF )
	{
		if ( isDelimiter( c ) || isStopChar ( c ) )
		{
			if ( !isSpecialChar && token.length() >0 )
			{
				this->addToken( token );
				token.empty();
				++count;
				isSpecialChar = true;
			}
			else
			{
				isSpecialChar = false;
			}

		}
		else
		{
			token.append( (char)c );
			isSpecialChar = false;
		}
	}
	if ( token.length() > 0 )
	{
		++count;
		this->addToken( token );
	}
	// special processing need to be done, if less than NGRAM_N tokens in the whole input text.
	if ( count < this->getN() )
	{
		preParse( count );
	}

	/*	int padding = ngramN - count % ngramN;

	for ( int i=0; i< padding; i++)
	{
	this->addToken( "_" );
	}
	*/
	fclose( fp );
}

void WordNgrams::addToken ( const string & token )
{
printf ("WordNgrams::addToken adding %s\n", token.c_str());
	char buff[32];
	char * s = token.isNumber() ?  "<NUMBER>" : token.c_str();
	int index = this->AddToWordTable( s );
	
	if ( strcmp ( s, this->wordTable.getKey (index) ) != 0)
	{
	   printf ("Warning wrong word\n");
	}
	
	this->encodeInteger( index , ENCODE_BASE, buff );
	
	int newIndex = this->decodeInteger (buff, ENCODE_BASE);
	
	if ( index != newIndex)
	{
	   printf ("WARNING wrong index\n");
	}
	
	//printf("wordNgram::addtoken %s.\n", token.c_str() );
	this->Ngrams::addToken( buff );
}

void WordNgrams::preParse( int count )
{
	TokenNode * p, * newHead;
	p = newHead = head;
	string ngram;
	ngram.reserve(256);
	while ( newHead )
	{
		p = newHead;
		ngram.empty();
		for ( unsigned short i = 0; i< count; i++ )
		{
			ngram += p->token;

			this->addNgram( ngram.c_str(), i+1 );
			
			if ( p == tail )
			{
				break;
			}
			else
			{
				ngram += ENCODE_WORD_DELIMITER;
			}
			p = p->next;

		}
		newHead = newHead->next;
	}
}

void WordNgrams::parse()
{
	TokenNode * tokenNode, * newHead = head;
	string ngram;
	ngram.reserve( 256 );
	unsigned short ngramOrder;
	while ( newHead )
	{
	   ngramOrder = 0;
		tokenNode = newHead;
		ngram.empty();
		while ( tokenNode )
		{
			ngram += tokenNode->token;
			++ngramOrder;
			tokenNode = tokenNode->next;
			if ( tokenNode )
			{
				ngram += ENCODE_WORD_DELIMITER;
			}
		}
		
		printf("%d ngram %s.\n", ngramOrder, ngram.c_str() );
		if ( ngramOrder > 0 )
		{
			this->addNgram( ngram.c_str(), ngramOrder );
		}
		newHead = newHead->next;
	}
}


unsigned WordNgrams::AddToWordTable( const char * word )
{
	unsigned id;
	unsigned * value = this->wordTable.getValue( word );
	if ( value )
	{
		id = *value;
	}
	else
	{
		id = wordTable.count();
		wordTable.add( word, id );
	}
	return id;		
}

void WordNgrams::output()
{
	int ngramN = this->getN();
	
	printf("BEGIN OUTPUT\n");
	printf("Total %d unique ngram in %d ngrams.\n", this->count(), this->total() );
	fprintf( stderr, "Total %d unique ngram in %d ngrams.\n", this->count(), this->total() );

	for ( int i=1; i<=ngramN; i++ )
	{
		// Get sorted item list
		Vector< NgramToken * > ngramVector;
		this->getNgrams( ngramVector, i );

		printf("\n%d-GRAMS\n", i);
		printf("Total %d unique ngrams in %d %d-grams.\n", this->count( i ), this->total( i ), i );
		fprintf( stderr, "Total %d unique ngrams in %d %d-grams.\n", this->count( i ), this->total( i ), i );
		printf("------------------------\n");

		size_t count = ngramVector.count();
		ngramVector.sort( INgrams::compareFunction );

		for ( unsigned j=0; j < count; j++ )
		{
			NgramToken * ngramToken = ngramVector[ j ];
			//outputWordNgram( ngramToken.ngram, ngramToken.value.frequency, i );
			printf("%s\t%d\n", ngramToken->ngram.c_str(), ngramToken->value.frequency );
			delete ngramToken;
		}
	}

}

void WordNgrams::decodeWordNgram( const string & ngram, int n, string & decodedNgram )
{
	//printf("outputWordNgram %s.\n", ngram.c_str() );
	int index = 0, loop = 0;
	const char * buff = ngram.c_str();
   //		unsigned char buff[32];
	while ( loop++ < n )
	{
      /*while ( *p != ENCODE_WORD_DELIMITER && *p != ENCODE_NULL )
		{
			buff[ index ++ ] = (unsigned char)*p++;
		}
		++p;
		buff[ index ] = ENCODE_NULL;
		index = 0;
      */
   
      //printf("ID -- %d.\n", this->decodeInteger( buff, ENCODE_BASE ) );
	   decodedNgram += this->wordTable.getKey( this->decodeInteger( ngram.c_str(), ENCODE_BASE ) );
	
	   if ( loop < n )
	   {
		   decodedNgram.append( '_' );
		}
	}
}

void WordNgrams::getNgrams( vector< NgramToken * > & ngramVector, int n )
{

	// Get sorted item list
	Vector< TstItem< NgramValue > * > & itemVector = this->getItems( );
	size_t count = itemVector.count();
	string decodedKey;
	decodedKey.reserve( 256 );
	
	printf ("Total %d ngrams\n", count);
	for ( unsigned i=0; i < count; i++ )
	{
		TstItem< NgramValue > * item = itemVector[i];
		if ( item->value.n == n )
		{
			// decode the key to readable string
			decodedKey.empty();
			this->decodeWordNgram( item->key, n, decodedKey );
			ngramVector.add( new NgramToken( decodedKey, item->value ) );
			//ngramVector.add( NgramToken( decodedKey, item->value ) );
		}
	}
}
