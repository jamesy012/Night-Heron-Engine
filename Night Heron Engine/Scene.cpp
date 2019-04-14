#include "Scene.h"

#include "Generated/Objects_Night_Heron_Engine.h"

#include "Object.h"

bool Scene::Load_Internal(CMArray<CMString> a_Splits) {
	uint line = 0;
	uint stage = 0;
	while (line < a_Splits.Length() - 1) {
		stage++;
		if (stage == 1) {
			m_Name = a_Splits[line++];
		}
		if (stage == 2) {
			int objects = CMString::StringToInt(a_Splits[line++]);
			CMArray<CMArray<CMString>> splits;
			splits.Reserve(objects);
			CMArray<CMString> Objects;
			int currObject = -1;
			int curLine = 0;
			while (currObject != objects + 1) {
				if (line >= a_Splits.Length()) {
					break;
				}
				CMString currLine = a_Splits[line++];
				if (currLine.Contains("Object:")) {
					currObject++;
					curLine = 0;
					splits.Add(CMArray<CMString>());
					continue;
				}
				curLine++;
				splits[currObject].Add(currLine);
				if (curLine == 2) {
					Objects.Add(currLine);
				}
			}
			currObject = 0;
			for (int i = 0; i < objects; i++) {
				Object* obj = (Object*)GET_OBJ(Objects[i].Get());
				obj->Load_Internal(splits[i]);
				m_Objects.Add(obj);
			}
		}
	}

	return true;
}

CMString Scene::GetData_Internal() {
	CMString data;
	data += m_Name + "\n";

	data += CMString::IntToString(m_Objects.Length()) + '\n';
	for (uint i = 0; i < m_Objects.Length(); i++) {
		data += "Object:\n";
		data += m_Objects[i]->GetData_Internal() + '\n';
	}


	return data;
}

void Scene::AddObject(Object* a_Object) {
	m_Objects.Add(a_Object);
}
