/*
 * BTree.cc - implementation of methods of class BTree as declared in BTree.h.
 *
 * Several of the methods in this file must be implemented by students for
 * cs212 project 4
 *
 * If this file is compiled with the symbol PROFESSOR_VERSION defined,
 * it expects a file of that name to exist, and uses the code from
 * that file in place of selected dummy methods left to be written by students.
 *
 * Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 *
 * Begun Modifications of BTree.cc on April 6th, 2014
 * by Adam Bartholomew and Joshua Richard
 *
 * Finished on April 15th, 2014
 *
 */

/* NOTE TO THE PROFESSOR
 * 
 * All test cases work for insert() except for when promoting a key from a split block
 * and the resulting promotion leads to the need for the parent block to be split.
 */

#include <iostream>
using namespace std;
#include "BTree.h"
#include "BTreeFile.h"
#include "BTreeBlock.h"

BTree::BTree(string name)
: _file(* new BTreeFile(name))
{ }

#ifndef PROFESSOR_VERSION

/* insert()
 *
 * Insert a given key and value into a tree
 *
 * @param string key - key to be inserted into BTree
 * @param string value - value associated with the key
 *
 */
void BTree::insert(string key, string value)
{
	string & lookupValue = value;

	// first check to see if the block is in the tree

	// key already exists in the tree
	if(lookup(key, lookupValue))
	{
		cout << "Key already exists, you cannot add the same key twice." << endl;
	}
	// key doesn't already exist in the tree
	else
	{
		// variables
		BTreeBlock root;

		// tree has no root (tree is empty)
		if(_file.getBlock(_file.getRoot(), root) == false)
		{
			// allocate a new bloack for the root
			BTreeBlock newRoot;
			_file.setRoot(_file.allocateBlock());

			// get the empty root
			_file.getBlock(_file.getRoot(), newRoot);

			// add in the first key, value pair into the tree and set its children
			newRoot.setKey(0, key);
			newRoot.setValue(0, value);
			newRoot.setNumberOfKeys(1);
			newRoot.setChild(0, 0);
			newRoot.setChild(1, 0);

			// put the block back into the tree
			_file.putBlock(_file.getRoot(), newRoot);
		}
		// tree already has a root
		else
		{
			string splitKey = "";
			string splitValue = "";
			BTreeFile::BlockNumber rChildSplitBlock = 0;
			bool doWeSplit = false;
			insertR(key, value, _file.getRoot(), _file.getRoot(), splitKey, splitValue, rChildSplitBlock, doWeSplit);
		}
	}
}

/* insertR()
 *
 * Private recursive auxiliary function for insert().
 *
 * @param string key - key to insert
 * @param string value - value to insert
 * @param BTreeFile::BlockNumber blockNumber - the block number of the block to check the keys of
 * @param BTreeFile::BlockNumber parentNumber - the block number of the parent block to check the keys of
 * @param string & splitKey - key for the splitting point, promoted during a split
 * @param string & splitValue - value for the splitting point, promoted during a split
 * @param BTreeFile::BlockNumber & rChildSplitBlock - it's the r child split block
 * @param bool & doWeSplit - boolean telling the calling method if we need to split the block before it
 *
 */
