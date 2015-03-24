#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define pi 3.14159265359

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
	struct complex t = z;
	for (int j = 0; j < i-1; j++){
		t = multComplex(t, z);
	}
	return t;
}

struct complex divComplex(const struct complex z, const struct complex c) {
		return createComplexFromPolar(z.r / c.r, z.phi-c.phi);
}

bool compComplex(const struct complex z, const struct complex c, float comp){
	if (fabs(z.re - c.re) <= comp && fabs(z.im - c.im) <= comp)
		return true;
	return false;
}

__kernel void newtonFraktal(__global const int* res, __global const int* zoom, __global int* offset, __global const double* param, __global int* result, __global int* resType){
	const int x = get_global_id(0) + offset[0];
	const int y = get_global_id(1) + offset[1];
	
	const int xRes = res[0];
	const int yRes = res[1];
	
	const double a = (x - (xRes / 2)) == 0 ? 0 : (double)((x - (double)(xRes / 2)) / zoom[0]);
	const double b = (y - (yRes / 2)) == 0 ? 0 : (double)((y - (double)(yRes / 2)) / zoom[1]);

	struct complex z = createComplexFromKarthes(a, b);
	
	//struct complex c = createComplexFromKarthes(param[0], param[1]);

	struct complex x1 = createComplexFromKarthes(0.7071068, 0.7071068);
	struct complex x2 = createComplexFromKarthes(0.7071068, -0.7071068);
	struct complex x3 = createComplexFromKarthes(-0.7071068, 0.7071068);
	struct complex x4 = createComplexFromKarthes(-0.7071068, -0.7071068);
	
	struct complex f, d;

	resType[x + xRes * y] = 11;

	int i = 0;
	while (i < 6000 && fabs(z.r) < 10000){
		f = addComplexScalar(powComplex(z, 4), 1);
		d = multComplexScalar(powComplex(z, 3), 3);

		z = subComplex(z, divComplex(f, d));
		
		i++;
		if (compComplex(z, x1, 0.0000001)){
			resType[x + xRes * y] = 0;
			break;
		} else if (compComplex(z, x2, 0.0000001)){
			resType[x + xRes * y] = 1;
			break;
		} else if (compComplex(z, x3, 0.0000001)){
			resType[x + xRes * y] = 2;
			break;
		} else if (compComplex(z, x4, 0.0000001)){
			resType[x + xRes * y] = 3;
			break;
		}
	}
	if (fabs(z.r) >= 1000){
		resType[x + xRes * y] = 10;
	}
	result[x + xRes * y] = i;
}

__kernel void getMinMax(__global const int* data, __global const int* length, __global int* minMax){
	const int x = get_global_id(0)*length[0];
	const int xM = get_global_id(0);
	const int y = get_global_id(1)*length[0];

	minMax[xM + y*get_global_size(0)] = min(data[x + y*get_global_size(0)], data[x + y*get_global_size(0) + 1]);
	for (int i = 2; i < length[0]; i++){
		minMax[xM + y*get_global_size(0)] = min(data[x + y*get_global_size(0) + i], minMax[xM + y*get_global_size(0)]);
	}

	minMax[xM + y*get_global_size(0) + 1] = max(data[x + y*get_global_size(0)], data[x + y*get_global_size(0) + 1]);
	for (int i = 2; i < length[0]; i++){
		minMax[xM + y*get_global_size(0) + 1] = max(data[x + y*get_global_size(0) + i], minMax[xM + y*get_global_size(0) + 1]);
	}

}