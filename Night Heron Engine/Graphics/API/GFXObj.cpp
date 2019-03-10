#include "GFXObj.h"

void GFXObj::SetDebugObjName(CMString a_NewName) {
	m_DebugName = a_NewName;
	SetDebugObjName_Internal();
}
