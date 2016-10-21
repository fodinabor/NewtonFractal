#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define pi 3.14159265359
#define RESOLUTION 0.0000000001

struct complex {
	double im;
	double re;
	double r;
	double phi;

	char isPolar;
	char isKarthes;
};

struct complex createComplexFromPolar(double _r, double _phi){
	struct complex t;
	t.r = _r;
	t.phi = _phi;
	t.isPolar = true;

	t.re = _r*cos(_phi);
	t.im = _r*sin(_phi);
	t.isKarthes = true;

	return t;
}

struct complex createComplexFromKarthes(double real, double imag){
	struct complex t;
	t.re = real;
	t.im = imag;
	t.isKarthes = true;

	t.phi = atan2(imag, real);
	t.r = sqrt(t.re*t.re + t.im*t.im);
	t.isPolar = true;

	return t;
}

struct complex recreateComplexFromKarthes(struct complex t){
	return createComplexFromKarthes(t.re, t.im);
}

struct complex recreateComplexFromPolar(struct complex t){
	return createComplexFromPolar(t.r, t.phi);
}

struct complex getComplexKarthes(struct complex t) {
	if (t.isKarthes)
		return t;
	else {
		t.re = t.r*cos(t.phi);
		t.im = t.r*sin(t.phi);
		t.isKarthes = true;
		return t;
	}
		//return recreateComplexFromPolar(t);
}

struct complex getComplexPolar(struct complex t) {
	if (t.isPolar)
		return t;
	else {
		t.phi = atan2(t.im, t.re);
		t.r = sqrt(t.re*t.re + t.im*t.im);
		t.isPolar = true;
		return t;
	}
		//return recreateComplexFromKarthes(t);
}

struct complex addComplex(const struct complex a, const struct complex b){
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

	struct complex t;
	t.re = c.re + z.re;
	t.im = c.im + z.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;

	//return createComplexFromKarthes(c.re + z.re, c.im + z.im);
}

struct complex subComplex(const struct complex a, const struct complex b){
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

	struct complex t;
	t.re = z.re - c.re;
	t.im = z.im - c.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;

	//return createComplexFromKarthes(z.re - c.re, z.im - c.im);
}

struct complex addComplexScalar(const struct complex a, const double n){
	struct complex z = getComplexKarthes(a);
	
	struct complex t;
	t.re = z.re + n;
	t.im = z.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;

	//return createComplexFromKarthes(z.re + n,z.im);
}

struct complex subComplexScalar(const struct complex a, const double n){
	struct complex z = getComplexKarthes(a);

	struct complex t;
	t.re = z.re - n;
	t.im = z.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;

	//return createComplexFromKarthes(z.re - n, z.im);
}

struct complex multComplexScalar(const struct complex a, const double n){
	struct complex z = getComplexKarthes(a);

	struct complex t;
	t.re = z.re * n;
	t.im = z.im * n;
	t.isKarthes = true;
	t.isPolar = false;
	return t;

	//return createComplexFromKarthes(z.re * n,z.im * n);
}

struct complex multComplex(const struct complex a, const struct complex b) {
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

	struct complex t;
	t.re = z.re*c.re - z.im*c.im;
	t.im = z.re*c.im + z.im*c.re;
	t.isKarthes = true;
	t.isPolar = false;
	return t;

	//return createComplexFromKarthes(z.re*c.re-z.im*c.im, z.re*c.im+z.im*c.re);
	//return createComplexFromPolar(z.r*c.r, z.phi + c.phi);
}

struct complex powComplex(const struct complex a, int i){
	struct complex z = getComplexPolar(a);

	struct complex t;
	t.r = pow(z.r, (double)i);
	t.phi = z.phi * i;
	t.isKarthes = false;
	t.isPolar = true;
	return t;

	//return createComplexFromPolar(pow(z.r, (double)i), z.phi * i);
}

struct complex divComplex(const struct complex a, const struct complex b) {
	struct complex z = getComplexPolar(a);
	struct complex c = getComplexPolar(b);

	struct complex t;
	t.r = z.r / c.r;
	t.phi = z.phi - c.phi;
	t.isKarthes = false;
	t.isPolar = true;
	return t;
		//return createComplexFromPolar(z.r / c.r, z.phi-c.phi);
}

bool compComplex(const struct complex a, const struct complex b, double comp){
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

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

__kernel void newtonFraktal(__global const int* res, __global const double* zoom, __global const int* offset,
							__global const double* center, __global const struct complex *zeros,
							__global const struct complex *params, __global const struct complex *paramsD, __global const int* paramc,
							__global double* result, __global int* resType, __global int* iterations){
	const int x = get_global_id(0) + offset[0];
	const int y = get_global_id(1) + offset[1];
	
	const int xRes = res[0];
	const int yRes = res[1];
	
	const double a = (double)(zoom[0] * ((double)x - ((double)xRes / 2.0))) / xRes + center[0];
	const double b = (double)(zoom[1] * (((double)yRes / 2.0) - (double)y)) / yRes + center[1];

	struct complex z = createComplexFromKarthes(a, b);
	struct complex f, d, zo;
	resType[x + xRes * y] = -1;

	bool found = false;
	int i = 0;
	while (i < 600 /*&& fabs(z.r) < 100000*/ && !found){
		f = computeFunction(z, params, paramc[0]);
		d = computeFunction(z, paramsD, paramc[1]);

		zo = z;

		z = subComplex(z, divComplex(f, d));
		
		i++;
		
		for (int j = 0; j < paramc[0] - 1; j++){
			if (compComplex(z, zeros[j], RESOLUTION)){
				resType[x + xRes * y] = j;
				result[x + xRes * y] = (log(RESOLUTION) - log(getComplexPolar(subComplex(zo, zeros[j])).r)) / (log(getComplexPolar(subComplex(z, zeros[j])).r) - log(getComplexPolar(subComplex(zo, zeros[j])).r));
				found = true;
				break;
			}
		}

		if (compComplex(z, zo, RESOLUTION/100) && !found){
			resType[x + xRes * y] = -1;
			break;
		}
	}
	/*if (fabs(z.r) >= 100000){
		resType[x + xRes * y] = -2;
	}*/
	iterations[x + xRes * y] = i;
}