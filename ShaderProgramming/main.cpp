#include <glad/glad.h>
#include <glfw3.h>
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
#include <iostream>

#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"

#include "tables.h"

#include "TextRenderer.h"

const float SLICE_HEIGHT = (256 * 2 / (96 * 256));

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void renderScene(const Shader &shader);
void renderCube();
void renderQuad();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float cameraYDelta = 0;

// light
glm::vec3 lightPos(5.0f, 10.0f, 3.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int scrollInput = 0;


// particles

glm::ivec3 nParticles = glm::ivec3(50, 50, 50);
float speed = 35.0f;
float angle = 0.0f;
glm::vec4 bh1(5, 0, 0, 1);
glm::vec4 bh2(-5, 0, 0, 1);
unsigned int totalParticles = nParticles.x * nParticles.y * nParticles.z;

float particlesTime = 0.0f;
float particlesDelta = 0.0f;

unsigned int particlesVAO = 0;
unsigned int bhVAO = 0, bhBuf = 0;

// end particles 

// scene for shadow mapping
unsigned int planeVAO;

// VERTICES POINTS 96 x 96 for MARCHING CUBES
//
float dummyVertices[96 * 96 * 2];
void dummyFunction1();
unsigned int loadTexture(char const* path);
// -------------------------


void initBuffers()
{
	

	std::vector<float> initPos;
	std::vector<float> initVel;
	std::vector<float> initLife;
	std::vector<float> initType;
	std::vector<float> initSize;

	glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
	float dx = 2.0f / (nParticles.x - 1),
		dy = 2.0f / (nParticles.y - 1),
		dz = 2.0f / (nParticles.z - 1);

	glm::mat4 transf = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, -1));
	for (int i = 0; i < totalParticles; i++)
	{
		initPos.push_back(0.0f);
		initPos.push_back(0.0f);
		initPos.push_back(0.0f);
		initPos.push_back(1.0f);

		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 20));
		initLife.push_back(r);

		//std::cout << r << std::endl;

		float velx = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.05;
		float velz = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX))) - 0.05;
		initVel.push_back(velx / 10);
		initVel.push_back(0);
		initVel.push_back(velz / 10);
		initVel.push_back(0);

		initType.push_back(1);
		initSize.push_back(0.02);
	}

	unsigned int bufs[5];
	glGenBuffers(5, bufs);
	unsigned int posBuf = bufs[0];
	unsigned int velBuf = bufs[1];
	unsigned int lifeBuf = bufs[2];
	unsigned int typeBuf = bufs[3];
	unsigned int sizeBuf = bufs[4];

	unsigned int bufSize = totalParticles * 4 * sizeof(float);

	// The buffer for positions
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initPos[0], GL_DYNAMIC_DRAW);

	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufSize, &initPos[0]);
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, bufSize, totalParticles * sizeof(float), &initType[0]);

	//glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut2);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, (1280 * 16), &g_triTable);

	// velocities
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &initVel[0], GL_DYNAMIC_COPY);

	// lifeSpan
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, lifeBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, totalParticles * sizeof(float), &initLife[0], GL_DYNAMIC_COPY);

	// particle type
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, typeBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, totalParticles * sizeof(float), &initType[0], GL_DYNAMIC_COPY);

	// particle size
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, sizeBuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, totalParticles * sizeof(float), &initSize[0], GL_DYNAMIC_COPY);

	// set up the vao
	glGenVertexArrays(1, &particlesVAO);
	glBindVertexArray(particlesVAO);

	glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);	
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void renderParticles(Shader computeShader, Shader renderShader, glm::mat4 view, glm::mat4 model, glm::mat4 projection, int texture, glm::vec3 cameraPos)
{
	glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1));
	glm::vec3 att1 = glm::vec3(rot*bh1);
	glm::vec3 att2 = glm::vec3(rot*bh2);
	
	computeShader.use();
	computeShader.setVec3("BlackHolePos1", att1);
	computeShader.setVec3("BlackHolePos2", att2);
	computeShader.setFloat("DeltaT", deltaTime);
	glDispatchCompute(totalParticles / 1000, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// draw the scene
	renderShader.use();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 mv = view * model;
	glm::mat3 norm = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));

	renderShader.setMat4("ModelViewMatrix", mv);
	renderShader.setMat3("NormalMatrix", norm);
	renderShader.setMat4("MVP", projection * mv);
	renderShader.setMat4("ViewProjectionMatrix", view * projection);
	renderShader.setVec3("CameraPos", cameraPos);
	renderShader.setFloat("DeltaT", deltaTime);

	// Draw Particles
	glPointSize(5.0f);
	renderShader.setVec4("Color", glm::vec4(0, 0, 1.0, 0.2));
	glBindVertexArray(particlesVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_POINTS, 0, totalParticles);
	glBindVertexArray(0);
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);



	// SHADER PROGRAMS
	// ------------------------------------
	Shader ourShader("vertex_shader.vs", "fragment_shader.fs");
	ourShader.linkProgram();
	Shader screenShader("noiseShader.vert", "noiseShader.frag", "geometry_shader.gsh");
	screenShader.linkProgram();

	Shader computeShader("particles.comp");
	computeShader.linkProgram();

	//Shader particleShader("particles.vert", "particles.frag", "particles.geom");
	Shader particleShader("particles.vert", "particles.frag", "particles.geom");
	particleShader.linkProgram();

	Shader textShader("text.vert", "text.frag");
	textShader.linkProgram();
	textShader.use();
	textShader.setMat4("projection", glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT)));

	Shader shader("shader.vert", "shader.frag");
	shader.linkProgram();

	Shader tessShader("tessShader.vert", "tessShader.frag", nullptr, "tessShader.tesc", "tessShader.tese");
	tessShader.linkProgram();

	Shader simpleDepthShader("depthShader.vert", "depthShader.frag");
	simpleDepthShader.linkProgram();

	Shader blurShader("blurShader.vert", "blurShader.frag");
	blurShader.linkProgram();

	// TRANSFORM SHADER PROGRAM
	// ------------------------------------
	Shader marchingCubesShader("marchingCubes.vert", "marchingCubes.frag", "marchingCubes.geom");
	marchingCubesShader.linkProgram();

	// screen quad VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

