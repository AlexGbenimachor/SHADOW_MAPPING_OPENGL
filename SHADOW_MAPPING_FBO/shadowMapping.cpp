// STD LIBRARY
#include <math.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stack>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// OPENGL LIBRARY

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "loadShaderfolder/loadShader.hpp"
#include "loadShaderfolder/myCompute.hpp"


using namespace std;
using namespace glm;


//################################################ALL YOUR CLASSES GOES HERE######################################################

//build Ray class

//float t = INFINITY;
class Ray{
      public:
            Ray(){}
            Ray(const vec3& origin, const vec3& direction){
                this->On = origin;
                this->dir = direction;
            }
            vec3 origin(){return On;}
            vec3 direction(){return dir;}
            
            vec3 P(const float& t){return On + t * dir;}
            
       private:
            vec3 On;
            vec3 dir;

};

class u_object{
      public:
            u_object(){}
            u_object(const vec3& position, const vec3& obSize, const int& objtype){
                   this->Pos = position;
                   this->Sz = obSize;
                   this->type  = objtype;
                   
           
           };
           vec3 position(){return Pos;}
           vec3 obSize(){return Sz;}
           int objtype(){return type;}
      private:
             vec3 Pos;
             vec3 Sz;
             int type;







};

//create sphere class here...
class Sphere{

    private:
     int numVertices;
     int numIndices;
     vector<int> indices;
     vector<vec2> texCoords;
     vector<vec3> vertices;
     vector<vec3> normals;
     //void init(int);
     //float toRadians(float degrees);

    public:
      

      Sphere(){//declare a default value Sphere precision
        init(48);
      }
      Sphere(int prec){
        init(prec);
      };
      
      int getNumVertices();
      int getNumIndices();
      vector<int> getIndices();
      vector<vec3> getVertices();
      vector<vec2> getTexCoords();
      vector<vec3> getNormals();

      float toRadians(int degrees){return (degrees * 2.0f * 3.14159f) / 360.0f;}

      void init(int prec){
           numVertices = (prec+1)*(prec+1);//initialize the number of vertices required to render Sphere
           numIndices  = prec * prec * 6;  //initialization of the number of indices to render the triangle needed to render sphere..
           for(int i=0; i<numVertices; i++){vertices.push_back(vec3());};
           for(int i=0; i<numVertices; i++){texCoords.push_back(vec2());};
           for(int i=0; i<numVertices; i++){normals.push_back(vec3());};
           for(int i=0; i<numIndices; i++){indices.push_back(0);};

           //calculate the number triangles per vertices...
           for(int i=0; i<=prec; i++){
            for(int j=0; j<=prec; j++){
               float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
               float x =  -(float)cos(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
               float z = (float)sin(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
               vertices[i*(prec + 1) + j] = glm::vec3(x, y, z);
			   texCoords[i*(prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
			   normals[i*(prec + 1) + j] = glm::vec3(x,y,z);

            }
           }
           //compute the indicies of the each triangle
           for (int i = 0; i<prec; i++) {
               for (int j = 0; j<prec; j++) {
                indices[6 * (i*prec + j) + 0] = i*(prec + 1) + j;
                indices[6 * (i*prec + j) + 1] = i*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 2] = (i + 1)*(prec + 1) + j;
                indices[6 * (i*prec + j) + 3] = i*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 4] = (i + 1)*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 5] = (i + 1)*(prec + 1) + j;
             }
            }


           

        }//void()

        


};

//define the accessors of the Sphere...
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
vector<int> Sphere::getIndices() { return indices; }
vector<vec3> Sphere::getVertices() { return vertices; }
vector<vec2> Sphere::getTexCoords() { return texCoords; }
vector<vec3> Sphere::getNormals() { return normals; }


// declare your classes here
class triangleFaces
{

public:
    vector<vec3> triangleVertex; // triangle vertices
    vector<vec3> colorVertex;    // color vertices
    vector<vec2> textCoord;
    vec3 objPos; // triangle position

    triangleFaces(){}; // empty constructor...

