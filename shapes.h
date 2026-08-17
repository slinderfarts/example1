#pragma once
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../../Header/globarVar.h"

#include "../../Header/OpenglStuff/texture.h"
#include "../../Header/OpenglStuff/buffers.h"

//enum class obj_t { twoDimensional, threeDimensional };
class shape_properties;
class shape2D_properties;

inline std::vector<shape_properties*>   shapes;
inline std::vector<shape_properties*>   shapesInstance;
inline std::vector<shape2D_properties*> shapes2D;

struct velocity_T {
	glm::vec3 value = glm::vec3(0);
	float magnitude = 0.0f;
};

struct instance_property {
	unsigned int VAO=0;
	unsigned int VBOoffsets=0;

	int instance_amount=0;
};

class shape_properties {
public:

	virtual inline void setPosition(glm::vec3 pos) {
		position = pos;
		setModelMat();
	}
	virtual inline void setLocalPosition(glm::vec3 pos) {
		position = glm::vec3(glm::mat4_cast(quaternion) * glm::vec4(pos, 1.0));
		setModelMat();
	}

	inline glm::vec3 getPosition() const {
		return position;
	}

	inline virtual void setQuaternion(glm::quat quat) {
		quaternion = quat;
		setModelMat();
	}
	inline glm::quat getQuaternion() const {
		return quaternion;
	}

	inline virtual void setSize(glm::vec3 s) {
		size = s;
		setModelMat();
	}
	inline glm::vec3 getSize() const {
		return size;
	}

	inline virtual void setTransparency(float n) {
		transparency = glm::clamp(n, 0.0f, 1.0f);
	}

	inline float getTransparency() const {
		return transparency;
	}
	
	inline virtual void setReflectiveness(float n) {
		reflective = glm::clamp(n, 0.0f, 1.0f);
	}

	inline float getReflectiveness() const {
		return reflective;
	}

	inline shape_T getType() const {
		return type;
	}

	inline void setTextureIndex(int i, unsigned int texture) {
		glUniform1i(texture, i);
	}

	inline void explicitAddDiffuse(Texture tex) {
		material.diffuse.emplace_back(std::move(tex));
	}

	inline void explicitRemoveDiffuse(Texture tex) {
		material.diffuse.erase(
			std::remove_if(
				material.diffuse.begin(), material.diffuse.end(),
				[&](const Texture& t) {
					return t.texbuf == tex.texbuf;
				}
			), material.diffuse.end()
		);
	}

	inline void explicitAddSpecular(Texture tex) {
		material.specular.emplace_back(std::move(tex));
	}

	inline void explicitRemoveSpecular(Texture tex) {
		material.specular.erase(
			std::remove_if(
				material.specular.begin(), material.specular.end(),
				[&](const Texture& t) {
					return t.texbuf == tex.texbuf;
				}
			), material.specular.end()
		);
	}

	inline void explicitAddDisplacement(Texture tex) {
		material.displacement.emplace_back(std::move(tex));
	}

	inline void explicitRemoveDisplacement(Texture tex) {
		material.displacement.erase(
			std::remove_if(
				material.displacement.begin(), material.displacement.end(),
				[&](const Texture& t) {
					return t.texbuf == tex.texbuf;
				}
			), material.displacement.end()
		);
	}
	
	inline void explicitAddNormalTex(Texture tex) {
		material.normals.emplace_back(std::move(tex));
	}

	inline void explicitRemoveNormalTex(Texture tex) {
		material.normals.erase(
			std::remove_if(
				material.normals.begin(), material.normals.end(),
				[&](const Texture& t) {
					return t.texbuf == tex.texbuf;
				}
			), material.normals.end()
		);
	}

	inline void addTexture(Texture tex) {
		switch (tex.type) {
		case textureType::diffuse: {
			explicitAddDiffuse(tex);
			break;
		}
		case textureType::specular: {
			explicitAddSpecular(tex);
			break;
		}
		case textureType::displacement: {
			explicitAddDisplacement(tex);
			break;
		}
		case textureType::normal: {
			explicitAddNormalTex(tex);
			break;
		}
		}
	}

	inline void removeTexture(Texture tex) {
		switch (tex.type) {
		case textureType::diffuse: {
			explicitRemoveDiffuse(tex);
			break;
		}
		case textureType::specular: {
			explicitRemoveSpecular(tex);
			break;
		}
		case textureType::displacement: {
			explicitRemoveDisplacement(tex);
			break;
		}
		case textureType::normal: {
			explicitRemoveNormalTex(tex);
			break;
		}
		}
	}

	inline std::vector<Texture>& getDiffuses() {
		return material.diffuse;
	}
	inline std::vector<Texture>& getSpeculars() { // remember make for normals too
		return material.specular;
	}
	inline std::vector<Texture>& getDisplacements() {
		return material.displacement;
	}
	inline const std::vector<Texture>& getNormals() const {
		return material.normals;
	}

	inline glm::mat4 getModelMatrix() const {
		return modelMat;
	}

	inline void setModelMat() {
		modelMat = glm::translate(glm::mat4(1.0f), position) 
			* glm::mat4_cast(quaternion) 
			* glm::scale(glm::mat4(1.0f), size);
	}


