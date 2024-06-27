#include <GL/glut.h>
#include <GL/gl.h>

int window;
void display();
void drawSquare(int x, int y);

void mouse(int bin, int state, int x, int y) {
    if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        drawSquare(x, y);
    }
}
         
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    system("pause");

    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    gluOrtho2D(0, 800, 0, 800);
    window = glutCreateWindow("Your First GLUT  Window!");
    glutSwapBuffers();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);   
    gluOrtho2D(0, 800, 0, 800);
    glutMainLoop();

}
void display() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClear(GL_COLOR_BUFFER_BIT);  
    glFlush();
}

void drawSquare(int x, int y) {
    glPointSize(10);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
   
    glBegin(GL_POINTS);
    glVertex2i(x, 800-y);
    glEnd();   
}







