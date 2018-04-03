
#include "GodzAtomics.h"
#include "GodzAssert.h"
#include <windows.h>

namespace GODZ
{


long AtomicIncrement(volatile long& value)
{
	return InterlockedIncrement(&value);
}

long AtomicDecrement(volatile long& value)
{
	return InterlockedDecrement(&value);
}

void AtomicSet(volatile long& value, long newValue)
{
	InterlockedExchange(&value, newValue);
}

//////////////////////////////////////////////////////////////////////////////

AtomicInt::AtomicInt()
	: m_value(0)
{
}

AtomicInt::AtomicInt(long temp)
	: m_value(temp)
{
}

AtomicInt& AtomicInt::operator=(long newValue)
{
	InterlockedExchange(&m_value, newValue);
	return *this;
}

AtomicInt::operator long() const
{
	return m_value;
}

// post-fix operator
AtomicInt AtomicInt::operator++(int)
{
	// Return the previous value
	AtomicInt temp = *this;
	InterlockedIncrement(&m_value);
	return temp;
}

// post-fix operator
AtomicInt AtomicInt::operator--(int)
{
	AtomicInt temp = *this;
	InterlockedDecrement(&m_value);
	return temp;
}

// pre-increment 
AtomicInt& AtomicInt::operator++()
{
	//Perform increment operation
	InterlockedIncrement(&m_value);
	return *this;
}

// pre-decrement
AtomicInt& AtomicInt::operator--()
{
	InterlockedDecrement(&m_value);
	return *this;
}
//////////////////////////////////////////////////////////////////////////////

AtomicBool::AtomicBool()
	: m_value(0)
{
}

AtomicBool::AtomicBool(bool temp)
	: m_value(temp)
{
}

AtomicBool& AtomicBool::operator=(bool newValue)
{
	LONG flagInt = newValue ? 1 : 0;
	InterlockedExchange(&m_value, flagInt);
	return *this;
}

AtomicBool::operator bool() const
{
	return m_value == 1 ? true : false ;
}


//////////////////////////////////////////////////////////////////////////////
GodzAtomic::GodzAtomic()
: m_value(0)
{
}



//returns true if this atomic variable can be locked . Returns false if it has
//already been locked
bool GodzAtomic::Set()
{
	//The InterlockedCompareExchange function performs an atomic comparison of the Destination value 
	//with the Comperand value. If the Destination value is equal to the Comperand value, the Exchange value 
	//is stored in the address specified by Destination. Otherwise, no operation is performed.
	//LONG InterlockedCompareExchange(LONG volatile* Destination, LONG Exchange, LONG Comperand);
	return InterlockedCompareExchange(&m_value, 1, 0 ) == 0;
}

//Returns true if this atomic has been set
bool GodzAtomic::IsSet() const
{
	//return InterlockedCompareExchange(&m_value, 1, 1 ) == 1;

	//if the value ever wavered- then we got some multithreading issue on our hands here
	godzassert(m_value < 2 && m_value >= 0);
	return m_value == 1;
}

void GodzAtomic::UnSet()
{
	long ret = InterlockedCompareExchange(&m_value, 0, 1 );
	godzassert(ret == 1); //How can you unset a flag that was not set already? error!
}



SynchronizedBlock::SynchronizedBlock(GodzAtomic& atom)
: m_lock(atom)
{
	while (!m_lock.Set());
}

SynchronizedBlock::~SynchronizedBlock()
{
	m_lock.UnSet();
}


} //namespace GODZ