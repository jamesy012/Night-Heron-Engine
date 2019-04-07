#include "Arguments.h"

#include "Util.h"

void Arguments::Generate(CMString a_Arguments) {
	CMArray<CMString> list = a_Arguments.Split('-');

	for (int i = 0; i < a_Arguments.Length();) {
		CMString arg;
		CMString value;
		char letter = a_Arguments[i];
		if (letter == '-') {
			letter = a_Arguments[++i];
			while (letter != ' ' && letter != 0) {
				arg += Util::ToLower(letter);
				letter = a_Arguments[++i];
			}
			if (letter == 0) {
				m_Arguments.Add({ arg, value });
				break;
			}
			letter = a_Arguments[++i];
			if (letter != '-') {
				while (letter != ' ' && letter != 0) {
					value += letter;
					letter = a_Arguments[++i];
				}
				++i;
			}
			m_Arguments.Add({ arg, value });
		}
	}
}

CMString Arguments::GetArgumentValue(CMString a_Argument) {
	Argument* arg = GetArgument(a_Argument);
	if (arg) {
		return arg->m_Value;
	}
	return CMString();
}

bool Arguments::IsArgument(CMString a_Argument) {
	return GetArgument(a_Argument);
}

Arguments::Argument* Arguments::GetArgument(CMString a_Argument) {
	a_Argument = a_Argument.ToLower();
	for (int i = 0; i < m_Arguments.Length(); i++) {
		if (a_Argument == m_Arguments[i].m_Arg) {
			return &m_Arguments[i];
		}
	}
	return nullptr;
}
