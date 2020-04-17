#include<GL/glew.h>
#include<iostream>
#include<GLFW/glfw3.h>

//A LOOOOT of this I'll remove. It's only a template right now. Once you set it up and run it you should see a blue rectangle get displayed. That would mean the set-up configured properly.
//To push changes on the bottom right you'll see a tab that says master. CLick Pull before you begin  to work and Push, once you're done with your commit. If some code doesn't work then Push it to a *DIFFERENT BRANCH* and describe error in commit. Once we get rid of error I'll merge the branch and handle that  shit.

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error Handeling stuff

    int res;
    glGetShaderiv(id, GL_COMPILE_STATUS, &res);//returns unsigned int or GLuint telling you whether compile worked or not

    if (res == GL_FALSE)
    {
        //get the length of the log because that's a thing
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);

        //get the message
        char* msg = (char*)alloca(len * sizeof(char));//Allocate right on stack how much space "msg" will take. Otherwise you'll have to do it on the heap and then free up that space once you leave this code area. (Need to figure out how you do that)

        glGetShaderInfoLog(id, len, &len, msg);//This retireves the infolog. OpenGL always making you take your hand around your neck to feed you someting oh gaaaaaawd

        std::cout << "stuff iz not wrk :( at: " << std::endl << (type == GL_VERTEX_SHADER ? "Vetex" : "fragment") << std::endl; //This is so I know what shader caused the error.
        //I will be stumped if it's not one of these two, so I can't just copy-paste this function elsewhere to check. The other option would be to add an If-Else and check for each Shader I've used.

        std::cout << msg << std::endl; //finally we print the fucking message

        glDeleteShader(id);//given how it didn't work you remove it.
        return 0; //I don't know what specifying -1 would do instead so 0 it is.

    }

    return id;
}

static unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader)
{
    unsigned int prog = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
    glAttachShader(prog, vs);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
    glAttachShader(prog, fs);

    glLinkProgram(prog);
    glValidateProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hellz yea bbbbyyy", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error with the way Glew got set-up";
    }//is to verify glew is linked statically and functioning while giving me access to all glew functions I'd need
    std::cout << glGetString(GL_VERSION) << std::endl;//gives me the version of OpenGL I am running on


    float pos[8] =
    {
        0.5f,  0.5f,
        0.5f,  -0.5f,
        -0.5f, -0.5f,
        -0.5f, 0.5f
    };


    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    //vsa will be the vertex shader and fsa the fragment shader. You should be using file to get them in but this default shit should help in debugging and initial tests
    std::string vsa =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  gl_Position = position;\n"
        "}\n";


    std::string fsa =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "  color = vec4(0.0,0.75,0.7,1.0);\n"
        "}\n";


    unsigned int shader = CreateShader(vsa, fsa);//compile shader stuff and link it.
    glUseProgram(shader);//here we be bindin shaders

        /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_QUADS, 0, 4);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);//deleting the shader we used to clean the code

    glfwTerminate();
    return 0;
}