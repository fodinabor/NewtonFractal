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

struct complex divComplex(const struct complex z, const struct complex c) {
	return createComplexFromPolar(z.r / c.r, z.phi - c.phi);
}

__kernel void newtonFraktal(__global const int* res, __global const double* param, __global const int* round, __global int* result){
	const int x = get_global_id(0)+res[0]*round[0];
	const int y = get_global_id(1)+res[1]*round[0];
	
	const int xRes = res[0];
	const int yRes = res[1];

	struct complex z = createComplexFromKarthes(x - (xRes / 2), y - (yRes / 2));

	struct complex c = createComplexFromKarthes(param[0], param[1]);

	int i = 0;
	while (z.r < 500){
		if (i >= 10000)
			break;
		z = subComplex(z, divComplex(addComplex(addComplex(multComplex(multComplex(z,z),c),multComplex(z,c)),c),addComplex(multComplexScalar(multComplex(z,c),2),c)));
		//z-(c*z*z + c*z + c) / ((c*z) * 2 + c);

		i++;
	}
	result[x + res[0]*y] = i;
}