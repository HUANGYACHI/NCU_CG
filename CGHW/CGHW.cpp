#include <GL/glut.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm> //sort
#include "Vector.h"
#include "Mesh.h"
using namespace std;

#define PAUSE printf("Press Enter key to continue..."); system("PAUSE"); 
#define PI 3.1415926

int window;
char mode = 'd';
int tempX = -1;
int tempY = -1;
int polTempX, polTempY;
bool clearVector = 0;
bool polStart = 0;
ifstream ifs;
bool noDraw = 0;
int width = 0;
int height = 0;
Matrix4 transformMatrix = Matrix4::Identity();
float view[4] = { 0.0 };
TrianglesMatrix Triangle;
Matrix4 EM;
Matrix4 PM;
Vector3 eyev;
Vector3 eyePosition;


class NODE {
    public:
        int x;
        int y;
        GLfloat red;
        GLfloat green;
        GLfloat blue;
};

class COLOR {
    public:
        GLfloat red;
        GLfloat green;
        GLfloat blue;
};

class OBJECT {
public:
    int triNum;
    GLfloat Or;
    GLfloat Og;
    GLfloat Ob;
    float Kd;
    float Ks;
    float N;
};

class LIGHT {
public:
    COLOR Ip;
    Vector3 I;
};

NODE temp;
vector<NODE> record;
vector<vector<vector<float>>> objects;
vector<vector<float>> objects2;
vector<float> objects1;
vector<vector<vector<float>>> viewObject;
COLOR ctemp;
vector<COLOR> color;
vector<TrianglesMatrix> Triangles;
vector<TrianglesMatrix> DisplayTriangles;
vector<Vector4> ClipTriangle;
vector<vector<Vector4>> ClipTriangles;
COLOR KI;
COLOR B;
vector<OBJECT> objectsInfo;
vector<LIGHT> lights;

// 即時更新
void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}

void drawPoints(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();   
}

void drawLines(int startX, int startY, int x, int y) { // 左上00
    //cout << "startX" << startX << " startY" << startY << " x" << x << " y" << y << " " << "\n";
    // 右到左畫線變為左到右
    if (startX > x) {
        swap(startX, x);
        swap(startY, y);
    }
    int a = (y - startY);
    int b = (startX - x);
    //cout << "a" << a << " b" << b << "\n";
    glBegin(GL_POINTS);
    glVertex2i(startX, startY);
    if (startY <= y && abs(a) <= abs(b)) { // 左上右下 0<m<1
        int d = a + b / 2;
        while (startX < x) {
            if (d <= 0) {
                startX++;
                d += a;
            }
            else {
                startX++;
                startY++;
                d += (a + b);
            }
            glVertex2i(startX, startY);
        }
    }
    else if (startY > y && abs(a) <= abs(b)) { // 右上左下 -1<m<0
        int d = a + b / 2;
        while (startX < x) {
            if (d <= 0) {
                startX++;
                startY--;
                d += (a - b);
            }
            else {
                startX++;
                d += a;
            }
            glVertex2i(startX, startY);
        }
    }
    else if (startY <= y && abs(a) > abs(b)) { // 左上右下 m>1
        int d = b + a / 2;
        while (startY < y) {
            if (d <= 0) {
                startX++;
                startY++;
                d += (a + b);
            }
            else {
                startY++;
                d += b;
            }
            glVertex2i(startX, startY);
        }
    }
    else if (startY > y && abs(a) > abs(b)) { // 右上左下 m<-1
        int d = b + a / 2;
        while (startY >= y) {
            if (d <= 0) {
                startY--;
                d -= b;
            }
            else {
                startX++;
                startY--;
                d += (a - b);
            }
            glVertex2i(startX, startY);
        }
    }
    glEnd();
}

void printMatrix() {
    cout << transformMatrix << endl;
}

