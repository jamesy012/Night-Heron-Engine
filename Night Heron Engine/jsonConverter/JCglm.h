#pragma once

#include <glm\glm.hpp>
#include "nlohmann/json.hpp"

namespace glm {

	void to_json(nlohmann::json& j, const vec2& p);

	void from_json(const nlohmann::json& j, vec2& p);

	void to_json(nlohmann::json& j, const vec3& p);

	void from_json(const nlohmann::json& j, vec3& p);

	void to_json(nlohmann::json& j, const vec4& p);

	void from_json(const nlohmann::json& j, vec4& p);
}

