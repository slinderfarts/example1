#include "../../Header/OpenglStuff/buffers.h"
#include "../../Header/OpenglStuff/mesh.h"
#include "../../Header/globarVar.h"
#include "../../Header/OpenglStuff/shapes.h"

#include <random>

template<>
void initVertex<vertex_t::triangle>() {
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &triangleEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

template<>
void initVertex<vertex_t::square>() {
	glGenVertexArrays(1, &squareVAO);
	glBindVertexArray(squareVAO);

	glGenBuffers(1, &squareVBO);
	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &squareEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

template<>
void initVertex<vertex_t::cube>() {
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	glGenBuffers(1, &cubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesCube), indicesCube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(6 * sizeof(float)));
	// offset is in index 3 4 5 6
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(8 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(7);

	glBindVertexArray(0);
}

void deleteVertexes() {
	if (triangleVAO != 0) {
		glDeleteVertexArrays(1, &triangleVAO);
		glDeleteBuffers(1, &triangleVBO);
		glDeleteBuffers(1, &triangleEBO);
	}
	if (squareVAO != 0) {
		glDeleteVertexArrays(1, &squareVAO);
		glDeleteBuffers(1, &squareVBO);
		glDeleteBuffers(1, &squareEBO);
	}
	if (cubeVAO != 0) {
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteBuffers(1, &cubeVBO);
		glDeleteBuffers(1, &cubeEBO);
	}
}

// FBO stuff

FBO::FBO(ENUM_FBO_DEPTH_STENCIL_ATT att, ENUM_FBO_TEX_RBO bufferT) {
	setFBOshadersUnilocID(&programBloom, programType::bloomRender);

	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

	auto createdatapixels = [&](unsigned int& datapixel, int i=0) {
		glGenTextures(1, &datapixel);
		glBindTexture(GL_TEXTURE_2D, datapixel);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
		//std::cout << "FBO Allocation Size: " << w << "x" << h << std::endl;
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D,
			datapixel, 0
		);
	};

	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	createdatapixels(DataPixelsID); 
	createdatapixels(brightPixels, 1);
	
	createdatapixels(textures_data.position_ID, 2);
	createdatapixels(textures_data.normal_ID, 3);



	glGenFramebuffers(2, BloomDataFBID);
	glBindFramebuffer(GL_FRAMEBUFFER, BloomDataFBID[0]);
	createdatapixels(BloomDataPixelsID[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, BloomDataFBID[1]);
	createdatapixels(BloomDataPixelsID[1]);

	//glGenFramebuffers(1, &gBufferFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
	//for (int i = 0; i < gBufferAmount; i++) {
	//	createdatapixels(gBufferFBO, i);
	//}
	// do blit on those multisampled-gbuffer to this texgbuffer
	//std::cout << "FBO Allocation Size: " << w << "x" << h << std::endl;

	initSSAO();

	if (bufferT == ENUM_FBO_TEX_RBO::RENDERBUFFER) {
		rbo = new RBO();

		info = ENUM_FBO_TEX_RBO::RENDERBUFFER;

		switch (att) {
		case ENUM_FBO_DEPTH_STENCIL_ATT::DEPTH_STENCIL: {
			format.internalFormat = GL_DEPTH24_STENCIL8;
			format.format = GL_DEPTH_STENCIL;
			format.attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			format.type = GL_UNSIGNED_INT_24_8;
			
			rbo->setDepthStencil(ID);
			break;
		}
		case ENUM_FBO_DEPTH_STENCIL_ATT::DEPTH: {
			format.internalFormat = GL_DEPTH_COMPONENT24;
			format.format = GL_DEPTH_COMPONENT;
			format.attachment = GL_DEPTH_ATTACHMENT;
			format.type = GL_UNSIGNED_INT;

			rbo->setDepth(ID);
			break;
		}
		case ENUM_FBO_DEPTH_STENCIL_ATT::STENCIL: {
			format.internalFormat = GL_STENCIL_INDEX8;
			format.format = GL_STENCIL_INDEX;
			format.attachment = GL_STENCIL_ATTACHMENT;
			format.type = GL_UNSIGNED_BYTE;

			rbo->setStencil(ID);
			break;
		}
		}
	}
	else if (bufferT == ENUM_FBO_TEX_RBO::TEXTURE) {
		info = ENUM_FBO_TEX_RBO::TEXTURE;

		switch (att) {
		case ENUM_FBO_DEPTH_STENCIL_ATT::DEPTH: {
			glGenTextures(1, &DepthOrAndStencilTexID);
			glBindTexture(GL_TEXTURE_2D, DepthOrAndStencilTexID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			int w, h;
			glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h,
				0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr
			);

			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, DepthOrAndStencilTexID, 0
			);

			format.internalFormat = GL_DEPTH_COMPONENT24;
			format.format = GL_DEPTH_COMPONENT;
			format.attachment = GL_DEPTH_ATTACHMENT;
			format.type = GL_UNSIGNED_INT;

			glBindTexture(GL_TEXTURE_2D, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "FRAMEBUFFER INCOMPLETE -------- ERROR" << std::endl;
			}

			break;
		}
		case ENUM_FBO_DEPTH_STENCIL_ATT::STENCIL: {
			glGenTextures(1, &DepthOrAndStencilTexID);
			glBindTexture(GL_TEXTURE_2D, DepthOrAndStencilTexID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			int w, h;
			glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, w, h, 0,
				GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, nullptr
			);

			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D, DepthOrAndStencilTexID, 0
			);

			format.internalFormat = GL_STENCIL_INDEX8;
			format.format = GL_STENCIL_INDEX8;
			format.attachment = GL_STENCIL_INDEX;
			format.type = GL_UNSIGNED_INT;

			glBindTexture(GL_TEXTURE_2D, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "FRAMEBUFFER INCOMPLETE -------- ERROR" << std::endl;
			}

			break;
		}
		case ENUM_FBO_DEPTH_STENCIL_ATT::DEPTH_STENCIL: {
			glGenTextures(1, &DepthOrAndStencilTexID);
			glBindTexture(GL_TEXTURE_2D, DepthOrAndStencilTexID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			int w, h;
			glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0,
				GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr
			);

			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
				GL_TEXTURE_2D, DepthOrAndStencilTexID, 0
			);

			format.internalFormat = GL_DEPTH24_STENCIL8;
			format.format = GL_DEPTH_STENCIL;
			format.attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			format.type = GL_UNSIGNED_INT_24_8;

			glBindTexture(GL_TEXTURE_2D, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "FRAMEBUFFER INCOMPLETE -------- ERROR" << std::endl;
			}

			break;
		}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);  //unbinds fbo
}