void clip3D(int polyNum, int polySize, int c) {
    vector<Vector4> polyPoints3D;

    //for (int c = 0; c < 6; c++) {
        for (int i = 0; i < polySize; i++) {
            int k = (i + 1) % polySize;
            Vector4 iv = ClipTriangles[polyNum][i];
            Vector4 kv = ClipTriangles[polyNum][k];
            float iPos = 0;
            float kPos = 0;
            switch (c)
            {
                case 1:
                    iPos = iv.w + iv.x;
                    kPos = kv.w + kv.x;
                    break;
                case 2:
                    iPos = iv.w - iv.x;
                    kPos = kv.w - kv.x;
                    break;
                case 3:
                    iPos = iv.w + iv.y;
                    kPos = kv.w + kv.y;
                    break;
                case 4:
                    iPos = iv.w - iv.y;
                    kPos = kv.w - kv.y;
                    break;
                case 5:
                    iPos = iv.z;
                    kPos = kv.z;
                    break;
                case 6:
                    iPos = iv.w - iv.z;
                    kPos = kv.w - kv.z;
                    break;
                default:
                    break;
            }

            // Case 1 : When both points are visible
            if (iPos >= 0 && kPos >= 0) {
                //Only second point is added
                Vector4 polyPoint(kv.x, kv.y, kv.z, kv.w);
                polyPoints3D.push_back(polyPoint);
            }

            // Case 2: When only second point is visible
            else if (iPos < 0 && kPos >= 0) {
                // Point of intersection with edge and the second point is added
                float t = iPos / (iPos - kPos);
                Vector4 polyPoint(iv.x + t * (kv.x - iv.x), iv.y + t * (kv.y - iv.y), iv.z + t * (kv.z - iv.z), iv.w + t * (kv.w - iv.w));
                polyPoints3D.push_back(polyPoint);

                Vector4 polyPoint2(kv.x, kv.y, kv.z, kv.w);
                polyPoints3D.push_back(polyPoint2);
            }

            // Case 3: When only first point is visible
            else if (iPos >= 0 && kPos < 0) {
                //Only point of intersection with edge is added
                float t = iPos / (iPos - kPos);
                Vector4 polyPoint(iv.x + t * (kv.x - iv.x), iv.y + t * (kv.y - iv.y), iv.z + t * (kv.z - iv.z), iv.w + t * (kv.w - iv.w));
                polyPoints3D.push_back(polyPoint);
            }

            // Case 4: When both points are unvisible
            else {
                //No points are added
            }

        }

        if (polyPoints3D.size() == 0) {
            noDraw = 1;
        }
        else {
            vector<vector<Vector4>>::iterator it = ClipTriangles.begin();
            ClipTriangles.erase(it + polyNum);
            ClipTriangles.insert(it + polyNum, polyPoints3D);
        }
    //}
}

bool inTri(float x, float y, int i) {
    for (int j = 0; j < ClipTriangles[i].size(); j++) { //edge
        Vector3 nlast(ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].XYZ());
        Vector3 nfirst(ClipTriangles[i][j].XYZ());
        Vector3 n(nlast - nfirst);
        if (((nfirst.y - y) * n.x) - ((nfirst.x - x) * n.y) > 0) {
            return false;
        }
    }
    return true;
}

