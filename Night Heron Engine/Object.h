#pragma once

#include "Types.h"
#include "Transform.h"

#include "Generated/Objects_Night_Heron_Engine.h"

ADD_OBJ(Object)
class Object {
public:
	Object();
	Object(CMString a_Name);
	Object(CMString a_Name, Transform a_Transform);
	~Object();

	CMString m_Name;
	Transform m_Transform;
};
