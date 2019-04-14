#pragma once

#include "Types.h"
#include "Transform.h"

#include "Interfaces/Saveable.h"

#include "Generated/Objects_Night_Heron_Engine.h"

ADD_OBJ(Object)
class Object : public Saveable {
public:
	Object();
	Object(CMString a_Name);
	Object(CMString a_Name, Transform a_Transform);
	~Object();

	//automate this part with the generated code
	virtual CMString GetObjectClassName() { return "Object"; };

	CMString m_Name;
	Transform m_Transform;

	// Inherited via Saveable
	virtual bool Load_Internal(CMArray<CMString> a_Splits) override;
	virtual CMString GetData_Internal() override;
};
