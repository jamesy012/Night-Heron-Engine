//Do not edit - automated file
#include "Objects_Generator.h"


namespace GENERATED_OBJ{

	GEN_OBJ_FACTORY* g_factory = new GEN_OBJ_FACTORY();


	unsigned int hash_c_string(const char* p) {
		unsigned int result = 0;
		const size_t prime = 31;
		unsigned int i = 0;
		while(p[i] != 0){
		//for (size_t i = 0; i < s; ++i) {
			result = p[i++] + (result * prime);
		}
		return result;
	}

	void RegisterClasses() {

	}

};