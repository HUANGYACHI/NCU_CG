#include <GL/glut.h>
#include <GL/gl.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

#define PAUSE printf("Press Enter key to continue..."); fgetc(stdin); 
#define PI 3.1415926

int window;
char mode = 'd';
int tempX = -1;
int tempY = -1;
int polTempX, polTempY;
bool clearVector = 0;
bool polStart = 0;
string fileName = "";
bool noDraw = 0;
double transformMatrix[3][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0} };
double square[3][4] = { {-1.0, 1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0} };
double triangle[3][3] = { {0.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, 1.0} };

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

NODE temp;
vector<NODE> record;
vector<vector<vector<double>>> objects;
vector<vector<double>> objects2;
vector<double> objects1;
vector<vector<vector<double>>> viewObject;
COLOR ctemp;
vector<COLOR> color;

// 即時更新
void timer(int val) {
    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
}

void drawPoints(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, 800-y);
    glEnd();   
}

void drawLines(int startX, int startY, int x, int y, GLfloat red, GLfloat green, GLfloat blue) { // 左上00
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
    glVertex2i(startX, 800 - startY);
    temp.x = startX;
    temp.y = startY;
    if (mode != 'p' && polStart) {
        temp.red = red;
        temp.green = green;
        temp.blue = blue;
        polStart = 0;
    }
    record.push_back(temp);

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
            glVertex2i(startX, 800 - startY);
            temp.x = startX;
            temp.y = startY;
            if (mode != 'p') {
                temp.red = red;
                temp.green = green;
                temp.blue = blue;
            }
            record.push_back(temp);
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
            glVertex2i(startX, 800 - startY);
            temp.x = startX;
            temp.y = startY;
            if (mode != 'p') {
                temp.red = red;
                temp.green = green;
                temp.blue = blue;
            }
            record.push_back(temp);
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
            glVertex2i(startX, 800 - startY);
            temp.x = startX;
            temp.y = startY;
            if (mode != 'p') {
                temp.red = red;
                temp.green = green;
                temp.blue = blue;
            }
            record.push_back(temp);
        }
    }
    else if (startY > y && abs(a) > abs(b)) { // 右上左下 m<-1
        int d =  b + a / 2;
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
            glVertex2i(startX, 800 - startY);
            temp.x = startX;
            temp.y = startY;
            if (mode != 'p') {
                temp.red = red;
                temp.green = green;
                temp.blue = blue;
            }
            record.push_back(temp);
        }
    }
    
    glEnd();
}

