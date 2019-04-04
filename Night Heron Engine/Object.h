#pragma once

#include "Types.h"
#include "Transform.h"

class Object {
public:
	Object();
	Object(CMString a_Name);
	Object(CMString a_Name, Transform a_Transform);
	~Object();

	CMString m_Name;
	Transform m_Transform;
};
