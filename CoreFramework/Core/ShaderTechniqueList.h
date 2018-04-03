
#if !defined(__SHADERTECHNIQUELIST_H__)
#define __SHADERTECHNIQUELIST_H__

#include "Godz.h"
#include <vector>

namespace GODZ
{
	struct GODZ_API ShaderTechniqueList
	{
		void AddTechnique(const char* name)
		{
			m_list.push_back(name);
		}

		size_t GetNumTechniques()
		{
			return m_list.size();
		}

		const char* GetTechniqueName(size_t index)
		{
			return m_list[index];
		}

		~ShaderTechniqueList()
		{
			m_list.clear();
		}

	private:
		std::vector<rstring> m_list;		//list of techniques
	};
}

#endif __SHADERTECHNIQUELIST_H__