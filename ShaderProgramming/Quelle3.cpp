//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include "stb_image.h"
//
//#include <glm/glm/glm.hpp>
//#include <glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>
//
//
//#include <iostream>
//
//#include "Camera.h"
//#include "Shader.h"
//#include "stb_image.h"
//
//
////PFNGLTEXIMAGE3DPROC glTexImage3D;
//
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow *window);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;	// time between current frame and last frame
//float lastFrame = 0.0f;
//
//
//
//// vertices points 96 x 96
//float dummyVertices[96 * 96 * 2];
//void dummyFunction();
//
//
//
//int main()
//{
//	// glfw: initialize and configure
//	// ------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
//#endif
//
//	// glfw window creation
//	// --------------------
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	// tell GLFW to capture our mouse
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	// glad: load all OpenGL function pointers
//	// ---------------------------------------
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return -1;
//	}
//
//	// configure global opengl state
//	// -----------------------------
//	glEnable(GL_DEPTH_TEST);
//
//	// build and compile our shader zprogram
//	// ------------------------------------
//	Shader ourShader("vertex_shader.vs", "fragment_shader.fs");
//	ourShader.linkProgram();
//	Shader screenShader("5.1.framebuffers_screen.vs", "5.1.framebuffers_screen.fs", "geometry_shader.gsh");
//	screenShader.linkProgram();
//
//
//	Shader testShader("vertTest.vert", "fragmentTest.frag", "geomTest.geom");
//	// THE TRANSFORM FEEDBACK
//	const char* feedbackVaryings[] = { "outValue" };
//	glTransformFeedbackVaryings(testShader.ID, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
//
//	testShader.linkProgram();
//
//
//	// set up vertex data (and buffer(s)) and configure vertex attributes
//	// ------------------------------------------------------------------
//	float vertices[] = {
//		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//
//		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//
//		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//
//		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//	};
//	// world space positions of our cubes
//	glm::vec3 cubePositions[] = {
//		glm::vec3(0.0f,  0.0f,  0.0f),
//		glm::vec3(2.0f,  5.0f, -15.0f),
//		glm::vec3(-1.5f, -2.2f, -2.5f),
//		glm::vec3(-3.8f, -2.0f, -12.3f),
//		glm::vec3(2.4f, -0.4f, -3.5f),
//		glm::vec3(-1.7f,  3.0f, -7.5f),
//		glm::vec3(1.3f, -2.0f, -2.5f),
//		glm::vec3(1.5f,  2.0f, -2.5f),
//		glm::vec3(1.5f,  0.2f, -1.5f),
//		glm::vec3(-1.3f,  1.0f, -1.5f)
//	};
//	unsigned int VBO, VAO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	// texture coord attribute
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//
//
//	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
//   // positions   // texCoords
//   -1.0f,  1.0f,  0.0f, 1.0f,
//   -1.0f, -1.0f,  0.0f, 0.0f,
//	1.0f, -1.0f,  1.0f, 0.0f,
//
//   -1.0f,  1.0f,  0.0f, 1.0f,
//	1.0f, -1.0f,  1.0f, 0.0f,
//	1.0f,  1.0f,  1.0f, 1.0f
//	};
//	// screen quad VAO
//	unsigned int quadVAO, quadVBO;
//	glGenVertexArrays(1, &quadVAO);
//	glGenBuffers(1, &quadVBO);
//	glBindVertexArray(quadVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//
//
//
//	// set the vertices for the marching cubes
//	dummyFunction();
//
//
//
//
//	struct Point {
//		float x;
//		float y;
//		float u;
//		float v;
//	};
//	//Point test = Point();
//	//float dummyVertices[48 * 48 * 2];
//
//
//	float testVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
//// positions   // texCoords
//   -0.8f,  0.8f,
//   -0.8f, -0.8f,
//	0.8f, -0.8f,
//	0.8f,  0.8f,
//	};
//	// STEP 3 
//	// ------
//		// Create input VBO and vertex format
//	float data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
//
//
//	// draw a "dummy" vertex buffer
//	unsigned int dummyVAO, dummyVBO;
//	glGenVertexArrays(1, &dummyVAO);
//	glGenBuffers(1, &dummyVBO);
//	glBindVertexArray(dummyVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, dummyVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(dummyVertices), &dummyVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//
//	//glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//
//
//
//
//	// THE BUFFER TO HOLD THE TRANSFORM FEEDBACK
//	unsigned int transformVBO;
//	glGenBuffers(1, &transformVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, transformVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(dummyVertices), nullptr, GL_STATIC_READ);
//
//
//	// of 96 x 96 points
//	// uv coords in range [0 - 1]
//
//
//
//
//	// load and create a texture 
//	// -------------------------
//	unsigned int texture1, texture2;
//	testShader.use();
//	testShader.setInt("screenTexture", 0);
//
//	screenShader.use();
//	screenShader.setInt("screenTexture", 0);
//
//
//
//	glEnable(GL_TEXTURE_3D);			// enable 3D texturing
//
//
//
//	/*
//	GLuint gArrayTexture(0);
//
//	glGenTextures(1, &gArrayTexture);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D_ARRAY, gArrayTexture);
//
//	//Create storage for the texture. (100 layers of 1x1 texels)
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//
//	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
//		1,                    //No mipmaps as textures are 1x1
//		GL_RGBA,              //Internal format
//		4, 4,                 //width,height
//		100                   //Number of layers
//	);
//	*/
//
//
//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
//	//float actualData[2 * 2 * 2];
//	//for (int i = 0; i < (2 * 2 * 2); i++)
//	//{
//	//	actualData[i] = 0.5f;
//	//}
//
//	//float* testData = actualData;
//	//// testing
//	////
//	//// Create one OpenGL texture
//
//	//unsigned int texture3;
//
//	//int widthTest, heightTest, depthTest;
//	//widthTest = 2;
//	//heightTest = 2;
//	//depthTest = 2;
//
//	////glEnable(GL_TEXTURE_2D_ARRAY);
//
//	////depth = 256;
//	////width = 96;
//	////height = 96;
//	//glGenTextures(1, &texture3);
//	////unsigned int textureID;
//
//	//// "Bind" the newly created texture : all future texture functions will modify this texture
//	//glBindTexture(GL_TEXTURE_3D, texture3);
//
//	////glBindTexture(GL_TEXTURE_2D_ARRAY, texture3);
//
//	////glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	////glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//
//	////glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R16F, 3, 3, 3);
//
//	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
//
//	//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R16F, widthTest, heightTest, depthTest);
//
//
//	//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, widthTest, heightTest, depthTest, GL_R16F, GL_FLOAT, testData);
//	////glPixelStorei(GL_UNPACK_ROW_LENGTH, widthTest);
//	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	//glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, widthTest, heightTest, depthTest, 0, GL_RED, GL_FLOAT, NULL);
//	// -------------------------------------
//	//glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, widthTest, heightTest, depthTest, 0, GL_RED, GL_FLOAT, testData);
//	// -------------------------------------
//
//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
//
//
//	// THE SAMPLER OBJECT
//	// clamps on XY, wraps on Z.
//
//	//glSamplerParameterf(sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
//
//
//
//
//
//
//
//
//
//
//
//	//glGenerateMipmap(GL_TEXTURE_3D);
//
//	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//
//
//	// LOAD DATA
//	//glGetTexImage(GL_TEXTURE_3D, 0, GL_R16F, GL_HALF_FLOAT,)
//
//	//(generate texel code omitted)
//	/*unsigned int texname;
//	glGenTextures(1, &texname);
//	glBindTexture(GL_TEXTURE_3D, texname);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
//	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, WIDTH, HEIGHT, DEPTH, 0, GL_RGB,
//		GL_UNSIGNED_BYTE, texels);
//*/
//// Give the image to OpenGL
//
////glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
////glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, 0, GL_R16F, GL_UNSIGNED_BYTE, 0,);
//
///*
//
//glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, depth, 0, GL_R16F, GL_HALF_FLOAT, 0);
//
//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, 1, GL_R16F, GL_HALF_FLOAT, data);
//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 1, width, height, 1, GL_R16F, GL_HALF_FLOAT, data);
//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 2, width, height, 1, GL_R16F, GL_HALF_FLOAT, data);
//
//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 512, 512, 3, 0, GL_BGR, GL_UNSIGNED_BYTE, 0);
//
//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, Z, 512, 512, 1, GL_BGR, GL_UNSIGNED_BYTE, imageData);
//
//	GLuint textureID;
//glGenTextures(1, &textureID);
//
//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//*/
//
////
//// 96 x 96 x 256
//
//
//
//	// TUTORIAL
//	// --------
//	// framebuffer configuration
//	// -------------------------
//	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//	unsigned int framebuffer;
//	glGenFramebuffers(1, &framebuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//
//	// create a color attachment 3D-texture
//	unsigned int texture3D;
//	glGenTextures(1, &texture3D);
//	glBindTexture(GL_TEXTURE_3D, texture3D);
//
//	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, 96, 96, 4, 0, GL_RED, GL_FLOAT, NULL);
//	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, 800, 600, 0, GL_RED, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
//
//	// glFramebufferTexture3D
//	glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texture3D, 0, 0);
//
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
//	// execute victory dance
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//	//glDeleteFramebuffers(1, &framebuffer);
//
//
//	unsigned int sampler_state;
//	glGenSamplers(1, &sampler_state);
//	glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// clamp on X
//	glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// clamp on Y
//	glSamplerParameteri(sampler_state, GL_TEXTURE_WRAP_R, GL_REPEAT);			// wrap on Z
//
//
//	glSamplerParameteri(sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glSamplerParameteri(sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//
//
//
//
//
//
//
//	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
//	// -------------------------------------------------------------------------------------------
//	//ourShader.use();
//	//ourShader.setInt("texture1", 0);
//	//ourShader.setInt("texture2", 1);
//	//ourShader.setInt("texture3", 2);
//
//
//	// render loop
//	// -----------
//	while (!glfwWindowShouldClose(window))
//	{
//		// per-frame time logic
//		// --------------------
//		float currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		// input
//		// -----
//		processInput(window);
//
//
//
//		// render
//		// ------
//		// bind to framebuffer and draw scene as we normally would to color texture 
//		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//
//
//		glViewport(0, 0, 96, 96);
//		// make sure we clear the framebuffers's content
//		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// bind textures on corresponding texture units
//		//glActiveTexture(GL_TEXTURE0);
//		//glBindTexture(GL_TEXTURE_2D, texture1);
//
//		//glActiveTexture(GL_TEXTURE1);
//		//glBindTexture(GL_TEXTURE_2D, texture2);
//
//		//glActiveTexture(GL_TEXTURE2);
//		//glBindTexture(GL_TEXTURE_3D, texture3);
//		glDisable(GL_DEPTH_TEST);
//		// activate shader
//		ourShader.use();
//
//		// pass projection matrix to shader (note that in this case it could change every frame)
//		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		ourShader.setMat4("projection", projection);
//
//		// camera/view transformation
//		glm::mat4 view = camera.GetViewMatrix();
//		ourShader.setMat4("view", view);
//
//		// render boxes
//		glBindVertexArray(VAO);
//		for (unsigned int i = 0; i < 10; i++)
//		{
//			// calculate the model matrix for each object and pass it to shader before drawing
//			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//			model = glm::translate(model, cubePositions[i]);
//			float angle = 20.0f * i;
//			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//			ourShader.setMat4("model", model);
//
//			//glDrawArrays(GL_TRIANGLES, 0, 36);
//		}
//
//		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//
//		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		screenShader.use();
//		glBindVertexArray(quadVAO);
//		glBindTexture(GL_TEXTURE_3D, texture3D);
//
//
//
//		// ------------------------------
//		for (int i = 0; i < 4; i++)
//		{
//			screenShader.setInt("layer", i);
//			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture3D, 0, i);
//			glDrawArrays(GL_TRIANGLES, 0, 6);
//		}
//		//-----------------------------
//
//
//
//
//
//
//
//
//
//		glBindVertexArray(0);
//
//		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		glViewport(0, 0, 800, 600);
//
//		//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
//		// clear all relevant buffers
//		glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		testShader.use();
//		glBindVertexArray(dummyVAO);
//
//		glBindSampler(texture3D, sampler_state);
//
//		glDrawArraysInstanced(GL_POINTS, 0, (96 * 96), 4);
//
//		glBindSampler(texture3D, 0);
//
//		//glDrawArrays(GL_POINTS, 0, (96*96*2));
//
//
//		// Perform feedback transform
//		// ------------------------------
//		//glEnable(GL_RASTERIZER_DISCARD);
//
//		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, transformVBO);
//
//		glBeginTransformFeedback(GL_POINTS);
//		glDrawArrays(GL_POINTS, 0, 5);
//		glEndTransformFeedback();
//
//		glDisable(GL_RASTERIZER_DISCARD);
//
//		glFlush();
//		// ------------------------------------
//		// Fetch and print results
//		GLfloat feedback[5];
//		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
//
//		printf("%f %f %f %f %f\n", feedback[0], feedback[1], feedback[2], feedback[3], feedback[4]);
//
//
//
//
//		// STEP 3
//		// --------
//
//		// DRAW Dummy vertex buffer of 96x96 points
//
//		// pixel shader disabled
//		// vertex shader: samples densities a
//		// geometry shader
//		// vertex buffer
//
//		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//		// -------------------------------------------------------------------------------
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	// optional: de-allocate all resources once they've outlived their purpose:
//	// ------------------------------------------------------------------------
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//
//	// glfw: terminate, clearing all previously allocated GLFW resources.
//	// ------------------------------------------------------------------
//	glfwTerminate();
//	return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow *window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}
//
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	camera.ProcessMouseScroll(yoffset);
//}
//
//void dummyFunction()
//{
//	for (int y = 0; y < 48; y++)
//	{
//		//dummyVertices[y] = y;
//		//dummyVertices[y + 1] = y;
//
//		//for(int x = 0; x < 192; x++)
//		//	dummyVertices[(y * 192) + x] = x;
//
//		for (int x = 0; x < 96; x++)
//		{
//
//			// x
//			if (x % 2 == 0)
//			{
//				dummyVertices[x + (96 * y)] = -x / 2 * 1.0f / 48.0f;
//				// y
//
//			}
//			else if (x % 2 == 1)
//			{
//				dummyVertices[x + (96 * y)] = y * 1.0f / 48.0f;
//			}
//		}
//	}
//
//	for (int y = 0; y < 48; y++)
//	{
//		//dummyVertices[y] = y;
//		//dummyVertices[y + 1] = y;
//
//		//for(int x = 0; x < 192; x++)
//		//	dummyVertices[(y * 192) + x] = x;
//
//		for (int x = 1; x < 96; x++)
//		{
//
//			// x
//			if (x % 2 == 0)
//			{
//				dummyVertices[x + (96 * y) + (48 * 96)] = x / 2 * 1.0f / 48.0f;
//				// y
//
//			}
//			else if (x % 2 == 1)
//			{
//				dummyVertices[x + (96 * y) + (48 * 96)] = y * 1.0f / 48.0f;
//			}
//		}
//	}
//
//	for (int y = 1; y < 48; y++)
//	{
//		//dummyVertices[y] = y;
//		//dummyVertices[y + 1] = y;
//
//		//for(int x = 0; x < 192; x++)
//		//	dummyVertices[(y * 192) + x] = x;
//
//		for (int x = 0; x < 96; x++)
//		{
//
//			// x
//			if (x % 2 == 0)
//			{
//				dummyVertices[x + (96 * y) + (48 * 96 * 2)] = -x / 2 * 1.0f / 48.0f;
//				// y
//
//			}
//			else if (x % 2 == 1)
//			{
//				dummyVertices[x + (96 * y) + (48 * 96 * 2)] = -y * 1.0f / 48.0f;
//			}
//		}
//	}
//
//	for (int y = 1; y < 48; y++)
//	{
//		//dummyVertices[y] = y;
//		//dummyVertices[y + 1] = y;
//
//		//for(int x = 0; x < 192; x++)
//		//	dummyVertices[(y * 192) + x] = x;
//
//		for (int x = 1; x < 96; x++)
//		{
//
//			// x
//			if (x % 2 == 0)
//			{
//				dummyVertices[x + (96 * y) + (48 * 96 * 3)] = x / 2 * 1.0f / 48.0f;
//				// y
//
//			}
//			else if (x % 2 == 1)
//			{
//				dummyVertices[x + (96 * y) + (48 * 96 * 3)] = -y * 1.0f / 48.0f;
//			}
//		}
//	}
//}