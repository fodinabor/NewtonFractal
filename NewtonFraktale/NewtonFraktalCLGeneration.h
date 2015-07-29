#pragma once

#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

class NewtonFraktalCLGeneration {
public:
	NewtonFraktalCLGeneration();
	~NewtonFraktalCLGeneration();
	
	void initCLAndRunNewton(cl_double* zoom, cl_int* res, struct cl_complex* params, struct cl_complex* paramsD, cl_int* paramc, int userChoiceP, int userChoice);
	void calcZeros();
	void runNewton(cl_double* zoom, cl_int* res, cl_double* center = NULL, struct cl_complex* params = NULL, struct cl_complex* paramsD = NULL, cl_int* paramc = NULL);

	cl_double* result;
	cl_int* iterations;
	cl_int* typeRes;
	cl_int err;

	//cl_int zerosc;
	struct cl_complex* zeros;
	cl_double* center;

	cl_int* paramc;
	struct cl_complex* params;
	struct cl_complex* paramsD;

	std::vector<std::vector<Polycode::String>>deviceStrs;
	std::vector<Polycode::String> platformStrs;

protected:
	cl::Device defaultDev;
	cl::Context context;
	cl::Program program;
	cl::Kernel kernel;
	cl::CommandQueue queue;

	cl::vector<cl::Platform> platforms;
};

