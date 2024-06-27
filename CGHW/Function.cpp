#include "Function.h"
#include "Vector.h"

#include <vector>
#include <string>
using namespace std;

void drawSquare(int x, int y, Vector3 &color) {
    glPointSize(1);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(color.x,color.y,color.z);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}



void drawLine(int x0, int y0, int x1, int y1, Vector3& color) {
    // To Do : Implement the Line algorithm.
}

void drawCircle(int x0, int y0, int x1, int y1, Vector3& color) {
   // To Do : Implement the Circle algorithm.
}

float dist(int x0, int y0, int x1, int y1) {
    return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));

}


const std::vector<std::string> split(const std::string& str, const std::string& pattern) {
    std::vector<std::string> result;
    std::string::size_type begin, end;

    end = str.find(pattern);
    begin = 0;

    while (end != std::string::npos) {
        if (end - begin != 0) {
            result.push_back(str.substr(begin, end - begin));
        }
        begin = end + pattern.size();
        end = str.find(pattern, begin);
    }

    if (begin != str.length()) {
        result.push_back(str.substr(begin));
    }
    return result;
}





float map(float i, float x1, float y1, float x2, float y2) {
    float r = (i - x1) / (y1 - x1);
    return x2 + (y2 - x2) * r;
}
vector<Vector3> clipping_algorithm(const Vector3* v,const vector<Vector3> polygon_points) {
    vector<Vector3> output_list = polygon_points;
    vector<Vector3> view_point;
    view_point.push_back(Vector3(v[0].x, v[0].y, 0));
    view_point.push_back(Vector3(v[0].x, v[1].y, 0));
    view_point.push_back(Vector3(v[1].x, v[1].y, 0));
    view_point.push_back(Vector3(v[1].x, v[0].y, 0));
    for (int i = 0; i < view_point.size(); i += 1) {
        vector<Vector3> input_list = output_list;
        output_list.clear();

        for (int j = 0; j < input_list.size(); j += 1) {
            Vector3 current_point = input_list[j];
            Vector3 prev_point = input_list[(j - 1 + input_list.size()) % input_list.size()];

            Vector3 intersecting_point = compute_intersection(prev_point, current_point, view_point[(i - 1 + view_point.size()) % view_point.size()], view_point[i]);

            if (inside_line(current_point.x, current_point.y, view_point[(i - 1 + view_point.size()) % view_point.size()], view_point[i])) {
                if (!inside_line(prev_point.x, prev_point.y, view_point[(i - 1 + view_point.size()) % view_point.size()], view_point[i])) {
                    output_list.push_back(intersecting_point);
                }
                output_list.push_back(current_point);
            }
            else if (inside_line(prev_point.x, prev_point.y, view_point[(i - 1 + view_point.size()) % view_point.size()], view_point[i])) {
                output_list.push_back(intersecting_point);
            }


        }



    }

    return output_list;


}

bool inside_line(float x, float y, Vector3 p1, Vector3 p2) {
    // To Do : Find the point(x,y) is indised the line(p1,p2)
}

Vector3 compute_intersection(Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4) {
    // Find the intersection of the Line L1(p1,p2) and L2(p3,p4)
     
}


Vector3 HSVtoRGB(float H, float S, float V) {
    if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
        cout << "The givem HSV values are not in valid range" << endl;
        return Vector3();
    }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    int R = (r + m) * 255;
    int G = (g + m) * 255;
    int B = (b + m) * 255;
    cout << "R : " << R << endl;
    cout << "G : " << G << endl;
    cout << "B : " << B << endl;

    return Vector3((float)R/255.0, (float)G/255.0, (float)B/255.0);
}

bool pnpoly(float x, float y, std::vector<Vector3> b) {
    // To Do : Find the point(x,y) is insided the polygon. Where b is the vertexes of the polygon.
}
