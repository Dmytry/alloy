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

#ifndef BODYPHYSICS_EX_H_
#define BODYPHYSICS_EX_H_

#include "physics/softbody/Body.h"
#include "ui/AlloyApplication.h"
#include "ui/AlloyParameterPane.h"
#include "graphics/AlloyMesh.h"
#include "graphics/shaders/CommonShaders.h"
#include "graphics/GLFrameBuffer.h"
struct DrawBody {
	aly::softbody::Body body;
	aly::Mesh mesh;
	DrawBody(aly::float3 spacing = aly::float3(1.0f, 1.0f, 1.0f)) :body(spacing),mesh() {

	}
};
typedef std::shared_ptr<DrawBody> DrawBodyPtr;
class SoftBodyEx : public aly::Application {
protected:
	bool frameBuffersDirty;
	std::unique_ptr<aly::Mesh> grid;
	aly::ImageGlyphPtr imageGlyph;
	aly::CompositePtr resizeableRegion;
	aly::CompositePtr renderRegion;
	std::vector<DrawBodyPtr> bodies;
	aly::Number alpha;
	aly::Number neighborSize;
	aly::Number maxVelocity;
	aly::Number dropHeight;
	aly::Number fractureDistanceTolerance;
	aly::Number fractureRotationTolerance;
	aly::Number damping;
	bool fracturing;

	aly::GLFrameBuffer renderFrameBuffer;
	aly::GLFrameBuffer colorFrameBuffer;
	aly::GLFrameBuffer depthFrameBuffer;
	aly::GLFrameBuffer wireframeFrameBuffer;
	
	aly::WireframeShader wireframeShader;
	aly::DepthAndNormalShader depthAndNormalShader;
	aly::ImageShader imageShader;
	aly::PhongShader phongShader;
	aly::Camera camera;

	aly::TextLabelPtr textLabel;
	aly::WorkerTaskPtr worker;
	aly::box3f objectBBox;
	void initializeFrameBuffers(aly::AlloyContext* context);
public:
	void addBody();
	void resetBodies();
	bool updatePhysics();
	SoftBodyEx();
	bool init(aly::Composite& rootNode);
	void draw(aly::AlloyContext* context);
};

#endif 
