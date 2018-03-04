/*
   Example of glReadPixels.
 
   Due to OpenGL's behaviour, the image produced will be flipped. This is not covered here, as this only is a raw example.
*/
 

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct
{
    u_char head[12];
    u_short dx /* Width */, dy /* Height */, head2;
    u_char pic[768 * 1024 * 10][3];
} typetga;
typetga tga;

char captureName[256];
u_long captureNo;
u_char tgahead[12] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void capture(GLFWwindow* window)
{
    int screenWidth, screenHeight; /* We need current rendering dimensions, not the already defined constants */
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight); /* Get size, store into specified variables  */
    
    /* Prepare the targa header */
    memcpy(tga.head, tgahead, 12);
    tga.dx = screenWidth;
    tga.dy = screenHeight;
    tga.head2 = 0x2018;
    
    /* Read pixels from [0, 0] to screenWidth, screenHeight, mode (RGB), type (3 bytes RGB), store into tga.pic */
    glReadPixels(0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, tga.pic[0]);
    
    /* Store "Capture_%04lu.tga" + captureNo into captureName, increase frame count */
    sprintf(captureName, "Capture_%04lu.tga" /* 'lu' for unsigned long */, captureNo); captureNo++;
    
    /* Write file */
    FILE* cc = fopen(captureName, "wb");
    fwrite(&tga, 1, (18 + 3 * screenWidth * screenHeight), cc);
    fclose(cc);
}



void controls(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
        if(key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, GL_TRUE);
}

GLFWwindow* initWindow(const int resX, const int resY)
{
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); /* 4x antialiasing */
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    /* Open a window and create a OpenGL context */
    GLFWwindow* window = glfwCreateWindow(resX, resY, "glReadPixels_example", NULL, NULL);
    
    if(window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return NULL;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, controls);
    
    /* Get info of GPU and supported OpenGL version */
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return window;
}

void drawCube()
{
    GLfloat vertices[] =
    {
        -1, -1, -1,   -1, -1,  1,   -1,  1,  1,   -1,  1, -1, /* Cube surface 1 */
         1, -1, -1,    1, -1,  1,    1,  1,  1,    1,  1, -1, /* Cube surface 2 */
        -1, -1, -1,   -1, -1,  1,    1, -1,  1,    1, -1, -1, /* Cube surface 3 */
        -1,  1, -1,   -1,  1,  1,    1,  1,  1,    1,  1, -1, /* Cube surface 4 */
        -1, -1, -1,   -1,  1, -1,    1,  1, -1,    1, -1, -1, /* Cube surface 5 */
        -1, -1,  1,   -1,  1,  1,    1,  1,  1,    1, -1,  1  /* Cube surface 6 */
    }; /* Vertices, points to draw */
    
    GLfloat colors[] =
    {
        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0, /* Cube surface 1 (color) */
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0, /* Cube surface 2 (color) */
        0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, /* Cube surface 3 (color) */
        1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0, /* Cube surface 4 (color) */
        0, 1, 1,   0, 1, 1,   0, 1, 1,   0, 1, 1, /* Cube surface 5 (color) */
        1, 0, 1,   1, 0, 1,   1, 0, 1,   1, 0, 1  /* Cube surface 6 (color) */
    }; /* Colors, apply color to vertices */
    
    static float alpha = 0;
    /* Rotate cube */
    glRotatef(alpha, 1, 1, 0);
    
    /* Color array, vertex array */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    
    /* Send data : 24 vertices */
    glDrawArrays(GL_QUADS, 0, 24);
    
    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    alpha += 1;
}

void display(GLFWwindow* window)
{
    while(!glfwWindowShouldClose(window))
    {
        /* Scale to window size */
        GLint windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        /* Draw */
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective(60, (double)windowWidth / (double)windowHeight, 0.1, 100);
        
        
        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0, 0, -5);
        
        drawCube();
        
        /* Read pixels */
        capture(window);
        
        /* Update screen buffers */
        glfwSwapBuffers(window);
        
        /* Check for events */
        glfwPollEvents();
    }
}

int main(int argc, char* argv[])
{
    GLFWwindow* window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    if(NULL != window)
    {
        display(window);
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
