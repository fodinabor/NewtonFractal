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
#include <Polycode.h>
#include "NewtonFraktalCLGeneration.h"
#include <complex>
#include "Polynom.h"
#include <iostream>

#define RESOLUTION 0.0000000001
#define FLOAT_RESOLUTION 0.000001

using namespace Polycode;

bool NewtonFraktalCLGeneration::useDouble = true;

struct cl_double_complex createComplexFromKarthes(cl_double real, cl_double imag){
	struct cl_double_complex t;
	t.re = real;
	t.im = imag;
	t.isKarthes = true;

	t.phi = atan2(imag, real);
	t.r = sqrt(t.re*t.re + t.im*t.im);
	t.isPolar = true;

	return t;
}

struct cl_float_complex createComplexFromKarthes(cl_float real, cl_float imag) {
	struct cl_float_complex t;
	t.re = real;
	t.im = imag;
	t.isKarthes = true;

	t.phi = atan2(imag, real);
	t.r = sqrt(t.re*t.re + t.im*t.im);
	t.isPolar = true;

	return t;
}

bool usingDouble() {
	return NewtonFraktalCLGeneration::useDouble;
}

NewtonFraktalCLGeneration::NewtonFraktalCLGeneration(){
	err = CL_DEVICE_NOT_FOUND;

	cl::Platform::get(&platforms);

	if (platforms.size() == 0){
		err = CL_PLATFORM_NOT_FOUND_KHR;
		return;
	}

	std::string deviceName, vendor;
	for (int i = 0; i < platforms.size(); i++){
		platforms[i].getInfo(CL_PLATFORM_NAME, &vendor);
		platformStrs.push_back(vendor);
		
		cl::vector<cl::Device> pDevices;
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &pDevices);

		std::vector<String> strs;
		for (int j = 0; j < pDevices.size(); j++){
			pDevices[j].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
			strs.push_back(deviceName);
		}
		deviceStrs.push_back(strs);
	}

	result_double = nullptr;
	result_float = nullptr;
	iterations = nullptr;
	typeRes = nullptr;
}

void NewtonFraktalCLGeneration::initCLAndRunNewton(const cl_double* zoom, const cl_int* res, const struct cl_double_complex* params, const struct cl_double_complex* paramsD, const cl_int* paramc, int userChoiceP, int userChoice){
	err = CL_SUCCESS;
	try {
		std::string deviceName, vendor, extensionsStl;
		bool foundFP64 = false;

		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[userChoiceP])(), 0 };
		context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

		cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

		if (devices.size() == 0){
			err = CL_DEVICE_NOT_FOUND;
			return;
		}

		devices[userChoice].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
		devices[userChoice].getInfo<std::string>(CL_DEVICE_VENDOR, &vendor);
		
		Logger::log("[OpenCL] Using Device: %s %s\n", vendor.c_str(), deviceName.c_str());

		defaultDev = devices[userChoice];

		defaultDev.getInfo<std::string>(CL_DEVICE_EXTENSIONS, &extensionsStl);

		String extensionString = extensionsStl;
		std::vector<String> extensions = extensionString.split(" ");
		for (int j = 0; j < extensions.size(); j++) {
			if (extensions[j] == String("cl_khr_fp64")) {
				foundFP64 = true;
				break;
			}
		}

		if (!foundFP64)
			Logger::log("[OpenCL] The Device does not support the cl_khr_fp64 extension. Using single precision.\n");

		useDouble = foundFP64;

		defaultDev.getInfo<cl_ulong>(CL_DEVICE_GLOBAL_MEM_SIZE, &memSize);
		maxPixelPerCall = memSize / 32;

		FILE* f;
		if (useDouble) {
			if (fopen_s(&f, "newton_double.cl", "r") != 0) {
				Logger::log("ERROR: could not open file \"newton_double.cl\"\n");
				return;
			}
		} else {
			if (fopen_s(&f, "newton_float.cl", "r") != 0) {
				Logger::log("ERROR: could not open file \"newton_float.cl\"\n");
				return;
			}
		}

		char* buf = (char*) malloc(100 * sizeof(char));
		char* temp = buf;
		int recv_size = 0, total_recv = 0;
		int i = 1;
		while ((recv_size = fread_s(temp, sizeof(char) * 100, sizeof(char), 100, f)) > 0) {
			total_recv += recv_size;
			buf = (char*) realloc(buf, total_recv + 100 * sizeof(char));
			temp = buf + total_recv;
		}
		buf[total_recv] = '\0';

		cl::Program::Sources source(1, std::make_pair(buf, strlen(buf)));
		program = cl::Program(context, source);
		program.build(devices);

		free(buf);

		if (useDouble) {
			cl_double center[] = {0.0, 0.0};
			runNewton(zoom, res, center, params, paramsD, paramc);
		} else {
			cl_float center[] = {0.0, 0.0};
			
			cl_float *zoom_float = new cl_float[2];
			convertDoubleArrayToFloat(zoom, zoom_float, 2);
			
			cl_float_complex *params_float = (cl_float_complex*)malloc(sizeof(cl_float_complex) * paramc[0]);
			convertDoubleComplexArrayToFloat(params, params_float, paramc[0]);

			cl_float_complex *paramsD_float = (cl_float_complex*) malloc(sizeof(cl_float_complex) * paramc[0]);
			convertDoubleComplexArrayToFloat(paramsD, paramsD_float, paramc[1]);

			runNewton(zoom_float, res, center, params_float, paramsD_float, paramc);
		}

	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDev);
		this->err = err.err();
	}
}

