// Copyright 2015 HEy_Yo

#include "Include.h"
#include "LoadShader.h"
#include "ObjLoader.h"
#include "Angel.h"
#include<gl/FreeImage.h>

enum VAO_IDs {Triangles, NumVAOs};
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs {vPosition = 0};
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 3;



typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

std::vector<tg::shape_r> shapes[10];   //暂时最多10个模型
std::vector<tg::material_r> materials;
size_t faceCount[10];
GLuint *m_vao[10];
GLuint *m_vbo[10];
GLuint *m_ebo[10];
size_t *e_sizePerFace[10];

int m_num = 8;                 //载入模型的个数！

size_t size = 0;
size_t n_size = 0;
size_t e_size = 0;


GLfloat dx = 0, dy = 0, dz = 0;
GLfloat ax = 0, ay = 0, az = 0;
GLfloat mx = 0, my = 0;
GLint MouseDown = 0;
GLfloat aspect = 1;
GLuint move_loc;
GLuint theta_loc;
GLuint model_view_loc, projection_loc, model_move_loc;
float step = 1.0f;


//贴图
struct _BMP{
	unsigned long sizeX;//X方向大小
	unsigned long sizeY;//Y方向大小
	unsigned char *data;//数据地址
};
typedef struct _BMP BMP;
GLuint texture[10];//存储10个纹理

/*
** 利用freeimage加载bmp图像
*/

GLboolean LoadBmp(const char*Tfilename, BMP* texture_image)
{
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(Tfilename, 0);
	FIBITMAP *dib = FreeImage_Load(fifmt, Tfilename, 0);
	dib = FreeImage_ConvertTo24Bits(dib);

	int Twidth = FreeImage_GetWidth(dib);
	int Theight = FreeImage_GetHeight(dib);

	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);
	int pix = 0;

	if (texture_image == NULL)
		return FALSE;
	
	texture_image->data = (BYTE *)malloc(Twidth * Theight * 3);
	texture_image->sizeX = Twidth;
	texture_image->sizeY = Theight;

	for (pix = 0; pix < Twidth * Theight; pix++) {
		texture_image->data[pix * 3 + 0] = pixels[pix * 3 + 2];
		texture_image->data[pix * 3 + 1] = pixels[pix * 3 + 1];
		texture_image->data[pix * 3 + 2] = pixels[pix * 3 + 0];

	}

	FreeImage_Unload(dib);

	return TRUE;
}

int LoadGLTextures(const char **tex_id)
// 载入位图(调用上面的代码)并转换成纹理
{
	int Status = FALSE; // 状态指示器
	BMP *textureImage; // 创建纹理的存储空间
	textureImage =(BMP*) malloc(sizeof(BMP));

	// 载入位图，检查有无错误，如果位图没找到则退出
	for (int i = 0; i < m_num; i++){
		if (LoadBmp(tex_id[i], textureImage))
		{
			Status = TRUE; // 将 Status 设为 TRUE
			glGenTextures(1, &texture[i]); // 创建纹理
			// 使用来自位图数据生成 的典型纹理
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			// 生成纹理
			glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImage->sizeX, textureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
		}
	}
	
	if (textureImage) // 纹理是否存在
	{
		if (textureImage->data) // 纹理图像是否存在
		{
			free(textureImage->data); // 释放纹理图像占用的内存
		}
		free(textureImage); // 释放图像结构
	}

	return Status; // 返回 Status
}

void myIdle()
{
	Sleep(10);
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		dz += step;
		break;
	case 'W':
		dz += step;
		break;
	case 's':
		dz -= step;
		break;
	case 'S':
		dz -= step;
		break;
	case 'a':
		dx -= step;
		break;
	case 'A':
		dx -= step;
		break;
	case 'd':
		dx += step;
		break;
	case 'D':
		dx += step;
		break;
	case 'q':
		dy += step;
		break;
	case 'Q':
		dy += step;
		break;
	case 'e':
		dy -= step;
		break;
	case 'E':
		dy -= step;
		break;
	case 'j':
		step += 1.0f;
		break;
	case 'J':
		step += 1.0f;
		break;
	case 'k':
		if (step>1)step -= 1.0f;
		break;
	case 'K':
		if (step>1)step -= 1.0f;
		break;
	}
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_DOWN) MouseDown = 1, mx = x, my = y;
	else if (button == GLUT_KEY_PAGE_UP)
	{
		dz += 1.0f;
	}
	else if (button == GLUT_KEY_PAGE_DOWN)
	{
		dz -= 1.0f;
	}
	else
	{
		MouseDown = 0;
	}
}

