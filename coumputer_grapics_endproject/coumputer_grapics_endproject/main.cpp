#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>  // GLFW 대신 GLUT 헤더를 포함

#pragma comment(lib, "glew32.lib")
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>


#include <assimp/Importer.hpp>

//#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
//#include "Window.h"
#include "Model.h"
#include "Animation.h"
#include "Animator.h"
//#include "ScenePanel.h"
#include "FrameBuffer.h"
#include "FreeCamera.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#define WIDTH 1280
#define HEIGHT 720

int lastX, lastY;

// Global variables
Window* mainWindow;
//Camera* camera;
CameraBase* currCamera;
FreeCamera* freeCamera;


GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

// Vertex Shader
static const char* vShaderPath = "vertex.glsl";

// Fragment Shader
static const char* fShaderPath = "fragment.glsl";

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
std::vector<Entity*> entityList;


//Model* model_2B;
Model* mainModel;
Model* currModel;

Animator* animator;

Animation* idleAnim;
Animation* runAnim;

DirectionalLight* directionalLight;
PointLight* pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

//ScenePanel* scenePanel;

// Shader handles
GLuint loc_modelMat = 0;
GLuint loc_PVM = 0;
GLuint loc_diffuseSampler = 0;
GLuint loc_normalSampler = 0;
GLuint loc_normalMat = 0;
GLuint loc_eyePos = 0;
GLuint loc_finalBonesMatrices = 0;

glm::mat3 GetNormalMat(glm::mat4& modelMat)
{
    return glm::mat3(glm::transpose(glm::inverse(modelMat)));
}
// Shader creation
void CreateShader()
{
    Shader* shader = new Shader;
    shader->CreateFromFiles(vShaderPath, fShaderPath);
    shaderList.push_back(shader);
}

void GetShaderHandles()
{
    // Get shader handles
    loc_modelMat = shaderList[0]->GetModelMatLoc();
    loc_PVM = shaderList[0]->GetPVMLoc();
    loc_normalMat = shaderList[0]->GetNormalMatLoc();
    loc_eyePos = shaderList[0]->GetEyePosLoc();
    loc_finalBonesMatrices = shaderList[0]->GetFinalBonesMatricesLoc();
    loc_diffuseSampler = shaderList[0]->GetColorSamplerLoc();
    loc_normalSampler = shaderList[0]->GetNormalSamplerLoc();
}

// Keyboard and mouse control

void TimerFunction(int value);

void processKeyboard(unsigned char key, int x, int y) {
    currCamera->KeyControl(key, deltaTime);
    if (key == '1') {
        if (animator->GetCurrAnimation() != idleAnim)
            animator->PlayAnimation(idleAnim);
    }
    if (key == '2') {
        if (animator->GetCurrAnimation() != runAnim)
            animator->PlayAnimation(runAnim);
    }
}

void MoveCamera()
{
    //currCamera->KeyControl(mainWindow->getsKeys(), deltaTime);

    //currCamera->ScrollControl(mainWindow->GetScrollYChange());
    currCamera->Update();
}

bool click = 0;
void processMouse(int x, int y) {
    if (!click) {
        GLfloat XChange = x - lastX;
        GLfloat YChange = lastY - y;
        currCamera->MouseControl(XChange, YChange);
    }
    lastX = x;
    lastY = y;
    currCamera->Update();
}

void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        click = 1;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        click = 0;
    }
}
void Motion(int x, int y) {
    lastX = x;
    lastY = y;
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid render()
{

    GLfloat now = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = now - lastTime;
    lastTime = now;
    //std::cout << "deltaTime"<< deltaTime << std::endl;

    //--------------

    // Clear the window
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //무언가 바인드

    animator->UpdateAnimation(deltaTime);


    glm::mat4 viewMat = currCamera->GetViewMatrix();
    glm::mat4 projMat = currCamera->GetProjectionMatrix(1280, 720);
    glm::vec3 camPos = currCamera->GetPosition();

    /*skybox->DrawSkybox(viewMat, projMat);

    terrain->UseShader();
    terrain->GetShader()->UseDirectionalLight(directionalLight);
    terrain->GetShader()->UsePointLights(pointLights, pointLightCount);
    terrain->GetShader()->UseEyePos(camPos);
    terrain->DrawTerrain(viewMat, projMat);*/


    shaderList[0]->UseShader();
    {
        GetShaderHandles();

        Model* currModel = mainModel;

        glm::mat4 modelMat = currModel->GetModelMat();
        glm::mat4 PVM = projMat * viewMat * modelMat;
        glm::mat3 normalMat = GetNormalMat(modelMat);

        glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));


        glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
        glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        shaderList[0]->UseEyePos(camPos);
        shaderList[0]->UseDirectionalLight(directionalLight);
        shaderList[0]->UsePointLights(pointLights, pointLightCount);

        shaderList[0]->UseMaterial(mainModel->GetMaterial());

        const auto& transforms = animator->GetFinalBoneMatrices();
        shaderList[0]->UseFinalBoneMatrices(transforms);

        glUniform1i(loc_diffuseSampler, 0);
        glUniform1i(loc_normalSampler, 1);


        mainModel->RenderModel();

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cout << "error : " << error << std::endl;
        }
    }


    glutSwapBuffers();  // Swap buffers to render
}

