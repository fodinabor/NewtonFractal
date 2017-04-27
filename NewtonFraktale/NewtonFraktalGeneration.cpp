/*
The MIT License (MIT)

Copyright (c) 2017 By Joachim Meyer

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

#include "NewtonFraktalGeneration.h"
#include "Polynom.h"
#include <windows.h>
#include <ctime>
#include <sstream>
#include <iomanip>

const vector<int> NewtonFraktalGeneration::colorsHex = {
	0x0000FF,
	0x00FF00,
	0xFF0000,
	0xFFFF00,
	0x00FFFF,
	0xFF00FF,
	0x88FF00,
	0x88FF88,
	0xFF88FF,
	0x8888FF,
	0xFFC000,
	0x00FFC0,
	0xC0FFC0,
	0xC000FF,
	0xC0FF00,
	0x9900ff,
	0x0099ff,
	0xff0099,
	0xff9900,
	0xff5500,
	0x55ff00,
	0x00ff55,
	0x0055ff
};

vector<Color> NewtonFraktalGeneration::colors;

NewtonFraktalGenerator::NewtonFraktalGenerator() {}

void NewtonFraktalGenerator::runNewton(NewtonFraktal *fraktal) {
	Logger::log("Using CPU Generation\n");
	std::complex<double> z, zo, p, d;
	const int xRes = fraktal->getWidth();
	const int yRes = fraktal->getHeight();

	Polynom* polynom = fraktal->getPolynom();
	Polynom* derivation = fraktal->getDerivation();

	vector<complex<double>> zeros = polynom->getZeros();

	for (int y = 0; y < yRes; y++) {
		for (int x = 0; x < xRes; x++) {
			const double a = (double)fraktal->getArea()[0] * ((double)x - ((double)xRes / 2.0)) / xRes + fraktal->getCenter()[0];
			const double b = (double)fraktal->getArea()[1] * (((double)yRes / 2.0) - (double)y) / yRes + fraktal->getCenter()[1];

			z = std::complex<double>(a, b);
			fraktal->getTypes()[x + xRes * y] = -1;
			fraktal->getValues()[x + xRes * y] = 0;

			bool found = false;
			int i = 0;
			while (i < 600 && !found) {
				p = polynom->getValue(z);
				d = derivation->getValue(z);

				zo = z;

				z = z - p / d;

				i++;

				for (int j = 0; j < zeros.size(); j++) {
					if (compComplex(z, zeros[j], RESOLUTION)) {
						fraktal->getTypes()[x + xRes * y] = j;
						fraktal->getValues()[x + y * xRes] = (log(RESOLUTION) - log(abs(zo - zeros[j]))) / (log(abs(z - zeros[j])) - log(abs(zo - zeros[j])));
						found = true;
						break;
					}
				}

				if (compComplex(z, zo, RESOLUTION / 100) && !found) {
					fraktal->getTypes()[x + xRes * y] = -1;
					break;
				}
			}
			fraktal->getIterations()[x + xRes * y] = i;
		}
	}
}

bool NewtonFraktalGenerator::compComplex(const std::complex<double> z, const std::complex<double> c, double comp) {
	if (fabs(z.real() - c.real()) <= comp && fabs(z.imag() - c.imag()) <= comp)
		return true;
	return false;
}

NewtonFraktalGeneration::NewtonFraktalGeneration() {
	defaultGenerationMode = GENERATION_MODE_CPU;
	defaultGenerator = registerGenerator(new NewtonFraktalGenerator, GENERATION_MODE_CPU);

	for (int i = 0; i < colorsHex.size(); i++) {
		Color col;
		col.setColorHexRGB(colorsHex[i]);
		colors.push_back(col);
	}
}

NewtonFraktalGeneration::~NewtonFraktalGeneration() {}

void NewtonFraktalGeneration::generate(NewtonFraktal *fraktal, int mode, bool save) {
	fraktal->allocGenRes();
	NewtonFraktalGenerator* generator = getGeneratorForMode(mode);

	clock_t gen_begin = clock();
	generator->runNewton(fraktal);
	clock_t gen_end = clock();

	fraktal->draw();

	if (save) {
		FILE* logFile;
		fopen_s(&logFile, "polynoms.log", "a");
		String timeS = String::IntToString(time(NULL)), polynomS = fraktal->getPolynom()->getString();
		fprintf(logFile, "Time: %s, Polynom: %s, Center: %.15Lf, %.15Lf, Area size: x: %.15Lf y: %.15Lf, Resolution: %dx%d, Contrast: %f, The computation took: %s secs, searching the max: %s secs, drawing: %s secs\n", timeS.c_str(), polynomS.c_str(), fraktal->getCenter()[0], fraktal->getCenter()[1], fraktal->getArea()[0], fraktal->getArea()[1], fraktal->getResolution()[0], fraktal->getResolution()[1], fraktal->getContrast(), String::NumberToString(double(gen_end - gen_begin) / CLOCKS_PER_SEC).c_str(), String::NumberToString(double(fraktal->max_end - fraktal->max_begin) / CLOCKS_PER_SEC).c_str(), String::NumberToString(double(fraktal->draw_end - fraktal->draw_begin) / CLOCKS_PER_SEC).c_str());
		fclose(logFile);
		fraktal->savePNG(timeS + ".png");
	}
}

void NewtonFraktalGeneration::generate(NewtonFraktal * fraktal, NewtonFraktalGenerator * generator, bool save) {
	fraktal->allocGenRes();

	clock_t gen_begin = clock();
	generator->runNewton(fraktal);
	clock_t gen_end = clock();

	fraktal->draw();

	if (save) {
		FILE* logFile;
		fopen_s(&logFile, "polynoms.log", "a");
		String timeS = String::IntToString(time(NULL)), polynomS = fraktal->getPolynom()->getString();
		fprintf(logFile, "Time: %s, Polynom: %s, Center: %.15Lf, %.15Lf, Area size: x: %.15Lf y: %.15Lf, Resolution: %dx%d, Contrast: %f, The computation took: %s secs, searching the max: %s secs, drawing: %s secs\n", timeS.c_str(), polynomS.c_str(), fraktal->getCenter()[0], fraktal->getCenter()[1], fraktal->getArea()[0], fraktal->getArea()[1], fraktal->getResolution()[0], fraktal->getResolution()[1], fraktal->getContrast(), String::NumberToString(double(gen_end - gen_begin) / CLOCKS_PER_SEC).c_str(), String::NumberToString(double(fraktal->max_end - fraktal->max_begin) / CLOCKS_PER_SEC).c_str(), String::NumberToString(double(fraktal->draw_end - fraktal->draw_begin) / CLOCKS_PER_SEC).c_str());
		fclose(logFile);
		fraktal->savePNG(timeS + ".png");
	}
}

void NewtonFraktalGeneration::generateZoom(BezierCurve * areaCurve, BezierCurve * centerCurve, int framerate, int duration, int * resolution, Polynom * polynom, double contrast) {
	double area, ratio = (double)resolution[1] / (double)resolution[0], step;
	int frameCount = duration * framerate, nameLen = String::IntToString(frameCount).length();
	//String setName = String::IntToString(time(NULL)), encoderCall = "cd " + setName + " && " + getExePath() + "\\mencoder \"mf://*.png\" -mf w=" + String::IntToString(resolution[0]) + ":h=" + String::IntToString(resolution[0]) + ":fps=" + String::IntToString(framerate) + " -o " + setName + ".avi -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell";
	String setName = String::IntToString(time(NULL)), encoderCall = "cd " + setName + " && " + getExePath() + "\\ffmpeg -s " + String::IntToString(resolution[0]) + "x" + String::IntToString(resolution[0]) + " -r " + String::IntToString(framerate) + " -i fractal%0" + String::IntToString(nameLen) + "d.png -vcodec libx264 -crf 15 -pix_fmt yuv420p " + setName + ".avi";
	Vector3 center;

	CreateDirectory(setName.getWDataWithEncoding(String::ENCODING_UTF8), NULL);

	//areaCurve->cacheHeightValues = true;
	//areaCurve->setHeightCacheResolution(frameCount);
	areaCurve->evaluationAccuracy = 0.0001;
	centerCurve->evaluationAccuracy = 0.0001;
	//centerCurve->cacheHeightValues = true;
	//centerCurve->setHeightCacheResolution(frameCount);
	BezierCurve::cacheHeightValues = false;

	step = 1.0 / frameCount;

	BezierCurve* valueCurve = new BezierCurve();
	valueCurve->evaluationAccuracy = 0.001;
	valueCurve->addControlPoint2dWithHandles(((double)frameCount) * -0.01, 1.5, ((double)frameCount) * 0.0, 1.0, ((double)frameCount) * 0.01, 0.5 * contrast);
	valueCurve->addControlPoint2dWithHandles(((double)frameCount) * 0.375 * contrast, 0.01, ((double)frameCount) * 1.0, 0.0, ((double)frameCount) * 1.3, -0.01);

	double i = 0;
	for (int j = 0; j <= frameCount; j++) {
		i = 1.0 - valueCurve->getYValueAtX(j);

		NewtonFraktal *fraktal = new NewtonFraktal(resolution[0], resolution[1]);
		area = areaCurve->getYValueAtX(i);
		center = centerCurve->getPointAt(i);
		fraktal->setArea(area, area * ratio);
		fraktal->setCenter(center.y, center.z);
		fraktal->setContrast(contrast);
		fraktal->setPolynom(polynom);

		generate(fraktal, defaultGenerator, false);

		std::stringstream ss;
		ss << std::setw(nameLen) << std::setfill('0') << j + 1;
		String name = ss.str();
		fraktal->savePNG(setName + "/fractal" + name + ".png");

		BezierCurve::cacheHeightValues = false;
		Logger::log("%i: i: %.15f center: %.15f, %.15f area: %.15f\n", j, i, centerCurve->getYValueAtX(i), centerCurve->getPointAt(i).z, areaCurve->getYValueAtX(i));

		delete fraktal;
	}

	int ret = system(encoderCall.c_str());
	Logger::log("Encoder exit code: %i\n", ret);
	system((String("cd ") + getExePath()).c_str());
}

NewtonFraktalGenerator* NewtonFraktalGeneration::registerGenerator(NewtonFraktalGenerator * newGen, int generatorType) {
	newGen->generatorType = generatorType;
	generators.push_back(newGen);
	return newGen;
}

void NewtonFraktalGeneration::setDefaultGenerationMode(int newDef) {
	defaultGenerationMode = newDef;
	defaultGenerator = getGeneratorForMode(defaultGenerationMode);
}

int NewtonFraktalGeneration::getDefaultGenerationMode() {
	return defaultGenerationMode;
}

NewtonFraktalGenerator * NewtonFraktalGeneration::getGeneratorForMode(int mode) {
	for (int i = 0; i < generators.size(); i++) {
		if (generators[i]->generatorType == mode && generators[i]->status == 0)
			return generators[i];
	}
	for (int i = 0; i < generators.size(); i++) {
		if (generators[i]->generatorType == NewtonFraktalGeneration::GENERATION_MODE_CPU)
			return generators[i];
	}
	generators.push_back(new NewtonFraktalGenerator());
	generators[generators.size() - 1]->generatorType = GENERATION_MODE_CPU;
	return generators[generators.size() - 1];
}

String NewtonFraktalGeneration::getExePath() {
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	size_t pos = String(buffer).find_last_of("\\/");
	return String(buffer).substr(0, pos);
}
