#pragma once
#include "Vector.h"
#include "Function.h"

#include <vector>
class Shape {
public:
	Shape();	
	Vector3 color;
	bool enable;
	Matrix4 transform_matrix = Matrix4::Identity();

public:
	
	
	virtual void Draw();
	void SetColor(Vector3 &c);
};