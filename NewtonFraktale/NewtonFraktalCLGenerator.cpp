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
#include "NewtonFraktalCLGenerator.h"
#include <complex>
#include "Polynom.h"
#include "NewtonFraktalGlobals.h"
#include <iostream>
#include "CPUID.h"

using namespace Polycode;

bool NewtonFraktalCLGenerator::useDouble = true;

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
	return NewtonFraktalCLGenerator::useDouble;
}

std::vector<String> NewtonFraktalCLGenerator::getPlatforms() {
	cl_int err = CL_DEVICE_NOT_FOUND;
	cl::vector<cl::Platform> platforms;
	std::vector<String> platformStrs;

	cl::Platform::get(&platforms);

	if (platforms.size() == 0) {
		err = CL_PLATFORM_NOT_FOUND_KHR;
		return std::vector<String>();
	}

	std::string deviceName, vendor;
	for (int i = 0; i < platforms.size(); i++) {
		platforms[i].getInfo(CL_PLATFORM_NAME, &vendor);
		platformStrs.push_back(vendor);
	}

	return platformStrs;
}

std::vector<std::vector<String>> NewtonFraktalCLGenerator::getDevices() {
	cl_int err = CL_DEVICE_NOT_FOUND;
	cl::vector<cl::Platform> platforms;
	std::vector<std::vector<String>> deviceStrs;

	cl::Platform::get(&platforms);

	if (platforms.size() == 0) {
		err = CL_PLATFORM_NOT_FOUND_KHR;
		return std::vector<std::vector<String>>();
	}

	std::string deviceName, vendor;
	for (int i = 0; i < platforms.size(); i++) {
		platforms[i].getInfo(CL_PLATFORM_NAME, &vendor);

		cl::vector<cl::Device> pDevices;
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &pDevices);

		std::vector<String> strs;
		for (int j = 0; j < pDevices.size(); j++) {
			pDevices[j].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
			strs.push_back(deviceName);
		}
		deviceStrs.push_back(strs);
	}
	return deviceStrs;
}

NewtonFraktalCLGenerator::NewtonFraktalCLGenerator(){
	err = CL_DEVICE_NOT_FOUND;

	cl::Platform::get(&platforms);

	if (platforms.size() == 0) {
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
}

NewtonFraktalCLGenerator::~NewtonFraktalCLGenerator() {}

void NewtonFraktalCLGenerator::initCL(int userChoiceP, int userChoice) {
	err = CL_SUCCESS;
	this->status = this->err;
	try {
		std::string deviceName, vendor, extensionsStl;
		bool foundFP64 = false;

		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[userChoiceP])(), 0 };
		context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

		cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

		if (devices.size() == 0) {
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

		//foundFP64 = false;

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

		char* buf = (char*)_aligned_malloc(100 * sizeof(char), MEM_ALIGN);
		char* temp = buf;
		size_t recv_size = 0, total_recv = 0;
		int i = 1;
		while ((recv_size = fread_s(temp, sizeof(char) * 100, sizeof(char), 100, f)) > 0) {
			total_recv += recv_size;
			buf = (char*)_aligned_realloc(buf, total_recv + 100 * sizeof(char), MEM_ALIGN);
			temp = buf + total_recv;
		}
		buf[total_recv] = '\0';

		cl::Program::Sources source(1, std::make_pair(buf, strlen(buf)));
		program = cl::Program(context, source);
		program.build(devices);

		_aligned_free(buf);
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDev);
		this->err = err.err();
		this->status = this->err;
	}
}

void NewtonFraktalCLGenerator::calcZeros(const struct cl_double_complex* params, const struct cl_double_complex* paramsD, const cl_int* paramc, struct cl_double_complex* zerosOut){
	try {
		kernel = cl::Kernel(program, "findZeros", &err);
		//Logger::log("cl_complex size: %d \n", sizeof(struct cl_complex));
		cl_int resZ[] = { 400, 400 };
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_double_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_double_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (resZ[0] * resZ[1]) * sizeof(struct cl_double_complex));

		struct cl_double_complex *zeros = (struct cl_double_complex*)_aligned_malloc(resZ[0] * resZ[1] * sizeof(struct cl_double_complex), MEM_ALIGN);

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
				} else if (compComplex(zerosCompl[j], t, RESOLUTION * 100)) {
					break;
				}
			}
			if (zerosCompl.size() == paramc[0] - 1)
				break;
		}

		_aligned_free(zeros);
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

