#include <Polycode.h>
#include "NewtonFraktalCLGeneration.h"
#include <complex>
#include "Polynom.h"
#include <iostream>

#define RESOLUTION 0.0000000001

using namespace Polycode;

struct cl_complex createComplexFromKarthes(cl_double real, cl_double imag){
	struct cl_complex t;
	t.re = real;
	t.im = imag;
	t.isKarthes = true;

	t.phi = atan2(imag, real);
	t.r = sqrt(t.re*t.re + t.im*t.im);
	t.isPolar = true;

	return t;
}

NewtonFraktalCLGeneration::NewtonFraktalCLGeneration(){
	err = CL_DEVICE_NOT_FOUND;

	cl::Platform::get(&platforms);

	if (platforms.size() == 0){
		return;
	}

	std::string deviceName, vendor;
	for (int i = 0; i < platforms.size(); i++){
		platforms[i].getInfo(CL_PLATFORM_NAME, &vendor);
		platformStrs.push_back(vendor);
		cl::vector<cl::Device> pDevices;
		//try {
			platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &pDevices);
		//} catch (cl::Error& err){
			//Logger::log("No Device of the specified type found for Platform %s.\n", vendor.c_str());
		//}
		std::vector<String> strs;
		for (int j = 0; j < pDevices.size(); j++){
			pDevices[j].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
			strs.push_back(deviceName);
		}
		deviceStrs.push_back(strs);
	}
}

void NewtonFraktalCLGeneration::initCLAndRunNewton(cl_double* zoom, cl_int* res, struct cl_complex* params, struct cl_complex* paramsD, cl_int* paramc, int userChoiceP, int userChoice){
	this->params = params;
	this->paramsD = paramsD;
	this->paramc = paramc;
	
	this->center = (cl_double*)malloc(2 * sizeof(cl_double*));
	center[0] = 0;
	center[1] = 0;

	FILE* f;
	if (fopen_s(&f, "newton.cl", "r") != 0){
		return;
	}
	char* buf = (char*)malloc(100 * sizeof(char));
	char* temp = buf;
	int recv_size = 0, total_recv = 0;
	int i = 1;
	while ((recv_size = fread_s(temp, sizeof(char) * 100, sizeof(char), 100, f)) > 0){
		total_recv += recv_size;
		buf = (char*)realloc(buf, total_recv + 100 * sizeof(char));
		temp = buf + total_recv;
	}
	buf[total_recv] = '\0';

	err = CL_SUCCESS;
	try {
		std::string deviceName, vendor;

		cl_context_properties properties[] =
		{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[userChoiceP])(), 0 };
		context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

		cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

		if (devices.size() == 0){
			return;
		}

		devices[userChoice].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
		devices[userChoice].getInfo<std::string>(CL_DEVICE_VENDOR, &vendor);

		Logger::log("[OpenCL] Using Device: %s %s\n", vendor.c_str(), deviceName.c_str());

		defaultDev = devices[userChoice];

		cl::Program::Sources source(1, std::make_pair(buf, strlen(buf)));
		program = cl::Program(context, source);
		program.build(devices);

		result = (cl_double*)malloc((res[0] * res[1] + 1) * sizeof(cl_double));
		typeRes = (cl_int*)malloc((res[0] * res[1] + 1)* sizeof(cl_int));
		iterations = (cl_int*)malloc((res[0] * res[1] + 1) * sizeof(cl_int));

		runNewton(zoom, res);

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

void NewtonFraktalCLGeneration::calcZeros(){
	try{

		kernel = cl::Kernel(program, "findZeros", &err);
		//Logger::log("cl_complex size: %d \n", sizeof(struct cl_complex));
		cl_int resZ[] = { 400, 400 };
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, paramc[0] * sizeof(struct cl_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, paramc[1] * sizeof(struct cl_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (resZ[0] * resZ[1]) * sizeof(struct cl_complex));

		zeros = (struct cl_complex*)calloc(resZ[0] * resZ[1], sizeof(struct cl_complex));

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), resZ);
		queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, this->paramc[0] * sizeof(struct cl_complex), this->params);
		queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, this->paramc[1] * sizeof(struct cl_complex), this->paramsD);
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), this->paramc);

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
		this->zeros = (struct cl_complex*)calloc(zerosCompl.size(), sizeof(struct cl_complex));
		for (int i = 0; i < zerosCompl.size(); i++){
			this->zeros[i] = createComplexFromKarthes(zerosCompl[i].real(),zerosCompl[i].imag());
			Logger::log("Zero %d: (%f|%f)\n", i, this->zeros[i].re, this->zeros[i].im);
		}

	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		this->err = err.err();
	}
}

