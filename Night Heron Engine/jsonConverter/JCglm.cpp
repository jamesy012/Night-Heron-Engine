#include "JCglm.h"

void glm::to_json(nlohmann::json & j, const vec2 & p) {
	j = nlohmann::json{ { "x", p.x }, { "y", p.y } };
}

void glm::from_json(const nlohmann::json & j, vec2 & p) {
	j.at("x").get_to(p.x);
	j.at("y").get_to(p.y);
}

void glm::to_json(nlohmann::json & j, const vec3 & p) {
	j = nlohmann::json{ { "x", p.x }, { "y", p.y }, { "z", p.z } };
}

void glm::from_json(const nlohmann::json & j, vec3 & p) {
	j.at("x").get_to(p.x);
	j.at("y").get_to(p.y);
	j.at("z").get_to(p.z);
}

void glm::to_json(nlohmann::json & j, const vec4 & p) {
	j = nlohmann::json{ { "x", p.x }, { "y", p.y }, { "z", p.z }, { "w", p.w } };
}

void glm::from_json(const nlohmann::json & j, vec4 & p) {
	j.at("x").get_to(p.x);
	j.at("y").get_to(p.y);
	j.at("z").get_to(p.z);
	j.at("w").get_to(p.w);
}
