/*
 * AlloyButton.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: blake
 */

#include "ui/AlloyButton.h"
#include "ui/AlloyApplication.h"
#include "ui/AlloyDrawUtil.h"

namespace aly{
TextButton::TextButton(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool truncate) :
		Region(label), truncate(truncate) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTEST);
	textColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	fontSize = UnitPerPX(1.0f, -10);
	borderWidth = UnitPX(0.0f);
	this->aspectRule = AspectRule::FixedHeight;
}
void TextButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	if (hover) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);

	} else {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x + xoff,
				bounds.position.y + yoff, bounds.dimensions.x,
				bounds.dimensions.y, context->theme.CORNER_RADIUS, 4,
				context->theme.LIGHTEST.toSemiTransparent(0.0f),
				context->theme.DARK);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeWidth(nvg, lineWidth);
	if (hover) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgStrokeColor(nvg, *borderColor);
		nvgStroke(nvg);

	} else {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
		nvgStrokeColor(nvg, *borderColor);
		nvgStroke(nvg);
	}
	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFillColor(nvg, *textColor);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	pixel2 offset(0, 0);
	if (truncate) {
		pushScissor(nvg, getCursorBounds());
	}
	nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);
	if (truncate) {
		popScissor(nvg);
	}

}
TextIconButton::TextIconButton(const std::string& label, int iconCode,
		const AUnit2D& position, const AUnit2D& dimensions,
		const HorizontalAlignment& alignment,
		const IconAlignment& iconAlignment, bool truncate) :
		Composite(label), iconCodeString(CodePointToUTF8(iconCode)), iconAlignment(
				iconAlignment), alignment(alignment), truncate(truncate), label(
				label) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	textColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	fontSize = UnitPerPX(1.0f, -10);
}

