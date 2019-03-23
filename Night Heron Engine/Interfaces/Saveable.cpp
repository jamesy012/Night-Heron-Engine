#include "Saveable.h"

bool Saveable::Load() {
	CMString data = Util::LoadTextFromPath(m_FilePath.m_FilePath);
	if (data.Length() < 5) {
		return false;
	}
	CMArray<CMString> splits = data.Split('\n');


	return Load_Internal(splits);
}
