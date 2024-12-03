#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>  // GLFW 대신 GLUT 헤더를 포함

#pragma comment(lib, "glew32.lib")
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Model.h"
#include "Animation.h"
#include "Animator.h"
#include "FrameBuffer.h"
#include "FreeCamera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Skybox.h"

#include "ShaderHandle.h"

#define WIDTH 1280
#define HEIGHT 720

int lastX, lastY;
bool key_f1 = 1;

// Global variables
Window* mainWindow;
//Camera* camera;
CameraBase* currCamera;
FreeCamera* freeCamera;
PlayerCamera* playerCamera;


GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

std::vector<Mesh*> meshList;
std::vector<Entity*> entityList;


//Model* model_2B;
Model* mainModel;
Model* cube;
Model* currModel;

Player* player;

Animator* animator;

Animation* idleAnim;
Animation* danceAnim;
Animation* runAnim;

DirectionalLight* directionalLight;
PointLight* pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

Skybox* skybox;

extern GLuint loc_modelMat;
extern GLuint loc_PVM;
extern GLuint loc_diffuseSampler;
extern GLuint loc_normalSampler;
extern GLuint loc_normalMat;
extern GLuint loc_eyePos;
extern GLuint loc_finalBonesMatrices;
// Keyboard and mouse control

void TimerFunction(int value);

//키보드 눌림 함수
void processKeyboard(unsigned char key, int x, int y) {
    if (key_f1) {
        currCamera->KeyControl(key, deltaTime);
    }
    else {
        player->HandleInput(key,1, deltaTime);
    }
    if (key == '1') {
        if (animator->GetCurrAnimation() != idleAnim)
            animator->PlayAnimation(idleAnim);
    }
    if (key == '2') {
        if (animator->GetCurrAnimation() != danceAnim)
            animator->PlayAnimation(danceAnim);
    }
    if (key == '3') {
        if (animator->GetCurrAnimation() != runAnim)
            animator->PlayAnimation(runAnim);
    }

}
//키보드 떼어짐 함수
void processKeyboardUp(unsigned char key, int x, int y) {
    player->HandleInput(key, 0, deltaTime);
    if (key == 'w') {
        
    }
}

void SpecialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        key_f1 = !key_f1;
        break;
    case GLUT_KEY_F2:
        key_f1 = 0;
        currCamera = playerCamera;
        break;
    case GLUT_KEY_F3:
        currCamera = freeCamera;
        break;
    }
}

void MoveCamera()
{
    //currCamera->KeyControl(mainWindow->getsKeys(), deltaTime);
    //currCamera->ScrollControl(mainWindow->GetScrollYChange());
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

//--------------------------------

void update() {
    
    player->update(deltaTime);
    currCamera->Update();
}

void mainInit() {
    CreateShader();
    
    //빛
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

    // Skybox
    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Skybox/px.png");
    skyboxFaces.push_back("Skybox/nx.png");
    skyboxFaces.push_back("Skybox/py.png");
    skyboxFaces.push_back("Skybox/ny.png");
    skyboxFaces.push_back("Skybox/pz.png");
    skyboxFaces.push_back("Skybox/nz.png");
    skybox = new Skybox(skyboxFaces);

    // Model loading
    mainModel = new Model();
    std::string modelPath = "Knight/test.gltf";
    //std::string modelPath = "obj/test.gltf";
    mainModel->LoadModel(modelPath);
    entityList.push_back(mainModel);
    currModel = mainModel;

    //모델 90도 회전
    GLfloat* currRot = mainModel->GetRotate();
    float rotation = 90;
    float newRotx = currRot[0] + rotation;
    glm::vec3 newRot(newRotx, currRot[1], currRot[2]);
    mainModel->SetRotate(newRot);

    cube = new Model();
    modelPath = "obj/test.gltf";
    cube->LoadModel(modelPath);
    entityList.push_back(cube);

    animator = new Animator(nullptr);

    //플레이어 연결
    player = new Player(mainModel,animator);

    freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 2.f), 100.f, 0.3f);
    playerCamera = new PlayerCamera(player);
    currCamera = freeCamera;

    
    /*idleAnim = new Animation("Knight/idle.gltf", currModel);
    danceAnim = new Animation("Knight/dance.gltf", currModel);
    runAnim = new Animation("Knight/run.gltf", currModel);*/

}

GLvoid render()
{
    
    update();
    
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

    glm::mat4 viewMat = currCamera->GetViewMatrix();
    glm::mat4 projMat = currCamera->GetProjectionMatrix(1280, 720);
    glm::vec3 camPos = currCamera->GetPosition();

    skybox->DrawSkybox(viewMat, projMat);

    /*
    terrain->UseShader();
    terrain->GetShader()->UseDirectionalLight(directionalLight);
    terrain->GetShader()->UsePointLights(pointLights, pointLightCount);
    terrain->GetShader()->UseEyePos(camPos);
    terrain->DrawTerrain(viewMat, projMat);*/

    //전체가 플레이어 그리기
    
    
    player->draw(currCamera, directionalLight, pointLights, pointLightCount);
    /*{
        GetShaderHandles();

        Model* currModel = mainModel;

        glm::mat4 modelMat = mainModel->GetModelMat();
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
    }*/
    shaderList[0]->UseShader();
    {
       GetShaderHandles();

        glm::mat4 modelMat = cube->GetModelMat();
        glm::mat4 PVM = projMat * viewMat * modelMat;
        glm::mat3 normalMat = GetNormalMat(modelMat);

        glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
        glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        shaderList[0]->UseEyePos(camPos);
        shaderList[0]->UseDirectionalLight(directionalLight);
        shaderList[0]->UsePointLights(pointLights, pointLightCount);

        shaderList[0]->UseMaterial(cube->GetMaterial());

        /*const auto& transforms = animator->GetFinalBoneMatrices();
        shaderList[0]->UseFinalBoneMatrices(transforms);*/

        glUniform1i(loc_diffuseSampler, 0);
        glUniform1i(loc_normalSampler, 1);

        cube->RenderModel();

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

    mainInit();

    // Set callbacks
    glutDisplayFunc(render);
    glutReshapeFunc(handleResize);
    glutSpecialFunc(SpecialKeyboard);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(processKeyboard);
    glutKeyboardUpFunc(processKeyboardUp);
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