void TextIconButton::setLabel(const std::string& label) {
	this->label = label;
}
void TextIconButton::setIcon(int code) {
	iconCodeString = CodePointToUTF8(code);
}
void TextIconButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (hover) {

		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + xoff,
					bounds.position.y + yoff, bounds.dimensions.x,
					bounds.dimensions.y, context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
					bounds.dimensions.x, bounds.dimensions.y);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	} else {
		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
					bounds.dimensions.x - 2, bounds.dimensions.y - 2,
					context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
					bounds.dimensions.x - 2, bounds.dimensions.y - 2);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	nvgFillColor(nvg, *textColor);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	float iw = nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
			nullptr);

	float ww = tw + iw + AlloyApplicationContext()->theme.SPACING.x;
	pixel2 offset(0, 0);
	pixel xoffset = (pixel) xoff;
	if (alignment == HorizontalAlignment::Center) {
		xoffset += (bounds.dimensions.x - ww) / 2;
	} else if (alignment == HorizontalAlignment::Right) {
		xoffset += bounds.dimensions.x - ww
				- AlloyApplicationContext()->theme.SPACING.x;
	} else {
		xoffset += AlloyApplicationContext()->theme.SPACING.x;
	}
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
	if (truncate) {
		pushScissor(nvg, getCursorBounds());
	}
	if (iconAlignment == IconAlignment::Right) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgText(nvg, bounds.position.x + xoffset,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				label.c_str(), nullptr);

		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		nvgText(nvg,
				bounds.position.x + xoffset
						+ AlloyApplicationContext()->theme.SPACING.x + tw,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
	} else if (iconAlignment == IconAlignment::Left) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgText(nvg,
				bounds.position.x + xoffset
						+ AlloyApplicationContext()->theme.SPACING.x + iw,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				label.c_str(), nullptr);

		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		nvgText(nvg, bounds.position.x + xoffset,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
	}
	if (truncate) {
		popScissor(nvg);
	}
}
IconButton::IconButton(int iconCode, const AUnit2D& position,
		const AUnit2D& dimensions, IconType iconType, bool truncate) :
		Composite("Icon", position, dimensions), iconCodeString(
				CodePointToUTF8(iconCode)), iconType(iconType), truncate(
				truncate), rescale(true), nudge(0.0f, 0.0f), nudgeSize(0.0f) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	foregroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	iconColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);

	this->aspectRatio = 1.0f;
	this->aspectRule = AspectRule::FixedHeight;
}
void IconButton::setIcon(int iconCode) {
	iconCodeString = CodePointToUTF8(iconCode);
}
void IconButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel2 center = bounds.position + HALF_PIX(bounds.dimensions);
	pixel2 radii = HALF_PIX(bounds.dimensions);
	pixel2 offset;
	if (down && rescale) {
		offset = pixel2(1, 1);
	} else {
		offset = pixel2(0, 0);
	}
	float hoverOffset = (hover && rescale) ? 1.0f : 0.0f;
	Color bgColor = *backgroundColor;
	if (bgColor.a > 0) {
		nvgBeginPath(nvg);
		nvgFillColor(nvg, bgColor);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFill(nvg);
	}
	if (iconType == IconType::CIRCLE) {
		nvgBeginPath(nvg);
		nvgEllipse(nvg, center.x + offset.x, center.y + offset.y,
				radii.x - 1 + hoverOffset, radii.y - 1 + hoverOffset);
		nvgFillColor(nvg, *foregroundColor);
		nvgFill(nvg);
	} else {
		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + 1 + offset.x - hoverOffset,
					bounds.position.y + 1 + offset.y - hoverOffset,
					bounds.dimensions.x - 2 + hoverOffset * 2,
					bounds.dimensions.y - 2 + hoverOffset * 2,
					context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + 1 + offset.x - hoverOffset,
					bounds.position.y + 1 + offset.y - hoverOffset,
					bounds.dimensions.x - 2 + hoverOffset * 2,
					bounds.dimensions.y - 2 + hoverOffset * 2);
		}
		nvgFillColor(nvg, *foregroundColor);
		nvgFill(nvg);

	}
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	box2px ibounds = bounds;
	ibounds.position += offset;
	float th = ibounds.dimensions.y - 2 * lineWidth + nudgeSize;
	if (rescale) {
		th -= ((hover) ? 2 : 4);
	}
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	if (truncate) {
		pushScissor(nvg, getCursorBounds());
	}
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	drawText(nvg, ibounds.position + HALF_PIX(ibounds.dimensions) + nudge,
			iconCodeString, FontStyle::Normal,
			(hover && borderColor->a > 0) ? *borderColor : *iconColor,
			*backgroundColor, nullptr);
	if (truncate) {
		popScissor(nvg);
	}
	if (borderColor->a > 0) {
		if (iconType == IconType::CIRCLE) {

			nvgBeginPath(nvg);
			nvgEllipse(nvg, center.x + offset.x, center.y + offset.y,
					radii.x - HALF_PIX(lineWidth) + hoverOffset,
					radii.y - HALF_PIX(lineWidth) + hoverOffset);
			nvgStrokeColor(nvg, (hover) ? *borderColor : *iconColor);
			nvgStrokeWidth(nvg, lineWidth);
			nvgStroke(nvg);
		} else {
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg,
						bounds.position.x + offset.x + lineWidth - hoverOffset,
						bounds.position.y + offset.y + lineWidth - hoverOffset,
						bounds.dimensions.x - 2 * lineWidth
								+ hoverOffset * 2.0f,
						bounds.dimensions.y - 2 * lineWidth
								+ hoverOffset * 2.0f,
						context->theme.CORNER_RADIUS);
			} else {
				nvgRect(nvg,
						bounds.position.x + offset.x + lineWidth - hoverOffset,
						bounds.position.y + offset.y + lineWidth - hoverOffset,
						bounds.dimensions.x - 2 * lineWidth
								+ hoverOffset * 2.0f,
						bounds.dimensions.y - 2 * lineWidth
								+ hoverOffset * 2.0f);
			}
			nvgStrokeColor(nvg,
					(hover) ? context->theme.LIGHTEST : *borderColor);
			nvgStrokeWidth(nvg, lineWidth);
			nvgStroke(nvg);
		}
	}
}
}

