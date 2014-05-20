/*
 * BTreeBlock.h - declaration of the class BTreeBlock, used as part
 * of actually implementing a B Tree.  CS212 project 4
 *
 * Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 *
 */

#ifndef BTREE_BLOCK_H
#define BTREE_BLOCK_H

#include <string>
using namespace std;
#include "BTreeFile.h"

/*
 * An object of class BTreeBlock is used to hold an in-memory copy of
 * one block of the BTree.	The method BTreeFile::getBlock() is used to
 * copy a block from the disk file to a Block object; and the method
 * BTreeFile::putBlock is used to copy a Block object back to the file.
 *
 * Although a block on disk can hold only DEGREE - 1 keys and associated
 * values, and can store DEGREE block numbers of child blocks,
 * an in-memory copy is allowed to hold one more key, value, and child than
 * this to facilitate splitting.  Of course, an over-full in-memory copy will
 * never be read as such from disk, nor can it be transferred to disk before
 * it is split.
 */
 
class BTreeBlock
{

	public:

		// Constructor - create an empty block
		
		BTreeBlock();
		
		// Accessors for the various instance variables.
		// Parameter: index - index into array of keys, values, children
		
		unsigned getNumberOfKeys() const;
		string getKey(unsigned index) const;
		string getValue(unsigned index) const;
		BTreeFile::BlockNumber getChild(unsigned index) const;
		
		// Mutators for the various instance variables
		// Parameter: index - index into array of keys, values, children
		// Parameter: key/value/child - new value to set in specified slot
		
		void setNumberOfKeys(unsigned numberOfKeys);
		void setKey(unsigned index, string key);
		void setValue(unsigned index, string value);
		void setChild(unsigned index, BTreeFile::BlockNumber child);
		
	private:
	
		unsigned _numberOfKeys;
		string _key[DEGREE];
		string _value[DEGREE];
		BTreeFile::BlockNumber _child[DEGREE + 1];
};

# endif
