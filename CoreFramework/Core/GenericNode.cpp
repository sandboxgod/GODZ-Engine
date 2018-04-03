
#include "GenericNode.h"
#include<tchar.h>

using namespace GODZ;

/*
void GODZ::FreeNodeList(GenericNode* pNode)
{
	while(pNode)
	{
		GenericNode *old = pNode;
		pNode = pNode->next;
		delete old;
	}
}

NodeList::NodeList()
{
	m_pHead=0;
	m_pTail=0;
	this->m_nNumNodes=0;
}

NodeList::~NodeList()
{
	FreeNodeList(m_pHead);
}

void NodeList::Push(void *pData)
{
	m_nNumNodes++;
	if (m_pHead==0)
	{
		m_pHead = new GenericNode(pData);
		m_pTail = m_pHead;
	}
	else
	{
		m_pTail = m_pHead->Add(pData);
	}
}

void NodeList::Drop(void *pData)
{
	for(GenericNode *pTemp=m_pHead;pTemp;pTemp=pTemp->next)
	{
		if (pTemp->data == pData)
		{
			m_nNumNodes--;
			if (m_pTail == pData)
			{
				m_pTail = pTemp->prev;
			}

			if (pTemp == m_pHead)
			{
				m_pHead = pTemp->next;
			}

			pTemp->Release();
			return;
			
		}
	}
}

GenericNode* NodeList::GetFirst()
{
	return m_pHead;
}

GenericNode* NodeList::GetLast()
{
	return m_pTail;
}

udword NodeList::GetNumItems()
{
	return this->m_nNumNodes;
}

void* NodeList::Pop()
{
	if (m_pTail)
	{
		GenericNode* pTemp = m_pTail;
		void* pData = pTemp->data;
		m_pTail = m_pTail->prev;

		if (m_pHead == pTemp)
		{
			m_pHead = 0;
		}
		pTemp->Release();

		return pData;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

GenericNode::GenericNode(void *pkg)
		: next(NULL), prev(NULL), data(NULL) //,bReleased(0)
{
	data=pkg;
}

GenericNode::~GenericNode()
{
	//delete data;
	data=0;
}

GenericNode* GenericNode::Add(void *gp)
{
	if (next==NULL)
	{
		next=new GenericNode(gp);
		next->prev=this;
		return next;
	}
	else
	{
		return next->Add(gp);
	}
}

void GenericNode::Release()
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

	//bReleased=true;
	delete this;
}
*/