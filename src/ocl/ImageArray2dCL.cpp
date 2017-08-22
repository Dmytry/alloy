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

#include "ocl/ImageArray2dCL.h"
#include "ocl/ocl_runtime_error.h"
#include "ocl/ComputeCL.h"
#include <AlloyCommon.h>
namespace aly {
	ImageArray2dCL::ImageArray2dCL() :MemoryCL(), height(0), width(0), channels(0),arraySize(0), typeSize(0) {
	}
	ImageArray2dCL::~ImageArray2dCL() {
		if (buffer != nullptr)
			clReleaseMemObject(buffer);
		buffer = nullptr;
	}

	void ImageArray2dCL::create(cl_mem_flags f, const cl_image_format& formats, int w, int h, int c,int arraySize, int bytesPerChannel) {
		int err = -1;
		height = h;
		width = w;
		channels = c;
		typeSize = bytesPerChannel;
		if (buffer != nullptr)
			clReleaseMemObject(buffer);
		cl_image_desc desc;
		desc.image_type = CL_MEM_OBJECT_IMAGE2D_ARRAY;
		desc.image_width = w;
		desc.image_height = h;
		desc.image_depth = 0;
		desc.image_array_size = arraySize;
		desc.image_row_pitch = 0;
		desc.image_slice_pitch = 0;
		desc.num_mip_levels = 0;
		desc.num_samples = 0;
		desc.buffer = nullptr;
		buffer = clCreateImage(CLContext(), f, &formats, &desc, nullptr, &err);
		if (err != CL_SUCCESS) {
			throw ocl_runtime_error(aly::MakeString() << "Could not create image array of size ("<<arraySize<<") [w=" << w << ",h=" << h << ",c=" << c << ",b=" << typeSize << "].", err);
		}
		bufferSize = (size_t) w * (size_t) h * (size_t) c * (size_t) bytesPerChannel;
		this->arraySize=arraySize;
	}
	void ImageArray2dCL::read(size_t index,void * data, bool block) const {
		const size_t origin[3] = { 0, 0, index };
		const size_t region[3] = { (size_t)width, (size_t)height, 1};
		int err = clEnqueueReadImage(CLQueue(), buffer, (block) ? CL_TRUE : CL_FALSE, origin, region, width * channels * typeSize, 0, data, 0, nullptr,
				nullptr);
		clFlush(CLQueue());
		if (err != CL_SUCCESS)
			throw ocl_runtime_error(MakeString()<<"Could not read image from device. index="<<index, err);
	}

	void ImageArray2dCL::write(size_t index,const void* data, bool block) {
		const size_t origin[3] = { 0, 0, index };
		const size_t region[3] = { width, height, 1};
		int err = clEnqueueWriteImage(CLQueue(), buffer, (block) ? CL_TRUE : CL_FALSE, origin, region, width * channels * typeSize, 0, data, 0, nullptr,
				nullptr);
		if (err != CL_SUCCESS)
			throw ocl_runtime_error(MakeString()<<"Could not write image to device. index="<<index, err);
	}
	void ImageArray2dCL::write(size_t index,cl_mem mem) {
		const size_t origin[3] = { 0, 0, index };
		const size_t region[3] = { width, height, 1 };
		int err = clEnqueueCopyBufferToImage(CLQueue(), mem, buffer, 0, origin,region, 0, nullptr, nullptr);
		if (err != CL_SUCCESS)
			throw ocl_runtime_error("Could not copy buffer to image.", err);
	}
}