void drawCircles(int centerX, int centerY, int radiusX, int radiusY, GLfloat red, GLfloat green, GLfloat blue) {
    int R = sqrt(pow(radiusX - centerX, 2) + pow(radiusY - centerY, 2));
    int x = 0, y = R;
    int d = 1 - R;
    int incE = 3;
    int incSE = -2 * R + 5;

    glBegin(GL_POINTS);
    glVertex2i(centerX, 800 - (centerY + R));
    temp.x = centerX;
    temp.y = centerY + R;
    temp.red = red;
    temp.green = green;
    temp.blue = blue;
    record.push_back(temp);
    while (x < y) {
        if (d < 0) {
            d += incE;
            incE += 2;
            incSE += 2;
            x++;
        }
        else {
            d += incSE;
            incE += 2;
            incSE += 4;
            x++;
            y--;
        }
        glVertex2i(x + centerX, 800 - (y + centerY));
        temp.x = x + centerX;
        temp.y = y + centerY;
        record.push_back(temp);
        // 畫出另外七區
        glVertex2i(x + centerX, 800 - (-y + centerY));
        temp.x = x + centerX;
        temp.y = -y + centerY;
        record.push_back(temp);
        glVertex2i(y + centerX, 800 - (x + centerY));
        temp.x = y + centerX;
        temp.y = x + centerY;
        record.push_back(temp);
        glVertex2i(y + centerX, 800 - (-x + centerY));
        temp.x = y + centerX;
        temp.y = -x + centerY;
        record.push_back(temp);
        glVertex2i(-x + centerX, 800 - (y + centerY));
        temp.x = -x + centerX;
        temp.y = y + centerY;
        record.push_back(temp);
        glVertex2i(-x + centerX, 800 - (-y + centerY));
        temp.x = -x + centerX;
        temp.y = -y + centerY;
        record.push_back(temp);
        glVertex2i(-y + centerX, 800 - (x + centerY));
        temp.x = -y + centerX;
        temp.y = x + centerY;
        record.push_back(temp);
        glVertex2i(-y + centerX, 800 - (-x + centerY));
        temp.x = -y + centerX;
        temp.y = -x + centerY;
        record.push_back(temp);
    }
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

void mouse(int button, int state, int x, int y) {
    GLfloat red = (rand() % 10) / static_cast<GLfloat>(10);
    GLfloat green = (rand() % 10) / static_cast<GLfloat>(10);
    GLfloat blue = (rand() % 10) / static_cast<GLfloat>(10);
    if (mode != 'p' || polStart) {
        glColor3f(red, green, blue);
        temp.red = red;
        temp.green = green;
        temp.blue = blue;
        polStart = 0;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (clearVector) {
            record.clear();
            clearVector = 0;
        }
        switch (mode)
        {
            case 'd':   // point
                drawPoints(x, y);
                temp.x = x;
                temp.y = y;
                temp.red = red;
                temp.green = green;
                temp.blue = blue;
                record.push_back(temp);
                break;
            case 'l':   // line
                if (tempX > 0 && tempY > 0) {
                    drawLines(tempX, tempY, x, y, red, green, blue);
                    tempX = -1;
                    tempY = -1;
                }
                else {
                    tempX = x;
                    tempY = y;
                }
                break;
            case 'p':   // 多邊
                if (tempX > 0 && tempY > 0) {
                    drawLines(tempX, tempY, x, y, red, green, blue);
                    tempX = x;
                    tempY = y;
                }
                else {
                    tempX = x;
                    tempY = y;
                    polTempX = x;
                    polTempY = y;
                }
                break;
            case 'o':   // circle
                if (tempX > 0 && tempY > 0) {
                    drawCircles(tempX, tempY, x, y, red, green, blue);
                    tempX = -1;
                    tempY = -1;
                }
                else {
                    tempX = x;
                    tempY = y;
                }
                break;
            default:
                break;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (mode == 'p') {
            drawLines(tempX, tempY, polTempX, polTempY, red, green, blue);
            tempX = -1;
            tempY = -1;
            red = (rand() % 10) / static_cast<GLfloat>(10);
            green = (rand() % 10) / static_cast<GLfloat>(10);
            blue = (rand() % 10) / static_cast<GLfloat>(10);
            glColor3f(red, green, blue);
            temp.red = red;
            temp.green = green;
            temp.blue = blue;
        }
    }
}

void mouseMove(int x, int y) {
    if (mode == 'd') {
        drawPoints(x, y);
        temp.x = x;
        temp.y = y;
        record.push_back(temp);
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
        case 'd':   // point
            mode = 'd';
            break;
        case 'l':   // line
            mode = 'l';
            break;
        case 'p':   // 多邊
            mode = 'p';
            polStart = 1;
            break;
        case 'o':   // circle
            mode = 'o';
            break;
        case 'c':
            glClear(GL_COLOR_BUFFER_BIT);
            clearVector = 1;
            break;
        case 'r':   // undo
            for (int i = 0; i < record.size(); i++) {
                //cout << record[i].x;
                glColor3f(record[i].red, record[i].green, record[i].blue);
                glBegin(GL_POINTS);
                glVertex2i(record[i].x, 800 - record[i].y);
                glEnd();
            }
            clearVector = 0;
            break;
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

void printMatrix() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << transformMatrix[i][j] << " ";
        }
        cout << endl;
    }
}

void mulMatrix(double tm[3][3], double transMatrix[3][3]) { //新的TM = 新的M * 舊的TM
    //傳入了還會更新拉
    double matrixTemp[3][3] = { 0 };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrixTemp[i][j] = transMatrix[i][j];
        }
    }
    transformMatrix[0][0] = tm[0][0] * matrixTemp[0][0] + tm[0][1] * matrixTemp[1][0] + tm[0][2] * matrixTemp[2][0];
    transformMatrix[0][1] = tm[0][0] * matrixTemp[0][1] + tm[0][1] * matrixTemp[1][1] + tm[0][2] * matrixTemp[2][1];
    transformMatrix[0][2] = tm[0][0] * matrixTemp[0][2] + tm[0][1] * matrixTemp[1][2] + tm[0][2] * matrixTemp[2][2];
    transformMatrix[1][0] = tm[1][0] * matrixTemp[0][0] + tm[1][1] * matrixTemp[1][0] + tm[1][2] * matrixTemp[2][0];
    transformMatrix[1][1] = tm[1][0] * matrixTemp[0][1] + tm[1][1] * matrixTemp[1][1] + tm[1][2] * matrixTemp[2][1];
    transformMatrix[1][2] = tm[1][0] * matrixTemp[0][2] + tm[1][1] * matrixTemp[1][2] + tm[1][2] * matrixTemp[2][2];
    transformMatrix[2][0] = tm[2][0] * matrixTemp[0][0] + tm[2][1] * matrixTemp[1][0] + tm[2][2] * matrixTemp[2][0];
    transformMatrix[2][1] = tm[2][0] * matrixTemp[0][1] + tm[2][1] * matrixTemp[1][1] + tm[2][2] * matrixTemp[2][1];
    transformMatrix[2][2] = tm[2][0] * matrixTemp[0][2] + tm[2][1] * matrixTemp[1][2] + tm[2][2] * matrixTemp[2][2];
}

