/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GLApplication.cpp
Purpose: This file serves as the driver for the implementation of assignment 1.
Language: C++ and msvc compiler
Platform: Most up to date version of msvc compiler, opengl ver450. Only works on windows.
Project: mark.kouris_CS300_2
Author: Mark Kouris, mark.kouris, mark.kouris
Creation date: 10/12/21
End Header --------------------------------------------------------*/

#include <Windows.h>
// Include standard headers
#include <cstdio>
#include <vector>
#include <sstream>

//imgui
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Local / project headers
#include "orbitRender.h"
#include "Scene.h"
#include "shader.hpp"
#include "LightManager.h"
#include "Light.h"
#include "orbitSegment.h"

//function decs for input
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

////////////////// Globals For Imgui ////////////////////////////////////////////////////
void imGuiStartup();
GLFWwindow* window;
Scene* scene;
orbit* orbit_;
LightManager* manager_;
int windowWidth = 1024;
int windowHeight = 768;

//camera stuff
float lastX = (float)windowWidth / 2.0;
float lastY = (float)windowHeight / 2.0;
bool firstMouse = true;
int test = 0;
bool allowInput = true;
//timing stuff
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

////////////////// Model ImGui Variables: /////////////////////////////////////////////////////////

char pathBuff[1024] = { 0 };
bool* flipWindingOrder = new bool{ false };
const char* itemsNorm[] = { "None", "Vertex Normals", "Face Normals" };
static int itemsNorm_current_idx = 0; // Here we store our selection data as an index.
const char* itemsNorm_preview_value = itemsNorm[itemsNorm_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

////////////////// Camera ImGui Variables: ////////////////////////////////////////////////////////
float scroll_x_delta = 0.0f;

////////////////// Shader ImGui Variables: ////////////////////////////////////////////////////////

const char* itemsShader[] = {"Phong Shading"};
static int itemsShader_current_idx = 0; // Here we store our selection data as an index.
const char* itemsShader_preview_value = itemsShader[itemsShader_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

//paths to the shader files
std::string phongShadingVert = "../Common/shaders/phongShading.vert";
std::string phongShadingFrag = "../Common/shaders/phongShading.frag";

//which render target to visualize
const char* itemsRender[] = { "Position", "Normal", "Albeido", "All"};
static int itemsRender_current_idx = 3; // Here we store our selection data as an index.
const char* itemsRender_preview_value = itemsRender[itemsRender_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)


////////////////// Material ImGui Variables: //////////////////////////////////////////////////////

//material surface color tints
glm::vec4 ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);
glm::vec4 diffuse = glm::vec4(1.0f, 1.00f, 1.00f, 1.00f);
glm::vec4 specular = glm::vec4(1.00f, 1.00f, 1.00f, 1.00f);
glm::vec4 emmissive = glm::vec4(0.0f, 0.00f, 0.00f, 1.00f);

//visualize UV
bool uvVisualize = false;

//texture projection mode combo
const char* itemsTextMode[] = { "None", "Cylindrical", "Spherical" "Cube" };
static int itemsTextMode_current_idx = 0; // Here we store our selection data as an index.
const char* itemsTextMode_preview_value = itemsTextMode[itemsTextMode_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

//texture projection pipeline combo
const char* itemsTextPipeline[] = { "CPU", "GPU" };
static int itemsTextPipeline_current_idx = 1; // Here we store our selection data as an index.
const char* itemsTextPipeline_preview_value = itemsTextPipeline[itemsTextPipeline_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

//texture entity combo
const char* itemsTextEntity[] = { "Position", "Normal" };
static int itemsTextEntity_current_idx = 0; // Here we store our selection data as an index.
const char* itemsTextEntity_preview_value = itemsTextEntity[itemsTextEntity_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

////////////////// Global Constant ImGui Variables: ///////////////////////////////////////////////

glm::vec4 attenuationConstants = glm::vec4(1.00f, 0.220f, 0.200f, 1.00f);
glm::vec4 globalAmbient = glm::vec4(0.00f, 0.00f, 0.10f, 1.00f);
glm::vec4 fogColor = glm::vec4(0.2f, 0.4f, 0.55f, 1.00f);
glm::vec2 fogDist = glm::vec2(0.100f, 20.0f);

////////////////// Lighting ImGui Variables: //////////////////////////////////////////////////////

float* radiusVal = new float{ 0.75f };
bool orbitEnabled = false;
static bool active[16] = { true, false, false, false, false, false, false, false,
                   false, false, false, false, false, false, false, false }; //helper to see which lights are currently active in screen
int numActiveLights = 1;
int currScenario = 0;

//individual light parameters. 
const char* itemsLight[] = { "Point", "Directional", "Spot" }; //never modified, so don't have to make copies of
std::vector<const char*> itemsLight_preview_value; //will set in main
int itemsLight_current_idx[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // Here we store our selection data as an index.

///////////////////////////////////////////////////////////////////////////////////

void imGuiStartup()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        int warningHelper = getchar();
        return -1;
    }

    // Setting up OpenGL properties
    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
        "Assignment 1 CS350 Mark Kouris", // window title
        nullptr, // which monitor (if full-screen mode)
        nullptr); // if sharing context with another window
    if (window == nullptr)
    {
        fprintf(stderr,
            "Failed to open GLFW window. Check if your GPU is 4.0 compatible.\n");
        int warningHelper = getchar();
        glfwTerminate();
        return -1;
    }

    // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
    // context but does NOT make the created context "current". We MUST make it current with the following
    // call
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        int warningHelper = getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    //imgui startup functions
    imGuiStartup();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one

    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHTING);

    // Initialize the scene    
    scene = new Scene(windowWidth, windowHeight); //this call here would be where the data is read in.
    orbit_ = new orbit();
    manager_ = new LightManager();
    scene->Init();
    orbit_->generateOrbit(0.75f);
    orbit_->setMatricies(scene->cam, windowWidth, windowHeight);
    orbit_->setupOpenGL();

    for (int i = 0; i < 16; ++i) //fill it
    {
        itemsLight_preview_value.push_back(itemsLight[itemsLight_current_idx[i]]);

    }

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        //process input first
        // per-frame time logic
        // --------------------
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Now render the scene
        // Scene::Display method encapsulates pre-, render, and post- rendering operations
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Begin("Controls");

        if (ImGui::CollapsingHeader("Model"))
        {
            ImGui::Text("Model Loading");
            ImGui::InputText("Path", pathBuff, IM_ARRAYSIZE(pathBuff));
            //add a checkbox to flip the winding order
            ImGui::Checkbox("Flip Winding Order", flipWindingOrder);

            if (ImGui::Button("Load"))
            {
                scene->updateObj(pathBuff);
            }
            ImGui::Separator();
            ImGui::Text("Normal Visualization");
            if (ImGui::BeginCombo("Normal visualization Mode", itemsNorm[itemsNorm_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(itemsNorm); n++)
                {
                    const bool is_selected = (itemsNorm_current_idx == n);
                    if (ImGui::Selectable(itemsNorm[n], is_selected))
                        itemsNorm_current_idx = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();

                if (itemsNorm_current_idx == 0)
                {
                    scene->renderVertNormals = false;
                    scene->renderFaceNormals = false;
                }
                else if (itemsNorm_current_idx == 1)
                {
                    scene->renderVertNormals = true;
                    scene->renderFaceNormals = false;
                }
                else
                {
                    scene->renderVertNormals = false;
                    scene->renderFaceNormals = true;;
                }
            }

            //option for copying depth info over
            ImGui::Checkbox("Copy Over Depth info over", &(scene->copyDepth));
        }

        if (ImGui::CollapsingHeader("Camera"))
        {
            //camera controls
            ImGui::Text("The controls for the camera are as follows: ");
            ImGui::Text("Press Tab to toggle camera controls");
            ImGui::Text("Use the mouse to move camera like in a standard fps");
            ImGui::Text("Use WASD to move forward, back, left and right");

        }

        if (ImGui::CollapsingHeader("Shader"))
        {
            //dropdown for the type of texture projection (cylindrical, sphereical, cube)
            if (ImGui::BeginCombo("Shader Mode", itemsShader[itemsShader_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(itemsShader); n++)
                {
                    const bool is_selected = (itemsShader_current_idx == n);
                    if (ImGui::Selectable(itemsShader[n], is_selected))
                        itemsShader_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            //reload shaders button
            if (ImGui::Button("Reload Shader"))
            {

                if (itemsShader_current_idx == 0) //phong shader reload
                {
                    scene->LoadAllShaders(phongShadingVert, phongShadingFrag);
                }

            }

            //option to visualize individual render targets:
            if (ImGui::BeginCombo("Render Target", itemsRender[itemsRender_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(itemsRender); n++)
                {
                    const bool is_selected = (itemsRender_current_idx == n);
                    if (ImGui::Selectable(itemsRender[n], is_selected))
                    {
                        itemsRender_current_idx = n;
                        scene->gblData.renderTarget = itemsRender_current_idx;
                        scene->updateGBL();
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        if (ImGui::CollapsingHeader("Material"))
        {
            //Texture Projection
            ImGui::Text("Texture Projection");
            //checkbox for visualize UV (basically shows the texture or not)
            ImGui::Checkbox("Visualize UV", &uvVisualize);

            //dropdown for the type of texture projection (cylindrical, sphereical, cube)
            if (ImGui::BeginCombo("Texture Projection Mode", itemsTextMode[itemsTextMode_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(itemsTextMode); n++)
                {
                    const bool is_selected = (itemsTextMode_current_idx == n);
                    if (ImGui::Selectable(itemsTextMode[n], is_selected))
                        itemsTextMode_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            //texture projection pipeline drop down (either gpu or cpu, unsure if I need this)
            if (ImGui::BeginCombo("Texture Projection Pipeline", itemsTextPipeline[itemsTextPipeline_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(itemsTextPipeline); n++)
                {
                    const bool is_selected = (itemsTextPipeline_current_idx == n);
                    if (ImGui::Selectable(itemsTextPipeline[n], is_selected))
                        itemsTextPipeline_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // texutre entity drop down, either normal or position
            if (ImGui::BeginCombo("Texture Entity", itemsTextEntity[itemsTextEntity_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(itemsTextEntity); n++)
                {
                    const bool is_selected = (itemsTextEntity_current_idx == n);
                    if (ImGui::Selectable(itemsTextEntity[n], is_selected))
                        itemsTextEntity_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        if (ImGui::CollapsingHeader("Global Constants"))
        {
            //coefficients for the ambient, diffuse, and speculars
            if (ImGui::ColorEdit3("Diffuse Coefficient", &(scene->gblData.diffuseCoef[0])))
            {
                scene->updateGBL();
            }

            if (ImGui::ColorEdit3("Ambient Coefficient", &(scene->gblData.ambientCoef[0])))
            {
                scene->updateGBL();
            }

            if (ImGui::ColorEdit3("Specular Coefficient", &(scene->gblData.specularCoef[0])))
            {
                scene->updateGBL();
            }

            if (ImGui::SliderFloat("NS", &(scene->gblData.NS), 0.1f, 30.0f))
            {
                scene->updateGBL();
            }

            if (ImGui::ColorEdit3("emissive", &(scene->gblData.emissive[0])))
            {
                scene->updateGBL();
            }

            //vec3 for attenuation constants
            if (ImGui::InputFloat3("Attenuation Constants", &(scene->gblData.attenuationConstants[0])))
            {
                scene->updateGBL();
            }

            //rgb slider for global ambient
            if (ImGui::ColorEdit3("GlobalAmbient", &(scene->gblData.globalAmbient[0])))
            {
                scene->updateGBL();
            }

            //rgb slider for fog color
            if (ImGui::ColorEdit3("Fog Color", &(scene->gblData.fogColor[0])))
            {
                fogColor = vec4(scene->gblData.fogColor, 1.0f);
                scene->updateGBL();
            }

            // vec2 for fog distance
            if (ImGui::SliderFloat2("Fog Distances", &(scene->gblData.fogDistances[0]), 0.1f, 20.0f))
            {
                scene->updateGBL();
            }

            scene->updateGBL();
        }


        if (ImGui::CollapsingHeader("Lighting"))
        {
            ImGui::Text("Lighting Orbit");
            ImGui::Checkbox("Orbit Enabled", &orbitEnabled);
            if (orbitEnabled)
            {
                test += 1;
            }
            //check box for if the lights are orbiting
            //slider for speed, from -3 to 3
            //slider for radius, from 1.5 to 3.5
            ImGui::SliderFloat("Radius: ", radiusVal, 0.5, 1.5f, "%.2f");
            if (*radiusVal != orbit_->currRadius)
            {
                orbit_->generateOrbit(*radiusVal);
                orbit_->setMatricies(scene->cam);
                orbit_->setupOpenGL();
            }

            ImGui::Separator();
            ImGui::Text("Lighting Scenarios");
            //3 buttons for loading the 3 test scenarios
            if (ImGui::Button("Scenario 1"))
            {
                if (currScenario != 1)
                {   //based on combo preview value, load shader
                    manager_->pickScenario(1);
                    currScenario = 1;
                    for (int i = 0; i < 16; ++i)
                    {
                        if (i < 8)
                        {
                            active[i] = true;
                        }
                        else
                        {
                            active[i] = false;
                        }
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Scenario 2"))
            {
                if (currScenario != 2)
                {   //based on combo preview value, load shader
                    manager_->pickScenario(2);
                    currScenario = 2;
                    for (int i = 0; i < 16; ++i)
                    {
                        if (i < 8)
                        {
                            active[i] = true;
                        }
                        else
                        {
                            active[i] = false;
                        }
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Scenario 3"))
            {
                if (currScenario != 3)
                {   //based on combo preview value, load shader
                    manager_->pickScenario(3);
                    currScenario = 3;
                    for (int i = 0; i < 16; ++i)
                    {
                        if (i < 8)
                        {
                            active[i] = true;
                        }
                        else
                        {
                            active[i] = false;
                        }
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                //update enabled here so it updates in the shader as well
                if (currScenario != 0)
                {   //based on combo preview value, load shader
                    manager_->pickScenario(0);
                    currScenario = 0;
                    for (int i = 0; i < 16; ++i)
                    {
                        if (i == 0)
                        {
                            active[i] = true;
                        }
                        else
                        {
                            active[i] = false;
                        }
                    }
                }
            }

            ImGui::Separator();
            ImGui::Text("Light Sources");

            //loop to display all the active lights dynamically
            for (int i = 0; i < 16; ++i)
            {
                if (manager_->dataGlobal.l[i].enabled == 1)
                {
                    ImGui::PushID(i);
                    std::ostringstream oss;
                    oss << "Light #" << i + 1;

                    //this bool here will need to be from the light manager
                    if (ImGui::CollapsingHeader(oss.str().c_str(), &active[i]))
                    {
                        //for each light source, make a new collapsing header
                        //be able to change the following on the fly:
                        // type as a drop down, which is point direction and spot
                        // texutre entity drop down, either normal or position
                        if (ImGui::BeginCombo("Type", itemsLight_preview_value[i]))
                        {
                            for (int n = 0; n < IM_ARRAYSIZE(itemsLight); n++)
                            {
                                const bool is_selected = (itemsLight_current_idx[i] == n);
                                if (ImGui::Selectable(itemsLight[n], is_selected))
                                {
                                    itemsLight_preview_value[i] = itemsLight[n];
                                    itemsLight_current_idx[i] = n;
                                    manager_->setType(i, n);
                                }

                                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }

                            ImGui::EndCombo();
                        }

                        //angular position is the offset into the orbit
                        //rgb box for ambient
                        if (ImGui::SliderInt("Angular Position", &(manager_->dataGlobal.l[i].angularPos), 0, 180))
                        {
                            manager_->setAngularPos(i, manager_->dataGlobal.l[i].angularPos);
                        }

                        //rgb box for ambient
                        if (ImGui::ColorEdit3("Ambient", &(manager_->dataGlobal.l[i].ambient[0])))
                        {
                            manager_->setAmbient(i, manager_->dataGlobal.l[i].ambient);
                        }
                        //rgb box for diffuse
                        if (ImGui::ColorEdit3("Diffuse", &(manager_->dataGlobal.l[i].diffuse[0])))
                        {
                            manager_->setDiffuse(i, manager_->dataGlobal.l[i].diffuse);
                        }

                        //rgb box for specular
                        if (ImGui::ColorEdit3("Specular", &(manager_->dataGlobal.l[i].specular[0])))
                        {
                            manager_->setSpecular(i, manager_->dataGlobal.l[i].specular);
                        }

                        //special stuff for spotlights
                        if (itemsLight_current_idx[i] == 2)
                        {
                            if (ImGui::SliderFloat("Inner Angle", &(manager_->dataGlobal.l[i].innerAngle), 0, 90))
                            {
                                manager_->setInnerAngle(i, manager_->dataGlobal.l[i].innerAngle);
                            }

                            if (ImGui::SliderFloat("Outer Angle", &(manager_->dataGlobal.l[i].outerAngle), 0, 90))
                            {
                                manager_->setOuterAngle(i, manager_->dataGlobal.l[i].outerAngle);
                            }

                            if (ImGui::SliderFloat("Falloff", &(manager_->dataGlobal.l[i].fallout), 0, 10, "%.3f"))
                            {
                                manager_->setFallout(i, manager_->dataGlobal.l[i].fallout);
                            }
                        }
                    }
                    manager_->updateEachLight(&manager_->dataGlobal.l[i]);
                    ImGui::PopID();
                }

            }


            ImGui::Separator();
            //must be able to make a new light and add it to the list
            if (ImGui::Button("New Light"))
            {
                for (int i = 0; i < 16; ++i)
                {
                    if (manager_->dataGlobal.l[i].enabled == 0)
                    {
                        manager_->dataGlobal.l[i].enabled = 1;
                        manager_->lights[i]->data.enabled = 1;
                        active[i] = true;
                        break;
                    }
                }
            }
        }

        //framerate counter here
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / double(ImGui::GetIO().Framerate), double(ImGui::GetIO().Framerate));
        ImGui::End();

        //update the data after retrieving input
        orbit_->setMatricies(scene->cam, windowWidth, windowHeight);
        manager_->updateLights(orbit_, test);
        scene->Display();
        manager_->drawLights();
        orbit_->drawOrbit();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //clean up imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    scene->CleanUp();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

// input reading for camera

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //swaps between allowing and not allowing input
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && allowInput == true)
    {
        allowInput = false;
        scene->cam.enabled = !scene->cam.enabled;
        if (scene->cam.enabled == true)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && allowInput == false)
    {
        allowInput = true;
    }

    if (scene->cam.enabled)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            scene->cam.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            scene->cam.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            scene->cam.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            scene->cam.ProcessKeyboard(RIGHT, deltaTime);
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (scene->cam.enabled)
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

        scene->cam.ProcessMouseMovement(xoffset, yoffset);
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (scene->cam.enabled)
    {
        //scene->cam.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////