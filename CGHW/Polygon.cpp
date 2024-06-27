#include "Polygon.h"
#include "Function.h"


Poly::Poly() {	
	color = Vector3(1,1,1);
	enable = true;
}

Poly::Poly(Vector3 v,Vector3 c) {
	Add(v);
	color = c;
	enable = true;
}

void Poly::Add(Vector3 v) { 
	vertex.push_back(v);


}



void Poly::Draw() {
	
	if (vertex.size() <= 1) return;
	std::vector<Vector3> out;
	for (int i = 0; i < vertex.size(); i += 1) {
		out.push_back((transform_matrix * Vector4(vertex[i], 1)).XYZ());
	}

	for (int i = 0, j = out.size() - 1; i < out.size(); j = i++) {		
		if (!enable && i == 0) continue;
		Vector3 v1 = out[i];
		Vector3 v2 = out[j];
		
		drawLine(v1.x,v1.y,v2.x,v2.y,color);		
	}

	


}


void Poly::Fill(std::vector<Vector3> point) {
	Vector3 minv(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), 0);
	Vector3 maxv(-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), 0);


	// To Do : Find the boundary of the graphic.
	// To Do : Scan the boundary and use the pnpoly to check the point whether inside the graphic.
		
}

void Poly::Draw(Vector3 r1, Vector3 r2,Vector3 v1,Vector3 v2) {
	if (vertex.size() <= 1) return;
	std::vector<Vector3> out;
	for (int i = 0; i < vertex.size(); i += 1) {
		Vector3 p = (transform_matrix * Vector4(vertex[i], 1)).XYZ();
		
		p.x = map(p.x, r1.x, r2.x, v1.x, v2.x);
		p.y = map(p.y, r1.y, r2.y, v1.y, v2.y);

		out.push_back(p);
	}
	
	Vector3 v[2] {v1,v2};
	if (out.size() >= 3) {
		out = clipping_algorithm(v, out);
	}
	
	
	for (int i = 0, j = out.size() - 1; i < out.size(); j = i++) {
		if (!enable && i == 0) continue;
		Vector3 v1 = out[i];
		Vector3 v2 = out[j];
		
		drawLine(round(v1.x), round(v1.y), round(v2.x), round(v2.y), color);
	}


	Fill(out);

}

Point::Point(Vector3 v,Vector3 c) {
	vertex = v;
	color = c;
	
}





void Point::Draw() {
	
	drawSquare(vertex.x,vertex.y,color);	

}

