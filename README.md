# Learning OpenGL
## LearningOpenGL

学习记录(Windows 平台(mvsc2019/mvsc2022支持))

环境设置:GLAD/GLFW + CMAKE + MVSC

## 编译
```bash
mkdir build
mkdir bin
cd build
cmake ..
```
然后执行,自动构建每个章节对应的课后作业:
```bash
YS@DESKTOP-5JP5NOA MINGW64 /f/hsbDocments/cpp_dev/LearningOpenGL/build (master)$ cmake --build .
MSBuild version 17.6.3+07e294721 for .NET Framework

  chapter_1.vcxproj -> F:\hsbDocments\cpp_dev\LearningOpenGL\bin\Debug\chapter
  _1.exe
  chapter_2.vcxproj -> F:\hsbDocments\cpp_dev\LearningOpenGL\bin\Debug\chapter
  _2.exe
  chapter_3.vcxproj -> F:\hsbDocments\cpp_dev\LearningOpenGL\bin\Debug\chapter
  _3.exe
  chapter_4_1.vcxproj -> F:\hsbDocments\cpp_dev\LearningOpenGL\bin\Debug\chapt
  er_4_1.exe
  chapter_4_2.cpp
  chapter_4_2.vcxproj -> F:\hsbDocments\cpp_dev\LearningOpenGL\bin\Debug\chapt
  er_4_2.exe
```
最后:
```bash
cp 3rdparty/3rdparty/assimp/assimp-vc143-mt.dll /bin/Debug/3rdparty/assimp-vc143-mt.dll
```

## 启动程序
由于resource目录在顶层(需在根目录下执行)：
```
bin/Debug(Release)/chapter_1.exe
```
若想直接启动，需要复制一份resource至bin目录下的对应可执行文件中

[LearningOpenGL](https://learnopengl-cn.github.io/)

[author：Joey de Vries](http://joeydevries.com/)