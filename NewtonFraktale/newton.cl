//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

struct complex {
	float im;
	float re;
	float r;
	float phi;
};

struct complex createComplexFromPolar(float _r, float _phi){
	struct complex t;
	t.r = _r;
	t.phi = _phi;

	t.re = cos(t.phi);
	t.im = sin(t.phi);

	return t;
}

struct complex createComplexFromKarthes(float real, float imag){
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

struct complex addComplexScalar(const struct complex z, const float n){
	struct complex t;
	t.re = z.re + n;
	return recreateComplexFromKarthes(t);
}

struct complex subComplexScalar(const struct complex z, const float n){
	struct complex t;
	t.re = z.re - n;
	return recreateComplexFromKarthes(t);
}

struct complex multComplexScalar(const struct complex z, const float n) {
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

__kernel void newtonFraktal(__global const int* res, __global const float* param, __global int* result){
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	struct complex z = createComplexFromKarthes(param[x - res[0] / 2], param[y - res[0] / 2]);

	struct complex c = createComplexFromKarthes(param[x - res[1] / 2], param[y - res[1] / 2]);

	int i = 0;
	while (z.r < 1000){
		if (i >= 10000000)
			break;
		z = subComplex(z, divComplex(addComplex(addComplex(multComplex(multComplex(z,z),c),multComplex(z,c)),c),addComplex(multComplexScalar(multComplex(z,c),2),c)));
		//z-(c*z*z + c*z + c) / ((c*z) * 2 + c);

		i++;
	}
	result[x - get_global_offset(0) + y*res[0] - get_global_offset(1)] = i;
}