    triangleFaces(vector<vec3> triangleVertex, vector<vec3> colorVertex, vec3 objPos, vector<vec2> textCoord);
};

triangleFaces::triangleFaces(vector<vec3> triangleVertexs, vector<vec3> colors, vec3 objpos, vector<vec2> texTCoord)
{
    this->triangleVertex = triangleVertexs;
    this->colorVertex = colors;
    this->objPos = objpos;
    this->textCoord = texTCoord;
}
class rectangle
{

public:
    vector<vec3> vertexPos;
    vector<vec3> colorPos;
    vector<vec2> texturePos;
    rectangle(){}; // empty constructor
    rectangle(vector<vec3> vertexPos, vector<vec3> colorPos, vector<vec2> texturePos);
};

rectangle::rectangle(vector<vec3> vertexpos, vector<vec3> colorpos, vector<vec2> texturePos)
{
    this->vertexPos = vertexpos;
    this->colorPos = colorpos;
    this->texturePos=texturePos;
}

//############################################################ALL WINDOWS SETTING GOES HERE#####################################################
int width = 800;//1366;//1366;//1366;//
int height= 600;//768;//768; //768;//
int aspectRatio = width/height;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

// TIMING

float deltaTime = 0.0f;
float lastFrame = 0.0f;
// lighting properties...
//1. lightPos
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);//(1.2f, 1.0f, 2.0f);//(-0.2f, -1.0f, -0.3f);
vec3 lightPos(-2.0f, 4.0f, 4.0f);//(1.2f, 1.0f, 2.0f);
vec3 lightPos2(-1.5f, 1.0f, 2.0f);
//2. lightPos
vec3 lightPos3(-0.5f, 0.5f,-1.0f);
vec3 objColor(0.62f, 0.31f, 0.1f);
vec3 objColor2(0.71f, 0.40f, 0.11f);
vec3 lightColor( 1.0f, 1.0f, 1.0f);

vec3 viewPos(-1.5f, 1.5f, 4.0);


//camera properties

vec3 cameraPosition = camera.Position;
vec3 cameraDirection = camera.Front;

//defined vertex array object and buffer object...
unsigned int BaseVAO, BaseVBO, cubeVAO, cubeVBO, PlaneVAO, PlaneVBO, sphereVAO, sphereVBO, depthRenderBuffer,CubeMapShadowMap;
unsigned int QuadVAO, QuadVBO, SkyBoxVAO, SkyBoxVBO, depthMapFBO,  depthMap, reflectMap, reflectVBO,FBOCubeMapShadow;//, rbo, framebuffer;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLuint tileTexture, cubeTexture, sphereTexture, tennisTexture, textureColorbuffer;
//#################################################CALLBACK METHODS GOES HERE######################################################
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint loadcubemapimages(vector<string> faces);
GLuint loadTextures2Object(char const *imPath);



void renderScene(int ProgramID);
void renderSphere(int ProgramID);
void renderCube(int ProgramID);
void renderQuad(int ProgramID);
void PlaneObject(int programID);
void renderSkyBox();
void renderObject(int programID);
float intersectSphere(glm::vec3 position, float radius, Ray r);
float intersectBox(glm::vec3 position, glm::vec3 boxSize, Ray r);
//#####################################################NOTHING GOES HERE##########################################################