// Returns x-value of point of intersection of two lines
int x_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    int num = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
    int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return num / den;
}

// Returns y-value of point of intersection of two lines
int y_intersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    int num = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
    int den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return num / den;
}

void clip(int polyNum, int polySize, int x1, int y1, int x2, int y2) {
    vector<double> polyPointsX;
    vector<double> polyPointsY;

    for (int i = 0; i < polySize; i++) {
        int k = (i + 1) % polySize;
        int ix = round(viewObject[polyNum][0][i]);
        int iy = round(viewObject[polyNum][1][i]);
        int kx = round(viewObject[polyNum][0][k]);
        int ky = round(viewObject[polyNum][1][k]);
        int iPos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);
        int kPos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

        // Case 1 : When both points are inside
        if (iPos < 0 && kPos < 0) {
            //Only second point is added
            polyPointsX.push_back(kx);
            polyPointsY.push_back(ky);
        }

        // Case 2: When only first point is outside
        else if (iPos >= 0 && kPos < 0) {
            // Point of intersection with edge and the second point is added
            polyPointsX.push_back(x_intersect(x1, y1, x2, y2, ix, iy, kx, ky));
            polyPointsY.push_back(y_intersect(x1, y1, x2, y2, ix, iy, kx, ky));

            polyPointsX.push_back(kx);
            polyPointsY.push_back(ky);
        }

        // Case 3: When only second point is outside
        else if (iPos < 0 && kPos >= 0) {
            //Only point of intersection with edge is added
            polyPointsX.push_back(x_intersect(x1, y1, x2, y2, ix, iy, kx, ky));
            polyPointsY.push_back(y_intersect(x1, y1, x2, y2, ix, iy, kx, ky));
        }

        // Case 4: When both points are outside
        else {
            //No points are added
        }
    }

    if (polyPointsX.size() == 0) {
        noDraw = 1;
    } else {
        vector<double> polyPointsZ(polyPointsX.size(), 1);
        viewObject[polyNum].clear();
        viewObject[polyNum].push_back(polyPointsX);
        viewObject[polyNum].push_back(polyPointsY);
        viewObject[polyNum].push_back(polyPointsZ);
    }
    
}


