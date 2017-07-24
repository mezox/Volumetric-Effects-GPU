/*
Project:		Efficient computation of Lighting
Type:			Bachelor's thesis
Author:			Tomáš Kubovčík, xkubov02@stud.fit.vutbr.cz
Supervisor:		Ing. Tomáš Milet
School info:	Brno Univeristy of Technology (VUT)
Faculty of Information Technology (FIT)
Department of Computer Graphics and Multimedia (UPGM)

Project information
---------------------
The goal of this project is to efficiently compute lighting in scenes
with hundrends to thousands light sources. To handle this there have been
implemented lighting techniques as deferred shading, tiled deferred shading
and tiled forward shading. Application requires GPU supporting OpenGL 3.3+
but may be compatible with older versions. Application logic was implemented
using C/C++ with some external helper libraries to handle basic operations.

File information
-----------------
Header file of Texture loader class.
*/

/// <summary>
/// Texture class
/// </summary>
class Texture
{
	public:
		Texture(GLenum TextureTarget, const std::string& filename);

		bool Load(bool srgb);

		void Bind(GLenum textureUnit);
		void Bind(GLenum textureUnit, GLuint defaultTex);

	protected:
		std::string m_filename;
		GLuint m_textureObj;
		GLenum m_textureTarget;
};
