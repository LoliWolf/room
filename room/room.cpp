#define _CRT_SECURE_NO_WARNINGS
/*

#### The keys used are -

To move Eye point:
1. w: up
2. s: down
3. a: left
4. d: right
5. i: zoom in
6. o: zoom out

To move Camera point:
1. l: move nearer
2. k: move far

1. q to move to default position (both eyepoint & lookat point)

To control lights:
1. 1: to turn on/off light one
2. 2: to turn on/off lamp


*/

#include <GL/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

GLboolean redFlag = true, switchOne = false, switchTwo = false, switchLamp = false, amb1 = true, diff1 = true, spec1 = true, amb2 = true, diff2 = true, spec2 = true;
double windowHeight = 800, windowWidth = 600;
double eyeX = 7.0, eyeY = 2.0, eyeZ = 15.0, refX = 0, refY = 0, refZ = 0;
double theta = 180.0, y = 1.36, z = 7.97888;

unsigned int textureVec[5];
unsigned int loadGLTexture(const char* filename)
{
    unsigned int texture;

    static GLint   imagewidth;
    static GLint   imageheight;
    static GLint   pixellength;
    static GLubyte* pixeldata;
    FILE* pfile = fopen(filename, "rb");
    if (pfile == 0) exit(0);
    fseek(pfile, 0x0012, SEEK_SET); // skip 16 bits from beginning for bmp files
    // get the width of image
    fread(&imagewidth, sizeof(imagewidth), 1, pfile);
    // get the height of image
    fread(&imageheight, sizeof(imageheight), 1, pfile);
    // count the length of in the image by pixel
    // pixel data consists of three colors red, green and blue (Windows implement BGR)
    pixellength = imagewidth * 3;
    // pixel data width should be an integral multiple of 4, which is required by the .bmp file
    while (pixellength % 4 != 0)
        pixellength++;
    // pixel data length = width * height
    pixellength *= imageheight;
    // malloc for the image by pixel
    pixeldata = (GLubyte*)malloc(pixellength); // memory allocation
    if (pixeldata == 0)
        exit(0);
    // read the data of image as pixel
    fseek(pfile, 54, SEEK_SET);
    fread(pixeldata, pixellength, 1, pfile);
    // close file
    fclose(pfile);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imagewidth, imageheight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixeldata);

    return texture;
}

// 将立方体的八个顶点保存到一个数组里面
static GLfloat v_cube[8][3] =
{
    {0.0, 0.0, 0.0}, //0
    {0.0, 0.0, 3.0}, //1
    {3.0, 0.0, 3.0}, //2
    {3.0, 0.0, 0.0}, //3
    {0.0, 3.0, 0.0}, //4
    {0.0, 3.0, 3.0}, //5
    {3.0, 3.0, 3.0}, //6
    {3.0, 3.0, 0.0}  //7
};

//定义立方体使用的各个顶点数组的序号数组
//将要使用的顶点的序号保存到一个数组里面
static GLubyte quadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};


// 计算法线向量
static void getNormal3p
(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);
}

//*********************画立方体*********************************************
void drawCube1(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX = 0, GLfloat ambY = 0, GLfloat ambZ = 0, GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };  //（不发光）
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { shine };

    //设置材料属性
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);    // 材质的环境颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);    // 材质的散射颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);      // 材质的镜面反射颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);    // 镜面反射指数
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);    // 材质的发射光颜色（不发光）

     // 绘制六面体
    glBegin(GL_QUADS);  // 绘制由四个顶点组成的一组单独的四边形

    for (GLint i = 0; i < 6; i++) //  画六个面
    {
        getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
            v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
            v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);

        // 画四个点
        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
    }
    glEnd();
}


//**********************画四棱台***********************************
static GLfloat v_trapezoid[8][3] =
{
    {0.0, 0.0, 0.0}, //0
    {0.0, 0.0, 3.0}, //1
    {3.0, 0.0, 3.0}, //2
    {3.0, 0.0, 0.0}, //3
    {0.5, 3.0, 0.5}, //4
    {0.5, 3.0, 2.5}, //5
    {2.5, 3.0, 2.5}, //6
    {2.5, 3.0, 0.5}  //7
};

static GLubyte TquadIndices[6][4] =
{
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};

