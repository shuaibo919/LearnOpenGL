## 1.Basic Content
### 1.1 缓冲的概念
前缓冲区:屏幕上正在显示的图像

后缓冲区:正在渲染的图像
### 1.2 OPENGL
OpenGL是一组规范，他本身是一个巨大的状态机，我们通过改变的他状态来完成图形绘制

### 1.3 GLAD
一个扩展加载库，用来为我们加载并设定所有OpenGL函数指针，从而让我们能够使用所有（现代）OpenGL函数。

### 1.4 流程

Vertex Shader -> Shape Assembly -> Geometry Shader -> Rasterization -> Fragment Shader -> Blending


视口(Viewport)： 我们需要渲染的窗口。

图形管线(Graphics Pipeline)： 一个顶点在呈现为像素之前经过的全部过程。

### 1.5 VAO/VBO/EBO

顶点缓冲对象(Vertex Buffer Object)： 一个调用显存并存储所有顶点数据供显卡使用的缓冲对象。

顶点数组对象(Vertex Array Object)： 存储缓冲区和顶点属性状态。

元素缓冲对象(Element Buffer Object，EBO)，也叫索引缓冲对象(Index Buffer Object，IBO)： 一个存储元素索引供索引化绘制使用的缓冲对象。

|      macro      |        type        |
| :-------------: | :----------------: |
| GL_STATIC_DRAW  |    适用静态数据    |
| GL_STREAM_DRAW  |    适用流式数据    |
| GL_DYNAMIC_DRAW | 适用频繁改变的数据 |

```cpp
    ...
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    // VBO
    // 顶点缓冲对象(vertex buffer object)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    ...
```

### 1.6 GLSL(OpenGL Shading Language)

着色器(Shader)： 一个运行在显卡上的小型程序。很多阶段的图形管道都可以使用自定义的着色器来代替原有的功能。

Uniform： 一个特殊类型的GLSL变量。它是全局的（在一个着色器程序中每一个着色器都能够访问uniform变量），并且只需要被设定一次。

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
```
下面的代码用于创建顶点着色器
```cpp
    // 创建顶点着色器
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
```
下面的代码用于创建片段着色器

```cpp
    // 创建片段着色器
    const char *fragmentSahderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentSahderSource,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
```


### 1.7 纹理(Texture)
一种包裹着物体的特殊类型图像，给物体精细的视觉效果。

纹理环绕(Texture Wrapping)： 定义了一种当纹理顶点超出范围(0, 1)时指定OpenGL如何采样纹理的模式。

纹理过滤(Texture Filtering)： 定义了一种当有多种纹素选择时指定OpenGL如何采样纹理的模式。这通常在纹理被放大情况下发生。

多级渐远纹理(Mipmaps)： 被存储的材质的一些缩小版本，根据距观察者的距离会使用材质的合适大小。

纹理单元(Texture Units)： 通过绑定纹理到不同纹理单元从而允许多个纹理在同一对象上渲染。

### 1.8 空间
局部空间(Local Space)： 一个物体的初始空间。所有的坐标都是相对于物体的原点的。

世界空间(World Space)： 所有的坐标都相对于全局原点。

观察空间(View Space)： 所有的坐标都是从摄像机的视角观察的。

裁剪空间(Clip Space)： 所有的坐标都是从摄像机视角观察的，但是该空间应用了投影。这个空间应该是一个顶点坐标最终的空间，作为顶点着色器的输出。OpenGL负责处理剩下的事情（裁剪/透视除法）。

屏幕空间(Screen Space)： 所有的坐标都由屏幕视角来观察。坐标的范围是从0到屏幕的宽/高。
LookAt矩阵： 一种特殊类型的观察矩阵，它创建了一个坐标系，其中所有坐标都根据从一个位置正在观察目标的用户旋转或者平移。

欧拉角(Euler Angles)： 被定义为偏航角(Yaw)，俯仰角(Pitch)，和滚转角(Roll)从而允许我们通过这三个值构造任何3D方向。