void myMotion(int x, int y)
{
	if (MouseDown)
	{
		ax += (y - my) / 5.0f;
		ay += (x - mx) / 5.0f;
		mx = x;
		my = y;
	}
}

void myReshape(int width, int height)
{
	glViewport(0, 0, width, height);

	GLfloat aspect = GLfloat(width) / height;
	mat4 projection = Perspective(45.0, aspect, 0.5, 3.0);

	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	const vec3 viewer_pos(dx, dy, dz);
	mat4 mv = Translate(-viewer_pos)*
		RotateX(ax) *
		RotateY(ay) *
		RotateZ(az);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, mv);
	
	mat4 model_move[10];                 //移动不同模型到特定位置
	model_move[0] = Translate(vec3(-10.0, -10.5, 8.0));   //bear
	model_move[1] = Translate(vec3(-10.0, -10.5, 8.0));   //luyou
	model_move[2] = Translate(vec3(-8.0, -10.5, 8.0));   //dinosaur
	model_move[3] = Translate(vec3(-8.0, -9.5, 8.0));   //deer
	model_move[4] = Translate(vec3(-45.0, -10.0, 6.0));	//trees1
	model_move[5] = Translate(vec3(-10.0, -10.3, 0.0));		//mountain
	model_move[6] = Translate(vec3(-5.5, -8.0, 10.0));	//grass
	model_move[7] = Translate(vec3(10.5, -10.0, 10.7));	//trees2

	for (int k = 0; k < m_num; k++){
		glBindTexture(GL_TEXTURE_2D, texture[k]);    //对不同obj里的模型使用不同的纹理
		
		glUniformMatrix4fv(model_move_loc, 1, GL_TRUE, model_move[k]);

		for (int i = 0; i < faceCount[k]; ++i)
		{
			glBindVertexArray(m_vao[k][i]);
			//array绘制方式
			//glDrawArrays(GL_TRIANGLES, 0, sizePerFace[i]);

			//索引绘制方式
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[k][i]);
			
			glDrawElements(GL_TRIANGLES, e_sizePerFace[k][i], GL_UNSIGNED_INT, NULL);


		}
	}
	glFlush();
	glutSwapBuffers();
}

//导入obj
void loadModle()
{
	std::string objFile_1 = "bear.obj";
	std::string objFile_2 = "luyou.obj";
	std::string objFile_3 = "dinosaur.obj";
	std::string objFile_4 = "deer.obj";
	std::string objFile_5 = "trees1.obj";
	std::string objFile_6 = "mountain.obj";
	std::string objFile_7 = "grass.obj";
	std::string objFile_8 = "trees2.obj";

	std::string mtlPath = ".";
	tg::LoadObj(shapes[0], materials, objFile_1.c_str());
	tg::LoadObj(shapes[1], materials, objFile_2.c_str());
	tg::LoadObj(shapes[2], materials, objFile_3.c_str());
	tg::LoadObj(shapes[3], materials, objFile_4.c_str());
	tg::LoadObj(shapes[4], materials, objFile_5.c_str());
	tg::LoadObj(shapes[5], materials, objFile_6.c_str());
	tg::LoadObj(shapes[6], materials, objFile_7.c_str());
	tg::LoadObj(shapes[7], materials, objFile_8.c_str());

}

