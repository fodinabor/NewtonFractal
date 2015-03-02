#pragma once

#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

class NewtonFraktalCLGeneration {
public:
	NewtonFraktalCLGeneration(cl_double* param);
	~NewtonFraktalCLGeneration();
	
	void initCLAndRunNewton(cl_double* param);
	cl_int* getMinMax(int *data);

	cl_int* result;
	cl_int err;

protected:
	cl::Device defaultDev;
	cl::Context context;
	cl::Program program;
};

