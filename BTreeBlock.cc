/*
 * BTreeBlock.cc - implementation of methods of class BTreeBlock
 * as declared in BTreeBlock.h
 *
 * Copyright (c) 2001, 2002, 2003 - Russell C. Bjork
 *
 */

#include "BTreeBlock.h"
#include "BTreeFile.h"

BTreeBlock::BTreeBlock()
: _numberOfKeys(0)
{ }

unsigned BTreeBlock::getNumberOfKeys() const
{ return _numberOfKeys; }

string BTreeBlock::getKey(unsigned index) const
{ return _key[index]; }

string BTreeBlock::getValue(unsigned index) const
{ return _value[index]; }

BTreeFile::BlockNumber BTreeBlock::getChild(unsigned index) const
{ return _child[index]; }
		
void BTreeBlock::setNumberOfKeys(unsigned numberOfKeys)
{ _numberOfKeys = numberOfKeys; }

void BTreeBlock::setKey(unsigned index, string key)
{ _key[index] = key; }

void BTreeBlock::setValue(unsigned index, string value)
{ _value[index] = value; }

void BTreeBlock::setChild(unsigned index, BTreeFile::BlockNumber child)
{ _child[index] = child; }