void NewtonFraktalCLGenerator::calcZeros(const struct cl_float_complex* params, const struct cl_float_complex* paramsD, const cl_int* paramc, struct cl_float_complex* zerosOut) {
	try {
		kernel = cl::Kernel(program, "findZeros", &err);
		//Logger::log("cl_complex size: %d \n", sizeof(struct cl_complex));
		cl_int resZ[] = {400, 400};
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_float_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_float_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (resZ[0] * resZ[1]) * sizeof(struct cl_float_complex));

		struct cl_float_complex *zeros = (struct cl_float_complex*)_aligned_malloc(resZ[0] * resZ[1] * sizeof(struct cl_float_complex), MEM_ALIGN);

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

			size_t s = zerosCompl.size();
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

		_aligned_free(zeros);
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

void NewtonFraktalCLGenerator::runNewton(NewtonFraktal* fraktal) {
	cl_int paramc[2] = { fraktal->getPolynom()->getNumCoefficients(), fraktal->getDerivation()->getNumCoefficients() };
	struct cl_double_complex* zeros_double = nullptr;
	struct cl_float_complex* zeros_float = nullptr;
	cl_float *result_float = nullptr;

	if (!useDouble) {
		result_float = (cl_float*)_aligned_malloc((fraktal->getWidth() * fraktal->getHeight()) * sizeof(cl_float), MEM_ALIGN);
		zeros_float = (struct cl_float_complex*)_aligned_malloc(sizeof(struct cl_float_complex) * (paramc[0] - 1), MEM_ALIGN);
	} else 
		zeros_double = (struct cl_double_complex*)_aligned_malloc(sizeof(struct cl_double_complex) * (paramc[0] - 1), MEM_ALIGN);

	try {
		queue = cl::CommandQueue(context, defaultDev, 0, &err);
		if(useDouble)
			calcZeros(fraktal->getPolynom()->getCLCoefficients(), fraktal->getDerivation()->getCLCoefficients(), paramc, zeros_double);
		else
			calcZeros(fraktal->getPolynom()->getFloatCLCoefficients(), fraktal->getDerivation()->getFloatCLCoefficients(), paramc, zeros_float);

		if (err != CL_SUCCESS)
			return;

		kernel = cl::Kernel(program, "newtonFraktal", &err);

		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer offsetBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));

		cl::Buffer zerosBuf; 
		cl::Buffer paramsBuf;
		cl::Buffer paramsDBuf;
		cl::Buffer zoomBuf;
		cl::Buffer centerBuf;

		if (useDouble) {
			zerosBuf = cl::Buffer(context, CL_MEM_READ_ONLY, (paramc[0] - 1) * sizeof(struct cl_double_complex));
			paramsBuf = cl::Buffer(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_double_complex));
			paramsDBuf = cl::Buffer(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_double_complex));
			zoomBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));
			centerBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));

			queue.enqueueWriteBuffer(zoomBuf, CL_TRUE, 0, 2 * sizeof(cl_double), fraktal->getArea());
			queue.enqueueWriteBuffer(centerBuf, CL_TRUE, 0, 2 * sizeof(cl_double), fraktal->getCenter());
			queue.enqueueWriteBuffer(zerosBuf, CL_TRUE, 0, (paramc[0] - 1) * sizeof(struct cl_double_complex), zeros_double);
			queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, paramc[0] * sizeof(struct cl_double_complex), fraktal->getPolynom()->getCLCoefficients());
			queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, paramc[1] * sizeof(struct cl_double_complex), fraktal->getDerivation()->getCLCoefficients());
		} else {
			zerosBuf = cl::Buffer(context, CL_MEM_READ_ONLY, (paramc[0] - 1) * sizeof(struct cl_float_complex));
			paramsBuf = cl::Buffer(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_float_complex));
			paramsDBuf = cl::Buffer(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_float_complex));
			zoomBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_float));
			centerBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_float));

			cl_float *tmpArea = (cl_float*)_aligned_malloc(sizeof(cl_float) * 2, MEM_ALIGN);
			cl_float *tmpCenter = (cl_float*)_aligned_malloc(sizeof(cl_float) * 2, MEM_ALIGN);

			convertDoubleArrayToFloat(fraktal->getArea(), tmpArea, 2);
			convertDoubleArrayToFloat(fraktal->getCenter(), tmpCenter, 2);

			queue.enqueueWriteBuffer(zoomBuf, CL_TRUE, 0, 2 * sizeof(cl_float), tmpArea);
			queue.enqueueWriteBuffer(centerBuf, CL_TRUE, 0, 2 * sizeof(cl_float), tmpCenter);
			queue.enqueueWriteBuffer(zerosBuf, CL_TRUE, 0, (paramc[0] - 1) * sizeof(struct cl_float_complex), zeros_float);
			queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, paramc[0] * sizeof(struct cl_float_complex), fraktal->getPolynom()->getFloatCLCoefficients());
			queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, paramc[1] * sizeof(struct cl_float_complex), fraktal->getDerivation()->getFloatCLCoefficients());

			_aligned_free(tmpArea);
			_aligned_free(tmpCenter);
		}
		
		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), fraktal->getResolution());
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), paramc);
		

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

		int rounds = 1, pixelPerCall = fraktal->getWidth() * fraktal->getHeight();
		if (fraktal->getWidth() * fraktal->getHeight() > maxPixelPerCall) {
			Logger::log("Low Mem - parting %ld / %ld = ", fraktal->getWidth() * fraktal->getHeight(), maxPixelPerCall);
			rounds = (int)ceil((double)(fraktal->getWidth() * fraktal->getHeight()) / (double)maxPixelPerCall);
			Logger::log("%d <= ", rounds);
			rounds = rounds % 2 == 0 ? rounds : rounds + 1;
			pixelPerCall = fraktal->getWidth() * fraktal->getHeight() / rounds;
			Logger::log("%d ==> %d\n", rounds, pixelPerCall);
		}
		
		if(useDouble)
			outBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_double)));
		else
			outBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_float)));
		typeOutBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_int)));
		itOutBuf = cl::Buffer(context, CL_MEM_WRITE_ONLY, (pixelPerCall * sizeof(cl_int)));

		kernel.setArg(8, outBuf);
		kernel.setArg(9, typeOutBuf);
		kernel.setArg(10, itOutBuf);

		for (int i = 0; i < rounds; i++){
			offset[1] = (fraktal->getHeight() / rounds) * i;
			Logger::log("Launching round %d\n", i+1);

			queue.enqueueWriteBuffer(offsetBuf, CL_TRUE, 0, 2 * sizeof(cl_int), offset);
			kernel.setArg(2, offsetBuf);

			queue.enqueueNDRangeKernel(
				kernel,
				cl::NullRange,
				cl::NDRange(fraktal->getWidth(), fraktal->getHeight() / rounds),
				cl::NullRange);

			queue.finish();

			if(useDouble)
				queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_double), fraktal->getValues() + (fraktal->getWidth() * offset[1]));
			else
				queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_float), result_float + (fraktal->getWidth() * offset[1]));
			queue.enqueueReadBuffer(typeOutBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_int), fraktal->getTypes() + (fraktal->getWidth() * offset[1]));
			queue.enqueueReadBuffer(itOutBuf, CL_TRUE, 0, pixelPerCall * sizeof(cl_int), fraktal->getIterations() + (fraktal->getWidth() * offset[1]));
		}
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		this->err = err.err();
	}
	if (!useDouble) {
		convertFloatArrayToDouble(result_float, fraktal->getValues(), fraktal->getWidth() * fraktal->getHeight());
		_aligned_free(result_float);
		_aligned_free(zeros_float);
	} else 
		_aligned_free(zeros_double);
}

