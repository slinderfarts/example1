#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <iostream>

#include <array>
#include <gtc/matrix_transform.hpp>

#include "../../Header/OpenglStuff/shaders.h"
#include "../../Header/OpenglStuff/camera.h"

//#include "../../Header/OpenglStuff/shapes.h"
#include "../../Header/globarVar.h"

enum class vertex_t {
    triangle,
    square,
    cube
};

inline const float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

inline const float fullScreenVertex[]{
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

inline const float vertex[]{ // for 2D
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

inline float verticesCube[] = {
    // positions           // normals            // texcoords      // tangents     // bitangent is calculated in vertex shader        

    // Front (+Z)
    -0.5f,-0.5f, 0.5f,      0.0f,0.0f,1.0f,      0.0f,0.0f,        1.0f,0.0f,0.0f,      
     0.5f,-0.5f, 0.5f,      0.0f,0.0f,1.0f,      1.0f,0.0f,        1.0f,0.0f,0.0f,     
     0.5f, 0.5f, 0.5f,      0.0f,0.0f,1.0f,      1.0f,1.0f,        1.0f,0.0f,0.0f,     
    -0.5f, 0.5f, 0.5f,      0.0f,0.0f,1.0f,      0.0f,1.0f,        1.0f,0.0f,0.0f,     

    // Back (-Z)
    -0.5f,-0.5f,-0.5f,      0.0f,0.0f,-1.0f,     1.0f,0.0f,       -1.0f,0.0f,0.0f,      
     0.5f,-0.5f,-0.5f,      0.0f,0.0f,-1.0f,     0.0f,0.0f,       -1.0f,0.0f,0.0f,      
     0.5f, 0.5f,-0.5f,      0.0f,0.0f,-1.0f,     0.0f,1.0f,       -1.0f,0.0f,0.0f,     
    -0.5f, 0.5f,-0.5f,      0.0f,0.0f,-1.0f,     1.0f,1.0f,       -1.0f,0.0f,0.0f,     

    // Left (-X)
    -0.5f,-0.5f,-0.5f,     -1.0f,0.0f,0.0f,      0.0f,0.0f,        0.0f,0.0f,1.0f,    
    -0.5f,-0.5f, 0.5f,     -1.0f,0.0f,0.0f,      1.0f,0.0f,        0.0f,0.0f,1.0f,    
    -0.5f, 0.5f, 0.5f,     -1.0f,0.0f,0.0f,      1.0f,1.0f,        0.0f,0.0f,1.0f,    
    -0.5f, 0.5f,-0.5f,     -1.0f,0.0f,0.0f,      0.0f,1.0f,        0.0f,0.0f,1.0f,  

    // Right (+X)
     0.5f,-0.5f,-0.5f,      1.0f,0.0f,0.0f,      1.0f,0.0f,        0.0f,0.0f,-1.0f,   
     0.5f,-0.5f, 0.5f,      1.0f,0.0f,0.0f,      0.0f,0.0f,        0.0f,0.0f,-1.0f,   
     0.5f, 0.5f, 0.5f,      1.0f,0.0f,0.0f,      0.0f,1.0f,        0.0f,0.0f,-1.0f,     
     0.5f, 0.5f,-0.5f,      1.0f,0.0f,0.0f,      1.0f,1.0f,        0.0f,0.0f,-1.0f,   

     // Bottom (-Y)
     -0.5f,-0.5f,-0.5f,      0.0f,-1.0f,0.0f,     0.0f,1.0f,        1.0f,0.0f,0.0f,    
      0.5f,-0.5f,-0.5f,      0.0f,-1.0f,0.0f,     1.0f,1.0f,        1.0f,0.0f,0.0f, 
      0.5f,-0.5f, 0.5f,      0.0f,-1.0f,0.0f,     1.0f,0.0f,        1.0f,0.0f,0.0f,  
     -0.5f,-0.5f, 0.5f,      0.0f,-1.0f,0.0f,     0.0f,0.0f,        1.0f,0.0f,0.0f,    

     // Top (+Y)
     -0.5f, 0.5f,-0.5f,      0.0f,1.0f,0.0f,      0.0f,1.0f,        1.0f,0.0f,0.0f,    
      0.5f, 0.5f,-0.5f,      0.0f,1.0f,0.0f,      1.0f,1.0f,        1.0f,0.0f,0.0f,     
      0.5f, 0.5f, 0.5f,      0.0f,1.0f,0.0f,      1.0f,0.0f,        1.0f,0.0f,0.0f,    
     -0.5f, 0.5f, 0.5f,      0.0f,1.0f,0.0f,      0.0f,0.0f,        1.0f,0.0f,0.0f  
};

inline std::vector<glm::vec3> cubeVertexPosVec3 = {
    // Front
    glm::vec3(-0.5f,-0.5f, 0.5f),
    glm::vec3(0.5f,-0.5f, 0.5f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(-0.5f, 0.5f, 0.5f),

    // Back
    glm::vec3(-0.5f,-0.5f,-0.5f),
    glm::vec3(0.5f,-0.5f,-0.5f),
    glm::vec3(0.5f, 0.5f,-0.5f),
    glm::vec3(-0.5f, 0.5f,-0.5f),

    // Left
    glm::vec3(-0.5f,-0.5f,-0.5f),
    glm::vec3(-0.5f,-0.5f, 0.5f),
    glm::vec3(-0.5f, 0.5f, 0.5f),
    glm::vec3(-0.5f, 0.5f,-0.5f),

    // Right
    glm::vec3(0.5f,-0.5f,-0.5f),
    glm::vec3(0.5f,-0.5f, 0.5f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(0.5f, 0.5f,-0.5f),

    // Bottom
    glm::vec3(-0.5f,-0.5f,-0.5f),
    glm::vec3(0.5f,-0.5f,-0.5f),
    glm::vec3(0.5f,-0.5f, 0.5f),
    glm::vec3(-0.5f,-0.5f, 0.5f),

    // Top
    glm::vec3(-0.5f, 0.5f,-0.5f),
    glm::vec3(0.5f, 0.5f,-0.5f),
    glm::vec3(0.5f, 0.5f, 0.5f),
    glm::vec3(-0.5f, 0.5f, 0.5f)
};

inline unsigned int indicesCube[] = { // ccw
    0, 1, 2,    2, 3, 0,
    4, 7, 6,    6, 5, 4,
    8, 9, 10,   10, 11, 8,
    12, 15, 14, 14, 13, 12,
    16, 17, 18, 18, 19, 16,
    20, 23, 22, 22, 21, 20
};


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};


[[maybe_unused]] inline unsigned int screenVAO = 0;
[[maybe_unused]] inline unsigned int screenVBO = 0;

inline const unsigned int triangleIndices[]{
    0, 1, 2
};

inline const unsigned int squareIndices[]{
    0, 1, 2,
    2, 3, 0
};

template<vertex_t>
void initVertex() = delete;

template<>
void initVertex<vertex_t::triangle>();

template<>
void initVertex<vertex_t::square>();

template<>
void initVertex<vertex_t::cube>();

void deleteVertexes();

// renderer stuff
enum class ENUM_FBO_DEPTH_STENCIL_ATT { DEPTH, STENCIL, DEPTH_STENCIL };
enum class ENUM_FBO_TEX_RBO { TEXTURE, RENDERBUFFER };

enum class ENUM_TEXTURE_TYPE{ ALBEDO, DIFFUSE, SPECULAR, DISPLACEMENT, NORMAL};

struct formats {
    unsigned int internalFormat;
    unsigned int format;
    unsigned int attachment;
    unsigned int type;
};

struct SSAO_data_T {
    unsigned int SSAO_FBO = 0;
    unsigned int SSAO_color_att_ID = 0;
    unsigned int SSAO_scene_att1_ID = 0;
    unsigned int SSAO_noise_tex_ID = 0;
    //unsigned int noise_tex_id = 0;

    std::vector<glm::vec3> SSAO_kernel;
    std::vector<glm::vec3> SSAO_noise;
};

struct texture_data_T {
    unsigned int position_ID = 0;
    unsigned int normal_ID = 0;
};

//class FBO;
class RBO;

class FBO {
public:
    FBO(ENUM_FBO_DEPTH_STENCIL_ATT att, ENUM_FBO_TEX_RBO bufferT);

    ~FBO() {
        // renderer stuffs
        glDeleteFramebuffers(1, &ID);
        glDeleteFramebuffers(1, &multisampleID);
        glDeleteTextures(1, &DataPixelsID);
        glDeleteTextures(1, &brightPixels);
        glDeleteTextures(1, &bloomMultisamplePixelID);
        glDeleteTextures(1, &DepthOrAndStencilTexID);
        glDeleteTextures(1, &DataPixelMultiSamples);

        //tex_data
        glDeleteTextures(1, &textures_data.position_ID);
        glDeleteTextures(1, &textures_data.normal_ID);

        //bloom
        glDeleteFramebuffers(2, BloomDataFBID);
        glDeleteTextures(2, BloomDataPixelsID);

        //ssao
        glDeleteFramebuffers(1, &SSAO_data.SSAO_FBO);
        glDeleteTextures(1, &SSAO_data.SSAO_color_att_ID);
        glDeleteTextures(1, &SSAO_data.SSAO_noise_tex_ID);


        /*

        struct {
            unsigned int position_ID;
            unsigned int normal_ID;
            // soon other datas
        } textures_data; // note: data from the render framebuffer attachments
        */

        delete(rbo);
    }

    void initMultisampleFBO(ENUM_FBO_DEPTH_STENCIL_ATT att);

    inline unsigned int getID() const {
        return ID;
    }

    inline unsigned int getmultisampleID() const {
        return multisampleID;
    }

    inline unsigned int getmultisamplePixelID() const {
        return DataPixelMultiSamples;
    }

    inline unsigned int getIDPixels() const {
        return DataPixelsID;
    }

    inline unsigned int getIDBrightPixels() const {
        return brightPixels;
    }

    inline unsigned int getIDBloomPixels(int i) const {
        return BloomDataPixelsID[i];
    }

    inline unsigned int getIDBloomFBO(int i) const {
        return BloomDataFBID[i];
    }
    
    inline unsigned int getIDBloomMultiPixels() const {
        return bloomMultisamplePixelID;
    }

    inline unsigned int getID_DStex() const {
        return DepthOrAndStencilTexID;
    }

    //inline unsigned int getID_gBufferMultisampleTexArrayID() const { // skipped maybe next day
    //    return gBufferMultisampleTexArrayID;
    //}

    inline formats getFormat() const {
        return format;
    }

    inline unsigned int getInternalFormat() const {
        return format.internalFormat;
    }

    inline RBO* getRBO_ptr() {
        return rbo;
    }

    inline void use() {
        if (samples > 1)
            glBindFramebuffer(GL_FRAMEBUFFER, multisampleID);
        else
            glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }

    inline static void setDefaultFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    inline ENUM_FBO_TEX_RBO getInfo() const {
        return info;
    }

    void setOption(ENUM_FBO_TEX_RBO op);

    inline void setSamples(int n) {
        samples = n;
    }

    inline int getSamples()  const {
        return samples;
    }

    inline void setHorizontalBloomUniform(bool b) {
        glUniform1i(bloomUniloc.horizontal, b);
    }

    /*inline unsigned int getIndexGBufferMultisample(ENUM_TEXTURE_TYPE type) {
        switch (type) {
        case ENUM_TEXTURE_TYPE::ALBEDO: return gBufferMultisampleID[0];
        case ENUM_TEXTURE_TYPE::DIFFUSE: return gBufferMultisampleID[1];
        case ENUM_TEXTURE_TYPE::SPECULAR: return gBufferMultisampleID[2];
        case ENUM_TEXTURE_TYPE::DISPLACEMENT: return gBufferMultisampleID[3];
        case ENUM_TEXTURE_TYPE::NORMAL: return gBufferMultisampleID[4];
        default: return -1;
        }
    }

    inline unsigned int getIndexGBufferMultisample(std::size_t i) const {
        if (i > getTotalTexGbuffer()) return -1;
        return gBufferMultisampleID[i];
    }

    inline unsigned int getGBufferFBO() const {
        return gBufferFBO;
    }

    inline unsigned int getIndexGBufferTex(ENUM_TEXTURE_TYPE type) {
        switch (type) {
        case ENUM_TEXTURE_TYPE::ALBEDO: return gBufferTexID[0];
        case ENUM_TEXTURE_TYPE::DIFFUSE: return gBufferTexID[1];
        case ENUM_TEXTURE_TYPE::SPECULAR: return gBufferTexID[2];
        case ENUM_TEXTURE_TYPE::DISPLACEMENT: return gBufferTexID[3];
        case ENUM_TEXTURE_TYPE::NORMAL: return gBufferTexID[4];
        default: return -1;
        }
    }

    inline int getIndexGBufferTex(size_t i) const {
        if (i >= gBufferAmount) return -1;
        return gBufferTexID[i];
    }

    inline unsigned int getTotalTexGbuffer() const {
        return gBufferAmount;
    }
    */

    inline SSAO_data_T& getSSAO_data() {
        return SSAO_data;
    }

    inline texture_data_T& get_tex_datas() {
        return textures_data;
    }

    void renderSSAO(unsigned int scene_tex);

private:
    ////////////////////////////////GBUFFER LATER/////////////////////////////////////
              // ^^^^^
    // inside color att: the data textures and bloomMultisamplePixelID
                                                // brightness


    //unsigned int gBufferMultisampleTexArrayID;

    //static constexpr int gBufferAmount{ 4 };
    //unsigned int gBufferFBO;
    //unsigned int gBufferTexID[gBufferAmount];

    //unsigned int gBufferMultisampleFBO;
    //unsigned int gBufferMultisampleID[gBufferAmount];
    // diffuse, specular, displacement, normal
    //////////////////////////////////////////////////////////////////////////////////

    unsigned int ID = 0;
    unsigned int multisampleID = 0;

    unsigned int bloomMultisamplePixelID = 0;
    unsigned int DataPixelsID = 0;
    unsigned int brightPixels = 0;

    unsigned int BloomDataFBID[2] = {};
    unsigned int BloomDataPixelsID[2] = {};

    unsigned int DepthOrAndStencilTexID = 0;
    unsigned int DataPixelMultiSamples = 0;

    texture_data_T textures_data;

    SSAO_data_T SSAO_data;

    void initSSAO();

    inline void setFBOshadersUnilocID(program* prog, programType type) {
        prog->use();
        
        switch (type) {
        case programType::bloomRender: {
            bloomUniloc.horizontal = glGetUniformLocation(
                programBloom.getID(), "horizontal"
            );
            break;
        }
        default: std::cout << "error on setting shader FBO uniloc\n";  return;
        }
    }

    //unsigned int attachment;
    //unsigned int readType;
    RBO* rbo = nullptr;

    ENUM_FBO_TEX_RBO info;
    formats format; // for depth or/and stencil

    int samples; // if 1> samples then make it multisample mode fbo, 
};

class RBO { // rbo being properly set in FBO constructor, for now rbo stencil or/and depth the only one that can anti alias
public:
    ~RBO() {
        glDeleteRenderbuffers(1, &ID);
        glDeleteRenderbuffers(1, &multisampleID);
    }

    inline unsigned int getID() const {
        return ID;
    }

    inline unsigned int getMultiID() const{
        return multisampleID;
    }

    void setDepthStencil(unsigned int FBO_ID);
    void setDepth(unsigned int FBO_ID);
    void setStencil(unsigned int FBO_ID);
    // remember set storage multisample if over 1 sample

    void initMultisampleRBO(
        ENUM_FBO_DEPTH_STENCIL_ATT att,
        int sampleN,
        unsigned int FBO_ID
    );

private:
    unsigned int ID;
    unsigned int multisampleID;
};


class Renderer {
public:
    Renderer(ENUM_FBO_DEPTH_STENCIL_ATT att, ENUM_FBO_TEX_RBO bufferT)
    {
        //std::cout << "test2111\n";
        framebuffer = new FBO(att, bufferT);
        //std::cout << "test2222\n";
        //framebuffer->initMultisampleFBO(att);
        //std::cout << "test2111231231\n";
    }

    ~Renderer() {
        delete(framebuffer);
    }

    static inline void freeScreenVertex() {
        glDeleteVertexArrays(1, &screenVAO);
        glDeleteBuffers(1, &screenVBO);
    }

    void resizeBuffer(int w, int h);

    inline unsigned int getFBO_ID() const {
        return framebuffer->getID();
    }

    inline void initMultiSampleFBO(ENUM_FBO_DEPTH_STENCIL_ATT att) {
        framebuffer->initMultisampleFBO(att);
    }
    inline void setSamples_FBO(int n) {
        framebuffer->setSamples(n);
    }

    inline int getSamples_FBO() const {
        return framebuffer->getSamples();
    }

    inline unsigned int getFBOmultisample_ID() const {
        return framebuffer->getmultisampleID();
    }

    //inline unsigned int getBloomMultisample_ID() const {
    //    return framebuffer->getIDBloomMultiPixels();
    //}
    
    inline FBO* getFBO_ptr() const {
        return framebuffer;
    }

    inline unsigned int getIDPixels() const {
        return framebuffer->getIDPixels();
    }

    inline unsigned int getIDBloomPixels(int i) const {
        return framebuffer->getIDBloomPixels(i);
    }

    inline void clearPixels() {
        int fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
        // ^^^ so if multisample itll just grab the multisample fbo in here

        if (!uses) framebuffer->use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void use();
    void unuse();

    void render() const; // no prog and id pixels args anymore

    static void makeVertex_Screen() {
        glGenVertexArrays(1, &screenVAO);
        glBindVertexArray(screenVAO);

        glGenBuffers(1, &screenVBO);
        glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fullScreenVertex), fullScreenVertex, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    FBO* framebuffer;
    bool uses = false;
};


//class shadow {};

struct shadowProperty {//: shadow {
    glm::vec3 eye;
    glm::mat4 dir;
    glm::mat4 view;
    glm::vec3 eyeOffset;
    glm::mat4 projection;
};

struct cubeShadowProperty {//: shadow {
    glm::mat4 projection;
    std::array<glm::mat4, 6> shadowView;
    glm::vec3 position;
    float far_plane;
};

class shadowFBO_holder {};

class shadowFBO: public shadowFBO_holder {
public:
    shadowFBO();

    inline unsigned int get_ID() const {
        return ID;
    }
    inline unsigned int getDepth_ID() const {
        return depthID;
    }
    inline unsigned int getShadowMap_ID() const {
        return shadowMapID;
    }

    void shadowMap();

    void renderDepthShapeDebug(Renderer* rend);
    void configureDirectionalShadow(glm::vec3 eye, glm::vec3 target, glm::vec3 upvector);

    inline void setEyeOffset_P(glm::vec3 eye) {
        property.eyeOffset = eye;
    }
    inline void setProjection_P(glm::mat4 projection) {
        property.projection = projection;
    }

    inline glm::mat4 getViewShadow_P() const {
        return property.view;
    }
    inline glm::mat4 getDirShadow_P() const {
        return property.dir;
    }
    inline glm::vec3 getEye_P() const {
        return property.eye;
    }
    inline glm::vec3 getEyeOffset_P() const {
        return property.eyeOffset;
    }
    inline glm::mat4 getProjection_P() const {
        return property.projection;
    }
    inline shadowProperty getShadowProperties() const {
        return property;
    }

    //void updateProperty();
private:
    unsigned int ID;
    unsigned int depthID;

    unsigned int shadowMapID;
    //void shadowDepthMap();

    shadowProperty property;
};

class cubeShadowFBO: public shadowFBO_holder {
public:
    cubeShadowFBO();

    inline unsigned int get_ID() const {
        return ID;
    }
    inline unsigned int getDepth_ID() const {
        return depthID;
    }

    cubeShadowProperty& getProperty() {
        return property;
    }

    inline void setTransformShadow() { // set this to the geometry shader later u forgot abt it
        // y as -1 bc opengl sees the top bottom switched
        property.shadowView[0] = property.projection * glm::lookAt(
            property.position, property.position + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)
        );
        property.shadowView[1] = property.projection * glm::lookAt(
            property.position, property.position + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)
        );
        property.shadowView[2] = property.projection * glm::lookAt(
            property.position, property.position + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)
        );
        property.shadowView[3] = property.projection * glm::lookAt(
            property.position, property.position + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)
        );
        property.shadowView[4] = property.projection * glm::lookAt(
            property.position, property.position + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)
        );
        property.shadowView[5] = property.projection * glm::lookAt(
            property.position, property.position + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)
        );

    }

    void shadowmap();

private:
    unsigned int ID;
    unsigned int depthID;

    cubeShadowProperty property;
};



// global uniforms
inline constexpr int mat4size = sizeof(float) * 16;

class globalUniform {
public:
    globalUniform() {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
        glBufferData(
            GL_UNIFORM_BUFFER,
            mat4size * 3, // pers, view, LSmatrix
            nullptr, GL_DYNAMIC_DRAW
        );
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, ID);
    }

    void setCurrentPerspective();
    void setCurrentView();
    void setLightSpaceMatrix();

    inline void use() const {
        glBindBuffer(GL_UNIFORM_BUFFER, ID);
    }

private:
    unsigned int ID;
};
