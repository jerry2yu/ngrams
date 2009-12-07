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

#ifndef _WORD_NGRAMS_H_
#define _WORD_NGRAMS_H_

#include "ngrams.h"
/**
* class for all word ngrams related operations
*
* Revisions:
* Feb 18, 2006. Jerry Yu
* Initial implementation
*/
class WordNgrams : public Ngrams
{
	enum { ENCODE_BASE = 254, // base for converting word id
		ENCODE_NULL = 0xFF, // character for NULL
		ENCODE_WORD_DELIMITER = 0xFE, // character for word seperator
		WORD_DELIMITER = '\t' 
	}; 

public:

   /**
    * Constructor
    */
	WordNgrams( int newNgramN, const char * newInFileName, const char * newOutFileName, const char * newDelimiters = Config::getDefaultDelimiters(), const char * newStopChars = Config::getDefaultStopChars() );
   
   /**
    * Destructor
    */
	virtual ~WordNgrams();

	/**
	* readin whole text file and feed in all tokens.
	*/

	void addTokens();

	/**
	* feed a token in, the token will be processed internally to generating ngram
	*
	* the token will be word string, 
	*/

	void addToken ( const string & token );

	/**
	* sort ngrams by frequency/ngram/or both, then output
	*/

	virtual void output();

private:

	TernarySearchTree<unsigned> wordTable; //save all the unique words with a unique id

	// convert number base 10 to a number string in different base 
	// base: max to ENCODE_BASE, we need leave one ascii as end of string sign, one for word seperator
	void encodeInteger( int num, int bas, char* buff )
	{
	int oldNumber = num;
		unsigned short index = 0;
		int rem;
		
		while ( num >= bas )
		{
			rem = num % bas;

			if ( rem == 0 )
			{
				rem = ENCODE_NULL;
			}
			num /= bas;
			sprintf( buff + index++, "%c", rem );
		}
		
		if ( num == 0 )
		{
			num = ENCODE_NULL;
		}
		sprintf( buff + index, "%c", num );
		
		//printf ("%d => %s\n", oldNumber, buff);
		
		int newNumber = this->decodeInteger ( buff, bas );
		
		if (oldNumber!= newNumber)
		{
		   printf ("Warning wrong encoding\n");
		}

	}

	// decode a number string in base x to an integer in base 10
	// bas: max to ENCODE_BASE, we need leave one ascii as end of string sign and one for word seperator
	int decodeInteger( const char * buffer, int bas )
	{
		int num=0, accumulate = 1;
		unsigned short index = 0;
		unsigned char c;
		while ( ( c = buffer[index] ) != 0 )
		{
			//printf("%d.", c );
			if ( index++ > 0 )
			{
				accumulate *= bas;
			}
			num += ( c == ENCODE_NULL ? 0 : c ) * accumulate;
		}
		//printf("\n");
		return num;
	}

	/**
	* Generate ngrams when queue has NGRAM_N - 1 tokens.
	* the token queue need to be processed specially for the first NGRAM_N - 1 tokens
	*/
	void preParse( int count );

	/**
	* Once the queue is full, it will start to pop out ngrams
	* for each new token pushing in
	*/

	void parse();

	/**
	* add each word to the word table
	* the word table is used to generate unique id for each word
	* that id will be further encoded to base 254 to make it more compact
	* before being inserted into ternary search tree
	*/
	unsigned AddToWordTable( const char * word );

	/**
	* convert from id ( encoded ) into word ngram
	*/

	void decodeWordNgram( const string & ngram, int n, string & decodedNgram );

	/** 
	* get ngram list for given n
	*/

	void getNgrams( vector< NgramToken * > & ngramVector, int n );

};
#endif