	virtual void Draw() = 0;
	//virtual void DrawShadow(shadowFBO* shadowfbo, Renderer* rend) = 0;
	virtual void DrawDepthShadow() = 0;

	void uniLocSetValues(unsigned int program, programType pType, shadowFBO_holder* sFBO = nullptr);
	static void setUniLocID(unsigned int program3d, programType pType); // change later seperate thing

	//instancing
	virtual void initInstancing() = 0;

	//template<int n>
	//void setInstancingOffsetsVBO(const std::array<glm::vec3, n>& offsetsBuf) = 0;

	inline virtual void useInstancing() {
		if (isInstancingInit) use_instancing = true;
		shapesInstance.emplace_back(this);

		std::erase_if(shapes, [this](const shape_properties* shape) {
			return this == shape;
			}
		);
	}
	inline void endInstancing() {
		if (isInstancingInit) use_instancing = false;

		shapes.emplace_back(this);

		std::erase_if(shapesInstance, [this](const shape_properties* shape) {
				return this == shape;
			}
		);
	}

	inline instance_property& getInstance() {
		return instance;
	}

	inline virtual void setInstanceAmount(int n) {
		instance.instance_amount = n;
	}

	inline int getInstanceAmount() const {
		return instance.instance_amount;
	}

	// reallocate the vbo depending the n thing ok
	std::string name;

	float shininess{ 32.0f }; // default value 
	// (note: if shininess too small, shading breaks)

	float thicknessHighlight{ 1.01f };
	

	bool highlight{ false };
	bool anchored{ false };

	glm::vec4 highlightColor{ glm::vec4(1.0f) };
	glm::vec4 color;

	velocity_T velocity;

protected:
	Material material;

	shape_properties(shape_T shapeType) :
		position(glm::vec3(0.0f)), 
		quaternion(glm::quat(1, 0, 0, 0)),
		size(glm::vec3(1.0f)), color(glm::vec4(1.0f)),
		type(shapeType),// objType(objT), 
		shininess(32.0f), name{},
		isInstancingInit(false),
		use_instancing(false)
	{
		if (shapeType != shape_T::mesh)
			shapes.push_back(this);

		setModelMat();
	};

	virtual ~shape_properties() {
		eraseFromShape();
	};

	glm::vec3 position;
	glm::quat quaternion;
	glm::vec3 size;

	glm::mat4 modelMat;

	shape_T type;

	float transparency{ 0.0f };
	float reflective{ 0 };

	bool highlightMode{ false };

	instance_property instance;
	bool use_instancing;
	bool isInstancingInit;

	//float height_scale[texDisplacementSize];

	//obj_t objType{ obj_t::threeDimensional };
private:
	inline void eraseFromShape() {
		shapes.erase(
			std::remove(shapes.begin(), shapes.end(), this),
			shapes.end()
		);
	}
};


//2D shape properties

class shape2D_properties {
public:
	shape2D_properties(shape_T shapeType) :
		normalizedPosNDC(glm::vec3(0.0f)),
		size(glm::vec3(1.0f)),
		color(glm::vec4(1.0f)),
		offsetPos(glm::vec2(0.0f)),
		type(shapeType)//, objType(objT)
	{
		shapes2D.push_back(this);
	};

	void setNDC(float x, float y) {
		normalizedPosNDC = glm::vec2(x, y);
	}

	void updateNDC(GLFWwindow* win) {
		int w, h;
		glfwGetFramebufferSize(win, &w, &h);

		positionNDC = normalizedPosNDC * glm::vec2(w, h); // to pixel scalar
	}

	/*inline void setColor(glm::vec4 col) {
		color = col;
	}

	inline void setOffset(glm::vec3 pos) {
		position = pos;
	}

	inline void setSize(glm::vec3 s) {
		size = s;
	}*/

	virtual void Draw() = 0;
	void uniLocSetValues();

	static void setUniLocID(unsigned int program2d);

	glm::vec4 color;
	glm::vec2 positionNDC;
	glm::vec2 normalizedPosNDC;
	glm::vec2 offsetPos;
	int zIndex = 1;

	glm::vec3 size;
	shape_T type;

private:
	//obj_t objType{ obj_t::twoDimensional };
};

// shapes

class triangle : public shape2D_properties {
public:
	triangle() : shape2D_properties(shape_T::triangle) {}
	void Draw() override;
};


class square : public shape2D_properties {
public:
	square() : shape2D_properties(shape_T::square) {}
	void Draw() override;
};

class cube : public shape_properties {
public:
	cube(): shape_properties(shape_T::cube)
	{}

	void Draw() override;
	//void DrawShadow(shadowFBO* shadowfbo, Renderer* rend) override; deprecated
	void DrawDepthShadow() override;
	//instancing
	void initInstancing() override;

	template<int n>
	void setInstancingOffsetsVBO(const std::array<glm::mat4, n>& offsetsBuf) {
		glBindBuffer(GL_ARRAY_BUFFER, instance.VBOoffsets);
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(glm::mat4) * offsetsBuf.size(), 
			offsetsBuf.data(), 
			GL_STATIC_DRAW
		);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};
};
