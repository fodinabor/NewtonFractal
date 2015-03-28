#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define pi 3.14159265359
#define RESOLUTION 0.00000001

struct complex {
	double im;
	double re;
	double r;
	double phi;
};

struct complex createComplexFromPolar(double _r, double _phi){
	struct complex t;
	t.r = _r;
	t.phi = _phi;

	t.re = _r*cos(_phi);
	t.im = _r*sin(_phi);

	return t;
}

struct complex createComplexFromKarthes(double real, double imag){
	struct complex t;
	t.re = real;
	t.im = imag;

	t.phi = atan2(imag, real);
	t.r = sqrt(t.re*t.re + t.im*t.im);

	return t;
}

struct complex recreateComplexFromKarthes(struct complex t){
	return createComplexFromKarthes(t.re, t.im);
}

struct complex recreateComplexFromPolar(struct complex t){
	return createComplexFromPolar(t.r, t.phi);
}

struct complex addComplex(const struct complex z, const struct complex c){
	return createComplexFromKarthes(c.re + z.re, c.im + z.im);
}

struct complex subComplex(const struct complex z, const struct complex c){
	return createComplexFromKarthes(z.re - c.re, z.im - c.im);
}

struct complex addComplexScalar(const struct complex z, const double n){
	return createComplexFromKarthes(z.re + n,z.im);
}

struct complex subComplexScalar(const struct complex z, const double n){
	return createComplexFromKarthes(z.re - n, z.im);
}

struct complex multComplexScalar(const struct complex z, const double n){
	return createComplexFromKarthes(z.re * n,z.im * n);
}

struct complex multComplex(const struct complex z, const struct complex c) {
	return createComplexFromKarthes(z.re*c.re-z.im*c.im, z.re*c.im+z.im*c.re);
	//return createComplexFromPolar(z.r*c.r, z.phi + c.phi);
}

struct complex powComplex(const struct complex z, int i){
	return createComplexFromPolar(pow(z.r, (double)i), z.phi * i);
}

struct complex divComplex(const struct complex z, const struct complex c) {
		return createComplexFromPolar(z.r / c.r, z.phi-c.phi);
}

bool compComplex(const struct complex z, const struct complex c, double comp){
	if (fabs(z.re - c.re) <= comp && fabs(z.im - c.im) <= comp)
		return true;
	return false;
}

struct complex computeFunction(const struct complex z, __global const struct complex* params, const int paramc){
	struct complex sum = createComplexFromKarthes(0,0);
	struct complex pow;
	for (unsigned int i = 0; i<paramc; i++) {
		pow = powComplex(z, i);
		sum = addComplex(sum, multComplex(pow, params[i]));
	}
	return sum;
}

__kernel void findZeros(__global const int* res, 
						__global const struct complex* params, __global const struct complex* paramsD, __global const int* paramc,
						__global struct complex* out){
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	struct complex z = createComplexFromKarthes(x - res[0] / 2, y - res[1] / 2);
	struct complex zo = z;

	if (z.re == 0 && z.im == 0){
		out[x + y*res[0]] = createComplexFromKarthes(100000, 100000);
		return;
	}

	for (int i = 0; i < 1000; i++){
		zo = z;
		z = subComplex(z, divComplex(computeFunction(z, params, paramc[0]), computeFunction(z, paramsD, paramc[1])));
		if (compComplex(z, zo, RESOLUTION)){
			out[x + y*res[0]] = z;
			return;
		}
	}
	
	out[x + y*res[0]] = createComplexFromKarthes(100000, 100000);
}

__kernel void newtonFraktal(__global const int* res, __global const int* zoom, __global const int* offset,
							__global const double* center, __global const struct complex *zeros,
							__global const struct complex *params, __global const struct complex *paramsD, __global const int* paramc,
							__global int* result, __global int* resType){
	const int x = get_global_id(0) + offset[0];
	const int y = get_global_id(1) + offset[1];
	
	const int xRes = res[0];
	const int yRes = res[1];
	
	const double a = (double)((x - (double)(xRes / 2)) / zoom[0]) + center[0];
	const double b = (double)((y - (double)(yRes / 2)) / zoom[1]) + center[1];

	struct complex z = createComplexFromKarthes(a, b);
	struct complex f, d, zo;

	resType[x + xRes * y] = 11;

	bool found = false;
	int i = 0;
	while (i < 6000 && fabs(z.r) < 10000 && !found){
		f = computeFunction(z, params, paramc[0]);//addComplexScalar(powComplex(z, 4), 1);
		d = computeFunction(z, paramsD, paramc[1]);//multComplexScalar(powComplex(z, 3), 3);

		z = subComplex(z, divComplex(f, d));
		
		i++;
		
		for (int j = 0; j < paramc[0] - 1; j++){
			if (compComplex(z, zeros[j], RESOLUTION)){
				resType[x + xRes * y] = j;
				found = true;
				break;
			}
		}

		if (compComplex(z, zo, RESOLUTION/100)){
			resType[x + xRes * y] = 12;
			break;
		}
		// else if (compComplex(z, x3, 0.0000001)){
		//	resType[x + xRes * y] = 2;
		//	break;
		//} else if (compComplex(z, x4, 0.0000001)){
		//	resType[x + xRes * y] = 3;
		//	break;
		//}
	}
	if (fabs(z.r) >= 10000){
		resType[x + xRes * y] = 12;
	}
	result[x + xRes * y] = i;
}