bool compComplex(const std::complex<cl_double> z, const std::complex<cl_double> c, double comp){
	if (fabs(z.real() - c.real()) <= comp && fabs(z.imag() - c.imag()) <= comp)
		return true;
	return false;
}

void NewtonFraktalCLGeneration::calcZeros(const struct cl_double_complex* params, const struct cl_double_complex* paramsD, const cl_int* paramc, struct cl_double_complex* zerosOut){
	try{
		kernel = cl::Kernel(program, "findZeros", &err);
		//Logger::log("cl_complex size: %d \n", sizeof(struct cl_complex));
		cl_int resZ[] = { 400, 400 };
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_double_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_double_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (resZ[0] * resZ[1]) * sizeof(struct cl_double_complex));

		struct cl_double_complex *zeros = (struct cl_double_complex*)calloc(resZ[0] * resZ[1], sizeof(struct cl_double_complex));

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), resZ);
		queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, paramc[0] * sizeof(struct cl_double_complex), params);
		queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, paramc[1] * sizeof(struct cl_double_complex), paramsD);
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), paramc);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, paramsBuf);
		kernel.setArg(2, paramsDBuf);
		kernel.setArg(3, paramcBuf);
		kernel.setArg(4, outBuf);

		queue.enqueueNDRangeKernel(
			kernel,
			cl::NullRange,
			cl::NDRange(resZ[0], resZ[1]),
			cl::NullRange);

		queue.finish();

		queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, resZ[0] * resZ[1] * sizeof(cl_double), zeros);
		
		std::vector<std::complex<cl_double>> zerosCompl;
		for (int i = 0; i < resZ[0] * resZ[1]; i++){
			std::complex<cl_double> t = std::complex<cl_double>(zeros[i].re, zeros[i].im);
			if (t.real() == 100000 && t.imag() == 100000)
				continue;

			int s = zerosCompl.size();
			for (int j = 0; j <= s; j++){
				if (j == zerosCompl.size()){
					zerosCompl.push_back(t);
				} else if (compComplex(zerosCompl[j], t, RESOLUTION)) {
					break;
				}
			}
			if (zerosCompl.size() == paramc[0]-1)
				break;
		}

		free(zeros);
		//zeros = (struct cl_double_complex*)calloc(zerosCompl.size(), sizeof(struct cl_double_complex));
		for (int i = 0; i < zerosCompl.size(); i++){
			zerosOut[i] = createComplexFromKarthes(zerosCompl[i].real(),zerosCompl[i].imag());
			Logger::log("Zero %d: (%f|%f)\n", i, zerosOut[i].re, zerosOut[i].im);
		}

	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDev);
		this->err = err.err();
	}
}

