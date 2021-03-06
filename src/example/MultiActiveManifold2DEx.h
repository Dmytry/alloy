/*
 * Copyright(C) 2016, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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

#ifndef MULTIACTIVECONTOUR2D_EX_H_
#define MULTIACTIVECONTOUR2D_EX_H_

#include "ui/AlloySimulation.h"
#include "vision/ManifoldCache2D.h"
#include "ui/AlloyApplication.h"
#include "math/AlloyVector.h"
#include "vision/MultiActiveContour2D.h"
#include "ui/AlloyWorker.h"
#include "ui/AlloyTimeline.h"
class MultiActiveManifold2DEx: public aly::Application {
protected:
	float currentIso;
	int example;
	aly::ImageRGBA img;
	aly::Image1f gray;
	bool parametersDirty;
	bool frameBuffersDirty;
	bool running = false;
	aly::Image2f vecField;
	aly::Number lineWidth;
	aly::Number particleSize;

	aly::Color pointColor;
	aly::Color normalColor;
	aly::Color springlColor;
	aly::Color matchColor;
	aly::Color vecfieldColor;
	aly::AdjustableCompositePtr resizeableRegion;
	std::shared_ptr<aly::MultiActiveContour2D> simulation;
	aly::IconButtonPtr playButton,stopButton;
    std::shared_ptr<aly::TimelineSlider> timelineSlider;
	void createCircleLevelSet(int w, int h, int rows, int cols, float r, aly::Image1f& levelset, aly::Image1i& labelImage);

	void createTextLevelSet(aly::Image1f& levelSet,aly::Image1f& gray,int w,int h,const std::string& text,float textSize,float maxDistance);
public:
	std::shared_ptr<aly::ManifoldCache2D> cache;
	MultiActiveManifold2DEx(int example=0);
	virtual void draw(aly::AlloyContext* context) override;
	bool init(aly::Composite& rootNode);
};
class MultiActiveContour2DEx : public MultiActiveManifold2DEx{
public:
	MultiActiveContour2DEx():MultiActiveManifold2DEx(0){
	}
};
class MultiSpringls2DEx : public MultiActiveManifold2DEx{
public:
	MultiSpringls2DEx():MultiActiveManifold2DEx(1){
	}
};
class MultiSpringlsSecondOrder2DEx : public MultiActiveManifold2DEx{
public:
	MultiSpringlsSecondOrder2DEx():MultiActiveManifold2DEx(2){
	}
};
#endif

