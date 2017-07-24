#pragma once

#include "glm/vec3.hpp"
#include "GL/glew.h"
#include <vector>

namespace vfx
{
	enum class BlurStage
	{
		Horizontal,
		Vertical,
		Depth
	};

	class Image3D
	{
	public:
		Image3D(const glm::uvec3& rSize,
				bool blurrable = false,
				GLenum format = GL_RGBA16F,
				GLenum magFilter = GL_LINEAR,
				GLenum minFilter = GL_LINEAR,
				GLenum wrapMode = GL_CLAMP_TO_EDGE,
				GLint baseLevel = 0,
				GLint maxLevel = 0);
		virtual ~Image3D();

		/// \brief	Initializer for shared blur targets.
		static void createBlurTempTargets(const glm::uvec3& size, GLenum format = GL_RGBA16F, GLenum magFilter = GL_LINEAR, GLenum minFilter = GL_LINEAR, GLenum wrapMode = GL_CLAMP_TO_EDGE, GLint baseLevel = 0, GLint maxLevel = 0);

		/// \brief	Binds the image.
		///
		/// \param textureUnit Selected texture unit.
		void bind(int textureUnit) const;

		/// \brief	Unbinds image.
		void unbind() const;

		/// \brief	Image size getter.
		glm::vec3 getSize() const;

		/// \brief	Image format getter.
		GLenum getFormat() const;

		/// \brief	Blurred image handle getter.
		GLuint getBlurredObjectID() const;

		/// \brief	Image handle getter.
		GLuint getObjectID() const;

		/// \brief	Applies 3D gaussian blur filter on this image
		///
		/// \param sigma Blur strength amplifier.
		/// \param size Size of the kernel.
		void blur(float sigma, unsigned int size);

		/// \brief	Clears texture by filling witih predefined value (0)
		void clear() const;

		/// \brief	Resets this instance by deleting wrapped texture
		///			and reseting its properties.
		void reset();

	private:
		void createTexture(GLuint& handle, GLenum magFilter = GL_LINEAR, GLenum minFilter = GL_LINEAR, GLenum wrapMode = GL_CLAMP_TO_EDGE, GLint baseLevel = 0, GLint maxLevel = 0);

		/// \brief Computes kernel mask for gaussian blur.
		///
		/// \param size Size of kernel.
		/// \param sigma Blur strength amplifier.
		std::vector<float> computeFilterKernel(int size, float sigma) const;

		/// \brief Computes offsets for current pass of separated gaussian blur filter.
		///
		/// \param stage Filter stage (separated)
		/// \param size Size of kernel.
		std::vector<glm::ivec3> computeBlurOffsets(BlurStage stage, int size) const;

	private:
		glm::uvec3 mSize;		//!< Image dimensions.
		GLenum mFormat;			//!< Image format.
		bool mIsInitialized;	//!< Iinitialization flag.
		bool mIsBlurImageInitialized;

		GLuint mOffsetSSBO;		//!< Shader storage object for blur data.
		GLuint mWeightSSBO;		//!< Shader storage object for blur data.
		GLuint mBlurredImage;	//!< Blurred image handle
		GLuint mObjectID;

		static GLuint mBlurTempPing;	//!< Temporary storage texture for blurring.
		static GLuint mBlurTempPong;	//!< Temporary storage texture for blurring.
		static bool mBlurTexInitialized;
	};
}