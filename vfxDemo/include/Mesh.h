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
Mesh class header file.
*/

#include <vector>
#include <GL/glew.h>

#include <Importer.hpp> // C++ importer interface
#include <scene.h>      // Output data structure
#include <postprocess.h>// Post processing flags
#include <glm/glm.hpp>

#include "texture.h"

#define POS_VBO 0
#define TEXCOORD_VBO 1
#define NORMAL_VBO 2
#define TANGENT_VBO 3
#define BITANGENT_VBO 4
#define INDICES_VBO 5

/// <summary>
/// Class that represents loaded Mesh and its renderer
/// </summary>
class Mesh
{
    public:
        Mesh();
        ~Mesh();

        bool LoadMesh(const std::string& Filename);
		void Render(GLuint shader);
		void RenderSimple();
		glm::vec3 getKd(){ return Kds; }
		float getSpecExponent() { return specularExponents[0]; }

    private:
        void Clear();

        struct MeshEntry {

			MeshEntry()
			{
				numIndices = 0;
				baseVertex = 0;
				baseIndex = 0;
				materialIndex = 0;
			}

			unsigned int baseVertex;
			unsigned int baseIndex;
			unsigned int numIndices;
			unsigned int materialIndex;
        };

        std::vector<MeshEntry> meshes;

		//TEXTURES
        std::vector<Texture*> diff_textures;
		std::vector<Texture*> bump_textures;
		std::vector<Texture*> spec_textures;
		glm::vec3 Kds;
		std::vector<float> specularExponents;

		GLuint defaultTextureOne; /**< all 1, single pixel texture to use when no texture is loaded. */
		GLuint defaultNormalTexture;  /**< { 0.5, 0.5, 1, 1 }, single pixel float texture to use when no normal texture is loaded. */

		//CPU DATA
		//vectors for positions, texcoords, normals, tangents and bitangents, indices
		std::vector<float> vp;
		std::vector<float> vt;
		std::vector<float> vn;
		std::vector<float> vtn;
		std::vector<float> vbtn;
		std::vector<unsigned int> vindices;

		//GPU DATA
		//Vertex Buffer Objects
		GLuint buffers[6];

		//Vertex Array Object
		GLuint vao;		
};
