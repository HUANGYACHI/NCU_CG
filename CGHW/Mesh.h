#pragma once
#include "Vector.h"
#include <string>
#include <vector>
class Mesh {
public:
	Mesh();
	Mesh(std::string str);
	
	std::vector<int> triangles;
	std::vector<int> uv_triangles;
	std::vector<int> normal_triangles;

	std::vector<Vector3> verties;
	std::vector<Vector3> uvs;
	std::vector<Vector3> normals;
	std::vector<Vector4> tangents;
	
	Matrix4 transform_matrix = Matrix4::Identity();
	void calcTangent();
	void calcNormal();


};