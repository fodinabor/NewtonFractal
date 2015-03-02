#include <Polycode.h>
#include "NewtonFraktalCLGeneration.h"

#include <iostream>

using namespace Polycode;

NewtonFraktalCLGeneration::NewtonFraktalCLGeneration(cl_double* param){
	initCLAndRunNewton(param);
}

void NewtonFraktalCLGeneration::initCLAndRunNewton(cl_double* param){
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

		cl_context_properties properties[] =
		{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
		context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

		cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

		std::string deviceName, vendor;
		devices[0].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
		devices[0].getInfo<std::string>(CL_DEVICE_VENDOR, &vendor);

		Logger::log("[OpenCL] Using Device: %s %s\n", vendor.c_str(), deviceName.c_str());
		Logger::log("[OpenCL] All Platorms:\n");
		for (int i = 0; i < platforms.size(); i++){
			platforms[i].getInfo(CL_PLATFORM_NAME, &vendor);
			Logger::log("%s Devices:\n", vendor.c_str());
			cl::vector<cl::Device> pDevices;
			platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &pDevices);
			for (int j = 0; j < pDevices.size(); j++){
				pDevices[j].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
				Logger::log("%s\n", deviceName.c_str());
			}
		}

		//if (strchr("NVIDIA", *vendor.c_str())){
		//	if (platforms.size() > 1){
		//		cl_context_properties properties[] =
		//		{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[1])(), 0 };
		//		context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

		//		devices = context.getInfo<CL_CONTEXT_DEVICES>();

		//		std::string deviceName, vendor;
		//		devices[0].getInfo<std::string>(CL_DEVICE_NAME, &deviceName);
		//		devices[0].getInfo<std::string>(CL_DEVICE_VENDOR, &vendor);

		//		Logger::log("[OpenCL] Using Device: %s %s\n", vendor.c_str(), deviceName.c_str());
		//	} else {
		//		err = -1;
		//		return;
		//	}
		//}

		defaultDev = devices[0];

		cl::Program::Sources source(1, std::make_pair(buf, strlen(buf)));
		program = cl::Program(context, source);
		program.build(devices);

		cl::Kernel kernel(program, "newtonFraktal", &err);

		int *res = new int[2];
		res[0] = Services()->getCore()->getXRes(), res[1] = Services()->getCore()->getYRes();
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));
		cl::Buffer paramBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));
		cl::Buffer offsetBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_int));

		result = (cl_int*)calloc(res[0] * res[1], sizeof(cl_int) + 1);

		cl::CommandQueue queue(context, defaultDev, 0, &err);
		cl::Event event = cl::Event();

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(cl_int), res);
		queue.enqueueWriteBuffer(paramBuf, CL_TRUE, 0, 2 * sizeof(cl_double), param);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, paramBuf);

		cl_int* offset = new cl_int[2];

		offset[0] = 0;
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_int)));
		kernel.setArg(2, outBuf);

		for (int i = 0; i < 4; i++){

			offset[1] = (res[1] / 4) * i;
			queue.enqueueWriteBuffer(offsetBuf, CL_TRUE, 0, 2 * sizeof(cl_int), offset);
			kernel.setArg(3, offsetBuf);

			queue.enqueueBarrier();

			queue.enqueueNDRangeKernel(
				kernel,
				cl::NullRange,
				cl::NDRange(res[0], res[1] / 4),
				cl::NullRange,
				NULL,
				&event);

			queue.finish();
		}
		queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_int), result);
	} catch (cl::Error& err) {
		Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
		std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(defaultDev);
		this->err = err.err();
	}
}

cl_int* NewtonFraktalCLGeneration::getMinMax(cl_int* data){
	int *ret = new int[2];
	if (err == CL_SUCCESS){
		try{
			int length = 20;

			cl::Kernel kernel(program, "getMinMax", &err);

			int *res = new int[2];
			res[0] = Services()->getCore()->getXRes(), res[1] = Services()->getCore()->getYRes();

			cl::Buffer lengthBuf(context, CL_MEM_READ_ONLY, sizeof(cl_int));
			
			cl_int* out = (cl_int*)calloc(res[0] * res[1] / length, sizeof(cl_int));
			
			cl::CommandQueue queue(context, defaultDev, 0, &err);

			kernel.setArg(1, lengthBuf);
			
			int rounds = 1;

			while (res[0] * res[1] / (length*rounds) >= 2){

				cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY, (res[0] * res[1] * sizeof(cl_int) / (length * (rounds + 1))));
				cl::Buffer dataBuf(context, CL_MEM_READ_ONLY, (res[0] * res[1] * sizeof(cl_int) / (length * rounds)));

				out = (cl_int*)calloc(res[0] * res[1] / (length * (rounds + 1)), sizeof(cl_int));

				queue.enqueueWriteBuffer(dataBuf, CL_TRUE, 0, (res[0] * res[1] * sizeof(cl_int) / (length * rounds)), data);
			
				kernel.setArg(0, dataBuf);
				kernel.setArg(2, outBuf);

				queue.enqueueWriteBuffer(lengthBuf, CL_TRUE, 0, sizeof(cl_int), &length);

				queue.enqueueNDRangeKernel(
					kernel,
					cl::NullRange,
					cl::NDRange(res[0] * res[1] / (length * rounds)),
					cl::NullRange);

				queue.finish();

				queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, (res[0] * res[1] * sizeof(cl_int) / (length * (rounds + 1))), out);
				free(data);
				data = out;				

				rounds*=2;
			}
			ret = out;
		} catch (cl::Error& err) {
			Logger::log("[OpenCL] ERROR: %s (%d)\n", err.what(), err.err());
			this->err = err.err();
		}
	} else {
		cl_double param[] = { 1, 1 };
		initCLAndRunNewton(param);
	}

	return ret;
}

NewtonFraktalCLGeneration::~NewtonFraktalCLGeneration()
{
}
