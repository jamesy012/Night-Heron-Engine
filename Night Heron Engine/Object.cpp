#include "Object.h"

Object::Object() {
}

Object::Object(CMString a_Name, Transform a_Transform) : m_Name(a_Name), m_Transform(a_Transform) {
}
Object::Object(CMString a_Name) : m_Name(a_Name) {
}

Object::~Object() {
}