int main(){

    cout<<"REVIEW FRAMEBUFFER USING SHADOW MAPPING"<<endl;
    
    if (!glfwInit()){
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create windows
    GLFWwindow *window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(width, height, "REVIEW FRAMEBUFFER USING SHADOW MAPPING", NULL, NULL);

    if (window == NULL){
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }

    // create context window
    glfwMakeContextCurrent(window);
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    

    }

     /*  Callback function  */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    
    
    //DEFINE YOUR SHADERS...
    unsigned int MainProgramID = MyShader("shadowMappingVs.vs","shadowMappingFs.fs");
    unsigned int shadowProgramID = MyShader("shadowDepthMappingVs.vs","shadowDepthMappingFs.fs");
    unsigned int shadowQuadProgramID =  MyShader("shadowQuadMappingVs.vs","shadowQuadMappingFs.fs");
    
    vector<string> faces;
    faces.push_back("./Skybox/1.jpg");
    faces.push_back("./Skybox/2.jpg");
    faces.push_back("./Skybox/3.jpg");
    faces.push_back("./Skybox/4.jpg");
    faces.push_back("./Skybox/5.jpg");
    faces.push_back("./Skybox/6.jpg");

   
   

    cout<<"size of faces vector:"<<endl<< faces.size()<<endl;
    
    //cube map loader...
    GLuint skyboxMap =  loadcubemapimages(faces);

    //2d texture map loader...
    tileTexture= loadTextures2Object(faces[0].c_str());
    //glUniform1i(glGetUniformLocation(ShadowMainProgramID, "diffuseTexture"),0);//parsing textures...
    
    string cubepath = "1000_F_199149981_RG8gciij11WKAQ5nKi35Xx0ovesLCRaU.jpg";
    cubeTexture = loadTextures2Object(cubepath.c_str());
    
    string tennisPath ="TennisBall.jpg";
    tennisTexture = loadTextures2Object(tennisPath.c_str());
    
    
    glUniform1i(glGetUniformLocation(MainProgramID, "diffuseTexture"),0);
    glUniform1i(glGetUniformLocation(MainProgramID, "shadowMap"),1);
    
    //configure shadow depth map
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    
    
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    do{
        
         // bind to framebuffer and draw scene as we normally would to color texture 
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        
         
        mat4 model = mat4(1.0); 
        float aspectratio = (float)width/height;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        vec3 cameraPos = camera.Position;
       
        //shadow rendering...
        glUseProgram(shadowProgramID);
        glUniformMatrix4fv(glGetUniformLocation(shadowProgramID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shadowProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            renderScene(shadowProgramID);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

       
        // reset viewport
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        //normal rendering...
        glUseProgram(MainProgramID);
        glUniformMatrix4fv(glGetUniformLocation(MainProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(MainProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(MainProgramID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform3fv(glGetUniformLocation(MainProgramID, "viewPos"), 1, &cameraPos[0]);
        glUniform3fv(glGetUniformLocation(MainProgramID, "lightPos"), 1, &lightPos[0]);
        //glUniform1i(glGetUniformLocation(ShadowMainProgramID, "aspectRatio"), aspectratio);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        //glBindTexture(GL_TEXTURE_2D, tennisTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        
        renderScene(MainProgramID);
       
         //shadow rendering...
        glUseProgram(shadowQuadProgramID);
        glUniform1f(glGetUniformLocation(shadowQuadProgramID, "near_plane"), near_plane);
        glUniform1f(glGetUniformLocation(shadowQuadProgramID, "far_plane"), far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        
        //renderQuad(shadowQuadProgramID);
 

         //clearing the buffer memory...
          glBindVertexArray(0);
          glfwSwapBuffers(window);
          glfwPollEvents();

   }while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

         //clear Array Buffers
         glDeleteBuffers(1, &BaseVBO);
         glDeleteBuffers(1, &PlaneVBO);
         glDeleteBuffers(1, &sphereVBO);
         glDeleteBuffers(1,&SkyBoxVBO);
         glDeleteBuffers(1, &depthMapFBO);


         //clear vertex buffers
         glDeleteVertexArrays(1, &BaseVAO);
         glDeleteVertexArrays(1,&sphereVAO);
         glDeleteVertexArrays(1, &PlaneVAO);
         glDeleteVertexArrays(1, &SkyBoxVAO);
         glDeleteVertexArrays(1, &depthMap);
         
         //glDeleteRenderbuffers(1, &rbo);
         //glDeleteFramebuffers(1, &framebuffer);

        glfwTerminate();
        return 0;

}


void renderScene(int programID){
 
        //create floor 
        glm::mat4 model = glm::mat4(1.0f);
        glActiveTexture(GL_TEXTURE);
        glBindTexture(GL_TEXTURE_2D, tileTexture);
        glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));         
        PlaneObject(programID);
       
       
       //cube  one
       model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
       model = glm::scale(model, glm::vec3(0.5f));
       glActiveTexture(GL_TEXTURE);
       glBindTexture(GL_TEXTURE_2D, cubeTexture);
       glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
       renderCube(programID);
        
        //cube two
       model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
       model = glm::scale(model, glm::vec3(0.5f));
       glActiveTexture(GL_TEXTURE);
       glBindTexture(GL_TEXTURE_2D, cubeTexture);
       glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
       renderCube(programID);
       
       //cube three
       model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
       model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
       model = glm::scale(model, glm::vec3(0.25));
       glActiveTexture(GL_TEXTURE);
       glBindTexture(GL_TEXTURE_2D, cubeTexture);
       glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
       renderCube(programID);
       
       
       //render sphere one...
       
       model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(2.0f, 1.5f, 0.0));
       model = glm::scale(model, glm::vec3(0.5f));
       glActiveTexture(GL_TEXTURE);
       glBindTexture(GL_TEXTURE_2D, tennisTexture);
       glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
       renderSphere(programID);
       
       
       //render sphere two ...

       model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(4.0f, 0.0f, 1.0));
       model = glm::scale(model, glm::vec3(0.5f));
       glActiveTexture(GL_TEXTURE);
       glBindTexture(GL_TEXTURE_2D, tennisTexture);
       glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
       renderSphere(programID);
       
       
       
       //cube three
       model = glm::mat4(1.0f);
       model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 2.0));
       model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
       model = glm::scale(model, glm::vec3(0.25));
       glActiveTexture(GL_TEXTURE);
       glBindTexture(GL_TEXTURE_2D, tennisTexture);
       glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model)); 
       renderSphere(programID);
       
       
}







