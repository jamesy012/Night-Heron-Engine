#pragma once

#include "CommonTypes.h"

class GFXObj {
public:
	virtual void SetDebugObjName(CMString a_NewName);

protected:
	virtual void SetDebugObjName_Internal() = 0;
	CMString m_DebugName;
};