void readFile() {
    string buffer;

    vector<string> words;
    string word;
    while (getline(ifs, buffer)) {
        cout << buffer << "\n";
        if (buffer.size() == 0) continue;
        if (buffer[0] == '#') continue;
        while (1) {
            if (buffer.substr(0, buffer.find(" ")) != "") {
                words.push_back(buffer.substr(0, buffer.find(" "))); // 從第一個空白分割出左側子字串放入vector
            }
            buffer = buffer.substr(buffer.find(" ") + 1, buffer.length()); // 從第一個空白分割出右側子字串設為buffer

            // 取得最後一個字。最後一個字找不到空白了
            if (buffer.find(" ") == -1) {
                words.push_back(buffer);
                break;
            }
        }
        if (words[0] == "viewport") { //[1]vxl [2]vxr [3]vyb [4]vyt
            for (int i = 0; i < 4; i++) {
                view[i] = stof(words[i + 1]);
            }
        }
        else if (words[0] == "reset") {
            //clear transformMatrix
            transformMatrix = Matrix4::Identity();
            //printMatrix();
        }
        else if (words[0] == "translate") {
            //移動 +
            Matrix4 tm = Matrix4::Trans(Vector3(stof(words[1]), stof(words[2]), stof(words[3])));
            transformMatrix = tm * transformMatrix;
            printMatrix();
        }
        else if (words[0] == "scale") {
            //縮放 *
            Matrix4 tm = Matrix4::Scale(Vector3(stof(words[1]), stof(words[2]), stof(words[3])));
            transformMatrix = tm * transformMatrix;
            printMatrix();
        }
        else if (words[0] == "rotate") {
            //旋轉
            float rangex = stof(words[1]) * PI / 180;
            float rangey = stof(words[2]) * PI / 180;
            float rangez = stof(words[3]) * PI / 180;
            Matrix4 tmy = Matrix4::RotY(rangey);
            //cout << "range:" << range << " cos:" << cos(range) << " sin:" << sin(range) << endl;
            transformMatrix = tmy * transformMatrix;
            printMatrix();
            Matrix4 tmz = Matrix4::RotZ(rangez);
            transformMatrix = tmz * transformMatrix;
            printMatrix();
            Matrix4 tmx = Matrix4::RotX(rangex);
            transformMatrix = tmx * transformMatrix;
            printMatrix();
        }
        else if (words[0] == "clearData" || words[0] == "cleardata") {
            //清除物件
            objects.clear();
            viewObject.clear();

            DisplayTriangles.clear();
            ClipTriangles.clear();
            Triangles.clear();
            objectsInfo.clear();
        }
        else if (words[0] == "clearScreen") {
            //清除畫面
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            PAUSE;
        }
        else if (words[0] == "object") {
            //讀obj檔
            string objName = "Mesh/" + words[1];
            Mesh obj(objName);
            cout << obj.triangles.size() << endl;
            //triangles(存哪個verites) Verties(存xyz)
            for (int i = 0; i < obj.triangles.size() / 3; i++) { //幾個三角形
                Triangle = TrianglesMatrix::Triangle(obj.verties[obj.triangles[i * 3 + 0]], obj.verties[obj.triangles[i * 3 + 1]], obj.verties[obj.triangles[i * 3 + 2]]);
                //TM * tri
                Triangle = Triangle * transformMatrix;
                Triangles.push_back(Triangle);
            }
            OBJECT object;
            object.triNum = obj.triangles.size();
            object.Or = stof(words[2]);
            object.Og = stof(words[3]);
            object.Ob = stof(words[4]);
            object.Kd = stof(words[5]);
            object.Ks = stof(words[6]);
            object.N = stof(words[7]);
            objectsInfo.push_back(object);
        }
        else if (words[0] == "ambient") {
            KI.red = stof(words[1]);
            KI.green = stof(words[2]);
            KI.blue = stof(words[3]);
        }
        else if (words[0] == "background") {
            B.red = stof(words[1]);
            B.green = stof(words[2]);
            B.blue = stof(words[3]);
        }
        else if (words[0] == "light") {
            //光源
            COLOR lightColor;
            lightColor.red = stof(words[2]);
            lightColor.green = stof(words[3]);
            lightColor.blue = stof(words[4]);
            Vector3 lightPosition(stof(words[5]), stof(words[6]), stof(words[7]));
            if (lights.size() < stoi(words[1])) {
                LIGHT L;
                L.Ip = lightColor;
                L.I = lightPosition;
                lights.push_back(L);
            }
            else {
                lights[stoi(words[1])].Ip = lightColor;
                lights[stoi(words[1])].I = lightPosition;
            }
        }
        else if (words[0] == "observer") { //epx epy epz COIx COIy COIz Tilt Hither Yon Hav
            //MVP Matrix
            //EM (v)
            Vector3 view(stof(words[1]), stof(words[2]), stof(words[3]));
            eyePosition = view;
            Vector3 coi(stof(words[4]), stof(words[5]), stof(words[6]));
            Vector3 forward = (coi - view).Normalized();
            eyev = forward;
            Vector3 temp(0, 1, 0);
            Vector3 right = temp.Cross(forward).Normalized();
            Vector3 up = forward.Cross(right).Normalized();
            Matrix4 Mirrorx = Matrix4::Identity();
            Mirrorx.SetXAxis(Vector3(-1.0, 0.0, 0.0));
            Matrix4 GRM = Matrix4::GRM(right, up, forward);
            Matrix4 transEye = Matrix4::Trans(Vector3(-stof(words[1]), -stof(words[2]), -stof(words[3])));
            float Tilt = stof(words[7]) * PI / 180;
            Matrix4 tilt = Matrix4::RotZ(Tilt);
            EM = tilt * (Mirrorx * (GRM * transEye));
            cout << "V" << endl;
            cout << EM << endl;

            //PM (p)
            float Hither = stof(words[8]);
            float Yon = stof(words[9]);
            float FOV = stof(words[10]) * PI / 180;
            PM = Matrix4::PM(FOV, Hither, Yon);
            cout << "P" << endl;
            cout << PM << endl;
        }
        else if (words[0] == "display") {
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            glColor3f(1, 1, 1);
            //乘矩陣畫圖
            //EM * tri
            //PM * tri
            int objN = 0;
            int triN = objectsInfo[objN].triNum;
            
            for (int i = 0; i < Triangles.size(); i++) { //幾個三角形
                // illuminate
                if (i >= triN) {
                    objN++;
                    triN += objectsInfo[objN].triNum;
                }
                COLOR Diffuse{0, 0, 0}, Specular{0, 0, 0};
                for (int l = 0; l < lights.size(); l++) {
                    float NL = ((Triangles[i].YAxis() - Triangles[i].XAxis()).Cross(Triangles[i].ZAxis() - Triangles[i].YAxis()).Normalized().Dot(lights[l].I - Triangles[i].YAxis()));
                    Vector3 LaddV = (lights[l].I - Triangles[i].YAxis()) + (eyePosition - Triangles[i].YAxis());
                    float RVn = pow((LaddV / LaddV.Mag()).Dot((Triangles[i].YAxis() - Triangles[i].XAxis()).Cross(Triangles[i].ZAxis() - Triangles[i].YAxis()).Normalized()), objectsInfo[objN].N);
                    Diffuse.red += objectsInfo[objN].Kd * lights[l].Ip.red * NL;
                    Diffuse.green += objectsInfo[objN].Kd * lights[l].Ip.green * NL;
                    Diffuse.blue += objectsInfo[objN].Kd * lights[l].Ip.blue * NL;
                    Specular.red += objectsInfo[objN].Ks * lights[l].Ip.red * RVn;
                    Specular.green += objectsInfo[objN].Ks * lights[l].Ip.green * RVn;
                    Specular.blue += objectsInfo[objN].Ks * lights[l].Ip.blue * RVn;
                }
                COLOR I;
                I.red = (KI.red + Diffuse.red) * objectsInfo[objN].Or + Specular.red;
                I.green = (KI.green + Diffuse.green) * objectsInfo[objN].Og + Specular.green;
                I.blue = (KI.blue + Diffuse.blue) * objectsInfo[objN].Ob + Specular.blue;
                color.push_back(I);
                // I = KI*object.O + sum(fatt*object.kd*Ip(N*L))*object.O + sum(fatt*object.Ks*Ip(R*V)^object.N)

                Triangle = Triangles[i] * EM;
                Triangle = Triangle * PM;
                //Triangle = TrianglesMatrix::PD();
                DisplayTriangles.push_back(Triangle);
            }
            //[0]vxl [1]vxr [2]vyb [3]vyt
            int xmin = (view[0] + 1) * (width / 2);
            int xmax = (view[1] + 1) * (width / 2);
            int ymin = (view[2] + 1) * (height / 2);
            int ymax = (view[3] + 1) * (height / 2);
            drawLines(xmin, ymin, xmax, ymin);
            drawLines(xmax, ymin, xmax, ymax);
            drawLines(xmax, ymax, xmin, ymax);
            drawLines(xmin, ymax, xmin, ymin);
            glColor3f(0, 0.9, 1);
            //z-buffer init
            vector<vector<float>> ZBuff(height, vector<float>(width, FLT_MAX));
            vector<vector<COLOR>> CBuff(height, vector<COLOR>(width));
            for (int cy = ymin; cy < ymax; cy++) {
                for (int cx = xmin; cx < xmax; cx++) {
                    CBuff[cy][cx] = B;
                }
            }
            //映射 [1]vxl [2]vxr [3]vyb [4]vyt
            for (int i = 0; i < DisplayTriangles.size(); i++) {
                //glColor3f(color[i].red, color[i].green, color[i].blue);
                Vector4 v1 = DisplayTriangles[i].point1();
                Vector4 v2 = DisplayTriangles[i].point2();
                Vector4 v3 = DisplayTriangles[i].point3();
                ClipTriangle.push_back(v1);
                ClipTriangle.push_back(v2);
                ClipTriangle.push_back(v3);
                ClipTriangles.push_back(ClipTriangle);
                ClipTriangle.clear();
                //clip back
                Vector3 cv1 = (DisplayTriangles[i].XAxis() - DisplayTriangles[i].YAxis()).Normalized();
                Vector3 cv2 = (DisplayTriangles[i].ZAxis() - DisplayTriangles[i].YAxis()).Normalized();
                float angle = (cv1.Cross(cv2)).Angle(eyev) * 180 / PI;
                if (angle > 90 && angle < 270) continue;
                //3D Clipping
                clip3D(i, ClipTriangles[i].size(), 1);
                clip3D(i, ClipTriangles[i].size(), 2);
                clip3D(i, ClipTriangles[i].size(), 3);
                clip3D(i, ClipTriangles[i].size(), 4);
                clip3D(i, ClipTriangles[i].size(), 5);
                clip3D(i, ClipTriangles[i].size(), 6);
                if (noDraw) {
                    noDraw = 0;
                    continue;
                }
                for (int j = 0; j < ClipTriangles[i].size(); j++) {
                    ClipTriangles[i][j] = ClipTriangles[i][j] / ClipTriangles[i][j].w; //PD
                    ClipTriangles[i][j] = ClipTriangles[i][j].Map(-1, 1, -1, 1, xmin, xmax, ymin, ymax); //view
                }
                //z-buffer
                //zbuffer(i, ClipTriangles[i].size());
                int yMin = height;
                int yMax = 0;
                int xMin = width;
                int xMax = 0;
                vector<Vector3> nodes;
                for (int j = 0; j < ClipTriangles[i].size(); j++) {
                    if (ClipTriangles[i][j].y > yMax) {
                        yMax = ClipTriangles[i][j].y;
                    }
                    if (ClipTriangles[i][j].y < yMin) {
                        yMin = ClipTriangles[i][j].y;
                    }
                    if (ClipTriangles[i][j].x > xMax) {
                        xMax = ClipTriangles[i][j].x;
                    }
                    if (ClipTriangles[i][j].x < xMin) {
                        xMin = ClipTriangles[i][j].x;
                    }
                }
                Vector3 p1 = ClipTriangles[i][0].XYZ();
                Vector3 p2 = ClipTriangles[i][1].XYZ();
                Vector3 p3 = ClipTriangles[i][2].XYZ();
                Vector3 polyN(((p2 - p1).Cross(p3 - p2)).Normalized());
                for (int bx = xMin; bx < xMax + 1; bx++) {
                    for (int by = yMin; by < yMax + 1; by++) {
                        if (inTri(bx, by, i)) {
                            float z = (polyN.x * (bx - p1.x) + polyN.y * (by - p1.y)) / (-1 * polyN.z) + p1.z;
                            if (z < ZBuff[by][bx]) {
                                ZBuff[by][bx] = z;
                                CBuff[by][bx] = color[i];
                            }
                        }
                    }
                }
                //nodes記與y軸交點
                //for (int y = yMin; y < yMax; y++) { //scanline y
                //    //save nodes
                //    for (int j = 0; j < ClipTriangles[i].size(); j++) { //edge
                //        if (ClipTriangles[i][j].y < y && ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].y >= y
                //            || ClipTriangles[i][j].y >= y && ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].y < y) {
                //            int x = (i - ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].y) * (ClipTriangles[i][j].x - ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].x) / (ClipTriangles[i][j].y - ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].y) + ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].x;
                //            int z = (i - ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].y) * (ClipTriangles[i][j].z - ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].z) / (ClipTriangles[i][j].y - ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].y) + ClipTriangles[i][(j + 1) % ClipTriangles[i].size()].z;
                //            Vector3 node(x, y, z);
                //            nodes.push_back(node);
                //        }
                //    }
                //    //sort nodes
                //    sort(nodes.begin(), nodes.end(), [](Vector3 v1, Vector3 v2) {
                //        return v1.x < v2.x;
                //        });
                //    //rewrite zbuff
                //    for (int k = 0; k < nodes.size(); k += 2) {
                //        for (int xi = nodes[k].x; xi < nodes[k + 1].x + 1; xi++) {
                //            int changeZ = ((xi - nodes[k].x) * (nodes[k + 1].z - nodes[k].z) / (nodes[k + 1].x - nodes[k].x)) + nodes[k].z;
                //            if (changeZ < ZBuff[y][xi]) {
                //                ZBuff[y][xi] = changeZ;
                //                CBuff[y][xi] = color[i];
                //            }
                //        }
                //        //CBuff[nodes[k].y][nodes[k].x] = polycolor;
                //    }
                //    nodes.clear();
                //}
                //for each pixel (x, y) inside the poly
                //z = depth of pixel (x, y)
                /*if (z < ZBuff[y][x]) {
                    ZBuff[y][x] = z;
                    CBuff[y][x] = polycolor;
                }*/
                /*for (int k = 0; k < ClipTriangles[i].size(); k++) {
                    int l = (k + 1) % ClipTriangles[i].size();
                    drawLines(round(ClipTriangles[i][k].x), round(ClipTriangles[i][k].y), round(ClipTriangles[i][l].x), round(ClipTriangles[i][l].y));
                }*/
                for (int ey = 0; ey < ZBuff.size(); ey++) {
                    for (int ex = 0; ex < ZBuff[ey].size(); ex++) {
                        glColor3f(CBuff[ey][ex].red, CBuff[ey][ex].green, CBuff[ey][ex].blue);
                        drawPoints(ex + xmin, ey + ymin);
                    }
                }
            }
            DisplayTriangles.clear();
            ClipTriangles.clear();
            glFlush();
            PAUSE;
        }
        else if (words[0] == "end") {
            exit(0);
        }
        words.clear();
    }
    ifs.close();
}

void display() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPointSize(1);
    glFlush();
    readFile();
}
         
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    if (argv[1] != NULL) {
        ifs.open(argv[1]);
        if (!ifs.is_open()) {
            cout << "Failed to open file.\n";
        }
        string wh;
        getline(ifs, wh);
        //cout << wh << endl;
        width = stoi(wh.substr(0, wh.find(" ")));
        height = stoi(wh.substr(wh.find(" ") + 1, wh.length() + 1));
        cout << "w:" << width << " h:" << height << endl;
    }
    PAUSE;
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    window = glutCreateWindow("Your First GLUT  Window!");
    //glutSwapBuffers();
    glutDisplayFunc(display);
    // 更新
    glutTimerFunc(0, timer, 0);
    gluOrtho2D(0, width, 0, height);
    glutMainLoop();

    return 0;
}
