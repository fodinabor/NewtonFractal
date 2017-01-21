/*
The MIT License (MIT)

Copyright (c) 2016 By Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#define pi 3.14159265359
#define RESOLUTION 0.000001

struct complex {
	float im;
	float re;
	float r;
	float phi;

	char isPolar;
	char isKarthes;
};

struct complex createComplexFromPolar(float _r, float _phi) {
	struct complex t;
	t.r = _r;
	t.phi = _phi;
	t.isPolar = true;

	t.re = _r*cos(_phi);
	t.im = _r*sin(_phi);
	t.isKarthes = true;

	return t;
}

struct complex createComplexFromKarthes(float real, float imag) {
	struct complex t;
	t.re = real;
	t.im = imag;
	t.isKarthes = true;

	t.phi = atan2(imag, real);
	t.r = sqrt(t.re*t.re + t.im*t.im);
	t.isPolar = true;

	return t;
}

struct complex recreateComplexFromKarthes(struct complex t) {
	return createComplexFromKarthes(t.re, t.im);
}

struct complex recreateComplexFromPolar(struct complex t) {
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
}

struct complex addComplex(const struct complex a, const struct complex b) {
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

	struct complex t;
	t.re = c.re + z.re;
	t.im = c.im + z.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;
}

struct complex subComplex(const struct complex a, const struct complex b) {
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

	struct complex t;
	t.re = z.re - c.re;
	t.im = z.im - c.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;
}

struct complex addComplexScalar(const struct complex a, const float n) {
	struct complex z = getComplexKarthes(a);

	struct complex t;
	t.re = z.re + n;
	t.im = z.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;
}

struct complex subComplexScalar(const struct complex a, const float n) {
	struct complex z = getComplexKarthes(a);

	struct complex t;
	t.re = z.re - n;
	t.im = z.im;
	t.isKarthes = true;
	t.isPolar = false;
	return t;
}

struct complex multComplexScalar(const struct complex a, const float n) {
	struct complex z = getComplexKarthes(a);

	struct complex t;
	t.re = z.re * n;
	t.im = z.im * n;
	t.isKarthes = true;
	t.isPolar = false;
	return t;
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
}

struct complex powComplex(const struct complex a, int i) {
	struct complex z = getComplexPolar(a);

	struct complex t;
	t.r = pow(z.r, (float) i);
	t.phi = z.phi * i;
	t.isKarthes = false;
	t.isPolar = true;
	return t;
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
}

bool compComplex(const struct complex a, const struct complex b, float comp) {
	struct complex z = getComplexKarthes(a);
	struct complex c = getComplexKarthes(b);

	if (fabs(z.re - c.re) <= comp && fabs(z.im - c.im) <= comp)
		return true;
	return false;
}

struct complex computeFunction(const struct complex z, __global const struct complex* params, const int paramc) {
	struct complex sum = createComplexFromKarthes(0, 0);
	struct complex pow;
	for (unsigned int i = 1; i <= paramc; i++) {
		sum = addComplex(multComplex(sum, z), params[paramc - i]);
	}
	return sum;
}

__kernel void findZeros(__global const int* res,
	__global const struct complex* params, __global const struct complex* paramsD, __global const int* paramc,
	__global struct complex* out) {
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	struct complex z = createComplexFromKarthes(x - res[0] / 2, y - res[1] / 2);
	struct complex zo = z;

	if (z.re == 0 && z.im == 0) {
		out[x + y*res[0]] = createComplexFromKarthes(100000, 100000);
		return;
	}

	for (int i = 0; i < 50; i++) {
		zo = z;
		z = subComplex(z, divComplex(computeFunction(z, params, paramc[0]), computeFunction(z, paramsD, paramc[1])));
		if (compComplex(z, zo, RESOLUTION)) {
			out[x + y*res[0]] = z;
			return;
		}
	}

	out[x + y*res[0]] = createComplexFromKarthes(100000, 100000);
}

__kernel void newtonFraktal(__global const int* res, __global const float* zoom, __global const int* offset,
	__global const float* center, __global const struct complex *zeros,
	__global const struct complex *params, __global const struct complex *paramsD, __global const int* paramc,
	__global float* result, __global int* resType, __global int* iterations) {
	const int x = get_global_id(0) + offset[0];
	const int y = get_global_id(1) + offset[1];

	const int xRes = res[0];
	const int yRes = res[1];

	const int retPos = x + xRes * (y - offset[1]);

	const float a = (float) (zoom[0] * ((float) x - ((float) xRes / 2.0))) / xRes + center[0];
	const float b = (float) (zoom[1] * (((float) yRes / 2.0) - (float) y)) / yRes + center[1];

	struct complex z = createComplexFromKarthes(a, b);
	struct complex f, d, zo;
	resType[retPos] = -1;

	bool found = false;
	int i = 0;
	while (i < 500 && !found) {
		f = computeFunction(z, params, paramc[0]);
		d = computeFunction(z, paramsD, paramc[1]);

		zo = z;

		z = subComplex(z, divComplex(f, d));

		i++;

		for (int j = 0; j < paramc[0] - 1; j++) {
			if (compComplex(z, zeros[j], RESOLUTION)) {
				resType[retPos] = j;
				result[retPos] = (log(RESOLUTION) - log(getComplexPolar(subComplex(zo, zeros[j])).r)) / (log(getComplexPolar(subComplex(z, zeros[j])).r) - log(getComplexPolar(subComplex(zo, zeros[j])).r));
				found = true;
				break;
			}
		}

		if (compComplex(z, zo, RESOLUTION / 100) && !found) {
			resType[retPos] = -1;
			break;
		}
	}

	iterations[retPos] = i;
}