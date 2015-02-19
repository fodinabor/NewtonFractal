#include <Polycode.h>
#include "NewtonFraktalCLGeneration.h"

#include <iostream>

using namespace Polycode;

NewtonFraktalCLGeneration::NewtonFraktalCLGeneration(cl_double* param){
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
		
		cl_context_properties properties[] =
			{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
		cl::Context context(CL_DEVICE_TYPE_GPU, properties);
		
		cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
		
		cl::Program::Sources source(1, std::make_pair(buf,strlen(buf)));
		cl::Program program_ = cl::Program(context, source);
		program_.build(devices);
		
		cl::Kernel kernel(program_, "newtonFraktal", &err);

		int *res = new int[2];
		res[0] = Services()->getCore()->getXRes(), res[1] = Services()->getCore()->getYRes();
		cl::Buffer resBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(int));
		cl::Buffer paramBuf(context, CL_MEM_READ_ONLY, 2 * sizeof(cl_double));

		result = (cl_int*)calloc(res[0] * res[1], sizeof(cl_int) + 1);
		cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, res[0] * res[1] * sizeof(cl_int) + 1, result);

		cl::CommandQueue queue(context, devices[0], 0, &err);
		cl::Event event;

		queue.enqueueWriteBuffer(resBuf, CL_TRUE, 0, 2 * sizeof(int), res);
		queue.enqueueWriteBuffer(paramBuf, CL_TRUE, 0, 2 * sizeof(double), param);

		kernel.setArg(0, resBuf);
		kernel.setArg(1, paramBuf);
		kernel.setArg(2, outBuf);

		queue.enqueueNDRangeKernel(
			kernel,
			cl::NullRange,
			cl::NDRange(res[0], res[1]),
			cl::NullRange,
			NULL,
			&event);

		queue.finish();

		queue.enqueueReadBuffer(outBuf, CL_TRUE, 0, res[0] * res[1] * sizeof(cl_int) + 1, result);
	}
	catch (cl::Error& err) {
		std::cerr
			 << "ERROR: "
			 << err.what()
			 << "("
			 << err.err()
			 << ")"
			 << std::endl;
		this->err = err.err();
	}
}


NewtonFraktalCLGeneration::~NewtonFraktalCLGeneration()
{
}
