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
#include "skybox.h"


////////////////// Globals For Imgui ////////////////////////////////////////////////////
void imGuiStartup();
GLFWwindow* window;
Scene* scene;
orbit* orbit_;
LightManager* manager_;
skybox* skybox_;
int windowWidth = 1024;
int windowHeight = 768;

//temp for testing:
int test = 0;

////////////////// Model ImGui Variables: /////////////////////////////////////////////////////////

char pathBuff[1024] = { 0 };
bool* flipWindingOrder = new bool{ false };
const char* itemsNorm[] = { "None", "Vertex Normals", "Face Normals" };
static int itemsNorm_current_idx = 0; // Here we store our selection data as an index.
const char* itemsNorm_preview_value = itemsNorm[itemsNorm_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)

////////////////// Camera ImGui Variables: ////////////////////////////////////////////////////////
float scroll_x_delta = 0.0f;

////////////////// Shader ImGui Variables: ////////////////////////////////////////////////////////

//paths to the shader files
std::string phongShadingVert = "../Common/shaders/phongShading.vert";
std::string phongShadingFrag = "../Common/shaders/phongShading.frag";


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

//Environment mapping combo
const char* environMap[] = { "Fresnel", "Reflection Only", "Refraction Only"};
static int environMap_current_idx = 0; // Here we store our selection data as an index.
const char* environMap_preview_value = environMap[environMap_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)


////////////////// Lighting ImGui Variables: //////////////////////////////////////////////////////

float* radiusVal = new float{ 0.75f };
bool orbitEnabled = false;
static bool active[16] = {true, false, false, false, false, false, false, false, 
                   false, false, false, false, false, false, false, false}; //helper to see which lights are currently active in screen
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
        "Assignment 3 CS300 Mark Kouris", // window title
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
    skybox_ = new skybox(scene->programID); //so I can pass textures to main shader
    orbit_->generateOrbit(0.75f);
    orbit_->setPerspective(windowWidth, windowHeight);

    for (int i = 0; i < 16; ++i) //fill it
    {
        itemsLight_preview_value.push_back(itemsLight[itemsLight_current_idx[i]]);

    }

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        // Now render the scene
        // Scene::Display method encapsulates pre-, render, and post- rendering operations
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
                scene->loadObjData(pathBuff);
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
            }
        }

        if (ImGui::CollapsingHeader("Camera"))
        {
            //camera orbit scroll buttons
            ImGui::SmallButton("<");
            if (ImGui::IsItemActive())
            {
                scroll_x_delta -= ImGui::GetIO().DeltaTime * 100.0f;
                scene->cameraAngle = scroll_x_delta;
            }
            ImGui::SameLine();
            ImGui::SmallButton(">");
            if (ImGui::IsItemActive())
            {
                scroll_x_delta += ImGui::GetIO().DeltaTime * 100.0f;
                scene->cameraAngle = scroll_x_delta;
            }
            ImGui::SameLine();
            ImGui::Text("Camera Orbit");
            
        }

        if (ImGui::CollapsingHeader("Shader"))
        {
            //reload shaders button
            if (ImGui::Button("Reload Shader"))
            {
                scene->LoadAllShaders(); //only using phong shading for assignment 3
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
                scene->updateGBL();
            }

            // vec2 for fog distance
            if (ImGui::SliderFloat2("Fog Distances", &(scene->gblData.fogDistances[0]), 0.1f, 20.0f))
            {
                scene->updateGBL();
            }

            scene->updateGBL();
        }

        if (ImGui::CollapsingHeader("Reflection/Refraction"))
        {
            ImGui::Text("Blending");
            if (ImGui::SliderFloat("Phong - Environment mapping ", &(scene->gblData.blendVal), 0.0, 1.0f, "%.4f"))
            {
                scene->updateGBL();
            }
            ImGui::Separator();
            ImGui::Text("Environmental Mapping");
            //dropdown for the type of texture projection (cylindrical, sphereical, cube)
            if (ImGui::BeginCombo("Environmental Mapping Mode", environMap[environMap_current_idx]))
            {
                for (int n = 0; n < IM_ARRAYSIZE(environMap); n++)
                {
                    const bool is_selected = (environMap_current_idx == n);
                    if (ImGui::Selectable(environMap[n], is_selected))
                    {
                        environMap_current_idx = n;
                        scene->gblData.mappingType = n;
                        scene->updateGBL();
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();
            ImGui::Text("Refraction values for materials:");
            if (ImGui::Button("Air"))
            {
                scene->gblData.refractionVal = 1.000293f;
                scene->updateGBL();
            }
            ImGui::SameLine();
            if (ImGui::Button("Hydrogen"))
            {
                scene->gblData.refractionVal = 1.000132f;
                scene->updateGBL();
            }
            ImGui::SameLine();
            if (ImGui::Button("Water"))
            {
                scene->gblData.refractionVal = 1.333f;
                scene->updateGBL();
            }
            ImGui::SameLine();
            if (ImGui::Button("Olive Oil"))
            {
                scene->gblData.refractionVal = 1.47f;
            }
            ImGui::SameLine();
            if (ImGui::Button("Ice"))
            {
                scene->gblData.refractionVal = 1.31f;
                scene->updateGBL();
            }
            ImGui::SameLine();
            if (ImGui::Button("Quartz"))
            {
                scene->gblData.refractionVal = 1.46f;
                scene->updateGBL();
            }
            ImGui::SameLine();
            if (ImGui::Button("Diamond"))
            {
                scene->gblData.refractionVal = 2.42f;
                scene->updateGBL();
            }
            ImGui::SameLine();
            if (ImGui::Button("Acrylic"))
            {
                scene->gblData.refractionVal = 1.49f;
                scene->updateGBL();
            }

            if (ImGui::SliderFloat("Refraction Index: ", &(scene->gblData.refractionVal), 1.0f, 100.0f, "%.3f"))
            {
                scene->updateGBL();
            }

            if (ImGui::SliderFloat("Chromatic Aberration: ",&(scene->gblData.chromaticAberration), 0.0f, 1.0f, "%.3f"))
            {
                scene->updateGBL();
            }

            ImGui::Separator();
            ImGui::Text("Fresnel");
            if (ImGui::SliderFloat("Fresnel Power ", &(scene->gblData.fresnelPower), 0.1f, 10.0f, "%.1f"))
            {
                scene->updateGBL();
            }
            //phong - environmental mapping slider 
            //environment
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
                orbit_->setMatricies();
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

                        // angular position slider for 0 to 360
                        if (ImGui::SliderInt("Angular Position", &(manager_->dataGlobal.l[i].angularPosition), 0, 360))
                        {
                            manager_->setAngularPosition(i, manager_->dataGlobal.l[i].angularPosition);
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
        
        skybox_->displaySides(scroll_x_delta, windowWidth, windowHeight);
        orbit_->setMatricies();
        orbit_->radiansCamera = scroll_x_delta;
        manager_->updateLights(orbit_, test);
        scene->Display();
        orbit_->drawOrbit();
        manager_->drawLights();

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
    delete scene;
    delete orbit_;
    delete manager_;
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////