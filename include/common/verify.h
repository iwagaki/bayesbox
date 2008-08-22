#ifndef VERIFY_H_
#define VERIFY_H_

#include <cassert>
#define VERIFY(x) if (!(x)) { assert(x); abort(); }

#endif // VERIFY_H_
