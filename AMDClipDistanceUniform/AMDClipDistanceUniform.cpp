#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sstream>
#include <algorithm>
#include <fstream>

struct Shader {
	std::string version;
	int ClipDistanceCount = 0;
	std::string source;

	std::string versioned_source() {
		std::stringstream prepared_source;
		prepared_source << "#version " << version << std::endl;
		prepared_source << source;
		return prepared_source.str();
	}
};

Shader parse_args(int argc, char** argv) {
	std::string shader_filename;
	Shader shader;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--version") == 0 && i < argc - 1) {
			++i;
			shader.version = argv[i];
		}
		else if (strcmp(argv[i], "--clip-distance-count") == 0 && i < (argc - 1)) {
			++i;
			shader.ClipDistanceCount = std::stoi(argv[i]);
		} else {
			shader_filename = argv[i];
		}
	}
	std::ifstream shader_stream(shader_filename);
	std::ostringstream contents;
	contents << shader_stream.rdbuf();
	shader.source = contents.str();
	return shader;
}

void compileShader(GLuint prog, std::string const source) {
	const char *source_ptr = source.c_str();
	int source_size = (int)source.length();
	glShaderSource(prog, 1, &source_ptr, &source_size);
	glCompileShader(prog);
}

void print_active_shaders(GLuint prog) {
	GLsizei uniform_count;
	glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &uniform_count);

	GLsizei max_uniform_name;
	glGetProgramiv(prog, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name);

	auto uniform_name = std::make_unique<GLchar[]>(max_uniform_name);
	GLsizei uniform_size;
	GLenum uniform_type;

	for (GLint i = 0; i < uniform_count; ++i) {
		glGetActiveUniform(prog, i, max_uniform_name, NULL, &uniform_size, &uniform_type, uniform_name.get());
		GLuint location = glGetUniformLocation(prog, uniform_name.get());
		std::cout << "found " << uniform_name.get() << " at " << location << std::endl;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutCreateWindow("I never get displayed");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}
	std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	Shader vert_shader(parse_args(argc, argv));
	if(vert_shader.version == "") {
		vert_shader.version = "460";
	}
	std::cerr << "Will use GLSL version " << vert_shader.version << std::endl;
	if (vert_shader.ClipDistanceCount > 0) {
		std::cerr << "Enabling " << vert_shader.ClipDistanceCount << " clip distance plane" << (vert_shader.ClipDistanceCount > 1 ? "s" : "") << std::endl;
		for (int i = 0; i < vert_shader.ClipDistanceCount; i++) {
			glEnable(GL_CLIP_DISTANCE0 + i);
		}
	}

	GLuint vert_prog = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vert_prog, vert_shader.versioned_source());

	GLuint prog = glCreateProgram();

	glAttachShader(prog, vert_prog);

	glLinkProgram(prog);
	glUseProgram(prog);
	static char buffer[10000];
	std::stringstream error;
	glGetShaderInfoLog(vert_prog, 10000, NULL, buffer);
	if(*buffer)
	{
		error << buffer;
	}

	glGetProgramInfoLog(prog, 10000, NULL, buffer);
	if(*buffer)
	{
		error << buffer;
	}
	int linked;
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if(linked == 0)
	{
		// only print message if there's an error.
		std::cerr << "Shader init failed!" << std::endl << error.str() << std::endl;
	}
	print_active_shaders(prog);
}