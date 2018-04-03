
#include "GenericReferenceCounter.h"
#include "GodzAtomics.h"
#include "debug.h"

using namespace GODZ;

GenericReferenceCounter::GenericReferenceCounter()
: mCount(0)
{
}

//blocks copying other the other objects reference count
GenericReferenceCounter::GenericReferenceCounter(const GenericReferenceCounter& counter)
: mCount(0)
{
}

GenericReferenceCounter::~GenericReferenceCounter()
{
}

void GenericReferenceCounter::IncrementReferences()
{
	AtomicIncrement(mCount);
}


void GenericReferenceCounter::DecrementReferences()
{
	long newCount = AtomicDecrement(mCount);

	if (newCount <= 0)
	{
		delete this;
	}
}

//This should only be called if object was created on the stack and the owner
//is about to dispose of this object
void GenericReferenceCounter::DecrementStackReferences()
{
	long newCount = AtomicDecrement(mCount);

	//Someone is still holding a ref to this object :/
	godzassert(newCount <= 0);
}

long GenericReferenceCounter::GetNumReferences()
{
	return mCount;
}

void GenericReferenceCounter::ResetReferrers(long value)
{
	AtomicSet(mCount, value);
}