void PlaneObject(int programID){
     vector<vec3> PlaneVertices;
     vector<vec3> PlaneColorVertices;
     vector<vec2> PtextureCoord;
     rectangle planeFaces(vector<vec3>{vec3(25.0f, -0.5f,  25.0f),//plane vertex position...
                                       vec3(-25.0f, -0.5f,  25.0f),
                                       vec3(-25.0f, -0.5f, -25.0f),
                                       vec3( 25.0f, -0.5f,  25.0f),
                                       vec3(-25.0f, -0.5f, -25.0f),
                                       vec3( 25.0f, -0.5f, -25.0f)}, 
                          vector<vec3>{vec3(0.0f, 1.0f, 0.0f),//Normal...
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f), 
                                       vec3( 0.0f, 1.0f, 0.0f)}, 
                          vector<vec2>{vec2(25.0f,  0.0f),//Texture Coordinate..
                                       vec2(0.0f, 0.0f), 
                                       vec2( 0.0f, 25.0f),
                                       vec2(25.0f,  0.0f),
                                       vec2(0.0f,  25.0f),
                                       vec2(25.0f, 25.0f)}); 
                                       
     //plane properties
    for(int ix = 0; ix<planeFaces.vertexPos.size(); ix++){
        PlaneVertices.push_back(planeFaces.vertexPos[ix]);
        PlaneColorVertices.push_back(planeFaces.colorPos[ix]);
        PtextureCoord.push_back(planeFaces.texturePos[ix]);
    }
    
       //plane buffer VAO, VBO;
        glGenBuffers(1, &PlaneVBO);
        glGenVertexArrays(1, &PlaneVAO);
        //plane vertex
        //plane  vertex position to BUFFER
        glBindVertexArray(PlaneVAO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER, PlaneVertices.size() * sizeof(vec3), & PlaneVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        
        
        //bind plane vertex to Normal

        glBindVertexArray(PlaneVAO);
        glGenBuffers(1, &PlaneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER,  PlaneColorVertices.size()*sizeof(vec3), &PlaneColorVertices[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);

        
        
        
        
        //Plane texCoord
        glBindVertexArray(PlaneVAO);
        glGenBuffers(1, &PlaneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER,   PtextureCoord.size()*sizeof(vec2), &PtextureCoord[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        
        
        
        glBindVertexArray(PlaneVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);





}
void renderCube(int programID){

    
            //cube vertex
    vector<vec3> cubeVertices;
    vector<vec3> cubeColorVertices;
    vector<vec2> textureCoord;
    rectangle cubeFaces(vector<vec3>{//TOP SIDE 1
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f, -1.0f), 
                                        vec3(1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f),

                                        //RIGHT SIDE 2
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f), 
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),

                                        //BOTTOM SIDE 3
                                        vec3(-1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f), 
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f),

                                        //LEFT SIDE 4
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f, -1.0f, -1.0f), 
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f),

                                        //FRONT SIDE 5
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(1.0f, -1.0f, -1.0f), 
                                        vec3(1.0f, -1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),

                                        //BACK SIDE 6
                                        vec3(-1.0f,  1.0f, -1.0f),
                                        vec3(1.0f,  1.0f , 1.0f), 
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f)
    }, vector<vec3>{
            //NORMAL 01 / COLOR 01

            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            //NORMAL 02 / COLOR 02
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            //NORMAL 03 / COLOR 03
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            //NORMAL 04 / COLOR 04
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            //NORMAL 05/COLOR 05

            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            //NORMAL 06/COLOR 06
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f)
    }, vector<vec2>{
                //texture 01
                vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    //texture 02

                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    //texture 03

                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    //texture 04

                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    //texture 05

                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    //texture 06

                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f)
                        
            });
    //cube properties
    for(int ix = 0; ix<cubeFaces.vertexPos.size(); ix++){
        cubeVertices.push_back(cubeFaces.vertexPos[ix]);
        cubeColorVertices.push_back(cubeFaces.colorPos[ix]);
        textureCoord.push_back(cubeFaces.texturePos[ix]);
    }

    //cube VAO, VBO
