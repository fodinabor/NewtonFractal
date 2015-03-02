#pragma OPENCL EXTENSION cl_khr_fp64 : enable

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

	t.re = cos(t.phi);
	t.im = sin(t.phi);

	return t;
}

struct complex createComplexFromKarthes(double real, double imag){
	struct complex t;
	t.re = real;
	t.im = imag;

	t.phi = atan(imag / real);
	t.r = sqrt(pow(real, 2) + pow(imag, 2));

	return t;
}

struct complex recreateComplexFromKarthes(struct complex t){
	return t = createComplexFromKarthes(t.re, t.im);
}

struct complex recreateComplexFromPolar(struct complex t){
	return t = createComplexFromPolar(t.r, t.phi);
}

struct complex addComplex(const struct complex z, const struct complex c){
	struct complex t;
	t.re = c.re + z.re;
	t.im = c.im + z.re;
	return recreateComplexFromKarthes(t);
}

struct complex subComplex(const struct complex z, const struct complex c){
	struct complex t;
	t.re = z.re - c.re;
	t.im = z.im - c.im;
	return recreateComplexFromKarthes(t);
}

struct complex addComplexScalar(const struct complex z, const double n){
	struct complex t;
	t.re = z.re + n;
	return recreateComplexFromKarthes(t);
}

struct complex subComplexScalar(const struct complex z, const double n){
	struct complex t;
	t.re = z.re - n;
	return recreateComplexFromKarthes(t);
}

struct complex multComplexScalar(const struct complex z, const double n) {
	struct complex t;
	t.re = z.re * n;
	t.im = z.im * n;
	return recreateComplexFromKarthes(t);
}

struct complex multComplex(const struct complex z, const struct complex c) {
	struct complex t;
	t.re = z.re*c.re - z.im*c.re;
	t.im = z.re*c.im + z.im*c.re;
	return recreateComplexFromKarthes(t);
}

struct complex powComplex(const struct complex z, int i){
	struct complex t = z;
	for (int j = 0; j < i; j++){
		t = multComplex(t, z);
	}
}

struct complex divComplex(const struct complex z, const struct complex c) {
	return createComplexFromPolar(z.r / c.r, z.phi - c.phi);
}

__kernel void newtonFraktal(__global const int* res, __global const double* param, __global int* result, __global int* offset){
	const int x = get_global_id(0) + offset[0];
	const int y = get_global_id(1) + offset[1];
	
	const int xRes = res[0];
	const int yRes = res[1];

	struct complex z = createComplexFromKarthes(x - (xRes / 2), y - (yRes / 2));

	struct complex c = createComplexFromKarthes(param[0], param[1]);

	int i = 0;
	while (z.r < 800){
		if (i >= 4000)
			break;
		z = subComplex(z, divComplex(addComplex(addComplex(addComplex(multComplex(powComplex(z, 4), c), multComplex(powComplex(z, 3), c)), multComplex(powComplex(z, 2), c)), c), addComplex(addComplex(multComplexScalar(multComplex(powComplex(z, 3), c), 4), multComplexScalar(multComplex(powComplex(z,2), c), 3)), multComplexScalar(multComplex(c, z), 2))));
		//z-(c*z*z*z*z + c*z*z*z + c*z*z + c) /																																		((c*z*z*z) * 4 + (c*z*z) * 3 + (c*z) * 2);

		i++;
	}
	result[x + res[0] * y] = i;
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