void NewtonFraktalCLGeneration::calcZeros(const struct cl_float_complex* params, const struct cl_float_complex* paramsD, const cl_int* paramc, struct cl_float_complex* zerosOut) {
	try {
		kernel = cl::Kernel(program, "findZeros", &err);
		//Logger::log("cl_complex size: %d \n", sizeof(struct cl_complex));
		cl_int resZ[] = {400, 400};
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_float_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_float_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (resZ[0] * resZ[1]) * sizeof(struct cl_float_complex));

		struct cl_float_complex *zeros = (struct cl_float_complex*)calloc(resZ[0] * resZ[1], sizeof(struct cl_float_complex));

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), resZ);
		queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, paramc[0] * sizeof(struct cl_float_complex), params);
		queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, paramc[1] * sizeof(struct cl_float_complex), paramsD);
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), paramc);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, paramsBuf);
		kernel.setArg(2, paramsDBuf);
		kernel.setArg(3, paramcBuf);
		kernel.setArg(4, outBuf);

		queue.enqueueNDRangeKernel(
			kernel,
			cl::NullRange,
			cl::NDRange(resZ[0], resZ[1]),
			cl::NullRange);

		queue.finish();

		queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, resZ[0] * resZ[1] * sizeof(cl_float), zeros);

		std::vector<std::complex<cl_float>> zerosCompl;
		for (int i = 0; i < resZ[0] * resZ[1]; i++) {
			std::complex<cl_float> t = std::complex<cl_float>(zeros[i].re, zeros[i].im);
			if (t.real() == 100000 && t.imag() == 100000)
				continue;

			int s = zerosCompl.size();
			for (int j = 0; j <= s; j++) {
				if (j == zerosCompl.size()) {
					zerosCompl.push_back(t);
				} else if (compComplex(zerosCompl[j], t, FLOAT_RESOLUTION)) {
					break;
				}
			}
			if (zerosCompl.size() == paramc[0] - 1)
				break;
		}

		free(zeros);
		//zeros = (struct cl_double_complex*)calloc(zerosCompl.size(), sizeof(struct cl_double_complex));
		for (int i = 0; i < zerosCompl.size(); i++) {
			zerosOut[i] = createComplexFromKarthes(zerosCompl[i].real(), zerosCompl[i].imag());
			Logger::log("Zero %d: (%f|%f)\n", i, zerosOut[i].re, zerosOut[i].im);
		}

	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDev);
		this->err = err.err();
	}
}