//cube buffer VAO, VBO;
glGenBuffers(1, &cubeVBO);
glGenVertexArrays(1, &cubeVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(cubeVAO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(vec3), & cubeVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);


//bind cube vertex to Normal

glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,  cubeColorVertices.size()*sizeof(vec3), &cubeColorVertices[0], GL_STATIC_DRAW);   
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(1);

//cube texCoord
glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,   textureCoord.size()*sizeof(vec2), &textureCoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);

/*glActiveTexture(GL_TEXTURE0);
glUniform1i(glGetUniformLocation(programID, "diffuseTexture"), 0);
glBindTexture(GL_TEXTURE_2D,cubeTexture);*/


glBindVertexArray(cubeVAO);
glDrawArrays(GL_TRIANGLES, 0, 36);
glBindVertexArray(0);



}

Sphere mySphere(48);
void renderSphere(int programID){
     
     
    //sphere VAO and VBO 
   //let's create a sphere 
    
        vector<int> indx = mySphere.getIndices();
        vector<vec3> vert = mySphere.getVertices();
        vector<vec2> tex = mySphere.getTexCoords();
        vector<vec3> norm = mySphere.getNormals();

        vector<float> pvalues;// vertex positions		
        vector<float> tvalues;// texture coordinates		
        vector<float> nvalues;// normal vectors		

        int numIndices = mySphere.getNumIndices();
        for (int i = 0; i<numIndices; i++ ){
            //vertex position
            pvalues.push_back((vert[indx[i]]).x);
            pvalues.push_back((vert[indx[i]]).y);
            pvalues.push_back((vert[indx[i]]).z);

            //texture position
            tvalues.push_back((tex[indx[i]]).s);
            tvalues.push_back((tex[indx[i]]).t);

            //normal vectors
            nvalues.push_back((norm[indx[i]]).x);
            nvalues.push_back((norm[indx[i]]).y);
            nvalues.push_back((norm[indx[i]]).z);
        }


    
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);

        //pyramid position...
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0,4, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        
        //Texture vertex position buffer
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER,  tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);


        //Normal vertex position buffer... based on the last failed codes, because the VBO(vertex buffer object) wasn't well initialized and binded to the array vertex
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);   
        //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);


        /*glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(programID, "diffuseTexture"), 0);
        glBindTexture(GL_TEXTURE_2D,sphereTexture);*/



    glBindVertexArray(sphereVAO);
    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
    glBindVertexArray(0);

    



}