void FBO::initMultisampleFBO(ENUM_FBO_DEPTH_STENCIL_ATT att) { // fixed use renderbuffer
	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
	
	glGenFramebuffers(1, &multisampleID);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampleID);

	glGenTextures(1, &DataPixelMultiSamples);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, DataPixelMultiSamples);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, w, h, GL_TRUE);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, // + totalTexGBuffer, 
		GL_TEXTURE_2D_MULTISAMPLE, DataPixelMultiSamples, 0
	);

	//const int layer = 7;

	//glGenFramebuffers(1, &gBufferMultisampleFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, gBufferMultisampleFBO);
	/*int totalColorAtt = gBufferAmount;

	glGenFramebuffers(1, &multisampleID);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampleID);

	for (int i = 0; i < gBufferAmount; i++) {
		glGenTextures(GL_TEXTURE_2D_MULTISAMPLE, &gBufferMultisampleID[i]);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gBufferMultisampleID[i]);

		glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE,
			samples,
			GL_RGBA16F,
			w, h,
			GL_TRUE
		);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
			GL_TEXTURE_2D_MULTISAMPLE,
			gBufferMultisampleID[i], 0
		);
		// albedo, diffuse, specular, displacement, normal rn
	}
	*/

	glGenTextures(1, &bloomMultisamplePixelID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bloomMultisamplePixelID);
	glTexImage2DMultisample(
		GL_TEXTURE_2D_MULTISAMPLE,
		samples,
		GL_RGBA16F,
		w, h,
		GL_TRUE
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, // +gBufferAmount,
		GL_TEXTURE_2D_MULTISAMPLE, bloomMultisamplePixelID, 0
	);
	//totalColorAtt += 1;


	// my plan:
	// make it attach to 0 1 2 3 4 in the color attachment
	// render part, manually combine the diffuse, specular etc that got sent in
	// the indexes (not array index layers no more)
	
	//std::vector<unsigned int> attachments;

	unsigned int attachments[2]{
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1
	};
	glDrawBuffers(2, attachments);

	// GBUFFER
	//for (int i = 0; i < totalColorAtt; i++)
	//	attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);

	//glDrawBuffers(totalColorAtt, attachments.data());

	//std::cout << "tex3\n";

	//std::cout << samples << "       aoiwdjaoijwd12" << std::endl;
	rbo->initMultisampleRBO(att, samples, multisampleID);
	//std::cout << samples << "       aoiwdjaoijwd123" << std::endl;
	//std::cout << "tex4\n";

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer error: ";

		switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED:
			std::cerr << "GL_FRAMEBUFFER_UNDEFINED";
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED";
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
			break;

		default:
			std::cerr << "Unknown framebuffer error";
			break;
		}

		std::cerr << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::setOption(ENUM_FBO_TEX_RBO op) {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	switch (op) {
	case ENUM_FBO_TEX_RBO::TEXTURE: {
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, format.attachment,
			GL_TEXTURE_2D, DepthOrAndStencilTexID, 0
		);
		break;
	}

	case ENUM_FBO_TEX_RBO::RENDERBUFFER: {
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, format.attachment,
			GL_RENDERBUFFER, rbo->getID()
		);
		break;
	}
	}

	if (currentRenderer && ID == currentRenderer->getFBO_ID())
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::initSSAO() {
	ssao_uniloc.kernel_ptr = glGetUniformLocation(programSSAO3D.getID(), "ssao_kernel");

	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

	std::uniform_real_distribution<float>  randFloat(0.0f, 1.0f);
	std::default_random_engine generator;
	
	for (int i = 0; i < 64; i++) { // 4x4x4 3d 
		glm::vec3 sample{
			randFloat(generator) * 2 - 1,
			randFloat(generator) * 2 - 1,
			randFloat(generator)
		};

		sample = glm::normalize(sample);

		float scale = (float)i / 64.0f;
		scale = std::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		SSAO_data.SSAO_kernel.push_back(sample);
	}

	glUniform3fv(
		ssao_uniloc.kernel_ptr, 
		64, 
		glm::value_ptr(SSAO_data.SSAO_kernel[0])
	);

	for (int i = 0; i < 16; i++) { // 4x4 2d noise
		glm::vec3 noise{
			randFloat(generator) * 2.0f - 1.0f,
			randFloat(generator) * 2.0f - 1.0f,
			0.0f
		};

		SSAO_data.SSAO_noise.push_back(noise);
	}

	glGenTextures(1, &SSAO_data.SSAO_noise_tex_ID);
	glBindTexture(GL_TEXTURE_2D, SSAO_data.SSAO_noise_tex_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, GL_RGBA16F, 
		4, 4, 0, GL_RGBA, // 4, 4 for w, h because its a 4x4 2d noise
		GL_FLOAT, glm::value_ptr(SSAO_data.SSAO_noise[0])
	);

	glGenFramebuffers(1, &SSAO_data.SSAO_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_data.SSAO_FBO);

	glGenTextures(1, &SSAO_data.SSAO_color_att_ID);
	glBindTexture(GL_TEXTURE_2D, SSAO_data.SSAO_color_att_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
	
	glGenTextures(1, &SSAO_data.SSAO_scene_att1_ID);
	glBindTexture(GL_TEXTURE_2D, SSAO_data.SSAO_scene_att1_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		SSAO_data.SSAO_color_att_ID, 0
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
		SSAO_data.SSAO_scene_att1_ID, 0
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FBO::renderSSAO(unsigned int scene_tex) {
	// SSAO_color_att_ID the scene (DataPixelsID) before blooming
	// need smth to store the combined ssao and scene 
	// (SSAO_scene_att_ID (1) att1 of SSAO_FBO)
	// so basically SSAO_scene_att_ID is the new DataPixelsID now
}




//renderer
void Renderer::use() {
	if (currentRenderer != nullptr)
		currentRenderer->unuse();

	currentRenderer = this;
	framebuffer->use();
	uses = true;
}

void Renderer::unuse() {
	currentRenderer = nullptr;
	FBO::setDefaultFBO();
	uses = false;
}

void Renderer::render() const {
	programFBcustom.use();

	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindVertexArray(screenVAO);
	glDisable(GL_DEPTH_TEST);
	
	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
	//std::cout << w << ", " << h << '\n';

	FBO* fbo = getFBO_ptr();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getFBOmultisample_ID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getID());
	/*
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getGBufferFBO());

	int offset = fbo->getTotalTexGbuffer();
	for (int i = 0; i < offset; i++) {
		glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
		glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}*/

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1); // bright
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getID());
	//glReadBuffer(GL_COLOR_ATTACHMENT0 + offset);
	//glDrawBuffer(GL_COLOR_ATTACHMENT1); // bright

	


	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	fbo->renderSSAO(fbo->getID());

	// remember apply lighting
	// the plan here to just render all specular, diffuse, normal, position
	// etc in a texture (use array texture ok), so basically render it all first 
	// like that, replace the original vertex fragment shaders to those gbuffershaders
	// then like just render it normally, the lighting shader will be rendered here
	// use the texture array multisample

	bool horizontal = true; bool firstIteration = true;
	const int amount{ 10 };

	FBO* fboptr = getFBO_ptr();
	
	programBloom.use();
	for (int i = 0; i < amount; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboptr->getIDBloomFBO(horizontal));
		fboptr->setHorizontalBloomUniform(horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(
			GL_TEXTURE_2D, 
			firstIteration? fboptr->getIDBrightPixels() :
			fboptr->getIDBloomPixels(!horizontal)
		);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		horizontal = !horizontal;
		if (firstIteration) firstIteration = false;
	}



	FBO::setDefaultFBO();
	glStencilMask(0xFF);
	
	programFB.use();

	glActiveTexture(GL_TEXTURE0); // cant 20 somehow
	glBindTexture(GL_TEXTURE_2D, getIDPixels());

	glActiveTexture(GL_TEXTURE1); // cant 20 somehow
	glBindTexture(GL_TEXTURE_2D, fboptr->getIDBloomPixels(!horizontal));

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void Renderer::resizeBuffer(int w, int h) {
	if (currentRenderer != this) return;

	unsigned int texid = framebuffer->getIDPixels();
	unsigned int texidmulti = framebuffer->getmultisamplePixelID();

	glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
	
	glBindTexture(GL_TEXTURE_2D, framebuffer->getIDBrightPixels());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, framebuffer->getIDBloomPixels(0));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, framebuffer->getIDBloomPixels(1));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

	const texture_data_T& tex_datas = framebuffer->get_tex_datas();
	const SSAO_data_T& SSAO_datas = framebuffer->getSSAO_data();

	glBindTexture(GL_TEXTURE_2D, tex_datas.position_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, tex_datas.normal_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, SSAO_datas.SSAO_color_att_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
	
	glBindTexture(GL_TEXTURE_2D, SSAO_datas.SSAO_scene_att1_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
	
	/*for (int i = 0; i < getFBO_ptr()->getTotalTexGbuffer(); i++) {
		glBindTexture(
			GL_TEXTURE_2D_MULTISAMPLE,
			framebuffer->getIndexGBufferMultisample(i)
		);
		glBindTexture(
			GL_TEXTURE_2D, 
			framebuffer->getIndexGBufferTex(i)
		);

		glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
	}*/

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texidmulti);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->getSamples(), GL_RGBA16F, w, h, GL_TRUE);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->getIDBloomMultiPixels());
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->getSamples(), GL_RGBA16F, w, h, GL_TRUE);

	formats f = framebuffer->getFormat();

	int s = framebuffer->getSamples();

	std::cout << "\n\n\n\n\n\n\n\n";

	// rbo set size

	if (framebuffer->getInfo() == ENUM_FBO_TEX_RBO::TEXTURE) { // texture version is deprecated
		std::cout << "TEXTURE\n";
		
		glBindTexture(GL_TEXTURE_2D, framebuffer->getID_DStex());
		glTexImage2D(
			GL_TEXTURE_2D, 0, f.internalFormat, w, h,
			0, f.format, f.type, nullptr
		);
	}
	else if (framebuffer->getInfo() == ENUM_FBO_TEX_RBO::RENDERBUFFER) {
		std::cout << "RENDERBUFFER\n";
		RBO* rendBuf_obj = framebuffer->getRBO_ptr();
		
		if (s > 1) {
			glBindRenderbuffer(GL_RENDERBUFFER, rendBuf_obj->getMultiID());
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, s, f.internalFormat, w, h);
		}

		// for the default
		if (rendBuf_obj->getID() != 0) {
			glBindRenderbuffer(GL_RENDERBUFFER, rendBuf_obj->getID());
			glRenderbufferStorage(GL_RENDERBUFFER, f.internalFormat, w, h);
		}
		else {
			std::cout << "[RESIZE RBO] Attempting to resize RBO ID: " << rendBuf_obj->getID() << std::endl;
		}
		
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if (f.internalFormat == GL_DEPTH24_STENCIL8) {
			std::cout << "GL_DEPTH24_STENCIL8 used\n";
		}
		else
			std::cout << "error\n";
	}
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLFWwindow* win = glfwGetCurrentContext();
	int wfb, hfb;

	glfwGetFramebufferSize(win, &wfb, &hfb);
	
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int x = viewport[0];
	int y = viewport[1];
	int width = viewport[2];
	int height = viewport[3];

	std::cout << "framebuffer size: " << wfb << ", " << hfb << '\n';
	std::cout << 
		"viewport size: " 
		<< x << ", " 
		<< y << ", " 
		<< width << ", "
		<< height << '\n';

	std::cout << "callback size: " << w << ", " << h << '\n';
}

