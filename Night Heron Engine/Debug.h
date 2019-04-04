#pragma once

#include <cassert>

#define CMASSERT_MSG(Expression, Message) assert(!(Expression) && Message);
#define CMASSERT(Expression) CMASSERT_MSG(Expression, "");
