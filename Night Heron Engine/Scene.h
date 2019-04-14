#pragma once

#include "Interfaces/Saveable.h"
#include "Types.h"

class Object;

class Scene : public Saveable {
public:
	CMString m_Name;

	// Inherited via Saveable
	virtual bool Load_Internal(CMArray<CMString> a_Splits) override;

	virtual CMString GetData_Internal() override;

	void AddObject(Object* a_Object);
private:
	CMArray<Object*> m_Objects;
};