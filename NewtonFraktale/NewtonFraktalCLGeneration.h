#pragma once

#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

class NewtonFraktalCLGeneration {
public:
	NewtonFraktalCLGeneration(cl_int* zoom, cl_double* param, cl_int* res);
	~NewtonFraktalCLGeneration();
	
	void initCLAndRunNewton(cl_int* zoom, cl_double* param, cl_int* res);
	void runNewton(cl_int* zoom, cl_double* param, cl_int* res);

	cl_int* result;
	cl_int* typeRes;
	cl_int err;

protected:
	cl::Device defaultDev;
	cl::Context context;
	cl::Program program;
};

