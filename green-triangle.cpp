#include<stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>

struct Program {

	static GLuint Load(const char* vert, const char* geom, const char* frag)
	{
		GLuint prog = glCreateProgram();
		if (vert) AttachShader(prog, GL_VERTEX_SHADER, vert);
		if (geom) AttachShader(prog, GL_GEOMETRY_SHADER, geom);
		if (frag) AttachShader(prog, GL_FRAGMENT_SHADER, frag);
		glLinkProgram(prog);
		CheckStatus(prog);
		return prog;
	}

private:
	static void CheckStatus(GLuint obj)
	{
		GLint status = GL_FALSE, len = 10;
		if (glIsShader(obj)) glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
		if (glIsProgram(obj)) glGetProgramiv(obj, GL_LINK_STATUS, &status);
		if (status == GL_TRUE) return;
		if (glIsShader(obj)) glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
		if (glIsProgram(obj)) glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
		std::vector< char > log(len, 'X');
		if (glIsShader(obj)) glGetShaderInfoLog(obj, len, NULL, &log[0]);
		if (glIsProgram(obj)) glGetProgramInfoLog(obj, len, NULL, &log[0]);
		std::cerr << &log[0] << std::endl;
		exit(-1);
	}

	static void AttachShader(GLuint program, GLenum type, const char* src)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		CheckStatus(shader);
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}
};


#define GLSL(version, shader) "#version " #version "\n" #shader


const char* vert = GLSL
(
400 core,
layout(location = 0) in vec4 vPosition;
void main()
{
	gl_Position = vPosition;
}
);

const char* frag = GLSL
(
400 core,
out vec4 fColor;
void main()
{
	fColor = vec4(0.0, 1.0, 0.0, 1.0);
}
);

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

//const GLuint NumVertices = 9;
const GLuint NumVertices = 3;

void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	GLfloat vertices[NumVertices][2] = {
		{ -0.70, -0.70 }, // Triangle 1
		{ 0.65, -0.70 },
		{ -0.70, 0.65 },
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint program = Program::Load(vert, NULL, frag);
	glUseProgram(program);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glEnableVertexAttribArray(vPosition);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(VAOs[Triangles]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	//glutInitContextVersion(4, 0);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
		exit(EXIT_FAILURE);

	init();

	glutDisplayFunc(display);
	glutMainLoop();
}