void NewtonFraktalCLGenerator::freeMemory() {
	//if (result_double)
	//	_aligned_free(result_double);
	//if (result_float)
	//	_aligned_free(result_float);
	//if (iterations)
	//	_aligned_free(iterations);
	//if (typeRes)
	//	_aligned_free(typeRes);
}

extern "C" void SseDtoS_(const double* a, float* b);
extern "C" void AvxDtoS_(const double* a, float* b);

extern "C" void SseStoD_(const float* a, double* b);
extern "C" void AvxStoD_(const float* a, double* b);
//extern "C" void AvxDCtoSC_(const cl_double_complex* a, cl_float_complex* b);

void NewtonFraktalCLGenerator::convertDoubleArrayToFloat(const cl_double * in, cl_float* out, int size) {
	//LARGE_INTEGER start, end, freq;
	//long long tookC, tookASM;
	//QueryPerformanceFrequency(&freq);
	//QueryPerformanceCounter(&start);
	//for (int i = 0; i < size; i++) {
	//	out[i] = (cl_float) in[i];
	//}
	//QueryPerformanceCounter(&end);
	//tookC = end.QuadPart - start.QuadPart;
	//tookC *= 1000000;
	//tookC /= freq.QuadPart;

	//QueryPerformanceCounter(&start);
	//if (size % 4 == 0) {
	//	for (int i = 0; i < size; i = i + 4) {
	//		AvxDtoS_(in + i, out + i);
	//	}
	//} else {
	//	for (int i = 0; i < size; i = i + 2) {
	//		SseDtoS_(in + i, out + i);
	//	}
	//}
	int i = 0;
	if (InstructionSet::AVX()) {
		while (i < size) {
			if ((size - i) % 4 == 0) {
				AvxDtoS_(in + i, out + i);
				i += 4;
			} else if ((size - i) % 2 == 0) {
				SseDtoS_(in + i, out + i);
				i += 2;
			} else {
				out[i] = (float)in[i];
				i += 1;
			}
		}
	} else if (InstructionSet::SSE2()) {
		while (i < size) {
			if ((size - i) % 2 == 0) {
				SseDtoS_(in + i, out + i);
				i += 2;
			} else {
				out[i] = (float)in[i];
				i += 1;
			}
		}
	} else {
		while (i < size) {
			out[i] = (float)in[i];
			i += 1;
		}
	}
	
	//QueryPerformanceCounter(&end);
	//tookASM = end.QuadPart - start.QuadPart;
	//tookASM *= 1000000;
	//tookASM /= freq.QuadPart;
	//Logger::log("C: %d, ASM: %d\n", tookC, tookASM);
}

