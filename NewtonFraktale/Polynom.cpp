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
#include "Polynom.h"
#include "PolyLogger.h"
#include <cstring>

extern struct cl_complex createComplexFromKarthes(cl_double real, cl_double imag);

Polynom::Polynom(){}

Polynom::~Polynom(){}

void Polynom::addCoefficient(complex<cl_double> nC){
	coefficients.push_back(nC);
}

void Polynom::differentiate(){
	if (coefficients.size() == 0) return;
	for (unsigned int i = 0; i < coefficients.size()-1; i++){
		coefficients[i] = coefficients[i + 1] * (double)(i + 1);
	}
	coefficients.pop_back();
}

complex<cl_double> Polynom::getValue(complex<cl_double> nC){
	complex<cl_double> sum = complex<cl_double>(0, 0);
	complex<cl_double> pow;
	for (unsigned int i = 0; i < coefficients.size(); i++){
		//pow = std::pow(nC,i);
		//sum = sum + (coefficients[i] * pow);
		sum = coefficients[coefficients.size() - i - 1] + (sum * nC);
	}
	return sum;
}

int Polynom::getNumCoefficients() {
	return coefficients.size();
}

complex<cl_double> Polynom::getCoefficient(int i) {
	return coefficients[i];
}

String Polynom::printPolynom() {
	if (coefficients.size() == 0) {
		Logger::log("No Polynom to print");
		return String();
	}
	Logger::log("The Polynom we're using:\n");
	String ret;
	for (int i = this->coefficients.size() - 1; i >= 0; i--) {
		if ((coefficients[i].real() != 0) || (coefficients[i].imag() != 0)) {
			Logger::log("(%f+%fi)",coefficients[i].real(), coefficients[i].imag());
			ret += "( "+String::NumberToString(coefficients[i].real(),0) + " + " + String::NumberToString(coefficients[i].imag(),0) + " i)";
			if (i != 0) {
				Logger::log("*"); ret += "*";
			}
			if (i >= 1) {
				Logger::log("x"); ret += "x";
			}
			if (i >= 2) {
				Logger::log("^%d", i); ret += "^ " + String::IntToString(i);
			}
			if (i != 0) {
				Logger::log(" + "); ret += " + ";
			}
		}
	}
	Logger::log("\n");
	return ret;
}

Polynom* Polynom::readFromString(String polynom){
	Polynom* retPoly = new Polynom();

	int idx = polynom.length();
	bool first = true;
	String substr;
	while (idx > 0){
		idx = polynom.find_last_of("(");
		substr = polynom.substr(idx+2);
		int exp = 0;
		int re = 0, im = 0;
		if (idx > polynom.find_last_of("x^"))
			sscanf(substr.c_str(), "%d + %d", &re, &im);
		else
			sscanf(substr.c_str(), "%d + %d i)*x^ %d", &re, &im, &exp);
		
		int coefs = exp - retPoly->getNumCoefficients();
		for (int i = 0; i < coefs; i++){
			retPoly->addCoefficient(std::complex<double>(0, 0));
		}

		retPoly->addCoefficient(std::complex<double>(re, im));

		polynom = polynom.substr(0, idx - 1);
	}
	return retPoly;
}

struct cl_complex* Polynom::getCLCoefficients(){
	struct cl_complex* coefficientsCL = (struct cl_complex*)malloc(coefficients.size()*sizeof(struct cl_complex) * 2);
	for (int i = 0; i < coefficients.size(); i++){
		coefficientsCL[i] = createComplexFromKarthes(coefficients[i].real(),coefficients[i].imag());
	}
	return coefficientsCL;
}