void NewtonFraktalCLGeneration::runNewton(const cl_double * zoom, const cl_int * res, const cl_double * center, const cl_double_complex * params, const cl_double_complex * paramsD, const cl_int * paramc) {
	if (result_float)
		free(result_float);

	result_double = (cl_double*) calloc((res[0] * res[1]), sizeof(cl_double));
	typeRes = (cl_int*) calloc((res[0] * res[1]), sizeof(cl_int));
	iterations = (cl_int*) calloc((res[0] * res[1]), sizeof(cl_int));

	struct cl_double_complex* zeros = (struct cl_double_complex*)malloc(sizeof(struct cl_double_complex) * (paramc[0] - 1));

	try{
		queue = cl::CommandQueue(context, defaultDev, 0, &err);
		
		calcZeros(params, paramsD, paramc, zeros);
		if (err != CL_SUCCESS)
			return;

		kernel = cl::Kernel(program, "newtonFraktal", &err);

		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zerosBuf(context, CL_MEM_READ_ONLY, (paramc[0] - 1) * sizeof(struct cl_double_complex));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_double_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_double_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer offsetBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zoomBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));
		cl::Buffer centerBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));
		
		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), res);
		queue.enqueueWriteBuffer(zoomBuf, CL_TRUE, 0, 2 * sizeof(cl_double), zoom);
		queue.enqueueWriteBuffer(zerosBuf, CL_TRUE, 0, (paramc[0] - 1) * sizeof(struct cl_double_complex), zeros);
		queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, paramc[0] * sizeof(struct cl_double_complex), params);
		queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, paramc[1] * sizeof(struct cl_double_complex), paramsD);
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), paramc);
		queue.enqueueWriteBuffer(centerBuf, CL_TRUE, 0, 2 * sizeof(cl_double), center);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, zoomBuf);
		kernel.setArg(3, centerBuf);
		kernel.setArg(4, zerosBuf);
		kernel.setArg(5, paramsBuf);
		kernel.setArg(6, paramsDBuf);
		kernel.setArg(7, paramcBuf);

		cl_int* offset = new cl_int[2];
		offset[0] = 0;
		offset[1] = 0;

		cl::Buffer outBuf;
		cl::Buffer typeOutBuf;
		cl::Buffer itOutBuf;

		int rounds = 1, pixelPerCall = res[0] * res[1];
		if (res[0] * res[1] > maxPixelPerCall) {
			Logger::log("Low Mem - parting %ld / %ld = ", res[0] * res[1], maxPixelPerCall);
			rounds = (int)ceil((double)(res[0] * res[1]) / (double)maxPixelPerCall);
			Logger::log("%d <= ", rounds);
			rounds = rounds % 2 == 0 ? rounds : rounds + 1;
			pixelPerCall = res[0] * res[1] / rounds;
			Logger::log("%d ==> %d\n", rounds, pixelPerCall);
		}

		/*Logger::log("memSize: %lu, maxPixelPerCall: %lu, pixelPerCall: %ld, rounds: %d\n", memSize, maxPixelPerCall, pixelPerCall, rounds);*/

		outBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_double)));
		typeOutBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_int)));
		itOutBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_int)));

		//outBuf.setDestructorCallback(&freed_memory);

		kernel.setArg(8, outBuf);
		kernel.setArg(9, typeOutBuf);
		kernel.setArg(10, itOutBuf);

		for (int i = 0; i < rounds; i++){
			offset[1] = (res[1] / rounds) * i;
				Logger::log("Launching round %d\n", i+1);

				queue.enqueueWriteBuffer(offsetBuf, CL_TRUE, 0, 2 * sizeof(cl_int), offset);
				kernel.setArg(2, offsetBuf);

				queue.enqueueNDRangeKernel(
					kernel,
					cl::NullRange,
					cl::NDRange(res[0], res[1] / rounds),
					cl::NullRange);

				queue.finish();

				queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_double), result_double + (res[0] * offset[1]));
				queue.enqueueReadBuffer(typeOutBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_int), typeRes + (res[0] * offset[1]));
				queue.enqueueReadBuffer(itOutBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_int), iterations + (res[0] * offset[1]));
		}
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		this->err = err.err();
	}
	free(zeros);
}