int main(int argc, char** argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("OpenGL with Assimp and GLUT");

    // Initialize GLEW
    glewInit();

    /*mainWindow = new Window(WIDTH, HEIGHT);
    mainWindow->Initialise();*/


    CreateShader();

    directionalLight = new DirectionalLight(0.5f, 1.f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(0.f, 1.f, 1.f));
    entityList.push_back(directionalLight);

    pointLights[0] = new PointLight
    (0.f, 0.5f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(0.f, 1.5f, 0.2f),
        1.0f, 0.22f, 0.20f);
    pointLightCount++;
    pointLights[1] = new PointLight
    (0.0f, 0.5f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(-2.0f, 2.0f, -1.f),
        1.0, 0.045f, 0.0075f);
    pointLightCount++;
    for (int i = 0; i < pointLightCount; i++)
        entityList.push_back(pointLights[i]);

    // Camera setup
    //GLfloat initialPitch = 0.f;
    //GLfloat initialYaw = 90.f; // Camera starts facing -z axis
    //camera = new Camera(glm::vec3(0.f, 100.f, 0.f), glm::vec3(0.f, 1.f, 0.f), initialYaw, initialPitch, 5.f, 0.3f);


    // Model loading
    mainModel = new Model();
    std::string modelPath = "Knight/test.gltf";
    //std::string modelPath = "Bot/bot_run.gltf";
    mainModel->LoadModel(modelPath);
    entityList.push_back(mainModel);
    currModel = mainModel;

    GLfloat* currRot = mainModel->GetRotate();
    float rotation = 90;

    float newRotx = currRot[0] + rotation;
    glm::vec3 newRot(newRotx, currRot[1], currRot[2]);

    mainModel->SetRotate(newRot);

    freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 2.f), 100.f, 0.3f);
    //playerCamera = new PlayerCamera(player);
    currCamera = freeCamera;

    //GLfloat initialPitch = 0.f;
    //GLfloat initialYaw = -90.f; // 카메라가 -z축을 보고 있도록
    //camera = new Camera(glm::vec3(0.f, 0.f, 20.f), glm::vec3(0.f, 1.f, 0.f), initialYaw, initialPitch, 10.f, 0.3f);

    //idleAnim = new Animation("robot_run.fbx", currModel);
    //idleAnim = new Animation("Bot/bot_run.gltf", currModel);
    idleAnim = new Animation("Knight/idle.gltf", currModel);
    runAnim = new Animation("Knight/dance.gltf", currModel);

    animator = new Animator(nullptr);


    //FrameBuffer sceneBuffer(mainWindow->getBufferWidth(), mainWindow->getBufferHeight());
    //mainWindow->SetSceneBuffer(&sceneBuffer);


    // Set callbacks
    glutDisplayFunc(render);
    glutReshapeFunc(handleResize);

    glutMouseFunc(Mouse);
    glutKeyboardFunc(processKeyboard);
    glutPassiveMotionFunc(processMouse);
    glutMotionFunc(Motion);
    //glutTimerFunc(1, TimerFunction, 1);
    glutIdleFunc(render);  // Continuously render when idle

    // Start GLUT main loop
    glutMainLoop();

    return 0;
}

void TimerFunction(int value) {
    //glutPostRedisplay(); // 화면 재 출력
    glutTimerFunc(1, TimerFunction, 1);
}