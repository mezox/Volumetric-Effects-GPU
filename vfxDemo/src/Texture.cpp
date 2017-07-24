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
This file represents external texture loader (for mesh textures, ...). 
To load textures, stb_image.c library has been used.
*/

#include <iostream>
#include <stdexcept>
#include <string>
#include <GL\glew.h>
#include "stb_image.c"
#include "Texture.h"


/// <summary>
/// Initializes a new instance of the <see cref="Texture"/> class.
/// </summary>
/// <param name="textureTarget">The texture target.</param>
/// <param name="filename">The filename.</param>
Texture::Texture(GLenum textureTarget, const std::string& filename)
{
    m_textureTarget = textureTarget;
    m_filename      = filename;
}

/// <summary>
/// Loads the texture from file using stb_image.c library.
/// </summary>
/// <param name="srgb">if set to <c>true</c> set color model to SRGB, else RGB.</param>
/// <returns>TRUE on success.</returns>
bool Texture::Load(bool srgb)
{
	int x, y, n;
	int force_channels = 4;

	unsigned char* image_data = NULL;

	try
	{
		image_data = stbi_load(m_filename.c_str(), &x, &y, &n, force_channels);
	}
	catch (std::exception)
	{
		std::cerr << "ERROR: could not load" << m_filename << "\n";
	}

    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
	
	if (srgb)
	{
		glTexImage2D(m_textureTarget, 0, GL_SRGB_ALPHA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	}
	else
	{
		glTexImage2D(m_textureTarget, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	}

	//generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//free resources
	free(image_data);

    return true;
}


/// <summary>
/// Binds texture to specified texture unit.
/// </summary>
/// <param name="TextureUnit">texture unit.</param>
void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}

/// <summary>
/// Binds texture to specified texture unit, if texture does not cotain any data
/// bind default texture instead.
/// </summary>
/// <param name="TextureUnit">texture unit.</param>
/// <param name="defaultTex">default texture ID.</param>
void Texture::Bind(GLenum TextureUnit, GLuint defaultTex)
{
	if (m_textureObj == -1)
	{
		glActiveTexture(TextureUnit);
		glBindTexture(m_textureTarget, defaultTex);
	}
	else
	{
		glActiveTexture(TextureUnit);
		glBindTexture(m_textureTarget, m_textureObj);
	}
}
