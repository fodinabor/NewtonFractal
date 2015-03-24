#include <Polycode.h>
#include "NewtonFraktalCLGeneration.h"

#include <iostream>

using namespace Polycode;

NewtonFraktalCLGeneration::NewtonFraktalCLGeneration(cl_int* zoom, cl_double* param, cl_int* res){
	initCLAndRunNewton(zoom, param, res);
}

void NewtonFraktalCLGeneration::initCLAndRunNewton(cl_int* zoom, cl_double* param, cl_int* res){
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
		cl::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		if (platforms.size() == 0){
			return;
		}

		std::string deviceName, vendor;
		Logger::log("[OpenCL] All Platorms:\n\n");
		for (int i = 0; i < platforms.size(); i++){
			platforms[i].getInfo(CL_PLATFORM_NAME, &vendor);
			Logger::log("[%d] %s Devices:\n", i, vendor.c_str());
			cl::vector<cl::Device> pDevices;
			platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &pDevices);
			for (int j = 0; j < pDevices.size(); j++){
				pDevices[j].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
				Logger::log("[%d] %s\n", j, deviceName.c_str());
			}
			Logger::log("\n");
		}

		std::cout << "Please enter the platform you would like to use:" << std::endl;

		int userChoice = 0, userChoiceP = 0;
		std::cin >> userChoiceP;
		if (userChoiceP < 0){
			err = -1;
			return;
		}
		if (userChoiceP > platforms.size()){
			userChoiceP = 0;
		}

		std::cout << "Please enter the device you would like to use:" << std::endl;
		std::cin >> userChoice;

		cl::vector<cl::Device> pDevices;
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &pDevices);

		if (userChoice > pDevices.size()){
			userChoice = 0;
		}

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

		runNewton(zoom, param, res);

	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDev);
		this->err = err.err();
	}
}

void NewtonFraktalCLGeneration::runNewton(cl_int* zoom, cl_double* param, cl_int* res){
	try{
		cl::Kernel kernel(program, "newtonFraktal", &err);

		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));
		cl::Buffer offsetBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer zoomBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));

		result = (cl_int*)calloc(res[0] * res[1], sizeof(cl_int) + 1);
		typeRes = (cl_int*)calloc(res[0] * res[1], sizeof(cl_int) + 1);

		cl::CommandQueue queue(context, defaultDev, 0, &err);
		cl::Event event = cl::Event();

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), res);
		queue.enqueueWriteBuffer(zoomBuf, CL_TRUE, 0, 2 * sizeof(cl_int), zoom);
		queue.enqueueWriteBuffer(paramBuf, CL_TRUE, 0, 2 * sizeof(cl_double), param);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, zoomBuf);
		kernel.setArg(3, paramBuf);

		cl_int* offset = new cl_int[2];
		offset[0] = 0;

		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_int)));
		cl::Buffer typeOutBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_int)));
		kernel.setArg(4, outBuf);
		kernel.setArg(5, typeOutBuf);

		for (int i = 0; i < 4; i++){
			offset[0] = (res[0] / 4) * i;
			for (int j = 0; j < 4; j++){
				Logger::log("Launching round %d\n", j + 1 + 4*i);

				offset[1] = (res[1] / 4) * j;
				queue.enqueueWriteBuffer(offsetBuf, CL_TRUE, 0, 2 * sizeof(cl_int), offset);
				kernel.setArg(2, offsetBuf);

				queue.enqueueNDRangeKernel(
					kernel,
					cl::NullRange,
					cl::NDRange(res[0] / 4, res[1] / 4),
					cl::NullRange,
					NULL,
					&event);

				queue.finish();
			}
		}
		queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_int), result);
		queue.enqueueReadBuffer(typeOutBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_int), typeRes);
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		this->err = err.err();
	}
}

NewtonFraktalCLGeneration::~NewtonFraktalCLGeneration()
{
}