void init() {
	GLuint program = LoadShaders("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	for (int k = 0; k < m_num; k++){
	for (int i = 0; i < faceCount[k]; ++i)
	{
		//n_size += shapes[i].mesh.normals.size();//法线
		//size += shapes[i].mesh.positions.size();//点位置
		//e_size += shapes[i].mesh.indices.size();
		e_sizePerFace[k][i] = shapes[k][i].mesh.indices.size();			//每个面的索引数，其实大概是跟每个面的顶点数相等的


	}
}

	// 载入文理
	const char *tex[10];
	tex[0] = "bear.bmp";	//bear
	tex[1] = "luyou.bmp";	//luyou
	tex[2] = "dinosaur.bmp";	//dinosaur
	tex[3] = "deer.bmp";	//deer
	tex[4] = "trees1.bmp";	//trees1
	tex[5] = "mountain.bmp";//mountain
	tex[6] = "grass.bmp";	//grass
	tex[7] = "trees2.bmp";	//trees2

	LoadGLTextures(tex);
	
	  

	for (int k = 0; k < m_num; k++){
		
		
		for (int i = 0; i < faceCount[k]; ++i)
		{
			glGenVertexArrays(1, &m_vao[k][i]);
			glBindVertexArray(m_vao[k][i]);

			//这段就是用来处理索引数组缓存变量的啦
			glGenBuffers(1, &m_ebo[k][i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo[k][i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[k][i].mesh.indices.size()*sizeof(GLuint), &shapes[k][i].mesh.indices[0], GL_STATIC_DRAW);

			//顶点数组缓存

			glGenBuffers(1, &m_vbo[k][i]);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo[k][i]);
			glBufferData(GL_ARRAY_BUFFER, shapes[k][i].mesh.positions.size()*sizeof(GLfloat)+shapes[k][i].mesh.normals.size()*sizeof(GLfloat)+shapes[k][i].mesh.texcoords.size()*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, shapes[k][i].mesh.positions.size()*sizeof(GLfloat), &shapes[k][i].mesh.positions[0]);
			glBufferSubData(GL_ARRAY_BUFFER, shapes[k][i].mesh.positions.size()*sizeof(GLfloat), shapes[k][i].mesh.normals.size()*sizeof(GLfloat), &shapes[k][i].mesh.normals[0]);
			glBufferSubData(GL_ARRAY_BUFFER, shapes[k][i].mesh.normals.size()*sizeof(GLfloat), shapes[k][i].mesh.texcoords.size()*sizeof(GLfloat), &shapes[k][i].mesh.texcoords[0]);

			GLuint vPosition = glGetAttribLocation(program, "vPosition");
			glEnableVertexAttribArray(vPosition);
			glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

			GLuint vNormal = glGetAttribLocation(program, "vNormal");
			glEnableVertexAttribArray(vNormal);
			glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (const void*)(shapes[k][i].mesh.positions.size()*sizeof(GLfloat)));

			GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
			glEnableVertexAttribArray(vTexCoord);
			glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (const void*)(shapes[k][i].mesh.normals.size()*sizeof(GLfloat)));

			
		}
		glUniform1i(glGetUniformLocation(program, "texture"), 0);
		
	}

	

	point4 r_light_position(-8.0, 8.0, -8.0, 0.0);
	color4 r_light_ambient(0.0, 0.0, 0.0, 1.0);
	color4 r_light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 r_light_specular(1.0, 1.0, 1.0, 1.0);

	point4 g_light_position(8.0, 8.0, -3.0, 0.0);
	color4 g_light_ambient(0.0, 0.0, 0.0, 1.0);
	color4 g_light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 g_light_specular(1.0, 1.0, 1.0, 1.0);

	point4 b_light_position(1.0, -10.0, 5.0, 0.0);
	color4 b_light_ambient(0.0, 0.0, 0.0, 1.0);
	color4 b_light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 b_light_specular(1.0, 1.0, 1.0, 1.0);



	color4 material_ambient(1.0, 1.0, 1.0, 1.0);

	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float  material_shininess = 100.0;

	color4 r_ambient_product = r_light_ambient * material_ambient;
	color4 r_diffuse_product = r_light_diffuse * material_diffuse;
	color4 r_specular_product = r_light_specular * material_specular;

	color4 g_ambient_product = g_light_ambient * material_ambient;
	color4 g_diffuse_product = g_light_diffuse * material_diffuse;
	color4 g_specular_product = g_light_specular * material_specular;

	color4 b_ambient_product = b_light_ambient * material_ambient;
	color4 b_diffuse_product = b_light_diffuse * material_diffuse;
	color4 b_specular_product = b_light_specular * material_specular;


	glUniform4fv(glGetUniformLocation(program, "r_AmbientProduct"),
		1, r_ambient_product);
	glUniform4fv(glGetUniformLocation(program, "r_DiffuseProduct"),
		1, r_diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "r_SpecularProduct"),
		1, r_specular_product);
	glUniform4fv(glGetUniformLocation(program, "r_LightPosition"),
		1, r_light_position);

	glUniform4fv(glGetUniformLocation(program, "g_AmbientProduct"),
		1, g_ambient_product);
	glUniform4fv(glGetUniformLocation(program, "g_DiffuseProduct"),
		1, g_diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "g_SpecularProduct"),
		1, g_specular_product);
	glUniform4fv(glGetUniformLocation(program, "g_LightPosition"),
		1, g_light_position);

	glUniform4fv(glGetUniformLocation(program, "b_AmbientProduct"),
		1, b_ambient_product);
	glUniform4fv(glGetUniformLocation(program, "b_DiffuseProduct"),
		1, b_diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "b_SpecularProduct"),
		1, b_specular_product);
	glUniform4fv(glGetUniformLocation(program, "b_LightPosition"),
		1, b_light_position);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);




	//////////////////////////////////////////////////
	model_view_loc = glGetUniformLocation(program, "model_view");
	projection_loc = glGetUniformLocation(program, "projection");
	move_loc = glGetUniformLocation(program, "move");
	theta_loc = glGetUniformLocation(program, "theta");
	model_move_loc = glGetUniformLocation(program, "model_move");

	//Model = glm::rotate(Model, 0.0f, glm::vec3(0, 1, 0));
	//matview = glGetUniformLocation(program, "mat_view");
	//matmodel = glGetUniformLocation(program, "mat_model");
	//matproj = glGetUniformLocation(program, "mat_proj");
	//glUniformMatrix4fv(matview, 1, GL_FALSE, &View[0][0]);
	//glUniformMatrix4fv(matmodel, 1, GL_FALSE, &Model[0][0]);
	//glUniformMatrix4fv(matproj, 1, GL_FALSE, &Projection[0][0]);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glClearColor(0.9, 0.8, 0.7, 1.0);
}

