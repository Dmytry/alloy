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

#ifndef SRC_UI_ALLOYSLIDERWIDGET_H_
#define SRC_UI_ALLOYSLIDERWIDGET_H_
#include "ui/AlloyComposite.h"
#include "ui/AlloyTextWidget.h"
namespace aly{

enum class SliderHandleShape {Whole,Hat, HalfLeft, HalfRight};
class ScrollTrack: public Region {
public:
	const Orientation orientation;
	ScrollTrack(const std::string& name, Orientation orient) :
			Region(name), orientation(orient) {
	}
	virtual void draw(AlloyContext* context) override;
};
class SliderHandle: public Region {
protected:
	SliderHandleShape handleShape;
public:
	SliderHandle(const std::string& name, const SliderHandleShape& handleShape=SliderHandleShape::Whole) :
			Region(name),handleShape(handleShape) {
	}
	virtual void draw(AlloyContext* context) override;
};
class SliderTrack: public Composite {
protected:
	const Orientation orientation;
	float2 activeRegion;
	float currentPosition;
public:
	Color startColor, endColor;
	SliderTrack(const std::string& name, Orientation orientColor,
			const Color& st, const Color& ed);
	void setLower(float x) {
		activeRegion.x = x;
	}
	void setCurrent(float c) {
		currentPosition = c;
	}
	void setUpper(float y) {
		activeRegion.y = y;
	}
	virtual void draw(AlloyContext* context) override;
};
class Slider: public Composite {
protected:
	AColor textColor;
	AUnit1D fontSize;
	Number minValue;
	Number maxValue;
	Number value;
	TextLabelPtr sliderLabel;
	TextLabelPtr valueLabel;
	std::shared_ptr<SliderHandle> sliderHandle;
	std::shared_ptr<SliderTrack> sliderTrack;
	std::function<std::string(const Number& value)> labelFormatter;
	std::function<void(const Number& value)> onChangeEvent;
	virtual void update()=0;
	double sliderPosition;
public:
	void setSliderColor(const Color& startColor, const Color& endColor) {
		sliderTrack->startColor = startColor;
		sliderTrack->endColor = endColor;
	}
	void setMinValue(const Number& v) {
		minValue=v;
	}
	void setMaxValue(const Number& v) {
		maxValue = v;
	}
	Slider(const std::string& name, const Number& min, const Number& max,
			const Number& val) :
			Composite(name), minValue(min), maxValue(max), value(val), sliderPosition(
					0.0) {
		labelFormatter = [](const Number& value) {return value.toString();};
	}
	Slider(const std::string& name, const AUnit2D& pos, const AUnit2D& dims,
			const Number& min, const Number& max, const Number& val) :
			Composite(name, pos, dims), minValue(min), maxValue(max), value(
					val), sliderPosition(0.0) {
		labelFormatter = [](const Number& value) {return value.toString();};
	}
	double getBlendValue() const;
	void setBlendValue(double value);
	virtual void setValue(double value)=0;
	inline void setValue(int value) {
		setValue((double) value);
	}
	inline void setValue(float value) {
		setValue((double) value);
	}
	const Number& getValue() {
		return value;
	}
	const Number& getMinValue() {
		return minValue;
	}
	const Number& getMaxValue() {
		return maxValue;
	}
	inline void setOnChangeEvent(
			const std::function<void(const Number& value)>& func) {
		onChangeEvent = func;
	}
	inline void setLabelFormatter(
			const std::function<std::string(const Number& value)>& func) {
		labelFormatter = func;
	}
};

std::shared_ptr<Region> MakeRegion(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor = COLOR_NONE, const Color& lineColor = COLOR_WHITE,
		const AUnit1D& lineWidth = UnitPX(2.0f));
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Region & region) {
	ss << "Region: " << region.name << std::endl;
	ss << "\tOrigin: " << region.getOrigin() << std::endl;
	ss << "\tRelative Position: " << region.position << std::endl;
	ss << "\tRelative Dimensions: " << region.dimensions << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	ss << "\tAspect Ratio: " << region.getAspectRule() << std::endl;
	ss << "\tBackground Color: " << region.backgroundColor << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}
class HorizontalSlider: public Slider {
protected:
	virtual void update() override;
public:
	virtual void setValue(double value) override;
	bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event);
	bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event);
	HorizontalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool showLabel,
		const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	HorizontalSlider(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions,
		const Number& minValue = Float(0.0f),
		const Number& maxValue = Float(1.0f),
		const Number& value = Float(0.0f)):HorizontalSlider(label,position,dimensions,true,minValue,maxValue,value) {

	}
	virtual void draw(AlloyContext* context) override;
	virtual inline ~HorizontalSlider() {
	}
};
class RangeSlider : public Composite {
protected:
	AColor textColor;
	AUnit1D fontSize;
	Number minValue;
	Number maxValue;
	Number lowerValue;
	Number upperValue;
	TextLabelPtr sliderLabel;
	TextLabelPtr lowerValueLabel;
	TextLabelPtr upperValueLabel;
	std::shared_ptr<SliderHandle> lowerSliderHandle;
	std::shared_ptr<SliderHandle> upperSliderHandle;
	std::shared_ptr<SliderTrack> sliderTrack;
	std::function<std::string(const Number& value)> labelFormatter;
	void update();
	double2 sliderPosition;
public:
	std::function<void(const Number& lowerValue, const Number& upperValue)> onChangeEvent;
	void setSliderColor(const Color& startColor, const Color& endColor) {
		sliderTrack->startColor = startColor;
		sliderTrack->endColor = endColor;
	}
	RangeSlider(const std::string& name, const AUnit2D& pos,const AUnit2D& dims,const Number& min, const Number& max,
		const Number& lowerValue, const Number& upperValue,bool showLabel=true);
	double2 getBlendValue() const;
	void setBlendValue(double2 value);

	void setLowerValue(double value);
	void setUpperValue(double value);
	inline void setLowerValue(float value) {
		setLowerValue((double)value);
	}
	inline void setUpperValue(float value) {
		setUpperValue((double)value);
	}
	inline void setLowerValue(int value) {
		setLowerValue((double)value);
	}
	inline void setUpperValue(int value) {
		setUpperValue((double)value);
	}
	void setValue(double2 range);
	const Number& getLowerValue() {
		return lowerValue;
	}
	const Number& getUpperValue() {
		return upperValue;
	}
	inline void setOnChangeEvent(
		const std::function<void(const Number& lowerValue,const Number& upperValue)>& func) {
		onChangeEvent = func;
	}
	inline void setLabelFormatter(
		const std::function<std::string(const Number& value)>& func) {
		labelFormatter = func;
	}
	bool onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event);
	bool onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~RangeSlider() {
	}
};

class VerticalSlider: public Slider {
protected:
	virtual void update() override;
public:
	virtual void setValue(double value) override;
	bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event);
	bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event);
	VerticalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~VerticalSlider() {
	}
};

typedef std::shared_ptr<HorizontalSlider> HSliderPtr;
typedef std::shared_ptr<VerticalSlider> VSliderPtr;
typedef std::shared_ptr<HorizontalSlider> HorizontalSliderPtr;
typedef std::shared_ptr<VerticalSlider> VerticalSliderPtr;
typedef std::shared_ptr<RangeSlider> RangeSliderPtr;
}
#endif /* SRC_UI_ALLOYSLIDERWIDGET_H_ */
