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

Shader loadShader(Shader& shader, std::istream& is) {
	std::stringstream source;
	std::string word;
	is >> std::noskipws;
	while (is >> word) {
		if(word == "#version") {
			if(shader.version != "") { is >> shader.version; }
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // skip line
		}
		else {
			if (word.rfind("glClipDistance") == 0) {
				auto left = word.find_first_of("[");
				auto right = word.find_last_of("]", left);
				int index = std::stoi(word.substr(left+1, right-1));
				source << word;
				is >> word;
				if (word == "=") { index += 1; } // implicit declaration of glClipDistance size
				shader.ClipDistanceCount = std::max(shader.ClipDistanceCount, index);
			}
			source << word;
		}
	}
	shader.source = source.str();
	return shader;
}

Shader parse_args(int argc, char** argv) {
	std::string shader_filename;
	Shader shader;
	for (int i = 0; i < argc; ++i) {
		if (argv[i] == "--version" && i < argc - 1) {
			++i;
			shader.version = argv[i];
		}
		else {
			shader_filename = argv[i];
		}
	}
	std::ifstream shader_stream(shader_filename);
	return loadShader(shader, shader_stream);
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
	Shader vertShader(parse_args(argc, argv));
	if(vertShader.version == "") {
		vertShader.version = "460";
	}
	std::cerr << "Will use GLSL version " << vertShader.version << std::endl;
	if (vertShader.ClipDistanceCount > 0) {

	}
}