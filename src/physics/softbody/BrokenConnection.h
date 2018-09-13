#ifndef PHYS_BROKENCONNECTION_H
#define PHYS_BROKENCONNECTION_H
#include "physics/softbody/LatticeLocation.h"
namespace aly {
	namespace softbody {

		// Used only in bookkeeping - remembering fractures
		struct BrokenConnection
		{
			LatticeLocation *a, *b;
			BrokenConnection(LatticeLocation *a, LatticeLocation *b) : a(a), b(b) {}
		};
	}
}
#endif