#pragma region ShadowMapping

	// plane VAO from the shadow stuff
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	unsigned int woodTexture = loadTexture("wood.png");

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// create another texture
	unsigned int colorTexture;
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// create and attach depth texture as FBO's depth buffer
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	// we need another 2 frame buffers for good ol' gauss
	unsigned int pingpongFBO[2];
	unsigned int pingpongTextures[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongTextures);

	std::cout << "Ping: " << pingpongTextures[0] << " Pong: " << pingpongTextures[1] << std::endl;

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTextures[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}


	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);

	blurShader.use();
	blurShader.setInt("image", 0);

	// lighting info
	// -------------
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	int frames = 0;
	int printFrame = 0;
	double lastTime = glfwGetTime();

	std::string ui = "Gauss Iterationen: ";

	// shadow stuff end here
#pragma endregion Well Shadow Mapping

	// set the vertices for the marching cubes
	//dummyFunction();
	dummyFunction1();




	// STEP 3 
	// ------
	// Create input VBO and vertex format

	// draw a "dummy" vertex buffer
	// BUFFER TO PASS 96x96 Vertices to
	// -------------------------------
	unsigned int dummyVAO, dummyVBO;
	glGenVertexArrays(1, &dummyVAO);
	glGenBuffers(1, &dummyVBO);
	glBindVertexArray(dummyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, dummyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dummyVertices), &dummyVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	//glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	int diffuseMap = loadTexture("C:/Users/basti/source/repos/SpgWaterfalls/ShaderProgramming/bricks2.jpg");
	int normalMap = loadTexture("C:/Users/basti/source/repos/SpgWaterfalls/ShaderProgramming/bricks2_normal.jpg");
	int heightMap = loadTexture("C:/Users/basti/source/repos/SpgWaterfalls/ShaderProgramming/bricks2_disp.jpg");

	int particleSprite = loadTexture("C:/Users/basti/source/repos/SpgWaterfalls/ShaderProgramming/particle.png");
	int particleSprite2 = loadTexture("C:/Users/basti/source/repos/SpgWaterfalls/ShaderProgramming/surprised.png");

	// -------------------------
	marchingCubesShader.use();
	marchingCubesShader.setInt("screenTexture", 0);
	marchingCubesShader.setInt("diffuseMap", 1);
	marchingCubesShader.setInt("normalMap", 2);
	marchingCubesShader.setInt("heightMap", 3);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	particleShader.use();
	particleShader.setInt("tex0", 0);

	float slice_world_space_Y_coord[256];
	float slice_world_space_Y_coord_above[256];


	for (int i = 0; i < 256; i++)
	{
		slice_world_space_Y_coord[i] = 0.0f;
		slice_world_space_Y_coord_above[i] = 0.0f;
	}

	// [ 1 ]	UNIFORM BUFFER OBJECTS FOR MARCHING CUBES
	unsigned int uniformBlockMCIndex = glGetUniformBlockIndex(marchingCubesShader.ID, "g_mc_lut");
	glUniformBlockBinding(marchingCubesShader.ID, uniformBlockMCIndex, 0);

	// [ 2 ]	 CREATE THE UNIFORM BUFFER OBJECTS
	unsigned int ubog_mc_lut;
	glGenBuffers(1, &ubog_mc_lut);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferData(GL_UNIFORM_BUFFER, (256 + 12 * 6) * 16, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubog_mc_lut, 0, (256 + 12 * 6) * 16);

	// [ 3 ] FILL THE BUFFER
	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * 256, &case_to_numpolys);			// NUM TO POLYS
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, (16 * 256), 16 * 12, &cornerAmask0123);		// CORNERMASK	A0123
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, (16 * 256) + (16 * 12), 16 * 12, &cornerAmask4567);	// CORNERMASK	A4567
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, (16 * 256) + (16 * 12) * 2, 16 * 12, &cornerBmask0123);	// CORNERMASK	B0123
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, (16 * 256) + (16 * 12) * 3, 16 * 12, &cornerBmask4567);	// CORNERMASK	B0123
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, (16 * 256) + (16 * 12) * 4, 16 * 12, &vec_start);	// CORNERMASK	B0123
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut);
	glBufferSubData(GL_UNIFORM_BUFFER, (16 * 256) + (16 * 12) * 5, 16 * 12, &vec_dir);	// CORNERMASK	B0123
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glEnable(GL_TEXTURE_3D);			// enable 3D texturing


	// [ 1 ]	UNIFORM BUFFER OBJECTS FOR MARCHING CUBES
	unsigned int uniformBlockTriTableIndex = glGetUniformBlockIndex(marchingCubesShader.ID, "g_mc_lut2");
	glUniformBlockBinding(marchingCubesShader.ID, uniformBlockTriTableIndex, 1);

	// [ 2 ]	 CREATE THE UNIFORM BUFFER OBJECTS
	unsigned int ubog_mc_lut2;
	glGenBuffers(1, &ubog_mc_lut2);

	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut2);
	glBufferData(GL_UNIFORM_BUFFER, (1280 * 16), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubog_mc_lut2, 0, (1280 * 16));

	// [ 3 ] FILL THE BUFFER
	glBindBuffer(GL_UNIFORM_BUFFER, ubog_mc_lut2);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, (1280 * 16), &g_triTable);			// TRI_TABLE
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//
	// -------------------------------------------------


	// ----------------------------------
	//		 3D TEXTURE FRAMEBUFFER 
	// ----------------------------------
	//
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a color attachment 3D-texture
	unsigned int texture3D;
	glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_3D, texture3D);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, 96, 96, 256, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    // clamp on X
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    // clamp on Y
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	// glFramebufferTexture3D
	glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texture3D, 0, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	// execute victory dance
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//glDeleteFramebuffers(1, &framebuffer);

	// THE SAMPLER STATE
	// -----------------
	unsigned int sampler_state;
	glGenSamplers(1, &sampler_state);
	glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// clamp on X
	glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// clamp on Y
	glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_R, GL_REPEAT);			// wrap on Z


	glSamplerParameteri(sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	initBuffers();

	TextRenderer textRenderer = TextRenderer();
	// render loop
	// -----------

	int frameRate;
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		frameRate = 1 / deltaTime;

		// input
		// -----
		processInput(window);



		// render
		// ------
		// bind to framebuffer and draw scene as we normally would to color texture 
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// make sure we clear the framebuffers's content
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		// activate shader
		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);
		//only generate new 3dtexture when distance is moved
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, 96, 96);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);



		// -------------------------------------
		//		RENDER TO 3D TEXTURE
		// -------------------------------------
		screenShader.use();
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_3D, texture3D);

		// ------------------------------
		for (int i = 0; i < 256; i++)
		{
			screenShader.setInt("layer", i + scrollInput);
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture3D, 0, i);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		//
		//-------------------------------------
		glBindVertexArray(0);

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 96, 96);

		//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// clear all relevant buffers
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, 800, 600);

		// ---------------------------------------
		//		MARCHING CUBES RENDER
		// ---------------------------------------
		//glEnable(GL_RASTERIZER_DISCARD);
		marchingCubesShader.use();
		marchingCubesShader.setMat4("view", view);
		marchingCubesShader.setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		marchingCubesShader.setMat4("model", model);
		marchingCubesShader.setInt("yOffset", 0);
		marchingCubesShader.setVec3("viewPos", camera.Position);
		marchingCubesShader.setVec3("lightPos", lightPos);
		glBindVertexArray(dummyVAO);
		glBindSampler(texture3D, sampler_state);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, texture3D);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, heightMap);

		//glDrawArraysInstanced(GL_POINTS, 0, (96 * 96), 256);
		
		//marchingCubesShader.setInt("yOffset", -1);
		//glDrawArraysInstanced(GL_POINTS, 0, (96 * 96), 256);

		//marchingCubesShader.setInt("yOffset", -2);
		//glDrawArraysInstanced(GL_POINTS, 0, (96 * 96), 256);

		
		glBindSampler(texture3D, 0);

		//updateParticles(deltaTime);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		renderParticles(computeShader, particleShader, view, model, projection, particleSprite2, camera.Position);
		textRenderer.RenderText(textShader, std::to_string(frameRate), 25.0f, 575.0f, 0.5f, glm::vec3(1.0f, 0.71f, 0.76f));
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);


		// well i dont know
		tessShader.use();
		tessShader.setMat4("projection", projection);
		tessShader.setMat4("view", view);
		// set light uniforms
		tessShader.setVec3("viewPos", camera.Position);
		tessShader.setVec3("lightPos", lightPos);
		//shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, pingpongTextures[horizontal]);
		//renderScene(shader);
		model = glm::mat4(1.0f);
		tessShader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0, 6);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		// end of weird stuff

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		scrollInput+= 4.0f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		scrollInput-= 4.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void dummyFunction1()
{
	for (int y = 0; y < 96; y++)
	{
		for (int x = 0; x < (96 * 2); x++)
		{

			// x
			if (x % 2 == 0)
			{
				dummyVertices[x + (96 * 2 * y)] = x / 2 * 1.0f / 96.0f;
				// y

			}
			else if (x % 2 == 1)
			{
				dummyVertices[x + (96 * 2 * y)] = y * 1.0f / 96.0f;
			}
		}
	}
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// renders the 3D scene
// --------------------
void renderScene(const Shader &shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.setMat4("model", model);
	renderCube();
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}