// rbo

void RBO::initMultisampleRBO(
	ENUM_FBO_DEPTH_STENCIL_ATT att,
	int sampleN, unsigned int multisample_FBO_ID
)
{
	//std::cout << "aoee\n";
	glBindFramebuffer(GL_FRAMEBUFFER, multisample_FBO_ID);
	//std::cout << "aoee2313213\n";
	glGenRenderbuffers(1, &multisampleID);
	glBindRenderbuffer(GL_RENDERBUFFER, multisampleID);

	//std::cout << "aoiwjdaoijwd\n";

	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);

	switch (att) {
	case ENUM_FBO_DEPTH_STENCIL_ATT::DEPTH_STENCIL:
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleN, GL_DEPTH24_STENCIL8, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampleID);
		break;

	case ENUM_FBO_DEPTH_STENCIL_ATT::DEPTH:
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleN, GL_DEPTH_COMPONENT24, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampleID);
		break;

	case ENUM_FBO_DEPTH_STENCIL_ATT::STENCIL:
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleN, GL_STENCIL_INDEX8, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampleID);
		break;
	}
	//std::cout << "aoiwjdaoijwd1111111111\n";
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer error (this print from rbo)";

		std::cerr << std::endl;
	}

	if (currentRenderer && multisample_FBO_ID == currentRenderer->getFBOmultisample_ID())
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RBO::setDepthStencil(unsigned int FBO_ID) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
	glGenRenderbuffers(1, &ID);
	std::cout << "[INIT RBO] Created RBO ID: " << ID << std::endl;
	glBindRenderbuffer(GL_RENDERBUFFER, ID);

	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FRAMEBUFFER INCOMPLETE -------- ERROR" << std::endl;
	}

	if (currentRenderer && FBO_ID == currentRenderer->getFBO_ID())
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RBO::setDepth(unsigned int FBO_ID) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);

	glGenRenderbuffers(1, &ID);
	glBindRenderbuffer(GL_RENDERBUFFER, ID);

	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ID);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FRAMEBUFFER INCOMPLETE -------- ERROR" << std::endl;
	}

	if (currentRenderer && FBO_ID == currentRenderer->getFBO_ID())
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RBO::setStencil(unsigned int FBO_ID) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);

	glGenRenderbuffers(1, &ID);
	glBindRenderbuffer(GL_RENDERBUFFER, ID);

	int w, h;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FRAMEBUFFER INCOMPLETE -------- ERROR" << std::endl;
	}

	if (currentRenderer && FBO_ID == currentRenderer->getFBO_ID())
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// shadowFBO
//SHADOW_TEX_WH on globalVar file
shadowFBO::shadowFBO() : property{}, shadowFBO_holder{}{
	glGenFramebuffers(1, &ID);

	glGenTextures(1, &depthID);
	glBindTexture(GL_TEXTURE_2D, depthID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_TEX_WH, SHADOW_TEX_WH, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
	);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shadowFBO::renderDepthShapeDebug(Renderer* rend) {
	programShadowDepthTest.use();

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(0, 0, SHADOW_TEX_WH, SHADOW_TEX_WH);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (shape_properties* shape : shapes) {
		shape->uniLocSetValues(
			programShadowDepthTest.getID(),
			programType::shadowDepthMap
		);

		switch (shape->getType()) {
		case shape_T::cube:
			shape->DrawDepthShadow();
			break;
		case shape_T::mesh:
			Mesh& mesh = static_cast<Mesh&>(*shape);
			glBindVertexArray(mesh.getVAO());
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
			break;
		}
	}

	programShadowInstanceDepthTest.use();

	for (shape_properties* shape : shapesInstance) {
		shape->uniLocSetValues(
			programShadowInstanceDepthTest.getID(),
			programType::shadowDepthInstanceMap
		);

		switch (shape->getType()) {
			case shape_T::cube: {
				shape->DrawDepthShadow();
				break;
			}
			case shape_T::model:
			{
				const instance_property& instance = shape->getInstance();
				model& model_ = static_cast<model&>(*shape);
				for (const Mesh& mesh : model_.getMeshes()) {
					//glBindVertexArray(mesh.getVAO()); instance vao too soon
					glDrawElementsInstanced(
						GL_TRIANGLES, mesh.indices.size(), 
						GL_UNSIGNED_INT, 0, instance.instance_amount
					);
				}
				break;
			}
		}
	}
	
	//rend->use(); // already decides instance fbo bind or not
	//glBindFramebuffer(GL_FRAMEBUFFER, ID);

	GLFWwindow* win = glfwGetCurrentContext();
	int w, h;

	programShadowDepthVisual.use();
	glUniform1i(shadowVisualUniLoc.sampleDepth, 20);

	FBO::setDefaultFBO();
	//in loop already glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glfwGetFramebufferSize(win, &w, &h);
	//std::cout << "shadow reset wh: " << w << ", " << h << '\n';
	glViewport(0, 0, w, h);

	glBindVertexArray(screenVAO);

	glActiveTexture(GL_TEXTURE20);
	glBindTexture(GL_TEXTURE_2D, depthID);

	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	//rend->render(&programShadowMap, depthID);
}

void shadowFBO::shadowMap() {
	//shape_properties::setUniLocID(programShadowDepthTest.getID(), programType::shadowMap);
	// super evil line
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(0, 0, SHADOW_TEX_WH, SHADOW_TEX_WH);
	glClear(GL_DEPTH_BUFFER_BIT);

	//glUseProgram(programShadowMap.getID());
	programShadowDepthTest.use();
	
	for (shape_properties* shape : shapes) {
		shape->uniLocSetValues(
			programShadowDepthTest.getID(),
			programType::shadowDepthMap, this
		);

		if (shape->getType() == shape_T::model) {
			shape->DrawDepthShadow();
			continue;
		}

		shape->DrawDepthShadow();
	}

	programShadowInstanceDepthTest.use();
	for (shape_properties* shape : shapesInstance) {
		shape->uniLocSetValues(
			programShadowInstanceDepthTest.getID(),
			programType::shadowDepthInstanceMap, this
		);

		if (shape->getType() == shape_T::model) {
			shape->DrawDepthShadow();
			continue;
		}

		shape->DrawDepthShadow();
	}
	
	int w, h;
	GLFWwindow* win = glfwGetCurrentContext();
	glfwGetFramebufferSize(win, &w, &h);
	//std::cout << "shadow reset wh: " << w << ", " << h << '\n';
	glViewport(0, 0, w, h);
	
	//int samples = rend->getSamples_FBO();

	//if (samples > 1) note: INSTANCING SUPPORT LATER BC SHADERS DONT KNOW HOW
	//bc i stupid
	//	glBindFramebuffer(GL_FRAMEBUFFER, rend->getFBOmultisample_ID());
	//else

	//glBindFramebuffer(GL_FRAMEBUFFER, FBO_ID);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void shadowFBO::configureDirectionalShadow(glm::vec3 eye, glm::vec3 target, glm::vec3 upvector) {
	property.eye = eye;
	property.dir = glm::lookAt(
		eye + property.eyeOffset,
		target, 
		upvector
	);

	property.view = property.projection * property.dir; // somehow no work

	
	/*std::cout << "---     Shadow Matrix (property.view)     ---" << std::endl;
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			std::cout << property.view[col][row] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------" << std::endl;*/
}



// cubeshadowmap

cubeShadowFBO::cubeShadowFBO() : shadowFBO_holder{} {
	glGenFramebuffers(1, &ID);

	glGenTextures(1, &depthID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
			SHADOW_TEX_WH, SHADOW_TEX_WH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 
			nullptr
		);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cubeShadowFBO::shadowmap() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(0, 0, SHADOW_TEX_WH, SHADOW_TEX_WH);
	glClear(GL_DEPTH_BUFFER_BIT);

	programShadowCubeMap.use();

	for (shape_properties* shape : shapes) {
		shape->uniLocSetValues(
			programShadowCubeMap.getID(),
			programType::shadowDepthCubeMap, this
		);

		shape->DrawDepthShadow();
	}
	
	programInstanceShadowCubeMap.use();
	for (shape_properties* shape : shapesInstance) {
		shape->uniLocSetValues(
			programInstanceShadowCubeMap.getID(),
			programType::shadowDepthInstanceCubeMap, this
		);
		
		shape->DrawDepthShadow();
	}

	int w, h;
	GLFWwindow* win = glfwGetCurrentContext();
	glfwGetFramebufferSize(win, &w, &h);
	glViewport(0, 0, w, h);
}




//void shadowFBO::updateProperty() {

//}

// global uniform

void globalUniform::setCurrentPerspective() {
	//std::cout << "pers\n";
	const glm::mat4& pers = currentCamera->perspective;

	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, mat4size, &pers);
}

void globalUniform::setCurrentView() {
	//std::cout << "view\n";
	const glm::mat4& view = currentCamera->getViewRefConst();

	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferSubData(GL_UNIFORM_BUFFER, mat4size, mat4size, &view);
}

void globalUniform::setLightSpaceMatrix() {
	//std::cout << "pers\n";
	const glm::mat4& lightSpaceMat = directionalLight.view;

	glBindBuffer(GL_UNIFORM_BUFFER, ID);
	glBufferSubData(GL_UNIFORM_BUFFER, mat4size * 2, mat4size, &lightSpaceMat);
}