void NewtonFraktalCLGeneration::runNewton(const cl_float * zoom, const cl_int * res, const cl_float * center, const cl_float_complex * params, const cl_float_complex * paramsD, const cl_int * paramc) {
	if (result_double != nullptr)
		free(result_double);

	result_float = (cl_float*) calloc((res[0] * res[1]), sizeof(cl_float));
	typeRes = (cl_int*) calloc((res[0] * res[1]), sizeof(cl_int));
	iterations = (cl_int*) calloc((res[0] * res[1]), sizeof(cl_int));

	struct cl_float_complex* zeros = (struct cl_float_complex*)malloc(sizeof(struct cl_float_complex) * (paramc[0] - 1));

	try {
		queue = cl::CommandQueue(context, defaultDev, 0, &err);

		calcZeros(params, paramsD, paramc, zeros);
		if (err != CL_SUCCESS)
			return;

		kernel = cl::Kernel(program, "newtonFraktal", &err);

		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zerosBuf(context, CL_MEM_READ_ONLY, (paramc[0] - 1) * sizeof(struct cl_float_complex));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_float_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_float_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer offsetBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zoomBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_float));
		cl::Buffer centerBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_float));

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), res);
		queue.enqueueWriteBuffer(zoomBuf, CL_TRUE, 0, 2 * sizeof(cl_float), zoom);
		queue.enqueueWriteBuffer(zerosBuf, CL_TRUE, 0, (paramc[0] - 1) * sizeof(struct cl_float_complex), zeros);
		queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, paramc[0] * sizeof(struct cl_float_complex), params);
		queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, paramc[1] * sizeof(struct cl_float_complex), paramsD);
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), paramc);
		queue.enqueueWriteBuffer(centerBuf, CL_TRUE, 0, 2 * sizeof(cl_float), center);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, zoomBuf);
		kernel.setArg(3, centerBuf);
		kernel.setArg(4, zerosBuf);
		kernel.setArg(5, paramsBuf);
		kernel.setArg(6, paramsDBuf);
		kernel.setArg(7, paramcBuf);

		cl_int* offset = new cl_int[2];
		offset[0] = 0;
		offset[1] = 0;

		cl::Buffer outBuf;
		cl::Buffer typeOutBuf;
		cl::Buffer itOutBuf;

		int rounds = 1, pixelPerCall = res[0] * res[1];
		if (res[0] * res[1] > maxPixelPerCall) {
			Logger::log("Low Mem - parting %ld / %ld = ", res[0] * res[1], maxPixelPerCall);
			rounds = (int) ceil((double) (res[0] * res[1]) / (double) maxPixelPerCall);
			Logger::log("%d <= ", rounds);
			rounds = rounds % 2 == 0 ? rounds : rounds + 1;
			pixelPerCall = res[0] * res[1] / rounds;
			Logger::log("%d ==> %d\n", rounds, pixelPerCall);
		}

		/*Logger::log("memSize: %lu, maxPixelPerCall: %lu, pixelPerCall: %ld, rounds: %d\n", memSize, maxPixelPerCall, pixelPerCall, rounds);*/

		outBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_float)));
		typeOutBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_int)));
		itOutBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_int)));

		//outBuf.setDestructorCallback(&freed_memory);

		kernel.setArg(8, outBuf);
		kernel.setArg(9, typeOutBuf);
		kernel.setArg(10, itOutBuf);

		for (int i = 0; i < rounds; i++) {
			offset[1] = (res[1] / rounds) * i;
			Logger::log("Launching round %d\n", i + 1);

			queue.enqueueWriteBuffer(offsetBuf, CL_TRUE, 0, 2 * sizeof(cl_int), offset);
			kernel.setArg(2, offsetBuf);

			queue.enqueueNDRangeKernel(
				kernel,
				cl::NullRange,
				cl::NDRange(res[0], res[1] / rounds),
				cl::NullRange);

			queue.finish();

			queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_float), result_float + (res[0] * offset[1]));
			queue.enqueueReadBuffer(typeOutBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_int), typeRes + (res[0] * offset[1]));
			queue.enqueueReadBuffer(itOutBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_int), iterations + (res[0] * offset[1]));
		}
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		this->err = err.err();
	}
	free(zeros);
}

void NewtonFraktalCLGeneration::freeMemory() {
	if (result_double)
		free(result_double);
	if (result_float)
		free(result_float);
	if (iterations)
		free(iterations);
	if (typeRes)
		free(typeRes);
}

void NewtonFraktalCLGeneration::convertDoubleArrayToFloat(const cl_double * in, cl_float* out, int size) {	
	for (int i = 0; i < size; i++) {
		out[i] = (cl_float) in[i];
	}
}

void NewtonFraktalCLGeneration::convertDoubleComplexArrayToFloat(const cl_double_complex * in, cl_float_complex * out, int size) {
	for (int i = 0; i < size; i++) {
		out[i] = createComplexFromKarthes((cl_float) in[i].re, (cl_float) in[i].im);
	}
}

NewtonFraktalCLGeneration::~NewtonFraktalCLGeneration()
{
}

void __stdcall freed_memory(cl_mem id, void * data) {
	Logger::log("freed mem ID: %d\n", id);
}