void NewtonFraktalCLGeneration::runNewton(cl_double* zoom, cl_int* res, cl_double* center, struct cl_complex* params, struct cl_complex* paramsD, cl_int* paramc){
	if (paramc != NULL){
		this->params = params;
		this->paramsD = paramsD;
		this->paramc = paramc;
	}
	
	if (center != NULL){
		this->center = center;
	} else {
		center = this->center;
	}

	try{
		queue = cl::CommandQueue(context, defaultDev, 0, &err);
		
		calcZeros();
		if (err != CL_SUCCESS)
			return;

		kernel = cl::Kernel(program, "newtonFraktal", &err);

		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zerosBuf(context, CL_MEM_READ_ONLY, (this->paramc[0] - 1) * sizeof(struct cl_complex));
		cl::Buffer paramsBuf(context, CL_MEM_READ_ONLY, this->paramc[0] * sizeof(struct cl_complex));
		cl::Buffer paramsDBuf(context, CL_MEM_READ_ONLY, this->paramc[1] * sizeof(struct cl_complex));
		cl::Buffer paramcBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer offsetBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zoomBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));
		cl::Buffer centerBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));

		memset(result, 0, (res[0] * res[1] + 1) * sizeof(cl_double));
		memset(typeRes, 0, (res[0] * res[1] + 1)* sizeof(cl_int));
		memset(iterations, 0, (res[0] * res[1] + 1)* sizeof(cl_int));
		
		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), res);
		queue.enqueueWriteBuffer(zoomBuf, CL_TRUE, 0, 2 * sizeof(cl_double), zoom);
		queue.enqueueWriteBuffer(zerosBuf, CL_TRUE, 0, (this->paramc[0] - 1) * sizeof(struct cl_complex), this->zeros);
		queue.enqueueWriteBuffer(paramsBuf, CL_TRUE, 0, this->paramc[0] * sizeof(struct cl_complex), this->params);
		queue.enqueueWriteBuffer(paramsDBuf, CL_TRUE, 0, this->paramc[1] * sizeof(struct cl_complex), this->paramsD);
		queue.enqueueWriteBuffer(paramcBuf, CL_TRUE, 0, 2 * sizeof(cl_int), this->paramc);
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

		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_double)));
		cl::Buffer typeOutBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_int)));
		cl::Buffer itOutBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_int)));
		kernel.setArg(8, outBuf);
		kernel.setArg(9, typeOutBuf);
		kernel.setArg(10, itOutBuf);
		//for (int i = 0; i < 4; i++){
		//offset[0] = (res[0] / 4) * i;
			//for (int j = 0; j < 4; j++){
			//	Logger::log("Launching round %d\n", j + 1 + 4*i);

				//offset[1] = (res[1] / 4) * j;
				queue.enqueueWriteBuffer(offsetBuf, CL_TRUE, 0, 2 * sizeof(cl_int), offset);
				kernel.setArg(2, offsetBuf);

				queue.enqueueNDRangeKernel(
					kernel,
					cl::NullRange,
					cl::NDRange(res[0]/*/4*/, res[1]/*/4*/),
					cl::NullRange);

				queue.finish();
			//}
		//}
		queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_double), result);
		queue.enqueueReadBuffer(typeOutBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_int), typeRes);
		queue.enqueueReadBuffer(itOutBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_int), iterations);
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		this->err = err.err();
	}
}

NewtonFraktalCLGeneration::~NewtonFraktalCLGeneration()
{
}