void readFile() {
    ifstream ifs;
    string buffer;

    ifs.open(fileName);
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    }
    else {
        vector<string> words;
        string word;
        while (!ifs.eof()) {
            getline(ifs, buffer);
            cout << buffer << "\n";
            if (buffer[0] == '#') {
                // cout << "###" << "\n";
            }
            else {
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
                //cout << words[0] << endl;
                if (words[0] == "view") { //[1]wxl [2]wxr [3]wyb [4]wyt [5]vxl [6]vxr [7]vyb [8]vyt
                    // PAUSE;
                    //映射
                    //映射後x = ((映射前x - wxl) * (vxr - vxl) / (wxr - wxl)) + vxl
                    //映射後y = ((映射前y - wyb) * (vyt - vyb) / (wyt - wyb)) + vyb
                    glColor3f(1, 1, 1);
                    drawLines(stoi(words[5]), stoi(words[7]), stoi(words[6]), stoi(words[7]));
                    drawLines(stoi(words[6]), stoi(words[7]), stoi(words[6]), stoi(words[8]));
                    drawLines(stoi(words[6]), stoi(words[8]), stoi(words[5]), stoi(words[8]));
                    drawLines(stoi(words[5]), stoi(words[8]), stoi(words[5]), stoi(words[7]));
                    glFlush();
                    for (int i = 0; i < objects.size(); i++) {
                        glColor3f(color[i].red, color[i].green, color[i].blue);
                        for (int j = 0; j < objects[i].size(); j++) {
                            viewObject[i][j].clear();
                            for (int k = 0; k < objects[i][j].size(); k++) {
                                //cout << objects[i][j][k] << " ";
                                if (j == 0) { //x
                                    // viewObject[i][j][k] = (objects[i][j][k] - stod(words[1])) * (stod(words[6]) - stod(words[5])) / (stod(words[2]) - stod(words[1])) + stod(words[5]);
                                    viewObject[i][j].push_back((objects[i][j][k] - stod(words[1])) * (stod(words[6]) - stod(words[5])) / (stod(words[2]) - stod(words[1])) + stod(words[5]));
                                }
                                else if (j == 1) { //y
                                    // viewObject[i][j][k] = (objects[i][j][k] - stod(words[3])) * (stod(words[8]) - stod(words[7])) / (stod(words[4]) - stod(words[3])) + stod(words[7]);
                                    viewObject[i][j].push_back((objects[i][j][k] - stod(words[3])) * (stod(words[8]) - stod(words[7])) / (stod(words[4]) - stod(words[3])) + stod(words[7]));
                                }
                            }
                            //cout << endl;
                        }
                        //view 邊框逆時針輸入
                        clip(i, viewObject[i][0].size(), stoi(words[5]), stoi(words[7]), stoi(words[5]), stoi(words[8]));
                        clip(i, viewObject[i][0].size(), stoi(words[5]), stoi(words[8]), stoi(words[6]), stoi(words[8]));
                        clip(i, viewObject[i][0].size(), stoi(words[6]), stoi(words[8]), stoi(words[6]), stoi(words[7]));
                        clip(i, viewObject[i][0].size(), stoi(words[6]), stoi(words[7]), stoi(words[5]), stoi(words[7]));
                        //cout << endl;
                        int l = viewObject[i][0].size() - 1;
                        if (noDraw) {
                            noDraw = 0;
                        } else {
                            for (int k = 0; k < l; k++) {
                                drawLines(round(viewObject[i][0][k]), round(viewObject[i][1][k]), round(viewObject[i][0][k + 1]), round(viewObject[i][1][k + 1]));
                            }
                            drawLines(round(viewObject[i][0][l]), round(viewObject[i][1][l]), round(viewObject[i][0][0]), round(viewObject[i][1][0]));
                        }
                        glFlush();
                    }
                    PAUSE;
                }
                else if (words[0] == "reset") {
                    //clear transformMatrix
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            if (i == j)
                                transformMatrix[i][j] = 1;
                            else
                                transformMatrix[i][j] = 0;
                        }
                    }
                    //printMatrix();
                }
                else if (words[0] == "translate") {
                    //移動 +
                    double tm[3][3] = { {1.0, 0.0, stod(words[1])}, {0.0, 1.0, stod(words[2])}, {0.0, 0.0, 1.0} };
                    mulMatrix(tm, transformMatrix);
                    printMatrix();
                }
                else if (words[0] == "scale") {
                    //縮放 *
                    double tm[3][3] = { {stod(words[1]), 0.0, 0.0}, {0.0, stod(words[2]), 0.0}, {0.0, 0.0, 1.0} };
                    mulMatrix(tm, transformMatrix);
                    printMatrix();
                }
                else if (words[0] == "rotate") {
                    //旋轉
                    double range = stod(words[1]) * PI / 180;
                    double tm[3][3] = { {cos(range), -sin(range), 0.0}, {sin(range), cos(range), 0.0}, {0.0, 0.0, 1.0} };
                    //cout << "range:" << range << " cos:" << cos(range) << " sin:" << sin(range) << endl;
                    mulMatrix(tm, transformMatrix);
                    printMatrix();
                }
                else if (words[0] == "clearData") {
                    //清除物件
                    objects.clear();
                    viewObject.clear();
                }
                else if (words[0] == "clearScreen") {
                    //清除畫面
                    glClear(GL_COLOR_BUFFER_BIT);
                    glFlush();
                }
                else if (words[0] == "square") {
                    ctemp.red = (rand() % 10) / static_cast<GLfloat>(10);
                    ctemp.green = (rand() % 10) / static_cast<GLfloat>(10);
                    ctemp.blue = (rand() % 10) / static_cast<GLfloat>(10);
                    color.push_back(ctemp);
                    double transSquare[3][4] = { 0.0 };
                    transSquare[0][0] = transformMatrix[0][0] * square[0][0] + transformMatrix[0][1] * square[1][0] + transformMatrix[0][2] * square[2][0];
                    transSquare[0][1] = transformMatrix[0][0] * square[0][1] + transformMatrix[0][1] * square[1][1] + transformMatrix[0][2] * square[2][1];
                    transSquare[0][2] = transformMatrix[0][0] * square[0][2] + transformMatrix[0][1] * square[1][2] + transformMatrix[0][2] * square[2][2];
                    transSquare[0][3] = transformMatrix[0][0] * square[0][3] + transformMatrix[0][1] * square[1][3] + transformMatrix[0][2] * square[2][3];
                    transSquare[1][0] = transformMatrix[1][0] * square[0][0] + transformMatrix[1][1] * square[1][0] + transformMatrix[1][2] * square[2][0];
                    transSquare[1][1] = transformMatrix[1][0] * square[0][1] + transformMatrix[1][1] * square[1][1] + transformMatrix[1][2] * square[2][1];
                    transSquare[1][2] = transformMatrix[1][0] * square[0][2] + transformMatrix[1][1] * square[1][2] + transformMatrix[1][2] * square[2][2];
                    transSquare[1][3] = transformMatrix[1][0] * square[0][3] + transformMatrix[1][1] * square[1][3] + transformMatrix[1][2] * square[2][3];
                    transSquare[2][0] = transformMatrix[2][0] * square[0][0] + transformMatrix[2][1] * square[1][0] + transformMatrix[2][2] * square[2][0];
                    transSquare[2][1] = transformMatrix[2][0] * square[0][1] + transformMatrix[2][1] * square[1][1] + transformMatrix[2][2] * square[2][1];
                    transSquare[2][2] = transformMatrix[2][0] * square[0][2] + transformMatrix[2][1] * square[1][2] + transformMatrix[2][2] * square[2][2];
                    transSquare[2][3] = transformMatrix[2][0] * square[0][3] + transformMatrix[2][1] * square[1][3] + transformMatrix[2][2] * square[2][3];
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 4; j++) {
                            cout << transSquare[i][j] << " ";
                        }
                        cout << endl;
                    }
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 4; j++) {
                            objects1.push_back(transSquare[i][j]);
                        }
                        objects2.push_back(objects1);
                        objects1.clear();
                    }
                    objects.push_back(objects2);
                    viewObject.push_back(objects2);
                    objects2.clear();
                }
                else if (words[0] == "triangle") { //創物件 乘tranformMatrix 存起來
                    ctemp.red = (rand() % 10) / static_cast<GLfloat>(10);
                    ctemp.green = (rand() % 10) / static_cast<GLfloat>(10);
                    ctemp.blue = (rand() % 10) / static_cast<GLfloat>(10);
                    color.push_back(ctemp);
                    double transTriangle[3][3] = { 0.0 };
                    transTriangle[0][0] = transformMatrix[0][0] * triangle[0][0] + transformMatrix[0][1] * triangle[1][0] + transformMatrix[0][2] * triangle[2][0];
                    transTriangle[0][1] = transformMatrix[0][0] * triangle[0][1] + transformMatrix[0][1] * triangle[1][1] + transformMatrix[0][2] * triangle[2][1];
                    transTriangle[0][2] = transformMatrix[0][0] * triangle[0][2] + transformMatrix[0][1] * triangle[1][2] + transformMatrix[0][2] * triangle[2][2];
                    transTriangle[1][0] = transformMatrix[1][0] * triangle[0][0] + transformMatrix[1][1] * triangle[1][0] + transformMatrix[1][2] * triangle[2][0];
                    transTriangle[1][1] = transformMatrix[1][0] * triangle[0][1] + transformMatrix[1][1] * triangle[1][1] + transformMatrix[1][2] * triangle[2][1];
                    transTriangle[1][2] = transformMatrix[1][0] * triangle[0][2] + transformMatrix[1][1] * triangle[1][2] + transformMatrix[1][2] * triangle[2][2];
                    transTriangle[2][0] = transformMatrix[2][0] * triangle[0][0] + transformMatrix[2][1] * triangle[1][0] + transformMatrix[2][2] * triangle[2][0];
                    transTriangle[2][1] = transformMatrix[2][0] * triangle[0][1] + transformMatrix[2][1] * triangle[1][1] + transformMatrix[2][2] * triangle[2][1];
                    transTriangle[2][2] = transformMatrix[2][0] * triangle[0][2] + transformMatrix[2][1] * triangle[1][2] + transformMatrix[2][2] * triangle[2][2];
                    /*for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            cout << transTriangle[i][j] << " ";
                        }
                        cout << endl;
                    }*/
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            objects1.push_back(transTriangle[i][j]);
                        }
                        objects2.push_back(objects1);
                        objects1.clear();
                    }
                    objects.push_back(objects2);
                    viewObject.push_back(objects2);
                    objects2.clear();
                }
                else if (words[0] == "end") {
                    exit(0);
                }
                words.clear();
            }
        }
        ifs.close();
    }
}

void display() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPointSize(1);
    glFlush();
    readFile();
}
         
int main(int argc, char* argv[]) {
    fileName = argv[1];
    glutInit(&argc, argv);
    PAUSE
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    window = glutCreateWindow("Your First GLUT  Window!");
    //glutSwapBuffers();
    glutDisplayFunc(display);
    // 更新
    glutTimerFunc(0, timer, 0);
    // glutMouseFunc(mouse);
    // 滑鼠移動事件
    // glutMotionFunc(mouseMove);
    // 鍵盤事件
    // glutKeyboardFunc(keyboard);
    gluOrtho2D(0, 800, 0, 800);
    glutMainLoop();

    return 0;
}
