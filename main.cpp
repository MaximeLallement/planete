#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


#include <iostream>
#include <vector>
#include <cmath>

const char* VertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"\n"
"uniform mat4 Model;\n"
"uniform mat4 View;\n"
"uniform mat4 Projection;\n"
"\n"
"void main()\n"
"{\n"
"gl_Position =   Projection * View * Model * vec4(aPos, 1.0);\n" ///Projection * Model * View *
"}\0";


const char* FragmentShaderSourceOrange = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.9f, 0.3f, 1.0f);\n"
"}\n\0";

const char* FragmentShaderSourceYellow = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";





/*
	A chaque fois que l'utilisateur change les dimensions de sa fenêtre appele la fonction
	pour redéfinir les dimensions
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// Récupère l'input barre_espace et ferme la fenêtre si celui-ci est vrai
void processInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		glfwSetWindowShouldClose(window, true);
	}
}




int main(){

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1200, 800, "Atelier 3", NULL, NULL);
	if (window == NULL)
	{
		printf("Failed to create GLFW window");
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD");
		return -1;
	}

	glViewport(0, 0, 1200, 800);

	
	glfwSetWindowSizeCallback(window, framebuffer_size_callback);

	/*	SHADER	*/
	// Create a Vertex Shader as UnisgnedInt
	unsigned int vertexShader;
	// Specifie the shader TYPE
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Specifie the source code for this Shader
	glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
	// Compile it at run time
	glCompileShader(vertexShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Same as above for Fragment Shader
	//ORANGE
	unsigned int fragmentShaderOrange;
	fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderOrange, 1, &FragmentShaderSourceOrange, NULL);
	glCompileShader(fragmentShaderOrange);
	glGetShaderiv(fragmentShaderOrange, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderOrange, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
	}

	// YELLOW
	unsigned int fragmentShaderYellow;
	fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderYellow, 1, &FragmentShaderSourceYellow, NULL);
	glCompileShader(fragmentShaderYellow);
	glGetShaderiv(fragmentShaderYellow, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderYellow, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
	}

	// Creation of a ShaderProgram :
	// The ShaderProgram link the output of each shader to the input of the next shader
	unsigned int shaderProgramOrange;
	shaderProgramOrange = glCreateProgram();
	// I attach my shaders to my shaderProgram
	glAttachShader(shaderProgramOrange, vertexShader);
	glAttachShader(shaderProgramOrange, fragmentShaderOrange);
	// I link them to fninish the program
	glLinkProgram(shaderProgramOrange);
	// check for linking errors
	glGetProgramiv(shaderProgramOrange, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramOrange, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	
	// Once linked i can delete my shaders variable
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShaderOrange);

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	std::vector<int> lineIndices;

	int stackCount = 32;
	int sectorCount = 32;
	GLfloat radius = 10.0f;

	const float PI = acos(-1);

	/* {Artefact}
	vertices = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
	}; */
	float x, y, z, xy;                              // vertex position

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			//addVertex(x, y, z);
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

		}
	}

	/* {Artefact} 
	indices = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
	*/
	// indices
	//  k1--k1+1
	//  |  / |
	//  | /  |
	//  k2--k2+1
	unsigned int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				//addIndices(k1, k2, k1 + 1);   // k1---k2---k1+1
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1+1);
			}

			if (i != (stackCount - 1))
			{
				//addIndices(k1 + 1, k2, k2 + 1); // k1+1---k2---k2+1
				indices.push_back(k1+1);
				indices.push_back(k2);
				indices.push_back(k2+1);
			}
		}
	}


	/*	BUFFER OBJECT	*/
	// On doit créer et instancier les buffers dans cet ordre précis
	// VAO -> VBO -> VBO=>VAO
	unsigned int VAO,VBO,EBO;
	glGenVertexArrays(1 , &VAO);
	/* STORE DATA AND SET UP USING VAO & VBO */
	// VAO stores a VBO so we don't have to call it's configuration over an over
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	// VBO stores my vertices data in order to send them to the GPU
	// Sending to the GPU is slow so we want to send the maximum at each time
	// Once on the GPU datas are being accesses very fast
	// Create a buffer with it's unique id
	// Specifie the buffer TYPE
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Load data in the BUFFER
	// We call GL_ARRAY_BUFFER and OpenGL know it is VBO as we specified it
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),  &vertices[0], GL_STATIC_DRAW);
	/*	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
		GL_STATIC_DRAW: the data is set only once and used many times.
		GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	*/

	// Element array buffer
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//	LINKING VERTEX ATTRIBUTES
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 , 0);
	/*
		1 - wich vertex attributes we are configuring : we set position at location 0 in the vertexShader so it is 0
		2 - the size of the vertex attributes we set a vec3 so each vertex is composed of 3 values
		3 - the type of data in our case float
		4 - If we want to convert data
		5 - Stride = the space between consecutive vertex attributes so in how much space is the next vertex beginning
		6 -	the offset of where the position data begins in the buffer
	*/
	glEnableVertexAttribArray(0);

	
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
	


	std::cout << "verteces size : " << vertices.size() << std::endl;
	std::cout << "Indices size : " << indices.size() << std::endl;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render Loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgramOrange);

		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Projection = glm::mat4(1.0f);

		GLint model = glGetUniformLocation(shaderProgramOrange, "Model");
		Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(Model));

		GLint view = glGetUniformLocation(shaderProgramOrange, "View");
		View = glm::translate(View, glm::vec3(0.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(View));

		GLint projection = glGetUniformLocation(shaderProgramOrange, "Projection");
		Projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(Projection));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size() , GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, indices.data());


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers; (1, &EBO);
	glDeleteProgram(shaderProgramOrange);

	glfwTerminate();
	return 0;
}
