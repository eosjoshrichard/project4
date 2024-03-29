/*
 * BTree.h - declarations for the class BTree.	CS212 project 4
 *
 * Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 * Modified by: Adam Bartholomew and Joshua Richard
 *
 */

#ifndef BTREE_H
#define BTREE_H

#include <string>
using namespace std;
#include "BTreeFile.h"
#include "BTreeBlock.h"

class BTree
{
	public:
	
		// Constructor
		//
		// Parameter: name of file.	 If a file of this name exists, it is
		// opened and the tree stored in it is accessed; else a new file
		// containing an empty tree is created
		
		BTree(string name);
		
		// Insert a new key, value pair into this structure
		//
		// Parameters: key and associated value.  The key may not contain any
		// embedded spaces or control characters.
		
		void insert(string key, string value);
		
		// Lookup a key.  If found, set value to associated value and
		// return true, else leave value unchanged and return false
		
		bool lookup(string key, string & value) const;
		
		// Remove a key and its associated value.  Return true if
		// found and removed, false if not.
		
		bool remove(string key);
		
		// Print the contents of the entire tree to cout for testing/debugging
		// purposes.  
		
		void print() const;

		// Print the contents of a specific block to cout for testing/debugging
		// purposes
		
		void print(BTreeFile::BlockNumber blockNumber) const;
		
		// Destructor
		
		~BTree();
		
	private:

		// Private recursive auxiliary function for lookup()

		bool lookupR(string key, string & value, BTreeFile::BlockNumber blockNumber) const;

		// Private recursive auxiliary function for insert()

		void insertR(string key, string value, BTreeFile::BlockNumber blockNumber, BTreeFile::BlockNumber parentNumber,
		 			 string & splitKey, string & splitValue,  BTreeFile::BlockNumber & rChildSplitBlock,
		 			 bool & doWeSplit);

		BTreeFile & _file;
};

#endif