void NewtonFraktalCLGenerator::convertFloatArrayToDouble(const cl_float * in, cl_double * out, int size) {
	int i = 0;
	if (InstructionSet::AVX()) {
		while (i < size) {
			if ((size - i) % 4 == 0) {
				AvxStoD_(in + i, out + i);
				i += 4;
			} else if ((size - i) % 2 == 0) {
				SseStoD_(in + i, out + i);
				i += 2;
			} else {
				out[i] = (double)in[i];
				i += 1;
			}
		}
	} else if (InstructionSet::SSE2()) {
		while (i < size) {
			if ((size - i) % 2 == 0) {
				SseStoD_(in + i, out + i);
				i += 2;
			} else {
				out[i] = (double)in[i];
				i += 1;
			}
		}
	} else {
		while (i < size) {
			out[i] = (double)in[i];
			i += 1;
		}
	}
}

void NewtonFraktalCLGenerator::convertDoubleComplexArrayToFloat(const cl_double_complex * in, cl_float_complex * out, int size) {
	for (int i = 0; i < size; i++) {
		out[i] = createComplexFromKarthes((cl_float) in[i].re, (cl_float) in[i].im);
		//AvxDCtoSC_(in + i, out + i);
	}
}

void NewtonFraktalCLGenerator::convertFloatComplexArrayToDouble(const cl_float_complex * in, cl_double_complex * out, int size) {
	for (int i = 0; i < size; i++) {
		out[i] = createComplexFromKarthes((cl_double) in[i].re, (cl_double) in[i].im);
		//AvxDCtoSC_(in + i, out + i);
	}
}

void __stdcall freed_memory(cl_mem id, void * data) {
	Logger::log("freed mem ID: %d\n", id);
}