void BTree::insertR(string key, string value, BTreeFile::BlockNumber blockNumber, BTreeFile::BlockNumber parentNumber,
 					string & splitKey, string & splitValue,  BTreeFile::BlockNumber & rChildSplitBlock,
 					bool & doWeSplit)
{
	// retrieve the block from file
 	BTreeBlock block;
 	_file.getBlock(blockNumber, block);

 	// declare int for use in while loop below
 	unsigned i = 0;

 	// advance in the block until we reach a key greater then the one we are inserting or we
 	// reach the end of the block. this is used to find the correct index location of where
 	// we are inserting in the block.
 	while((i < block.getNumberOfKeys()) && (block.getKey(i) < key))
 		i++;

 	// we have found the correct block and index the key needs to be inserted into,
 	// now manage how the rest of the tree will react to the newly inserted key

	// current block is a leaf (we only insert into blocks that are leaves)
 	if(block.getChild(i) == 0)
 	{
 		// found the correct position within the block to insert the new key
 		if(block.getKey(i) > key)
 		{
 			// go backwards through the tree to move everything to make room for the new key
 			for(unsigned x = block.getNumberOfKeys(); x > i; x--)
 			{
 				// modify key, value, and child for each block that is to be moved
 				block.setKey(x, block.getKey(x-1));
				block.setValue(x, block.getValue(x-1));
				block.setChild(x, block.getChild(x-1));
 			}

 			// insert the new key, value, and child
 			block.setKey(i, key);
 			block.setValue(i, value);
 			block.setChild(i, 0);	 			
 		}
 		// have not found a key greater than the one we are inserting, must be at the end of the block
 		else
 		{
 			// insert the new key, value, and child
 			block.setKey(i, key);
 			block.setValue(i, value);
 			block.setChild(i, 0);
 		}
 		// change the number of keys and set the last child to be empty
 		block.setNumberOfKeys(block.getNumberOfKeys() + 1);
 		block.setChild(block.getNumberOfKeys(), 0);

 		// Block is overfull after the insertion
 		if(block.getNumberOfKeys() == DEGREE)
 			doWeSplit = true;
 		// Block is not overfull after insertion
 		else
 		{
 			doWeSplit = false;
 			_file.putBlock(blockNumber, block);
 		}
 	}
 	// current block is not a leaf, go further into the tree
 	else
 	{
 		// recursive call on the child
 		string splitKey = "";
		string splitValue = "";
		BTreeFile::BlockNumber rChildSplitBlock = 0;
		bool doWeSplit = false;
		insertR(key, value, block.getChild(i), blockNumber, splitKey, splitValue, rChildSplitBlock, doWeSplit);
 	}

 	// Splitting time
 	if(doWeSplit)
 	{
 		// We are splitting the root
 		if(blockNumber == _file.getRoot())
 		{
 			// get index for midpoint key of block
	 		int splitLocation;

	 		// degree is odd
	 		if(DEGREE % 2 != 0)
	 		{
				splitLocation = DEGREE / 2;
	 		}
	 		// degree is even
	 		else
	 		{
				splitLocation = (DEGREE / 2) - 1;
	 		}

	 		// get key and value at the midpoint key of the block
	 		splitKey = block.getKey(splitLocation);
	 		splitValue = block.getValue(splitLocation);

	 		// create two new blocks for the new right child of the root and the new root
	 		BTreeBlock newRootBlock;
	 		BTreeFile::BlockNumber newRootNumber = _file.allocateBlock();
	 		_file.getBlock(newRootNumber, newRootBlock);

	 		BTreeBlock newRightChild;
	 		BTreeFile::BlockNumber newRightChildNumber = _file.allocateBlock();
	 		_file.getBlock(newRightChildNumber, newRightChild);

	 		// use an index integer to keep track of our insertion location in the new block
	 		int newRightChildIndex = 0;

	 		// insert the right side of the block we just inserted into, into
	 		// the new block that we just created
	 		for(int i = splitLocation + 1; i < DEGREE; i++)
	 		{
	 			// insert the key and value into the new block at the correct location
	 			newRightChild.setKey(newRightChildIndex, block.getKey(i));
	 			newRightChild.setValue(newRightChildIndex, block.getValue(i));
	 			newRightChild.setChild(newRightChildIndex, block.getChild(i));
	 			newRightChild.setNumberOfKeys(newRightChild.getNumberOfKeys() + 1);

				// delete the old key and value from the old block at the correct location
	 			block.setNumberOfKeys(block.getNumberOfKeys() - 1);

	 			// advance index integer to insert into the next location next run through this for loop
	 			newRightChildIndex++;
	 		}

	 		rChildSplitBlock = newRightChildNumber;

	 		// Move the split location key into the new root block
	 		newRootBlock.setKey(0, splitKey);
	 		newRootBlock.setValue(0, splitValue);
	 		newRootBlock.setChild(0, blockNumber);
	 		newRootBlock.setChild(1, newRightChildNumber);
	 		newRootBlock.setNumberOfKeys(1);

	 		block.setNumberOfKeys(block.getNumberOfKeys() - 1);

	 		// Put all new blocks back into the file
	 		_file.putBlock(newRootNumber, newRootBlock);
	 		_file.putBlock(blockNumber, block);
	 		_file.putBlock(newRightChildNumber, newRightChild);
	 		_file.setRoot(newRootNumber);

	 		doWeSplit = false;
 		}
 		// We are splitting some other node besides the root
 		else
 		{
 			// get index for midpoint key of block
	 		int splitLocation;

	 		// degree is odd
	 		if(DEGREE % 2 != 0)
	 		{
				splitLocation = DEGREE / 2;
	 		}
	 		// degree is even
	 		else
	 		{
				splitLocation = (DEGREE / 2) - 1;
	 		}

	 		// get key and value at the midpoint key of the block
	 		splitKey = block.getKey(splitLocation);
	 		splitValue = block.getValue(splitLocation);

	 		// create a new block
	 		BTreeBlock newBlock;
	 		BTreeFile::BlockNumber newBlockNumber = _file.allocateBlock();
	 		_file.getBlock(newBlockNumber, newBlock);

	 		// use an index integer to keep track of our insertion location in the new block
	 		int newBlockIndex = 0;

	 		// insert the right side of the block we just inserted into, into
	 		// the new block that we just created
	 		for(int i = splitLocation + 1; i < DEGREE; i++)
	 		{
	 			// insert the key and value into the new block at the correct location
	 			newBlock.setKey(newBlockIndex, block.getKey(i));
	 			newBlock.setValue(newBlockIndex, block.getValue(i));
	 			newBlock.setChild(newBlockIndex, block.getChild(i));
	 			newBlock.setNumberOfKeys(newBlock.getNumberOfKeys() + 1);

	 			// advance index integer to insert into the next location next run through this for loop
	 			newBlockIndex++;
	 		}

	 		// set the last child in the new block
	 		newBlock.setChild(newBlock.getNumberOfKeys(), block.getChild(block.getNumberOfKeys()));

	 		// remove the splitting key from the original block
			block.setNumberOfKeys(block.getNumberOfKeys() - 1);

			// delete the old keys and values from the old block
	 		block.setNumberOfKeys(block.getNumberOfKeys() - newBlockIndex);

	 		// set the reference of the new child to this new block we just created and modified
	 		rChildSplitBlock = newBlockNumber;

	 		// Put back the modified original and newly created block
	 		_file.putBlock(blockNumber, block);
	 		_file.putBlock(newBlockNumber, newBlock);

	 		// modify the parent block
	 		BTreeBlock parentBlock;
	 		_file.getBlock(parentNumber, parentBlock);

	 		unsigned i = 0;
	 		while((i < parentBlock.getNumberOfKeys()) && (parentBlock.getKey(i) < key))
 				i++;

 			// found the correct position within the block to insert the new key
	 		if(parentBlock.getKey(i) > key)
	 		{
	 			// Move over the last child first
	 			parentBlock.setChild(parentBlock.getNumberOfKeys() + 1,
	 								 parentBlock.getChild(parentBlock.getNumberOfKeys()));

	 			// go backwards through the tree to move everything to make room for the new key
	 			for(unsigned x = parentBlock.getNumberOfKeys(); x > i; x--)
	 			{
	 				// modify key, value, and child for each block that is to be moved
	 				parentBlock.setKey(x, parentBlock.getKey(x-1));
					parentBlock.setValue(x, parentBlock.getValue(x-1));
					parentBlock.setChild(x, parentBlock.getChild(x-1));
	 			}

	 			// insert the new key, value, and child
	 			parentBlock.setKey(i, splitKey);
	 			parentBlock.setValue(i, splitValue);
	 			parentBlock.setChild(i+1, rChildSplitBlock);
	 			parentBlock.setNumberOfKeys(parentBlock.getNumberOfKeys() + 1);

	 		}
	 		// have not found a key greater than the one we are inserting, must be at the end of the block
	 		else
	 		{
	 			// insert the new key, value, and child
	 			parentBlock.setKey(i, splitKey);
	 			parentBlock.setValue(i, splitValue);
	 			parentBlock.setChild(i+1, rChildSplitBlock);
	 			parentBlock.setNumberOfKeys(parentBlock.getNumberOfKeys() + 1);
	 		}

	 		// parent block is now overfull after key promotion
	 		if(parentBlock.getNumberOfKeys() == DEGREE)
	 		{
	 			doWeSplit = true;
	 		}
	 		// parent block is not overfull after key promotion
	 		else
	 		{
	 			doWeSplit = false;

	 			// put back the modified parent into the tree
		 		_file.putBlock(parentNumber, parentBlock);
	 		}
 		}
 	}
}