int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Bling Bling动物园");
    glewExperimental = GL_TRUE;
	std::cout << "W and w -->z-" << std::endl;
	std::cout << "S and s -->z+" << std::endl;
	std::cout << "D and d -->x-" << std::endl;
	std::cout << "A and a -->x+" << std::endl;
	std::cout << "Q and q -->y-" << std::endl;
	std::cout << "E and e -->y+" << std::endl;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << glewGetErrorString(err) << std::endl;
        system("pause");
        exit(EXIT_FAILURE);
    }

    loadModle();
	for (int k = 0; k < m_num; k++){
		faceCount[k] = shapes[k].size();

		m_vao[k] = new GLuint[faceCount[k]];
		m_ebo[k] = new GLuint[faceCount[k]];
		m_vbo[k] = new GLuint[faceCount[k]];

		e_sizePerFace[k] = new GLuint[faceCount[k]];
	}
	glutReshapeFunc(&myReshape);
	glutKeyboardFunc(&myKeyboard);
	glutMouseFunc(&myMouse);
	glutMotionFunc(&myMotion);
	glutDisplayFunc(&display);
	glutIdleFunc(&myIdle);
    init();
	/*glutReshapeFunc(&myReshape);
	glutKeyboardFunc(&myKeyboard);
	glutMouseFunc(&myMouse);
	glutMotionFunc(&myMotion);
    glutDisplayFunc(&display);*/
    glutMainLoop();
	return 0;
}