void drawTrapezoid(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 50)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };     //（不发光）
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_emission[] = { difX, difY, difZ, 0.0 };
    GLfloat mat_shininess[] = { shine };

    //设置材料属性
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);    // 材质的环境颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);    // 材质的散射颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);    // 材质的镜面反射颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);    // 镜面反射指数

    // 判断是否亮灯
    if (switchLamp == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);  // 材质的发射光颜色
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);    // 材质的发射光颜色（不发光）
    }


    // 绘制六面体
    glBegin(GL_QUADS);// 绘制由四个顶点组成的一组单独的四边形

    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_trapezoid[TquadIndices[i][0]][0], v_trapezoid[TquadIndices[i][0]][1], v_trapezoid[TquadIndices[i][0]][2],
            v_trapezoid[TquadIndices[i][1]][0], v_trapezoid[TquadIndices[i][1]][1], v_trapezoid[TquadIndices[i][1]][2],
            v_trapezoid[TquadIndices[i][2]][0], v_trapezoid[TquadIndices[i][2]][1], v_trapezoid[TquadIndices[i][2]][2]);
        // 画四个点
        glVertex3fv(&v_trapezoid[TquadIndices[i][0]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][1]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][2]][0]);
        glVertex3fv(&v_trapezoid[TquadIndices[i][3]][0]);
    }
    glEnd();
}




//***********************画球体**********************************
void drawSphere(GLfloat difX, GLfloat difY, GLfloat difZ, GLfloat ambX, GLfloat ambY, GLfloat ambZ, GLfloat shine = 90)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { ambX, ambY, ambZ, 1.0 };
    GLfloat mat_diffuse[] = { difX, difY, difZ, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { shine };

    //设置材料属性
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);    // 材质的环境颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);    // 材质的散射颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);    // 材质的镜面反射颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);    // 镜面反射指数

    glutSolidSphere(3.0, 20, 16);    //半径为3.0, 20条纬线, 16条经线
}



void room()
{


     glPushMatrix();
     glTranslated(-2, -0.5, 0);
     glScalef(5, 1, 5);
     glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, textureVec[0]);
     glBegin(GL_QUADS);
     glNormal3f(0, 1, 0);
     glTexCoord2f(0, 0); glVertex3f(0, 0, 3);
     glTexCoord2f(8, 0); glVertex3f(3, 0, 3);
     glTexCoord2f(8, 8); glVertex3f(3, 0, 0);
     glTexCoord2f(0, 8); glVertex3f(0, 0, 0);
     glEnd();
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();

     glPushMatrix();
     glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, textureVec[1]);
     glBegin(GL_QUADS);
     glNormal3f(1, 0, 0);
     glTexCoord2f(0, 0); glVertex3f(-1.0, -0.5, 0);
     glTexCoord2f(1, 0); glVertex3f(-1.0, -0.5 ,15);
     glTexCoord2f(1, 1); glVertex3f(-1.0, 5, 15);
     glTexCoord2f(0, 1); glVertex3f(-1.0, 5, 0);
     glEnd();
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();


     glPushMatrix();
     glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, textureVec[1]);
     glBegin(GL_QUADS);
     glNormal3f(-1, 0, 0);
     glTexCoord2f(0, 0); glVertex3f(13, -0.5, 0);
     glTexCoord2f(1, 0); glVertex3f(13, -0.5, 15);
     glTexCoord2f(1, 1); glVertex3f(13, 5, 15);
     glTexCoord2f(0, 1); glVertex3f(13, 5, 0);
     glEnd();
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();


     glPushMatrix();
     glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, textureVec[1]);
     glBegin(GL_QUADS);
     glNormal3f(0, 0, 1);
     glTexCoord2f(0, 0); glVertex3f(-2, -0.5, 0);
     glTexCoord2f(1, 0); glVertex3f(13, -0.5, 0);
     glTexCoord2f(1, 1); glVertex3f(13, 5, 0);
     glTexCoord2f(0, 1); glVertex3f(-2, 5, 0);
     glEnd();
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();


     glPushMatrix();
     glEnable(GL_TEXTURE_2D);
     glBindTexture(GL_TEXTURE_2D, textureVec[1]);
     glBegin(GL_QUADS);
     glNormal3f(0, 0, -1);
     glTexCoord2f(0, 0); glVertex3f(13, -0.5, 15);
     glTexCoord2f(1, 0); glVertex3f(-2, -0.5, 15);
     glTexCoord2f(1, 1); glVertex3f(-2, 5, 15);
     glTexCoord2f(0, 1); glVertex3f(13, 5, 15);
     glEnd();
     glDisable(GL_TEXTURE_2D);
     glPopMatrix();

}