/* lookup()
 *
 * Checks to see if a key exists within the tree
 *
 * @param string key - key in question
 * @param string & value - reference to value to be searched for in BTree
 *
 * @return bool - true if found, false if not
 *
 */
bool BTree::lookup(string key, string & value) const
{
	// Get the root block of the tree
	BTreeBlock root;
	 _file.getBlock(_file.getRoot(), root);

	 // Tree is empty
	 if(root.getNumberOfKeys() == 0)
	 {
	 	return false;
	 }
	 // Tree is not empty
	 else
	 {
		return lookupR(key, value, _file.getRoot());	 	
	 }
}

/* Private recursive auxiliary function for lookup()
 *
 * @param string key - key in question
 * @param string & value - reference to value to be searched for in BTree
 * @param BTreeFile::BlockNumber blockNumber - block number to check the keys of
 *
 * @return bool - true if found, false if not
 *
 */
bool BTree::lookupR(string key, string & value, BTreeFile::BlockNumber blockNumber) const
{
	// Node is NULL
	if(blockNumber == 0)
	{
		return false;
	}
	// Node has stuff in it
	else
	{
		// Get the block from the file
		BTreeBlock block;
		_file.getBlock(blockNumber, block);

		int i = 0;

		// Find the location of where our key belongs
		while(i < block.getNumberOfKeys() && block.getKey(i) < key)
			i++;

		// We found the key in this block
		if(i < block.getNumberOfKeys() && block.getKey(i) == key)
		{
			value = block.getValue(i);
			return true;
		}
		// The key is in a child of this block
		else
		{
			return lookupR(key, value, block.getChild(i));
		}
	}
}

/* remove()
 *
 * This function removes a key from the BTree and then restructures the tree
 * so that it continues to follow the correct BTree format.
 *
 * @param string key - the key we are attempting to remove
 *
 * @return bool - returns true if removing was successful, false if not
 *
 */
bool BTree::remove(string key)
{
	// bonus credit not attempted.

	return false;
}

#else

#define QUOTE(Q) #Q
#define INCLUDE_NAME(X) QUOTE(X)
#include INCLUDE_NAME(PROFESSOR_VERSION)

#endif

void BTree::print() const
{
	cout << "BTree in file ";
	_file.printHeaderInfo();
	cout << endl;
		
	BTreeFile::BlockNumber root = _file.getRoot();
	if (root == 0)
		cout << "Empty tree" << endl;
	else
		_file.printBlock(root, true, 1);
}

void BTree::print(BTreeFile::BlockNumber blockNumber) const
{
	_file.printBlock(blockNumber, false, 1);
}

BTree::~BTree()
{
	delete (& _file);
}