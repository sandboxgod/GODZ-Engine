
#include "GenericObjData.h"


namespace GODZ
{

GenericPackage*				GenericObjData::m_pCurrentPackage=NULL;

size_t						GenericObjData::counter = 0;
atomic_ptr<GenericClass> GenericObjData::m_pClass[MAX_OBJECT_SIZE];

PackageList					GenericObjData::m_packageList;
bool						GenericObjData::mIsClassListBuilt = false;

}
