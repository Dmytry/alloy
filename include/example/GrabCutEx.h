/*
 * Copyright(C) 2018, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef GRABCUT_EX_H_
#define GRABCUT_EX_H_

#include "AlloyApplication.h"
#include "CommonShaders.h"
#include "AlloyMaxFlow.h"
class GrabCutEx: public aly::Application {
protected:
	const int nbrX[4]= { 1, -1, 0, 0 };
	const int nbrY[4]={ 0, 0, 1, -1 };

	aly::ImageRGBA image;
	aly::box2f selectedRegion;
	aly::MaxFlow maxFlow;
	aly::FastMaxFlow fastMaxFlow;
	aly::ImageGlyphPtr imageGlyph;
	int cycle;
	float colorDiff;
	float maxDist;
	const int MAX_CYCLES=7;
	void initSolver(aly::ImageRGBA& image,const aly::box2f& region);
	void initSolver(aly::ImageRGBA& image);
public:
	GrabCutEx();
	bool init(aly::Composite& rootNode);
};

#endif 