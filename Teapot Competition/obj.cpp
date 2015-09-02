#include "obj.h"

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <math.h>
#include <glm/glm.hpp>
#define PI 3.14159

obj::obj()
{
}

void obj::parse(const char * path){
	printf("Loading OBJ file %s...\n", path);
	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> temp_vertices;
	FILE * file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file!");
		getchar();
		return;
	}
	std::vector<glm::vec3> faces;
	std::vector<glm::vec3> face_normal;
	glm::vec3 zero_vec;
	zero_vec.x = 0;
	zero_vec.y = 0;
	zero_vec.z = 0;
	float y_max = 0;
	while (1){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
		if (strcmp(lineHeader, "v") == 0){
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
			if (vertex.y > y_max)
				y_max = vertex.y;
		}
		else if (strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			int matches = fscanf_s(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches != 3){
				printf("File can't be read\n"); 
				return;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			//
			glm::vec3 temp;
			temp.x = vertexIndex[0];
			temp.y = vertexIndex[1];
			temp.z = vertexIndex[2];
			faces.push_back(temp);

			//calc face normals
			glm::vec3 vertex_1 = temp_vertices[vertexIndex[0] - 1];
			glm::vec3 vertex_2 = temp_vertices[vertexIndex[1] - 1];
			glm::vec3 vertex_3 = temp_vertices[vertexIndex[2] - 1];

			glm::vec3 temp_normal = glm::cross((vertex_2 - vertex_1), (vertex_3 - vertex_1));
			temp_normal = glm::normalize(temp_normal);
			face_normal.push_back(temp_normal);
		}
	}
	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];

		// Put the attributes in buffers
		vertices.push_back(vertex);
		/*Given a vertex's position (x,y,z), its cylindrical coordinates would be (r cos(theta), y, r sin(theta)) for a radius r,
		an angle theta, and an altitude y, where the angle theta is atan2(z,x).
		Then you can set texture coordinates (s,t) as s = (theta+PI)/2PI and t = y/ymax*/
		float theta = atan2f(vertex.z, vertex.x);
		glm::vec2 temp_uv;
		temp_uv.s = (theta + PI) / (2 * PI);
		temp_uv.t = vertex.y / y_max;
		uvs.push_back(temp_uv);
	}

	std::vector<glm::vec3> vert_norms(faces.size());
	//loop through each face
	for (unsigned int k = 0; k < faces.size(); k++){
		glm::vec3 curr_face = faces[k];
		glm::vec3 curr_norm = face_normal[k];
		int vert1_idx = curr_face.x;
		int vert2_idx = curr_face.y;
		int vert3_idx = curr_face.z;

		vert_norms[vert1_idx - 1] += curr_norm;
		vert_norms[vert2_idx - 1] += curr_norm;
		vert_norms[vert3_idx - 1] += curr_norm;

		//printf("indexes %d, %d, %d \n", vert1_idx, vert2_idx, vert3_idx);
		//printf("normal (%f, %f, %f) \n", curr_norm.x, curr_norm.y, curr_norm.z);
		//printf("vert norm 1 (%f, %f, %f) \n", normals[0].x, normals[0].y, normals[0].z);
	}
	for (unsigned int k = 0; k < normals.size(); k++){
		normals[k] = glm::normalize(normals[k]);
		printf("normal %d (%f, %f, %f) \n", k, normals[k].x, normals[k].y, normals[k].z);
	}
	for (unsigned int i = 0; i<vertexIndices.size(); i++){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = vert_norms[vertexIndex - 1];

		// Put the attributes in buffers
		normals.push_back(vertex);
	}
	printf("loaded obj successfully \n");
}

obj::~obj()
{
}
