/* ===========================================================
	GenericNode

	Custom Container Class

	Created Mar 17,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "Godz.h"
#include "GDZArchive.h"

#if !defined(_GENERICNODE_H_)
#define _GENERICNODE_H_

namespace GODZ
{
	template<class T>
	struct GenericNode
	{
		T data;
		struct GenericNode<T> *next;
		struct GenericNode<T> *prev;

		GenericNode()
		{
			next=0;
			prev=0;
			data = 0;
		}

		GenericNode(T pData)
		{
			next=0;
			prev=0;
			data = pData;
		}

		//Removes itself plus the internal data it references.
		~GenericNode()
		{
			//data=0;
		}

		GenericNode* Add(T pData)
		{
			if (next==NULL)
			{
				next=new GenericNode<T>(pData);
				next->prev=this;
				return next;
			}
			else
			{
				return next->Add(pData);
			}
		}

		//drops itself from the linked list w/o removing it's siblings
		void Release()
		{
			if (prev!=NULL)
			{
				prev->next = next;
				if (next!=NULL)
					next->prev=prev;
			}
			else
			{
				if (next!=NULL)
					next->prev=NULL;
			}

			delete this;
		}
	};

	//releases all the nodes in this list
	template<class T>
	void FreeNodeList(GenericNode<T>* pNode)
	{
		while(pNode)
		{
			GenericNode<T> *old = pNode;
			pNode = pNode->next;
			delete old;
		}
	}

	//This container is a linked list + dynamic array rolled into one container. What it does is function
	//similar to a simple linked list initially. Items are 'pushed' unto the stack. However, items can also
	//be randomly accessed. On the first call, the static array is built (if you have not already called
	//BuildArray() to build it). From that point forward you can start accessing using the array operator. 
	//However, if you push or pop anything off the list- the static array has to be rebuilt. 
	//If used properly, you should be able to get better performance out of this container than 
	//other more heavyweight container types.
	template<class T>
	class NodeList
	{
	public:
		NodeList()
		{
			m_pHead=0;
			m_pTail=0;
			m_nNumNodes=0;
			m_arr=0;
		}

		NodeList(const NodeList& pList)
		{
			m_pHead=0;
			m_pTail=0;
			m_nNumNodes=0;
			m_arr=0;


			for(GenericNode<T> *pNode=pList.m_pHead;pNode!=0;pNode=pNode->next)
			{
				Push(pNode->data);
			}

			if (pList.m_arr)
			{
				BuildArray();
			}
		}

		NodeList& operator=(const NodeList& pList)
		{
			Clear();

			for(GenericNode<T> *pNode=pList.m_pHead;pNode!=0;pNode=pNode->next)
			{
				Push(pNode->data);
			}

			if (pList.m_arr)
			{
				BuildArray();
			}

			return *this;
		}

		~NodeList()
		{
			FreeNodeList(m_pHead);

			if (m_arr)
			{
				delete[] m_arr;
				m_arr=0;
			}
		}

		//Builds the internal array. It's a good idea if you plan on using the array operator to try to
		//avoid push/pop things off the stack to optimize performance.
		void BuildArray()
		{
			if (m_arr)
			{
				delete[] m_arr;
				m_arr=0;
			}
			
			m_arr = new T*[m_nNumNodes];
			size_t count = 0;
			for(GenericNode<T> *pNode=m_pHead;pNode!=0;pNode=pNode->next)
			{
				m_arr[count] = &pNode->data;
				count++;
			}
		}

		//Drops all the data contained in this list
		void Clear()
		{			
			GenericNode<T> *pTemp=m_pHead;
			while(pTemp)
			{
				GenericNode<T> *pThis = pTemp;
				pTemp=pTemp->next;
				delete pThis;
			}

			m_pHead=0;
			m_pTail=0;

			if (m_arr) //static array
			{
				delete[] m_arr;
				m_arr=0;
			}

			m_nNumNodes=0; //reset
		}

		bool Contains(T data)
		{
			for(GenericNode<T> *pTemp=m_pHead;pTemp;pTemp=pTemp->next)
			{
				if (pTemp->data == data)
					return true;
			}

			return false;
		}

		//Removes the node at the destination
		bool DropNodeAt(size_t index)
		{
			size_t count=0;
			for(GenericNode<T> *pTemp=m_pHead;pTemp;pTemp=pTemp->next)
			{
				if (index == count)
				{
					m_nNumNodes--;
					if (m_pTail == pTemp)
					{
						m_pTail = pTemp->prev;
					}

					if (pTemp == m_pHead)
					{
						m_pHead = pTemp->next;
					}

					pTemp->Release();

					if (m_arr) //static array
					{
						delete[] m_arr;
						m_arr=0;
					}

					return true;
			
				}

				count++;
			}			

			return 0;
		}

		//Removes the Node that contains the data from the list
		void Drop(T pData)
		{
			for(GenericNode<T> *pTemp=m_pHead;pTemp;pTemp=pTemp->next)
			{
				if (pTemp->data == pData)
				{
					m_nNumNodes--;
					if (m_pTail->data == pData)
					{
						m_pTail = pTemp->prev;
					}

					if (pTemp == m_pHead)
					{
						m_pHead = pTemp->next;
					}

					pTemp->Release();

					if (m_arr) //static array
					{
						delete[] m_arr;
						m_arr=0;
					}

					return;
			
				}
			}
		}
		
		//WARNING: this could be unreliable if someone has called GetFirst() and then called Add() on that
		//node (nodes get added to the list w/o us knowing)
		size_t GetNumItems()
		{
			return this->m_nNumNodes;
		}

		//Returns the first element or NULL if this list is empty
		GenericNode<T>* GetFirst()
		{
			return m_pHead;
		}

		//Returns the last element or NULL if this list is empty
		GenericNode<T>* GetLast()
		{
			return m_pTail;
		}

		//inserts a new node before the node at <pBefore>
		void Insert(T pData, GenericNode<T> *pBefore)
		{
			if (pBefore == m_pHead)
			{
				m_pHead = new GenericNode<T>(pData);
				pBefore->prev = m_pHead; //not first in the list anymore, now second
				m_pHead->next = pBefore;
			}
			else if (pBefore == m_pTail)
			{				
				m_pTail = new GenericNode<T>(pData);
				pBefore->next = m_pTail;
				m_pTail->prev = pBefore;
			}
			else
			{
				GenericNode<T> *pNode = new GenericNode<T>(pData);
				pNode->next = pBefore;
				pNode->prev = pBefore->prev;
				pBefore->prev->next = pNode;
				pBefore->prev = pNode;
			}

			m_nNumNodes++;
			if (m_arr) //static array
			{
				delete[] m_arr;
				m_arr=0;
			}
		}

		//Pushes an item into the list
		void Push(T pData)
		{
			if (m_arr) //static array
			{
				delete[] m_arr;
				m_arr=0;
			}

			m_nNumNodes++;
			if (m_pHead==0)
			{
				m_pHead = new GenericNode<T>(pData);
				m_pTail = m_pHead;
			}
			else
			{
				m_pTail = m_pHead->Add(pData);
			}
		}

		//removes last element from the list. returns the data the node contained.
		T Pop()
		{
			if (m_pTail)
			{
				GenericNode<T>* pTemp = m_pTail;
				T pData = pTemp->data;
				m_pTail = m_pTail->prev;

				if (m_pHead == pTemp)
				{
					m_pHead = 0;
				}
				pTemp->Release();

				if (m_arr) //static array
				{
					delete[] m_arr;
					m_arr=0;
				}

				return pData;
			}

			return 0;
		}

		//Serializes the contents of this container. Only works with primitive types.
		void Serialize(GDZArchive& ar)
		{
			//size_t numNodes = m_nNumNodes; //we have to use a local var because the Push function
			//if (ar.IsSaving())
			//{
				ar << m_nNumNodes;
			//}
			//else
			//{
			//	ar << numNodes;
			//}
			

			GenericNode<T> *pNode = m_pHead;
			for(size_t i=0;i<m_nNumNodes;i++)
			{
				if (!ar.IsSaving())
				{
					//T pData;
					//ar << (pData);
					//Push(pNode->data);

					GenericNode<T> *pNode = new GenericNode<T>();					
					if (!m_pHead)
					{
						m_pHead=pNode;
						m_pTail=pNode;
					}
					else
					{
						pNode->prev = m_pTail;
						m_pTail->next = pNode; //set the old last node to point to new sibling
						m_pTail = pNode;	//this is the new last node
					}	

					//Read in the data last - this way the serialization code can hack into the tail
					//an instantiate the object if they must.
					ar << pNode->data;
				}
				else
				{
					ar << pNode->data;
					pNode=pNode->next;
				}
			}
		}

		//Serializes the contents of this container. Works with any type that implements 
		//the Serialize(GDZArchive&) func. 
		void SerializeList(GDZArchive& ar)
		{
			size_t numNodes = m_nNumNodes; //we have to use a local var because the Push function
			if (ar.IsSaving())
			{
				ar << m_nNumNodes;
			}
			else
			{
				ar << numNodes;
			}
			

			GenericNode<T> *pNode = m_pHead;
			for(size_t i=0;i<numNodes;i++)
			{
				if (!ar.IsSaving())
				{
					T pData;
					//ar << *(GenericObject**)&pData;
					pData.Serialize(ar);
					Push(pData);
				}
				else
				{
					//ar << *(GenericObject**)&pNode->data;
					pNode->data.Serialize(ar);
					pNode=pNode->next;
				}
			}
		}

		//Accesses the member at the array index. 
		T& operator[](size_t index)
		{
			godzassert(index<m_nNumNodes);

			if (!m_arr)
			{
				Warn("Warning: NodeList had to build an array during runtime.\n");

				//ouch performance hit if you're not careful here- time to build that static array
				BuildArray();
			}

			return *m_arr[index];
		}

	private:
		GenericNode<T>* m_pHead;
		GenericNode<T>* m_pTail;
		size_t m_nNumNodes;			//dont ever change this type from size_t- it will break outside serialization code in engine

		T** m_arr;					//static array (updated each time the linked list has changed)
	};
}

#endif