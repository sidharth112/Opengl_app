#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <print>
#include <fstream>
#include <string>
#include <sstream>

//OpenGL ERROR Logging
#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall());
#define GLCallCheck(x) GLClearError();\
    x;\
    GLCheckError(__FUNCTION__, __FILE__, __LINE__);\


static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::println("OpenGL Error: {} in {} at {}:{}", error, function, file, line);
	}
}

static bool GLLogCall()
{
    while (GLenum error = glGetError())
    {
		std::println("OpenGL Error: {}", error);
		return false;
    }
    return true;
}
//End of OpenGL Error Logging
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    enum class ShaderType { None = -1, Vertex = 0, Fragment = 1 };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::None;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::Vertex;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::Fragment;
        }
        else if (type != ShaderType::None)
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		std::string message(length, ' ');
		glGetShaderInfoLog(id, length, &length, message.data());
		std::println("Failed to compile {} shader: {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", message);
		glDeleteShader(id);
		return 0;
	}
	return id;

}
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
	//unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    //unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello Sidharth", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK)
    {
        std::println("Error");
    }

    std::println("The present GL Version is {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
#if 0
    float positions[6] = {
    -0.5f , -0.5f,
     0.0f ,  0.5f,
     0.5f , -0.5f 
    };
#endif

    float positions[] = {
    -0.5f , -0.5f,
     0.5f , -0.5f,
     0.5f ,  0.5f,
	-0.5f ,  0.5f
	};

	unsigned int indices[] = {
		0, 1, 2, // First Triangle
		2, 3, 0  // Second Triangle
	};
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(indices), indices, GL_STATIC_DRAW);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    

#if 0
    std::string vertexShader = "#version 330 core\n"
        "layout(location = 0) in vec4 position;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = position;\n"
        "}\n";
    std::string fragmentShader = "#version 330 core\n"
        "layout(location = 0) out vec4 color;\n"
        "void main()\n"
        "{\n"
        "    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
#endif
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f);
    std::println("....Compiling shaders...");
	std::println("Vertex Shader:\n{}", source.VertexSource);
	std::println("Fragment Shader:\n{}", source.FragmentSource);
	


    //unsigned int shader = CreateShader(vertexShader, fragmentShader);
	//glUseProgram(shader);

    //Unbound everything
    glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float r = 0.0f;
	float g = 0.3f;
	float b = 0.8f;
	float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
		
        //GLClearError();

        glUseProgram(shader);
        glUniform4f(location, r, g, b, 1.0f);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);



        
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        //ASSERT(GLLogCall());
		//GLCheckError(__FUNCTION__, __FILE__, __LINE__);
        if (r > 1.0f)
        {
			increment = -0.05f;
		}
		else if (r < 0.0f)
		{
			increment = 0.05f;
        }
		r += increment;
		
#if 0
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f , -0.5f);
        glVertex2f( 0.0f ,  0.5f);
        glVertex2f( 0.5f , -0.5f);
        glEnd();
#endif
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

	glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
