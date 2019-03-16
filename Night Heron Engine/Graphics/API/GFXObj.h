#pragma once

#include "Types.h"

class GFXObj {
public:
	virtual void SetDebugObjName(CMString a_NewName);
	CMString GetDebugObjName() {
		return m_DebugName;
	};

protected:
	virtual void SetDebugObjName_Internal() {};
	CMString m_DebugName;
};