void renderQuad(int ProgramID){
    vector<vec3> quadVertices;
    vector<vec3> quadNormal;
    vector<vec2> quadTexcoord;
    rectangle Quad(vector<vec3>{
              vec3(-1.0f,  1.0f, 0.0f),
              vec3(-1.0f, -1.0f, 0.0f), 
              vec3(1.0f,  1.0f, 0.0f),
              vec3(1.0f, -1.0f, 0.0f)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    }, vector<vec3>{
             



    }, vector<vec2>{
        vec2(0.0f, 1.0f), 
        vec2(0.0f, 0.0f), 
        vec2(1.0f, 1.0f), 
        vec2(1.0f, 0.0f)
    });



     for(int ix = 0; ix<Quad.vertexPos.size(); ix++){
        quadVertices.push_back(Quad.vertexPos[ix]);
        //quadNormal.push_back(Quad.colorPos[ix]);
        quadTexcoord.push_back(Quad.texturePos[ix]);
    }



    //cube buffer VAO, VBO;
glGenBuffers(1, &QuadVBO);
glGenVertexArrays(1, &QuadVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(QuadVAO);
glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(vec3), &quadVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);


//bind cube vertex to Normal

/*glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,  cubeColorVertices.size()*sizeof(vec3), &cubeColorVertices[0], GL_STATIC_DRAW);   
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(1);*/

//cube texCoord
glBindVertexArray(QuadVAO);
glGenBuffers(1, &QuadVBO);
glBindBuffer(GL_ARRAY_BUFFER,QuadVBO);
glBufferData(GL_ARRAY_BUFFER,   quadTexcoord.size()*sizeof(vec2), &quadTexcoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);


glBindVertexArray(QuadVAO);
glDrawArrays(GL_TRIANGLES, 0, 6);
glBindVertexArray(0);



}

void renderSkyBox(){

     //SKYBOX
    vector<vec3>  SkyBoxVertices;
   vector<vec3>  SkyBoxColorVertices;
   rectangle SkyBoxFaces(vector<vec3>{//TOP SIDE 1
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f, -1.0f), 
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),

                                    //RIGHT SIDE 2
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),

                                     //BOTTOM SIDE 3
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),


                                     //LEFT SIDE 4
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),

                                    //FRONT SIDE 5
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    //FRONT SIDE 6
                                    vec3(-1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(1.0f, -1.0f,  1.0f)

   }, vector<vec3>{}, vector<vec2>{});
   //SKYBOX properties
   for(int ix = 0; ix< SkyBoxFaces.vertexPos.size(); ix++){
      SkyBoxVertices.push_back( SkyBoxFaces.vertexPos[ix]);
   }

    //cube VAO, VBO
    //cube buffer VAO, VBO;
    glGenBuffers(1, &SkyBoxVBO);
    glGenVertexArrays(1, &SkyBoxVAO);
    //cube vertex
    //bind cube vertex to BUFFER
    glBindVertexArray(SkyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, SkyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, SkyBoxVertices.size() * sizeof(vec3), &SkyBoxVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);




}




