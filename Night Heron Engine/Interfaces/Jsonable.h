#pragma once

#include "nlohmann/json_fwd.hpp"

class Jsonable {
public:
	virtual bool LoadData_Internal(nlohmann::json& a_Json) { return false; };
	virtual void SaveData_Internal(nlohmann::json& a_Json) {};
};