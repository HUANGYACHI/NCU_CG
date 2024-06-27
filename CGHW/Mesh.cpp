#include "Mesh.h"
#include <fstream>
#include <sstream>
Mesh::Mesh(std::string str) {
    std::ifstream fin(str);
    if (!fin) {
        return;
    }

    std::string line;
    while (!fin.eof()) {
        std::getline(fin, line);
        if (line.find("v ") == 0) {
           
            std::stringstream ss(line.c_str() + 2);
            float x, y, z;
            ss >> x >> y >> z;
            verties.push_back(Vector3(x,y,z));
        }
        else if (line.find("vn ") == 0) {
            std::stringstream ss(line.c_str() + 2);
            float x, y, z;
            ss >> x >> y >> z;
            normals.push_back(Vector3(x, y, z).Normalized());
        }
        else if (line.find("vt ") == 0) {
            std::stringstream ss(line.c_str() + 2);
            float x, y;
            ss >> x >> y ;
            //std::cout << x << " " << y  << std::endl;
            uvs.push_back(Vector3(x, y,0));
        }
        else if (line.find("f ") == 0) {
            //Count the slashes
            int num_slashes = 0;
            size_t last_slash_ix = 0;
            bool doubleslash = false;
            for (size_t i = 0; i < line.size(); ++i) {
                if (line[i] == '/') {
                    line[i] = ' ';
                    if (last_slash_ix == i - 1) {                      
                        doubleslash = true;
                    }
                    last_slash_ix = i;
                    num_slashes++;
                }
            }
            uint32_t a = 0, b = 0, c = 0, d = 0;
            uint32_t at = 0, bt = 0, ct = 0, dt = 0;
            uint32_t _tmp;
            std::stringstream ss(line.c_str() + 2);
            const bool wild = (line[2] == '*');
            const bool wild2 = (line[3] == '*');
            bool isQuad = false;

            //Interpret face based on slash
            if (wild) {
                assert(num_slashes == 0);
                const uint32_t v_ix = (uint32_t)verties.size() ;
                
                if (wild2) {
                    a = v_ix - 3; b = v_ix - 2; c = v_ix - 1; d = v_ix - 0;
                   
                    isQuad = true;
                }
                else {
                    a = v_ix - 2; b = v_ix - 1; c = v_ix;
                   
                }
            }
            else if (num_slashes == 0) {
                ss >> a >> b >> c >> d;
                at = a; bt = b; ct = c; dt = d;
                if (!ss.fail()) {
                    isQuad = true;
                }
            }
            else if (num_slashes == 3) {
                ss >> a >> at >> b >> bt >> c >> ct;
            }
            else if (num_slashes == 4) {
                isQuad = true;
                ss >> a >> at >> b >> bt >> c >> ct >> d >> dt;
            }
            else if (num_slashes == 6) {
                if (doubleslash) {
                    ss >> a >> _tmp >> b >> _tmp >> c >> _tmp;
                    at = a; bt = b; ct = c;
                }
                else {
                    ss >> a >> at >> _tmp >> b >> bt >> _tmp >> c >> ct >> _tmp;
                }
            }
            else if (num_slashes == 8) {
                isQuad = true;
                if (doubleslash) {
                    ss >> a >> _tmp >> b >> _tmp >> c >> _tmp >> d >> _tmp;
                    at = a; bt = b; ct = c; dt = d;
                }
                else {
                    ss >> a >> at >> _tmp >> b >> bt >> _tmp >> c >> ct >> _tmp >> d >> dt >> _tmp;
                }
            }
            else {
               // assert(false);
                continue;
            }

            //Add face to list
            triangles.push_back(a - 1); //Verties位置
            triangles.push_back(b - 1);
            triangles.push_back(c - 1);
            uv_triangles.push_back(at - 1); //vt位置
            uv_triangles.push_back(bt - 1);
            uv_triangles.push_back(ct - 1);
            
            if (isQuad) { //正方形的話切兩個三角形
                triangles.push_back(c - 1);
                triangles.push_back(d - 1);
                triangles.push_back(a - 1);
                uv_triangles.push_back(ct - 1);
                uv_triangles.push_back(dt - 1);
                uv_triangles.push_back(at - 1);
            }
        }
    }
    if (normals.size() == 0) calcNormal();
    calcTangent();
   
    //std::cout << uv_triangles.size()<< " " << triangles.size() << std::endl;
    //std::cout <<  uvs.size() << std::endl;

}

Mesh::Mesh() {

}

void Mesh::calcNormal() { //法向量
    std::vector<Vector3> normal;
    for (int i = 0; i < verties.size(); i += 1) {
        normal.push_back(Vector3(0,0,0));
    }

    
    for (int i = 0; i < triangles.size()/3; i += 1) {
        int i0 = triangles[i * 3 + 0];
        int i1 = triangles[i * 3 + 1];
        int i2 = triangles[i * 3 + 2];
        Vector3 N = (verties[i1] - verties[i0]).Cross(verties[i2] -verties[i0]).Normalized();
        
        //std::cout << verties[i0] << "     " << verties[i1] << "     " << verties[i2] << std::endl;
        //std::cout << N << " " << i0 << " " << i1 << " " << i2 << std::endl;


        normal[i0] += N;
        normal[i1] += N;
        normal[i2] += N;
    }
    
    for (int i = 0; i < verties.size(); i += 1) {
        //std::cout << normal[i] << std::endl;
        normals.push_back(normal[i].Normalized());
    }

   
}

void Mesh::calcTangent() {
    std::vector<Vector3> tangent;
    std::vector<Vector3> bitangent;
    for (int i = 0; i < verties.size(); i += 1) {
        tangent.push_back(Vector3(0, 0, 0));
        bitangent.push_back(Vector3(0, 0, 0));
    }


    for (int i = 0; i < triangles.size() / 3; i += 1) {
        int i0 = triangles[i * 3 + 0];
        int i1 = triangles[i * 3 + 1];
        int i2 = triangles[i * 3 + 2];
        Vector3 p0 = verties[i0];
        Vector3 p1 = verties[i1];
        Vector3 p2 = verties[i2];
        Vector3 w0 = uvs[0];
        Vector3 w1 = uvs[1];
        Vector3 w2 = uvs[2];

        Vector3 e1 = p1 - p0;
        Vector3 e2 = p2 - p0;
        float x1 = w1.x - w0.x;
        float x2 = w2.x - w0.x;
        float y1 = w1.y - w0.y;
        float y2 = w2.y - w0.y;
        float r = 1.0 / (x1 * y2 - x2 * y1);
        Vector3 t = ((e1 * y2) - (e2 * y1)) - r;
        Vector3 b = ((e2 * x1) - (e1 * x2)) - r;

        tangent[i0] += t;
        tangent[i1] += t;
        tangent[i2] += t;

        bitangent[i0] += b;
        bitangent[i0] += b;
        bitangent[i0] += b;

    }

    for (int i = 0; i < verties.size(); i += 1) {
        Vector3 t = tangent[i];
        Vector3 b = bitangent[i];
        Vector3 n = normals[i];

        Vector4 tan((t-n*n.Dot(t)).Normalized(), 0);
        tan.w = (n.Cross(t)).Dot(b) < 0.0 ? -1.0 : 1.0;
        
        tangents.push_back(tan);

    }




 
}