void bed()
{
    //bed headboard
    glPushMatrix();
    glScalef(0.1, 0.5, 0.9);
    glTranslatef(-2, -0.5, 6.2);
    drawCube1(0.2, 0.2, 0.2, 0.1, 0.1, 0.05);
    glPopMatrix();

    //bed body
    glPushMatrix();
    glScalef(1, 0.2, 0.9); //1, 0.2, 0.9
    glTranslatef(0, -0.5, 6.2);
    drawCube1(0.5, 0.5, 0.5, 0.25, 0.25, 0.25);
    glPopMatrix();

    //pillow 
    glPushMatrix();
    glTranslatef(0.5, 0.5, 6);
    glRotatef(20, 0, 0, 1);
    glScalef(0.1, 0.15, 0.6);
    drawCube1(0.8, 0.6, 0.6, 0.4, 0.2, 0.2);
    glPopMatrix();



}

void bedsideDrawer()
{
    //bedside drawer *************************************

      //side drawer
    glPushMatrix();
    glTranslatef(0.5, -0.1, 8.7); //0.5,-0.1,9
    glScalef(0.12, 0.2, 0.23);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.1, 0.05);
    glPopMatrix();

    //side drawer's drawer
    glPushMatrix();
    glTranslatef(0.88, 0, 8.8);
    glScalef(0.0001, 0.11, 0.18);
    drawCube1(0.2, 0.2, 0.2, 0.15, 0.1, 0.1);
    glPopMatrix();

    //side drawer's knob
    glPushMatrix();
    glTranslatef(0.9, 0.15, 9.05);
    glScalef(0.01, 0.02, 0.02);
    drawSphere(0.2, 0.1, 0.0, 0.05, 0.05, 0.0);
    glPopMatrix();
}

void lamp()
{
    //lamp base
    glPushMatrix();
    glTranslatef(.6, 0.5, 8.95);
    glScalef(0.07, 0.02, 0.07);
    drawCube1(0, 0, 0.4, 0, 0, 0.2);
    glPopMatrix();

    //lamp stand
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(.7, 0.35, 9.05);
    glScalef(0.01, 0.2, 0.01);
    drawCube1(0, 0, 0.4, 0.0, 0.0, 0.2);
    glPopMatrix();

    //lamp shade
    glColor3f(0.000, 0.000, 0.545);
    glPushMatrix();
    glTranslatef(.6, 0.9, 8.9);
    glScalef(0.08, 0.09, 0.08);
    drawTrapezoid(0.0, 0.0, 0.5, 0, 0, 0.25);
    
    glPopMatrix();
}

void Pic()
{
       
    glPushMatrix();

    GLfloat no_mat[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 32 };

    //设置材料属性
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);    // 材质的环境颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);    // 材质的散射颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);    // 材质的镜面反射颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);    // 镜面反射指数

    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureVec[2]);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.99, 1.4, 5);
    glTexCoord2f(1, 0); glVertex3f(-0.99, 1.4, 7);
    glTexCoord2f(1, 1); glVertex3f(-0.99, 3.2, 7);
    glTexCoord2f(0, 1); glVertex3f(-0.99, 3.2, 5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glPopMatrix();
}

void wardrobe()
{
    //wardrobe
    glPushMatrix();
    glTranslatef(0, 0, 4);
    glScalef(0.12, 0.6, 0.4);
    drawCube1(0.6, 0.3, 0.1, 0.3, 0.15, 0);
    glPopMatrix();

    //wardrobe's 1st drawer
    glPushMatrix();
    glTranslatef(0.36, 1.5, 4.04);
    glScalef(0.0001, 0.1, 0.4);
    drawCube1(0.4, 0.2, 0.2, 0.2, 0.1, 0.1);
    glPopMatrix();

    //wardrobe's 2nd drawer
    glPushMatrix();
    glTranslatef(0.36, 1, 4.05);
    glScalef(0.0001, 0.1, 0.4);
    drawCube1(0.4, 0.2, 0.2, 0.2, 0.1, 0.1);
    glPopMatrix();

    //wardrobe's 3rd drawer
    glPushMatrix();
    glTranslatef(0.36, 0.5, 4.05);
    glScalef(0.0001, 0.1, 0.4);
    drawCube1(0.4, 0.2, 0.2, 0.2, 0.1, 0.1);
    glPopMatrix();

    //wardrobe's 4th drawer
    glPushMatrix();
    glTranslatef(0.36, 0.0, 4.05);
    glScalef(0.0001, 0.1, 0.4);
    drawCube1(0.4, 0.2, 0.2, 0.2, 0.1, 0.1);
    glPopMatrix();

    //wardrobe's 1st drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 1.5, 4.3);
    glScalef(0.01, 0.03, 0.2);
    drawCube1(0.6, 0.15, 0.0, 0.3, 0.1, 0);
    glPopMatrix();

    //wardrobe's 2nd drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 1.1, 4.3);
    glScalef(0.01, 0.03, 0.2);
    drawCube1(0.6, 0.15, 0.0, 0.3, 0.1, 0);
    glPopMatrix();

    //wardrobe's 3rd drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 0.7, 4.3);
    glScalef(0.01, 0.03, 0.2);
    drawCube1(0.6, 0.15, 0.0, 0.3, 0.1, 0);
    glPopMatrix();

    //wardrobe's 4th drawer handle
    glColor3f(0.3, 0.1, 0);
    glPushMatrix();
    glTranslatef(0.37, 0.3, 4.3);
    glScalef(0.01, 0.03, 0.2);
    drawCube1(0.6, 0.15, 0.0, 0.3, 0.1, 0);
    glPopMatrix();

}



