/*
The MIT License (MIT)

Copyright (c) 2016 By Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_VECTOR
#include <CL/cl.hpp>

class NewtonFraktalCLGeneration {
public:
	NewtonFraktalCLGeneration();
	~NewtonFraktalCLGeneration();
	
	void initCLAndRunNewton(const cl_double* zoom, const cl_int* res, const struct cl_double_complex* params, const struct cl_double_complex* paramsD, const cl_int* paramc, int userChoiceP, int userChoice);
	void calcZeros(const struct cl_double_complex* params, const struct cl_double_complex* paramsD, const cl_int* paramc, struct cl_double_complex* zerosOut);
	void calcZeros(const struct cl_float_complex* params, const struct cl_float_complex* paramsD, const cl_int* paramc, struct cl_float_complex* zerosOut);
	void runNewton(const cl_double* zoom, const cl_int* res, const cl_double* center, const struct cl_double_complex* params, const struct cl_double_complex* paramsD, const cl_int* paramc);
	void runNewton(const cl_float* zoom, const cl_int* res, const cl_float* center, const struct cl_float_complex* params, const struct cl_float_complex* paramsD, const cl_int* paramc);

	void freeMemory();

	cl_double* result_double;
	cl_float* result_float;
	cl_int* iterations;
	cl_int* typeRes;
	cl_int err;

	std::vector<std::vector<Polycode::String>>deviceStrs;
	std::vector<Polycode::String> platformStrs;

	static bool useDouble;

protected:
	cl::Device defaultDev;
	cl::Context context;
	cl::Program program;
	cl::Kernel kernel;
	cl::CommandQueue queue;

	cl::vector<cl::Platform> platforms;

	cl_ulong memSize;
	cl_ulong maxPixelPerCall;

	void convertDoubleArrayToFloat(const cl_double* in, cl_float* out, int size);
	void convertDoubleComplexArrayToFloat(const cl_double_complex* in, cl_float_complex* out, int size);
};

void __stdcall freed_memory(cl_mem id, void* data);
bool usingDouble();
