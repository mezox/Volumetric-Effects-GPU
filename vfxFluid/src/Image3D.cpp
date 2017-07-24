#include "Image3D.h"
#include "vfxEngine.h"

namespace vfx
{
	GLuint Image3D::mBlurTempPing = 0;
	GLuint Image3D::mBlurTempPong = 0;
	bool Image3D::mBlurTexInitialized = false;

	Image3D::Image3D(const glm::uvec3& rSize,
		bool blurrable,
		GLenum format,
		GLenum magFilter,
		GLenum minFilter,
		GLenum wrapMode,
		GLint baseLevel,
		GLint maxLevel)
		: mFormat(format)
		, mSize(rSize)
		, mIsInitialized(false)
		, mIsBlurImageInitialized(false)
	{
		assert(rSize.x > 0 && rSize.y > 0 && rSize.z > 0);

		createTexture(mObjectID, magFilter, minFilter, wrapMode, baseLevel, maxLevel);

		GL_CHECK(glGenBuffers(1, &mOffsetSSBO));
		GL_CHECK(glGenBuffers(1, &mWeightSSBO));

		mIsInitialized = true;
	}

	Image3D::~Image3D()
	{
		reset();

		GL_CHECK(glDeleteBuffers(1, &mOffsetSSBO));
		GL_CHECK(glDeleteBuffers(1, &mWeightSSBO));
	}