void lightBulb1()
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
    GLfloat mat_diffuse[] = { 1.000, 0.843, 0.000, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat high_shininess[] = { 100.0 };
    GLfloat mat_emission[] = { 1.000, 1,1, 0.0 };

    glPushMatrix();
    glTranslatef(5, 5, 8);
    glScalef(0.2, 0.2, 0.2);

    //设置材料属性
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat); // 材质的环境颜色
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);// 材质的散射颜色
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat); // 材质的镜面反射颜色
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);// 材质的镜面反射颜色

    //判断开关是否打开
    if (switchOne == true) {
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    }
    else {
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    }

    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();
}





void lightOne()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 5.0, 5.0, 8.0, 1.0 }; 

    //glEnable( GL_LIGHT0);

    if (amb1 == true) { glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT0, GL_AMBIENT, no_light); }

    if (diff1 == true) { glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT0, GL_DIFFUSE, no_light); }

    if (spec1 == true) { glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT0, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();
}

void lampLight()
{
    glPushMatrix();
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.7, 1.5, 9.0, 1.0 }; 

    //glEnable( GL_LIGHT2);

    if (amb2 == true) { glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient); }
    else { glLightfv(GL_LIGHT2, GL_AMBIENT, no_light); }

    if (diff2 == true) { glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse); }
    else { glLightfv(GL_LIGHT2, GL_DIFFUSE, no_light); }

    if (spec2 == true) { glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular); }
    else { glLightfv(GL_LIGHT2, GL_SPECULAR, no_light); }

    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    GLfloat spot_direction[] = { 0.3, -1, -0.8 };
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35.0);
    glPopMatrix();
}
void cupboard()
{
    //cupboard body
    glPushMatrix();
    glTranslatef(4, 0, 4.4);
    glScalef(0.5, 1, 0.5);
    drawCube1(0.32, 0.16, 0.00, 0.16, 0.08, 0.00);
    glPopMatrix();

    //cupboard's last stripline
    glPushMatrix();
    glTranslatef(4, 0, 5.9);
    glScalef(0.5, 0.01, 0.0001);
    drawCube1(0.5, 0.2, 0.1, 0.25, 0.1, 0.05);
    glPopMatrix();

    //cupboard's lst horizontal stripline
    glPushMatrix();
    glTranslatef(5.5, 0, 5.9);
    glScalef(0.01, 1, 0.0001);
    drawCube1(0.5, 0.2, 0.1, 0.25, 0.1, 0.05);
    glPopMatrix();

    //cupboard's right side horizontal stripline
    glPushMatrix();
    glTranslatef(4.75, 1, 5.9);
    glScalef(0.01, 0.67, 0.0001);
    drawCube1(0.5, 0.2, 0.1, 0.25, 0.1, 0.05);
    glPopMatrix();

    //cupboard's left side horizontal stripline
    glPushMatrix();
    glTranslatef(4, 0, 5.9);
    glScalef(0.01, 1, 0.0001);
    drawCube1(0.5, 0.2, 0.1, 0.25, 0.1, 0.05);
    glPopMatrix();

    //cupboard's handle right
    glPushMatrix();
    glTranslatef(5, 1.4, 5.9);
    glScalef(0.02, 0.18, 0.01);
    drawCube1(0.5, 0.2, 0.1, 0.25, 0.1, 0.05);
    glPopMatrix();

    //cupboard's handle left
    glPushMatrix();
    glTranslatef(4.5, 1.4, 5.9);
    glScalef(0.02, 0.18, 0.01);
    drawCube1(0.5, 0.2, 0.1, 0.25, 0.1, 0.05);
    glPopMatrix();

    //cupboard's drawer's 1st handle
    glPushMatrix();
    glTranslatef(4.5, 0.7, 5.9);
    glScalef(0.16, 0.02, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();

    //cupboard's drawer's 2nd handle
    glPushMatrix();
    glTranslatef(4.5, 0.25, 5.9);
    glScalef(0.16, 0.02, 0.01);
    drawCube1(0.2, 0.1, 0.1, 0.1, 0.05, 0.05);
    glPopMatrix();
}



void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, refX, refY, refZ, 0, 1, 0); 

    glEnable(GL_LIGHTING);
    lightOne();
    lampLight();
    room();
    bed();
    bedsideDrawer();
    lamp();
    Pic();
    wardrobe();
    cupboard();
    lightBulb1();

    glDisable(GL_LIGHTING);

    glFlush();
    glutSwapBuffers();
}

void myKeyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w': // move eye point upwards along Y axis
        eyeY += 1.0;
        break;
    case 's': // move eye point downwards along Y axis
        eyeY -= 1.0;
        break;
    case 'a': // move eye point left along X axis
        eyeX -= 1.0;
        break;
    case 'd': // move eye point right along X axis
        eyeX += 1.0;
        break;
    case 'o':  //zoom out
        eyeZ += 1;
        break;
    case 'i': //zoom in
        eyeZ -= 1;
        break;
    case 'q': //back to default eye point and ref point
        eyeX = 7.0; eyeY = 2.0; eyeZ = 15.0;
        refX = 0.0; refY = 0.0; refZ = 0.0;
        break;
    case 'k':  //move ref point away from screen/ along z axis
        refZ += 1;
        break;
    case 'l': //move ref point towards screen/ along z axis
        refZ -= 1;
        break;
    case '1': //to turn on and off light one
        if (switchOne == false)
        {
            switchOne = true; amb1 = true; diff1 = true; spec1 = true;
            glEnable(GL_LIGHT0); break;
        }
        else if (switchOne == true)
        {
            switchOne = false; amb1 = false; diff1 = false; spec1 = false; glDisable(GL_LIGHT0); break;
        }
        
    case '2': //to turn on and off light one
        if (switchLamp == false)
        {
            switchLamp = true; amb2 = true; diff2 = true; spec2 = true;
            glEnable(GL_LIGHT2); break;
        }
        else if (switchLamp == true)
        {
            switchLamp = false; amb2 = false; diff2 = false; spec2 = false;
            glDisable(GL_LIGHT2); break;
        }
      
    case 27:    // Escape key
        exit(1);
    }

    glutPostRedisplay();
}


void animate()
{
    if (redFlag == true)
    {
        theta += 2;
        z -= 0.02; //0.016667;
        if (theta >= 196 && theta <= 210)
        {
            y = 1.44;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.42;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.4;
        }
        else if (theta >= 164 && theta <= 178)
        {
            y = 1.42;
        }

        if (theta == 210)
        {
            redFlag = false;
        }
    }
    else if (redFlag == false)
    {
        theta -= 2;
        z += 0.02;//0.016667;

        if (theta >= 196 && theta <= 210)
        {
            y = 1.44;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.42;
        }
        else if (theta >= 180 && theta <= 194)
        {
            y = 1.4;
        }
        else if (theta >= 164 && theta <= 178)
        {
            y = 1.42;
        }

        if (theta == 150)
        {
            redFlag = true;
        }
    }

    glutPostRedisplay();

}

void fullScreen(int w, int h)
{
    //Prevent a divide by zero, when window is too short;you cant make a window of zero width.
    if (h == 0)
        h = 1;
    float ratio = (GLfloat)w / (GLfloat)h;         //Calculate aspect ratio of the window

    glMatrixMode(GL_PROJECTION);                   //Use the Projection Matrix
    glLoadIdentity();                              //Reset Matrix

    glViewport(0, 0, w, h);                        //Set the viewport to be the entire window
    gluPerspective(60, ratio, 0.1, 500);             //Set the correct perspective.

    glMatrixMode(GL_MODELVIEW);                    //Get Back to the Modelview
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
   
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("Bedroom");
   

    textureVec[0] = loadGLTexture("./floor.bmp");
    textureVec[1] = loadGLTexture("./wall.bmp");
    textureVec[2] = loadGLTexture("./van.bmp");
   
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutReshapeFunc(fullScreen);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutIdleFunc(animate);
    glutMainLoop();
  
    return 0;
}

// i am zcw