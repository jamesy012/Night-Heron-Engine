#include "Object.h"

Object::Object() {
}

Object::Object(CMString a_Name, Transform a_Transform) : m_Name(a_Name), m_Transform(a_Transform) {
}
Object::Object(CMString a_Name) : m_Name(a_Name) {
}

Object::~Object() {
}

bool Object::Load_Internal(CMArray<CMString> a_Splits) {
	uint line = 0;
	uint stage = 0;
	while (line < a_Splits.Length() - 1) {
		stage++;
		if (stage == 1) {
			m_Name = a_Splits[line++];
		}
		if (stage == 1) {
			//object type
			line++;
		}
		if (stage == 2) {
			CMString position = a_Splits[line++];
			CMArray<CMString> split = position.Split(',');
			m_Transform.SetPosition(Vector3(CMString::StringToFloat(split[0]), CMString::StringToFloat(split[1]), CMString::StringToFloat(split[2])));
		}
		if (stage >= 3) {
			line++;
		}
	}

	return true;
}

CMString Object::GetData_Internal() {
	CMString data;
	data += m_Name + "\n";
	data += GetObjectClassName() + "\n";

	data += CMString::FloatToString(m_Transform.m_Position.x) + "," + CMString::FloatToString(m_Transform.m_Position.y) + "," + CMString::FloatToString(m_Transform.m_Position.z) + '\n';
	data += CMString::FloatToString(m_Transform.m_Rotation.x) + "," + CMString::FloatToString(m_Transform.m_Rotation.y) + "," + CMString::FloatToString(m_Transform.m_Rotation.z) + '\n';
	data += CMString::FloatToString(m_Transform.m_Scale.x) + "," + CMString::FloatToString(m_Transform.m_Scale.y) + "," + CMString::FloatToString(m_Transform.m_Scale.z) + '\n';

	return data;
}
