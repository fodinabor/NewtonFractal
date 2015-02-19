#pragma once

#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

class NewtonFraktalCLGeneration {
public:
	NewtonFraktalCLGeneration(cl_double* param);
	~NewtonFraktalCLGeneration();
	
	cl_int* result;
	cl_int err;
};