void renderObject(unsigned int ProgramID){
     
            //render cubes...
       for(int i = 0; i<4; i++){
            //bronze material properties...
            vec3  bronze_Amb = vec3(0.2125f, 0.1275f, 0.0540f);//ambient material
            vec3  bronze_diff = vec3(0.7140f, 0.4284f, 0.1814f );
            vec3  bronze_specular = vec3(0.3935f, 0.2719f, 0.1667f);
            vec3  bronze_light = vec3(0.7);
            
            float bronze_shininess = 64.0f;
            glUniform3fv(glGetUniformLocation(ProgramID, "viewPos"), 1, &cameraPosition[0]);
            glUniform3fv(glGetUniformLocation(ProgramID, "light.ambient"), 1, &bronze_light[0]);
            glUniform3fv(glGetUniformLocation(ProgramID, "material.ambient"), 1, &bronze_Amb[0]);
            glUniform3fv(glGetUniformLocation(ProgramID, "material.diffuse"), 1, &bronze_diff[0]);
            glUniform3fv(glGetUniformLocation(ProgramID, "material.specular"), 1, &bronze_specular[0]);
            glUniform1f(glGetUniformLocation(ProgramID,  "material.shininess"),bronze_shininess);
           // glUniform1f(glGetUniformLocation(OBJPROGRAMID,  "material.shininess"),bronze_shininess);



            //glUniform3fv(glGetUniformLocation(OBJPROGRAMID, "color"), 1, &colorBrown[0]);
            vec3 pos = vec3(sin(45.0f+i)+1.5,-cos(90.0f*i), (0.0f-0.5f)*i);//pos =vec3(-0.2+(i*-1.5), 0.0, 0.0);
            mat4 modelCube = mat4(1.0);
            modelCube = rotate(modelCube, (float)radians(90.0), vec3(0.0f, 0.0f, -1.0f));
            modelCube = translate(modelCube, pos);
            glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelCube));
            renderCube(ProgramID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //cout<<"x: "<<pos.x<<"y: "<<pos.y<<"z: "<<pos.z<<endl;
            

       }
       


       ///render red sphere
       for(int i =0; i<4; i++){   
         //material silver properties
        vec3  silver_Amb = vec3(0.19225, 0.19225, 0.19225);
        vec3  silver_diff = vec3(0.50754, 0.50754, 0.50754);
        vec3  silver_specular = vec3(0.508273, 0.508273, 0.508273);
         vec3 silver_light = vec3(1.0);
        float silver_shininess = 64.0f;
        glUniform3fv(glGetUniformLocation(ProgramID, "viewPos"), 1, &cameraPosition[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "light.ambient"), 1, &silver_light[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "material.ambient"), 1, &silver_Amb[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "material.diffuse"), 1, &silver_diff[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "material.specular"), 1, &silver_specular[0]);
        glUniform1f(glGetUniformLocation(ProgramID,  "material.shininess"),silver_shininess);

        mat4 modelSphereRedBalls = mat4(1.0);   
        modelSphereRedBalls = rotate(modelSphereRedBalls, (float)radians(90.0), vec3(0.0, 0.0, -1.0)); 
        modelSphereRedBalls = rotate(modelSphereRedBalls, (float)radians(90.0), vec3(0.0, -1.0, 0.0)); 
        modelSphereRedBalls = rotate(modelSphereRedBalls, (float)radians(90.0), vec3(-1.0, 0.0, 0.0)); 
        modelSphereRedBalls = translate(modelSphereRedBalls, vec3(-2.0+(i*2.5), 3.5, -4.5));
        //glUniform3fv(glGetUniformLocation(OBJPROGRAMID, "color"), 1, &colorRedBalls[0]);
        glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelSphereRedBalls));
        renderSphere(ProgramID);
        
        }

         ///render black sphere
       for(int i =0; i<4; i++){ 
        //material gold properties...
        vec3  gold_Amb = vec3(0.24725, 0.1995, 0.0745);
        vec3  gold_diff = vec3(0.75164,0.60648, 0.22648 );
        vec3  gold_specular = vec3(0.628281, 0.628281,0.555802);
        vec3 gold_light = vec3(1.0);
        float gold_shininess = 64.0f;
        glUniform3fv(glGetUniformLocation(ProgramID, "viewPos"), 1, &cameraPosition[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "light.ambient"), 1, &gold_light[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "material.ambient"), 1, &gold_Amb[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "material.diffuse"), 1, &gold_diff[0]);
        glUniform3fv(glGetUniformLocation(ProgramID, "material.specular"), 1, &gold_specular[0]);
        glUniform1f(glGetUniformLocation(ProgramID,  "material.shininess"),gold_shininess);

        mat4 modelBlackBalls = mat4(1.0);
        modelBlackBalls = rotate(modelBlackBalls, (float)radians(90.0), vec3(0.0, 0.0, 1.0)); 
        modelBlackBalls = rotate(modelBlackBalls, (float)radians(-90.0), vec3(0.0, 1.0, 0.0));
        modelBlackBalls = rotate(modelBlackBalls,(float)radians(90.0), vec3(1.0, 0.0, 0.0));
        modelBlackBalls = translate(modelBlackBalls, vec3(2.0+(i*2.5), 4.5, 4.0));
        //glUniform3fv(glGetUniformLocation(OBJPROGRAMID, "color"), 1, &colorBlackBalls[0]);
        glUniformMatrix4fv(glGetUniformLocation(ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(modelBlackBalls));
        renderSphere(ProgramID);
        
        }




}






