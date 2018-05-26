#ifndef OBJLOADER_H_INCLUDED
#define OBJLOADER_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "MTLLoader.h"
#include "Mesh.h"
#include "Util.h"

// class VertexNone 		{ public: constexpr static const char* name = "VertexNone"; };	
// class VertexPosition 	{ public: constexpr static const char* name = "VertexPosition"; };
// class VertexNormal 		{ public: constexpr static const char* name = "VertexNormal"; };
// class VertexTexCoord 	{ public: constexpr static const char* name = "VertexTexCoord"; };
// class VertexTexID 		{ public: constexpr static const char* name = "VertexTexID"; };
// class VertexColor		{ public: constexpr static const char* name = "VertexColor"; };

class VertexColor1 		{ public: constexpr static const char* name = "Color1"; };
class VertexColor2 		{ public: constexpr static const char* name = "Color2"; };
class VertexColor3 		{ public: constexpr static const char* name = "Color3"; };

class VertexTexID1 		{ public: constexpr static const char* name = "Tex1"; };
class VertexTexID2 		{ public: constexpr static const char* name = "Tex2"; };
class VertexTexID3 		{ public: constexpr static const char* name = "Tex3"; };
class VertexTexID4 		{ public: constexpr static const char* name = "Tex4"; };
class VertexTexID5 		{ public: constexpr static const char* name = "Tex5"; };
class VertexTexID6 		{ public: constexpr static const char* name = "Tex6"; };
class VertexTexID7 		{ public: constexpr static const char* name = "Tex7"; };
class VertexTexID8 		{ public: constexpr static const char* name = "Tex8"; };

template <typename VertexType>
class OBJLoader {
public:
	OBJLoader() {
		/// santinels so obj indexes will match
		texCoords.push_back(Math::Point2f());
		normals.push_back(Math::Point3f());
		positions.push_back(Math::Point3f());
	}

	std::string currentDirectory = ""; 

	Mesh<VertexType> mesh;
	MTLLoader mtlLoader; 
	
	std::vector <Math::Point3f> positions; 
	std::vector <Math::Point2f> texCoords;
	std::vector <Math::Point3f> normals;

	std::vector <std::vector <int>> faces; 
		
	int currentMtl = 0; 
	std::vector <int> mtlForFace;

	std::vector <std::tuple<int, int, int>> indexes; 
	std::map <std::tuple<int, int, int>, int> indexMap; 
	// 1/2/1 is transformed in 0 
	// 2/1/2 is tronsformed in 1 
	// 2/2/1 is transformed in 2	
	// order of aparence dictates the number wich is given   


	/// if OBJLoader is temp we construct the mesh and return it
	Mesh<VertexType>&& loadMesh (std::string directory, std::string filename) && {
		currentDirectory = directory; 
		_loadMesh(filename); 

		return std::move(mesh);
	}

	/// directory will be used for other files that are loaded 
	Mesh<VertexType>& loadMesh (std::string directory, std::string filename) & {
		currentDirectory = directory; 
		_loadMesh(filename);

		return mesh; 
	}

	Mesh<VertexType>& getMesh () {
		return mesh; 
	}	

	void _loadMesh (std::string filename) {
		std::string path = currentDirectory + filename;
		std::ifstream file(path.c_str());

		if (!file)
			throw std::runtime_error(".obj not found: " + filename);

		std::string line = ""; 
		while (getline(file, line)) {
			std::stringstream lineStream(line);

			std::string lineHeader = ""; 
			lineStream >> lineHeader; 

			if (lineHeader == "f") {
				parseFace(lineStream); 
			}
			else if (lineHeader == "v") {
				parsePosition(lineStream); 
			}
			else if (lineHeader == "vn") {
				parseNormal(lineStream); 
			}
			else if (lineHeader == "vt") {
				parseTexCoord(lineStream); 
			}
			else if (lineHeader == "usemtl") {
				parseUseMTL(lineStream); 
			}
			else if (lineHeader == "mtllib") {
				parseLibMTL(lineStream); 
			}
		}

		for (int index = 0; index < indexes.size(); index++) {
			VertexType vetex; 

			vetex.template setIfExists<VertexPosition>(positions[std::get<0>(indexes[index])]);
			vetex.template setIfExists<VertexNormal>(normals[std::get<1>(indexes[index])]);
			vetex.template setIfExists<VertexTexCoord>(texCoords[std::get<2>(indexes[index])]);

			mesh.addVertex(vetex); 
		}

		mesh.elementIndex = faces; 
		mesh.materialIndex = mtlForFace; 
		mesh.materials = mtlLoader.materials;
		
		file.close();
	}

	void parseUseMTL (std::stringstream& stream) {
		std::string mtlName; 
		stream >> mtlName; 

		currentMtl = mtlLoader.getMaterialIndex(mtlName);
	}

	void parseLibMTL (std::stringstream& stream) {
		std::string mtlName; 
		stream >> mtlName;

		mtlLoader.loadMtl(currentDirectory, mtlName);  
	}

	void parsePosition (std::stringstream& stream) {
		Math::Point3f position; 
		stream >> position; 

		positions.push_back(position);
	}

	void parseNormal (std::stringstream& stream) {
		Math::Point3f normal; 
		stream >> normal; 

		normals.push_back(normal);
	}

	void parseTexCoord (std::stringstream& stream) {
		Math::Point2f texCoord; 
		stream >> texCoord; 

		texCoords.push_back(texCoord);
	}

	void parseFace (std::stringstream& stream) {
		int faceVertexCount = 0;
		std::string faceVertexIndexes = ""; 

		faces.push_back(std::vector<int>()); 
		mtlForFace.push_back(currentMtl); 
	
		while (stream >> faceVertexIndexes) {
			auto vertexIDS = Util::split(faceVertexIndexes, '/');

			/// 0 means the index is not being used
			int posIndex = 0;
			int texIndex = 0;
			int normIndex = 0; 

			// int validIndexes = 0; 
			if (vertexIDS.size() > 1 && vertexIDS[0] != "") {	/// position 
				posIndex = Util::stringToNumber<int>(vertexIDS[0]); 
			}

			if (vertexIDS.size() > 2 && vertexIDS[1] != "") {	/// normal 
				texIndex = Util::stringToNumber<int>(vertexIDS[1]); 
			}

			if (vertexIDS.size() > 3 && vertexIDS[2] != "") {	/// texture coordinate
				normIndex = Util::stringToNumber<int>(vertexIDS[2]); 
			}

			auto fitObjIndex = [](int vecSize, int index) {
				if (index < 0) {
					index = std::min(
						std::max(
							vecSize + index + 1, 
							0
						), 
						vecSize - 1
					);
				}
				return index; 
			};

			posIndex = fitObjIndex(positions.size(), posIndex);
			normIndex = fitObjIndex(normals.size(), normIndex);
			texIndex = fitObjIndex(texCoords.size(), texIndex);

			auto vertexIndexes = std::make_tuple(posIndex, normIndex, texIndex); 

			int vertexIndex = 0; 

			if (indexMap.find(vertexIndexes) == indexMap.end()) {
				indexes.push_back(vertexIndexes);
				indexMap[vertexIndexes] = indexes.size() - 1;  
			}

			vertexIndex = indexMap[vertexIndexes];

			faces.back().push_back(vertexIndex);
		}
	}
};

#endif