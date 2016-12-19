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
	
	void initCLAndRunNewton(cl_double* zoom, cl_int* res, struct cl_complex* params, struct cl_complex* paramsD, cl_int* paramc, int userChoiceP, int userChoice);
	void calcZeros();
	void runNewton(cl_double* zoom, cl_int* res, cl_double* center = NULL, struct cl_complex* params = NULL, struct cl_complex* paramsD = NULL, cl_int* paramc = NULL);

	void free_memory();

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

	cl_ulong memSize;
	cl_ulong maxPixelPerCall;
};

void __stdcall freed_memory(cl_mem id, void* data);
