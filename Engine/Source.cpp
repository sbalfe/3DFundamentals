#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class customObj {

public:
	bool loadOBJ(
		const char* path,
		std::vector < glm::vec3 >& out_vertices,
		std::vector < glm::vec2 >& out_uvs,
		std::vector < glm::vec3 >& out_normals
	) {
		FILE* file = fopen(path, "r");
		if (file == NULL) {
			printf("cannot open file ! \n");
			return false;
		}

		while (1) {

			/* decent size for first word buffer */
			char lineHeader[128];

			// read first word of line

			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF) {
				break; //EOF = end of file therefore quit the loop[
			}
			lineHeader[sizeof(lineHeader)-1] = '\0';

			if (strcmp(lineHeader, "v") == 0) {

				glm::vec3 vertex;
				/* reads 3 floating points and stores in teh corresponding variadic args*/
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

				temp_vertices.push_back(vertex);

			}

			/* vertex uv coordaintes reading*/
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);

			}

			/* vertex normals reading  */
		
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}

			/* now process the faces */
			else if (strcmp(lineHeader, "f") == 0) {

				/* define each vertex as a string */
				std::string vertex1, vertex2, vertex3;


				/* create the indices */
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

				/* just parse the data with scanf as described in notes passing into each buffer */
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				
				/* ligthweight .obj loader ofc.*/
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return false;
				}

				/* add these to the array */
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				// For each vertex of each triangle
				for (unsigned int i = 0; i < vertexIndices.size(); i++) {
					unsigned int vertexIndex = vertexIndices[i];

					/* c++ starts at 0 therefore -1*/
					glm::vec3 vertex = temp_vertices[vertexIndex - 1];

					out_vertices.push_back(vertex);
				}

				for (unsigned int i = 0; i < uvIndices.size(); i++) {
					unsigned int vertexIndex = vertexIndices[i];

					/* c++ starts at 0 therefore -1*/
					glm::vec3 vertex = temp_vertices[vertexIndex - 1];

					out_uvs.push_back(vertex);
				}
				for (unsigned int i = 0; i < normalIndices.size(); i++) {
					unsigned int vertexIndex = vertexIndices[i];

					/* c++ starts at 0 therefore -1*/
					glm::vec3 vertex = temp_vertices[vertexIndex - 1];

					out_normals.push_back(vertex);
				}
		}
	}

}

private:

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
};

int main() {
	
	customObj handle;

	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.

	handle.loadOBJ("models\\bunny.obj", vertices, uvs, normals );

	for (auto vec : vertices) {
		std::cout << vec.x << std::endl;
	}
}