#include <stdio.h>
#include <string.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "math_3d.h"

GLuint VBO;
GLint gScaleLocation;

static const char* pVS = "#version 330				\n"
"layout (location = 0) in vec3 Position; \n"
"uniform float gScale;  \n"
"void main()				\n"
"{							\n"
"	gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0); \n"
"}";

static const char* pFS = " #version 330						   \n"
"out vec4 FragColor;						\n"
"void main()								 \n"
"{										  \n"
"	FragColor = vec4(1.0, 0.0, 0.0, 1.0);  \n"
"}";

static void RenderSceneCB()
{
  glClear(GL_COLOR_BUFFER_BIT);

  static float Scale = 0.0f;
  Scale += 0.001f;
  glUniform1f(gScaleLocation, sinf(Scale));

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray(0);

  glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
  glutDisplayFunc(&RenderSceneCB);
  glutIdleFunc(&RenderSceneCB);
}

static void CreateVertexBuffer()
{
  Vector3f Vertices[3];
  Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
  Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
  Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
  GLuint ShaderObj = glCreateShader(ShaderType);

  if (ShaderObj == 0) {
    fprintf(stderr, "Error creating shader type %d\n", ShaderType);
    exit(0);
  }

  const GLchar* p[1];
  p[0] = pShaderText;
  GLint Lengths[1];
  Lengths[0] = strlen(pShaderText);
  glShaderSource(ShaderObj, 1, p, Lengths);
  glCompileShader(ShaderObj);
  GLint success;
  glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
    fprintf(stderr, "Error Compiling shader type %d : '%s'\n", ShaderType, InfoLog);
    exit(1);
  }

  glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
  GLuint ShaderProgram = glCreateProgram();

  if (ShaderProgram == 0) {
    fprintf(stderr, "Error creating shader program\n");
    exit(1);
  }

  AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
  AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

  GLint Success = 0;
  GLchar ErrorLog[1024] = { 0 };

  glLinkProgram(ShaderProgram);
  glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
  if (Success == 0) {
    glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Error Linking shader Program: '%s'\n", ErrorLog);
    exit(1);
  }

  gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
  assert(gScaleLocation != 0xFFFFFFFF);

  glValidateProgram(ShaderProgram);
  glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
  if (!Success) {
    glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Invalid shader program : '%s'\n", ErrorLog);
    exit(1);
  }

  glUseProgram(ShaderProgram);
}

int main(int argc, char **argv) 
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(1024, 768);
  glutCreateWindow("Tutorial 05");

  InitializeGlutCallbacks();

  GLenum res = glewInit();
  if (res != GLEW_OK) {
    fprintf(stderr, "Error : '%s\n", glewGetErrorString(res));
    return 1;
  }

  printf("GL version : %s\n", glGetString(GL_VERSION));
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  CreateVertexBuffer();

  CompileShaders();

  glutMainLoop();
  return 0;
}