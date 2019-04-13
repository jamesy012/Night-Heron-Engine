//Do not edit - automated file
#pragma once
#include <algorithm>
#include <map>
#include <iostream>

#define ADD_OBJ(text) //g_factory.register_class<text>(#text)
#define REGISTER_OBJ(text) GENERATED_OBJ::g_factory->register_class<text>(#text);
enum _PROG_OBJ_ENUM {
	_PROG_HASH_Testing = 242207216,
};

//#include "Source.cpp" //Testing 


namespace GENERATED_OBJ{

template <class T> void* constructor() { return (void*)new T(); }

struct GEN_OBJ_FACTORY
{
   typedef void*(*constructor_t)();
   typedef std::map<std::string, constructor_t> map_type;
   map_type m_classes;

   template <class T>
   void register_class(std::string const& n)
   { m_classes.insert(std::make_pair(n, &constructor<T>)); }

   void* construct(std::string const& n)
   {
      map_type::iterator i = m_classes.find(n);
      if (i == m_classes.end()) return 0; // or throw or whatever you want
      return i->second();
   }
};

extern GEN_OBJ_FACTORY* g_factory;

//std::map<unsigned int,void*> OBJ_MAP = {};//	_PROG_HASH_Testing = 242207216,


	unsigned int hash_c_string(const char* p);

	void RegisterClasses();

};
static void* GET_OBJ(const char* a_Obj) {
std::cout << "Get OBJ " << a_Obj << " - " << std::hash<const char*>{}(a_Obj) << " - " << (242207216 == GENERATED_OBJ::hash_c_string(a_Obj)) << "\n";
	return GENERATED_OBJ::g_factory->construct(a_Obj);
}