void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime); // move up
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime); // move DOWN
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

GLuint loadcubemapimages(vector<string> faces){
    /* 
    TextureID-> return textureID
    ImageVars-> include the width, height and image array itself
    
    */
    GLuint TextureID;
    glGenTextures(1, &TextureID);
    glActiveTexture(GL_TEXTURE0);
    
    int width, height, nrChannels;
    unsigned char* image;
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
    
    for(int i =0; i<faces.size(); i++){
    
       image = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
       stbi_set_flip_vertically_on_load(true);
      
       
       if (image){
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
          stbi_image_free(image);
         
       }else{
          std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
          stbi_image_free(image);
         
       }
    
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    
    
    return TextureID;

}


//load texture 

GLuint loadTextures2Object(char const *imPath){

       GLuint textureID;
       unsigned char* image;
       glGenTextures(1, &textureID);
       int width,height, nrChannels;
       
       image = stbi_load(imPath , &width, &height, &nrChannels, 0);
       
       // Assign texture to ID
       glBindTexture(GL_TEXTURE_2D, textureID);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
       glGenerateMipmap(GL_TEXTURE_2D);	
 
       // Parameters
       
       if(image){
        GLenum imgFormat;
        if(nrChannels == 1)
          imgFormat = GL_RED;
        else if(nrChannels == 3)
          imgFormat = GL_RGB;
        else if(nrChannels == 4)
           imgFormat = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, imgFormat, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(image);
        stbi_set_flip_vertically_on_load(true);

        }else{
            cout << "Texture failed to load at path: " << imPath << std::endl;
            stbi_image_free(image);
        };
       
       return textureID;




}


float intersectSphere(glm::vec3 position, float radius, Ray r){
      float a  = dot(r.direction(), r.direction());
      float b  = dot(2.0f * r.direction(), r.origin()-position);
      float c  = dot(r.origin() - position,  r.origin() - position) - (radius * radius);
      float d  = (b*b) - 4.0f * a * c;
      
       if (d < 0) return -1;
      //compute the t1 and t2
      float t1 = (-b + sqrt(d))/(2*a);
      float t2 = (-b - sqrt(d))/(2*a);
      
      
      // compute the closest t 
      float tNear = std::min(t1, t2);
      float tFar=   std::max(t1, t2);
      
      if(tFar < 0) return -1;
      
      if(tNear<0) return tFar;

};




float intersectBox(glm::vec3 position, glm::vec3 boxSize, Ray r){
      float t;
      vec3 boxMin = position - boxSize / 2.0f;
      vec3 boxMax = position + boxSize / 2.0f;
      
      vec3 t1 = (boxMin - r.origin()) / r.direction(); //compute the t1
      vec3 t2 = (boxMax - r.origin()) / r.direction(); //compute the t2
      
      vec3 tMin = glm::min(t1, t2);
      vec3 tMax = glm::max(t1, t2);
      
      float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
      float tFar =  glm::min(glm::min(tMax.x, tMax.y), tMax.z);
      
      
      if (tNear >= tFar || tFar <= 0.0)
          { 
            t = -1.0;
            cout<<"t Near: "<<t<<endl;
            return t;
            
          }
          
          
      if (tNear < 0.0)
          {   
                t = tFar;
                cout<<"t Far:"<<t<<endl;
		return t;
          }



};


