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
This file implements Mesh class, which is used to load external scene models
and their textures especially in .obj format. To achieve this Assimp library 
has been used. This class also represents object renderer.
*/

#include <assert.h>
#include "Mesh.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <iostream>

#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }


/// <summary>
/// Initializes a new instance of the <see cref="Mesh"/> class.
/// </summary>
Mesh::Mesh()
{
}

/// <summary>
/// Finalizes an instance of the <see cref="Mesh"/> class.
/// </summary>
Mesh::~Mesh()
{
    Clear();
}

/// <summary>
/// Clears textures, VBOs, VAO.
/// </summary>
void Mesh::Clear()
{
    for (unsigned int i = 0 ; i < diff_textures.size() ; i++)
	{
        SAFE_DELETE(diff_textures[i]);
		SAFE_DELETE(bump_textures[i]);
		SAFE_DELETE(spec_textures[i]);
    }

	if (buffers[0] != 0)
	{
		glDeleteBuffers(6, buffers);
	}

	if (vao != 0)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}

/// <summary>
/// Loads the mesh from file.
/// </summary>
/// <param name="Filename">Model file.</param>
/// <returns></returns>
bool Mesh::LoadMesh(const std::string& Filename)
{
    Assimp::Importer Importer;
    bool rc = false;

    //clear previous loaded mesh
    Clear();

	//create vao for mesh
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create the buffers for the vertices atttributes
	glGenBuffers(6, buffers);

    //read file content, set post-processing flags
    const aiScene* oScene = Importer.ReadFile(Filename.c_str(),
                                              aiProcess_Triangulate |
                                              aiProcess_GenSmoothNormals |
											  aiProcess_CalcTangentSpace |
                                              aiProcess_FlipUVs);

    if(oScene)
	{
		//prints control info
       /* printf ("  %i animations\n", oScene->mNumAnimations);
        printf ("  %i cameras\n", oScene->mNumCameras);
        printf ("  %i lights\n", oScene->mNumLights);
        printf ("  %i materials\n", oScene->mNumMaterials);
        printf ("  %i meshes\n", oScene->mNumMeshes);
        printf ("  %i textures\n", oScene->mNumTextures);
		*/

		//set array size based on mesh/texture counts
        meshes.resize(oScene->mNumMeshes);
        diff_textures.resize(oScene->mNumMaterials);
		bump_textures.resize(oScene->mNumMaterials);
		spec_textures.resize(oScene->mNumMaterials);
		specularExponents.resize(oScene->mNumMaterials);

		unsigned int numIndices = 0;
		unsigned int numVertices = 0;

		/*
			All corresponding attributes are being stored in single VBO for all meshes:
			VBO for indices
			VBO for positions
			VBO for texcoords
			VBO for normals

			so I need to save indices to VBO where are data for specific mesh:
			baseVertex	- offset to position VBO
			baseIndex	- offset to indices VBO
			materialIndex	-	material corresponding to actual mesh
			numIndices	- indices count
		*/
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].materialIndex = oScene->mMeshes[i]->mMaterialIndex;
			meshes[i].numIndices = oScene->mMeshes[i]->mNumFaces * 3;
			meshes[i].baseVertex = numVertices;
			meshes[i].baseIndex = numIndices;

			numVertices += oScene->mMeshes[i]->mNumVertices;
			numIndices += meshes[i].numIndices;
		}

		printf("numindices: %d\n",numIndices);

		vp.reserve(numVertices);
		vn.reserve(numVertices);
		vt.reserve(numVertices);
		vtn.reserve(numVertices);
		vbtn.reserve(numVertices);

		vindices.reserve(numIndices);

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
            const aiMesh* mesh = oScene->mMeshes[i];

			for (unsigned int j = 0; j < mesh->mNumVertices; j++)
			{
				if (mesh->HasPositions())
				{
					const aiVector3D* vpos = &(mesh->mVertices[j]);

					vp.push_back(vpos->x);
					vp.push_back(vpos->y);
					vp.push_back(vpos->z);
				}

				if (mesh->HasNormals())
				{
					const aiVector3D* vnormal = &(mesh->mNormals[j]);

					vn.push_back(vnormal->x);
					vn.push_back(vnormal->y);
					vn.push_back(vnormal->z);
				}

				if (mesh->HasTextureCoords(0))
				{
					const aiVector3D* vtexture = &(mesh->mTextureCoords[0][j]);

					vt.push_back(vtexture->x);
					vt.push_back(vtexture->y);
				}

				if (mesh->HasTangentsAndBitangents())
				{
					const aiVector3D* vtan = &(mesh->mTangents[j]);
					const aiVector3D* vbitan = &(mesh->mBitangents[j]);

					vtn.push_back(vtan->x);
					vtn.push_back(vtan->y);
					vtn.push_back(vtan->z);

					vbtn.push_back(vbitan->x);
					vbtn.push_back(vbitan->y);
					vbtn.push_back(vbitan->z);
				}
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				const aiFace& Face = mesh->mFaces[i];

				assert(Face.mNumIndices == 3);

				vindices.push_back(Face.mIndices[0]);
				vindices.push_back(Face.mIndices[1]);
				vindices.push_back(Face.mIndices[2]);
			}
        }

		//create vbo for positions
		glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vp.size() * sizeof (float), &vp[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		//create vbo for tex coords
		glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXCOORD_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vt.size() * sizeof (float), &vt[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		
		//create vbo for normals
		glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof (float), &vn[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		//create vbo for tangents
		glBindBuffer(GL_ARRAY_BUFFER, buffers[TANGENT_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vtn.size() * sizeof (float), &vtn[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		//create vbo for bitangents
		glBindBuffer(GL_ARRAY_BUFFER, buffers[BITANGENT_VBO]);
		glBufferData(GL_ARRAY_BUFFER, vbtn.size() * sizeof (float), &vbtn[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		//create vbo for indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDICES_VBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vindices.size() * sizeof (unsigned int), &vindices[0], GL_STATIC_DRAW);

		//clear memory
		vp.clear();
		vp.shrink_to_fit();
		vn.clear();
		vn.shrink_to_fit();
		vt.clear();
		vt.shrink_to_fit();
		vtn.clear();
		vtn.shrink_to_fit();
		vbtn.clear();
		vbtn.shrink_to_fit();

		// Extract the directory part from the file name
        std::string::size_type SlashIndex = Filename.find_last_of("/");
        std::string Dir;

        if (SlashIndex == std::string::npos)
		{
            Dir = ".";
        }
        else if (SlashIndex == 0)
		{
            Dir = "/";
        }
        else 
		{
            Dir = Filename.substr(0, SlashIndex);
        }
        /*.................Initialization of meshes end....................*/

		glGenTextures(1, &defaultTextureOne);
		glBindTexture(GL_TEXTURE_2D, defaultTextureOne);
		glm::vec4 pixel = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, &pixel);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(1, &defaultNormalTexture);
		glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
		glm::vec4 pixelZ = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, &pixelZ);
		glBindTexture(GL_TEXTURE_2D, 0);

        /*.............Initialize the materials.............................*/
        for(unsigned int i = 0 ; i < oScene->mNumMaterials ; i++)
		{
			const aiMaterial* pMaterial = oScene->mMaterials[i];
            
			diff_textures[i] = NULL;
			bump_textures[i] = NULL;
			spec_textures[i] = NULL;

			aiString Path;

            if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {

                if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                    std::string FullPath = Dir + "/" + Path.data;
                    diff_textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

					aiColor3D color(0.f, 0.f, 0.f);
					pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
					
					Kds = glm::vec3(color.r, color.g, color.b);

					specularExponents[i] = 10;


                    if (!diff_textures[i]->Load(true)) {
                        printf("Error loading diff. texture '%s'\n", FullPath.c_str());
                        delete diff_textures[i];
                        diff_textures[i] = NULL;
                        rc = false;
                    }
                    else {
                        printf("Loaded diff. texture '%s'\n",FullPath.c_str());
                    }
                }
            }

			if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) {

				if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string FullPath = Dir + "/" + Path.data;
					spec_textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

					if (!spec_textures[i]->Load(true)) {
						printf("Error loading spec. texture '%s'\n", FullPath.c_str());
						delete spec_textures[i];
						spec_textures[i] = NULL;
						rc = false;
					}
					else {
						printf("Loaded spec. texture '%s'\n",FullPath.c_str());
					}
				}
			}

			if (pMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0) {

				if (pMaterial->GetTexture(aiTextureType_HEIGHT, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string FullPath = Dir + "/" + Path.data;
					bump_textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

					if (!bump_textures[i]->Load(false)) {
						printf("Error loading normal texture '%s'\n", FullPath.c_str());
						delete bump_textures[i];
						bump_textures[i] = NULL;
						rc = false;
					}
					else {
						printf("Loaded normal texture '%s'\n",FullPath.c_str());
					}
				}
			}
        }
        /*.................Initialization of materials end....................*/
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

	glBindVertexArray(0);

    return rc;
}

/// <summary>
/// Renders the simple scene with no lighting.
/// </summary>
void Mesh::RenderSimple(){

	//enable VAO
	glBindVertexArray(vao);

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		const unsigned int MaterialIndex = meshes[i].materialIndex;

		if (MaterialIndex < diff_textures.size() && diff_textures[MaterialIndex])
		{
				diff_textures[MaterialIndex]->Bind(GL_TEXTURE0);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, defaultTextureOne);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, meshes[i].numIndices, GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int)* meshes[i].baseIndex), meshes[i].baseVertex);
	}

	glBindVertexArray(0);
}