	void Image3D::createBlurTempTargets(const glm::uvec3& size,
										GLenum format,
										GLenum magFilter,
										GLenum minFilter,
										GLenum wrapMode,
										GLint baseLevel,
										GLint maxLevel)
	{
		if (mBlurTexInitialized)
		{
			GL_CHECK(glDeleteTextures(1, &mBlurTempPing));
			GL_CHECK(glDeleteTextures(1, &mBlurTempPong));
		}

		if (mBlurTempPing == 0 && mBlurTempPong == 0)
		{
			GL_CHECK(glGenTextures(1, &mBlurTempPing));
			GL_CHECK(glActiveTexture(GL_TEXTURE0));
			GL_CHECK(glBindTexture(GL_TEXTURE_3D, mBlurTempPing));
			GL_CHECK(glTexStorage3D(GL_TEXTURE_3D, 1, format, size.x, size.y, size.z));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapMode));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapMode));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapMode));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, baseLevel));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, maxLevel));
			GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));


			GL_CHECK(glGenTextures(1, &mBlurTempPong));
			GL_CHECK(glActiveTexture(GL_TEXTURE0));
			GL_CHECK(glBindTexture(GL_TEXTURE_3D, mBlurTempPong));
			GL_CHECK(glTexStorage3D(GL_TEXTURE_3D, 1, format, size.x, size.y, size.z));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapMode));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapMode));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapMode));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, baseLevel));
			GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, maxLevel));
			GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));

			mBlurTexInitialized = true;
			LOG_INFO("Image3D - Created temporary targets for gaussian blur");
		}
	}

	void Image3D::createTexture(GLuint& handle, GLenum magFilter, GLenum minFilter, GLenum wrapMode, GLint baseLevel, GLint maxLevel)
	{
		GL_CHECK(glGenTextures(1, &handle));
		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, handle));
		GL_CHECK(glTexStorage3D(GL_TEXTURE_3D, 1, mFormat, mSize.x, mSize.y, mSize.z));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapMode));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapMode));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapMode));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, baseLevel));
		GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, maxLevel));
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));
	}

	void Image3D::bind(int textureUnit) const
	{
		GL_CHECK(glActiveTexture(textureUnit));
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, mObjectID));
	}

	void Image3D::unbind() const
	{
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));
	}

	glm::vec3 Image3D::getSize() const
	{
		return mSize;
	}

	GLenum Image3D::getFormat() const
	{
		return mFormat;
	}

	GLuint Image3D::getBlurredObjectID() const
	{
		return mBlurredImage;
	}

	GLuint Image3D::getObjectID() const
	{
		return mObjectID;
	}

	void Image3D::blur(float sigma, unsigned int size)
	{
		if (sigma < 0.0001f) sigma = 0.0001f;

		if (!mIsBlurImageInitialized)
		{
			createTexture(mBlurredImage);
			mIsBlurImageInitialized = true;
		}

		auto weights = computeFilterKernel(size, sigma);
		auto offsets = computeBlurOffsets(BlurStage::Horizontal, size);

		// Copy weights to GPU
		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mWeightSSBO));
		GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), weights.data(), GL_STATIC_READ));

		// Copy horizontal offsets to GPU
		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mOffsetSSBO));
		GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size * 3 * sizeof(int), offsets.data(), GL_STATIC_READ));

		std::shared_ptr<Pipeline> pipeline;

		pipeline = system::Renderer::getInstance().getPipelineByName("blur");

		pipeline->Bind();
		pipeline->SetUniform("kernelSize", size);

		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mOffsetSSBO));
		GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mWeightSSBO));

		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, mObjectID));

		glm::uvec3 resolution = static_cast<glm::uvec3>(mSize);

		GL_CHECK(glBindImageTexture(0, mBlurTempPing, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F));
		GL_CHECK(glDispatchCompute(resolution.x / 8, resolution.y / 8, resolution.z / 8));
		GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		// Compute and copy vertical offsets to GPU
		offsets = computeBlurOffsets(BlurStage::Vertical, size);
		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mOffsetSSBO));
		GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size * 3 * sizeof(int), offsets.data(), GL_STATIC_READ));

		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, mBlurTempPing));

		GL_CHECK(glBindImageTexture(0, mBlurTempPong, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F));
		GL_CHECK(glDispatchCompute(resolution.x / 8, resolution.y / 8, resolution.z / 8));
		GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		// Compute and copy depth offsets to GPU
		offsets = computeBlurOffsets(BlurStage::Depth, size);
		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, mOffsetSSBO));
		GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size * 3 * sizeof(int), offsets.data(), GL_STATIC_READ));

		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_3D, mBlurTempPong));

		GL_CHECK(glBindImageTexture(0, mBlurredImage, 0, GL_TRUE, 0, GL_WRITE_ONLY, mFormat));
		GL_CHECK(glDispatchCompute(resolution.x / 8, resolution.y / 8, resolution.z / 8));
		GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));

		pipeline->Unbind();
	}

	std::vector<float> Image3D::computeFilterKernel(int size, float sigma) const
	{
		std::vector<float> weights(size);

		float offset = -(size - 1) / 2.0f;
		float sum = 0.0f;

		for (auto &weight : weights)
		{
			// compute gaussian
			weight = glm::exp(-offset * offset / (2.0f * sigma * sigma));
			
			sum += weight;
			offset += 1.0f;
		}

		// normalize kernel
		for (auto &weight : weights)
			weight /= sum;

		return weights;
	}

	std::vector<glm::ivec3> Image3D::computeBlurOffsets(BlurStage stage, int size) const
	{
		std::vector<glm::ivec3> offsets(size);

		int diff = size / 2;
		for (auto &offset : offsets)
		{
			switch (stage)
			{
			case BlurStage::Horizontal:
				offset = glm::ivec3(-diff, 0, 0);
				break;
			case BlurStage::Vertical:
				offset = glm::ivec3(0, -diff, 0);
				break;
			case BlurStage::Depth:
				offset = glm::ivec3(0, 0, -diff);
				break;
			default:
				break;
			}

			--diff;
		}

		return offsets;
	}

	void Image3D::clear() const
	{
		std::shared_ptr<Pipeline> pipeline;

		pipeline = system::Renderer::getInstance().getPipelineByName("clear");

		pipeline->Bind();

		GL_CHECK(glBindImageTexture(0, mObjectID, 0, GL_TRUE, 0, GL_WRITE_ONLY, mFormat));
		GL_CHECK(glDispatchCompute(mSize.x / 8, mSize.y / 8, mSize.z / 8));
		GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		pipeline->Unbind();
	}

	void Image3D::reset()
	{
		if (mIsInitialized)
		{
			GL_CHECK(glDeleteTextures(1, &mObjectID));
			GL_CHECK(glDeleteTextures(1, &mBlurredImage));

			mObjectID = 0;
			mFormat = 0;
			mSize = glm::uvec3(0, 0, 0);
			mIsInitialized = false;
			mIsBlurImageInitialized = false;
			LOG_DEBUG("Image3D - resetted image");
		}	
	}
}