/// <summary>
/// Renders the scene.
/// </summary>
/// <param name="shader">references shader ID to access shader uniforms.</param>
void Mesh::Render(GLuint shader)
{
	GLuint specularExponent_loc;

    //enable VAO
	glBindVertexArray(vao);

	if (shader)
	{
		specularExponent_loc = glGetUniformLocation(shader, "specExponent");
		
	}
		
    for(unsigned int i = 0 ; i < meshes.size() ; i++)
	{

        const unsigned int MaterialIndex = meshes[i].materialIndex;

        if (MaterialIndex < diff_textures.size() && diff_textures[MaterialIndex])
		{
			if (diff_textures[MaterialIndex] != NULL){
				diff_textures[MaterialIndex]->Bind(GL_TEXTURE0);

				if (shader)
				{
					GLuint Kd_loc = glGetUniformLocation(shader, "Kd");
					glUniform3f(Kd_loc, Kds.r, Kds.g, Kds.b);
				}
			}
			else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, defaultTextureOne);
			}

			if (bump_textures[MaterialIndex] != NULL){
				bump_textures[MaterialIndex]->Bind(GL_TEXTURE1);
			}
			else {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, defaultNormalTexture);
			}

			if (spec_textures[MaterialIndex] != NULL){
				spec_textures[MaterialIndex]->Bind(GL_TEXTURE2);
				
			}
			else {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, defaultTextureOne);
			}

			if (shader)
			{
				glUniform1f(specularExponent_loc, specularExponents[MaterialIndex]);
			}
        }

		glDrawElementsBaseVertex(GL_TRIANGLES, meshes[i].numIndices,GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int)* meshes[i].baseIndex), meshes[i].baseVertex);
    }

	glBindVertexArray(0);
}
