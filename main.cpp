// N-body Gravity and Collision simulator
// (c) D.J. Denton 2017 ICS3UI
// This program simulates collisions and gravity in 3D space between
// 6 or fewer spheres.

#include <iostream>
#include <stdio.h>
#include <glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <string>
// TEXTURES
#include <SOIL2.h>
// TRANSFORMATIONS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// TEXT
#include <ft2build.h>
//#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include <iomanip>

// Custom Shaders
#include "files/shader.h"
// Include camera class
#include "files/camera.h"
#include "mesh.h"
#include "files/model.h"
#include "files/object.h"
#include "files/gui.h"

#define PI 3.14159265359// A PI constant because I think glm works in radians
#define NUMBER_OF_OBJECTS 8//I don't want to just have a magic number, so I'm defining the number of objects here.
// In a real game engine, they'd have hundreds of models and would probably just use a vector instead of manually counting
#define NUMBER_OF_LIGHTS 2// The number of lights
#define POINT 0// Defines for the types of lights
#define DIRECTIONAL 1
#define SPOT 2

using namespace std;

const int WIDTH = 1000, HEIGHT = 800, SDL_WIDTH = 400;
int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 800;

// Function Prototypes
// Get keyboard input
//void KeyCallback(SDL_Window *window, int key, int scancode, int action, int mode);
// Function to control camera movement
void DoMovement (SDL_Event event);
// Variable to control when the similation should be running
bool simulate = true;

//create camera
Camera camera(glm::vec3 (0.0f, 0.0f, 3.0f));
// Lastx, for mouse movement
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLdouble deltaTime = 0.0f; // number of miliseconds since last frame
GLdouble simTime = 0.0f; // number of miliseconds since last frame
GLdouble lastFrame = 0.0f;

// For reading keyboard
const Uint8 *keys = SDL_GetKeyboardState(NULL);

int main(int argc, char *argv[])
{

//==============================================================================================================
// Initialize SDL
//==============================================================================================================
    SDL_Init(SDL_INIT_VIDEO);                                                                                                                           // Initializes the specific part of SDL that can use the opengl window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);                                                                      // Makes code forward compatable???
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);                                                        // Create a window variable and stencil buffer
    SDL_GLContext context = SDL_GL_CreateContext(window);                                                                                               // Create context. Must be deleted at the end.

    // SDL MOUSE STUFF
    SDL_ShowCursor(SDL_DISABLE); // Hide Cursor

    // Creates window surface
    SDL_Event windowEvent;																																// Create a variable that handles window events

    glewExperimental = GL_TRUE;
    glewInit();

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)																												// Initialize everything, and print an error if it doesn't initialize correctly
    {
        cout << "SDL could not initialize! SDL error: " << SDL_GetError() << endl;
    }

    if (NULL == window)																																	// Print error if window hasn't been created correctly
    {
        cout << "SDL could not create window! SDL error: " << SDL_GetError() << endl;
        return -1;
    }
//==============================================================================================================

//==============================================================================================================
// Initialize OpenGL
//==============================================================================================================
    // Enable Depth
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable Alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up shaders
    Shader shader ("resources/shaders/modelLoading.vs", "resources/shaders/modelLoading.frag");
    Shader postShader ("resources/shaders/GUI.vs", "resources/shaders/GUI.frag");
    Shader textShader ("resources/shaders/text.vs", "resources/shaders/text.frag");

    Object objects [NUMBER_OF_OBJECTS];// This line always makes me laugh

    // LOAD AND INITIATE THE DOMAIN
    objects[0].location = glm::vec3 (0.0f,0.0f,0.0f);
    objects[0].oldLocation = objects[0].location;
    objects[0].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[0].scale = glm::vec3 (15.0f,15.0f,15.0f);
    objects[0].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[0].oldVelocity = objects[0].velocity;
    objects[0].isSphere = false;
    objects[0].collision = true;
    objects[0].massNum = 0;
    objects[0].massExp = 0;
    objects[0].mass = 1.0;
    objects[0].elasticity = 0;
    objects[0].meshDir = "resources/models/Domain/Domain.obj";
    objects[0].hidden = false;

      // LOAD AND INITIATE ARROW
    objects[1].location = glm::vec3 (0.0f,0.0f,0.0f);
    objects[1].oldLocation = objects[1].location;
    objects[1].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[1].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[1].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[1].oldVelocity = objects[1].velocity;
    objects[1].isSphere = false;
    objects[1].collision = false;
    objects[1].massNum = 0;
    objects[1].massExp = 0;
    objects[1].mass = 0*pow(10,0);
    objects[1].elasticity = 0.0;
    objects[1].meshDir = "resources/models/Arrow/Arrow.obj";
    objects[1].hidden = true;

    // LOAD AND INITIATE SPHERE A
    objects[2].location = glm::vec3 (0.0f,0.0f,0.0f);
    objects[2].oldLocation = objects[2].location;
    objects[2].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[2].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[2].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[2].oldVelocity = objects[2].velocity;
    objects[2].isSphere = true;
    objects[2].collision = true;
    objects[2].massNum = 1;
    objects[2].massExp = 10;
    objects[2].mass =1*pow(10,10);
    objects[2].elasticity = 1.0;
    objects[2].meshDir = "resources/models/Ball_A/Ball_A.obj";
    objects[2].hidden = false;

    // LOAD AND INITIATE SPHERE B
    objects[3].location = glm::vec3 (3.0f,0.0f,0.0f);
    objects[3].oldLocation = objects[3].location;
    objects[3].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[3].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[3].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[3].oldVelocity = objects[3].velocity;
    objects[3].isSphere = true;
    objects[3].collision = true;
    objects[3].massNum = 1;
    objects[3].massExp = 10;
    objects[3].mass = 1*pow(10,10);
    objects[3].elasticity = 1.0;
    objects[3].meshDir = "resources/models/Ball_B/Ball_B.obj";
    objects[3].hidden = false;


    // LOAD AND INITIATE SPHERE C
    objects[4].location = glm::vec3 (6.0f,0.0f,0.0f);
    objects[4].oldLocation = objects[4].location;
    objects[4].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[4].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[4].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[4].oldVelocity = objects[4].velocity;
    objects[4].isSphere = true;
    objects[4].collision = true;
    objects[4].massNum = 1;
    objects[4].massExp = 10;
    objects[4].mass = 1*pow(10,10);
    objects[4].elasticity = 1.0;
    objects[4].meshDir = "resources/models/Ball_C/Ball_C.obj";
    objects[4].hidden = false;

    // LOAD AND INITIATE SPHERE D
    objects[5].location = glm::vec3 (-3.0f,0.0f,0.0f);
    objects[5].oldLocation = objects[5].location;
    objects[5].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[5].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[5].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[5].oldVelocity = objects[5].velocity;
    objects[5].isSphere = true;
    objects[5].collision = true;
    objects[5].massNum = 1;
    objects[5].massExp = 10;
    objects[5].mass = 1*pow(10,10);
    objects[5].elasticity = 1.0;
    objects[5].meshDir = "resources/models/Ball_D/Ball_D.obj";
    objects[5].hidden = false;

    // LOAD AND INITIATE SPHERE E
    objects[6].location = glm::vec3 (-6.0f,0.0f,0.0f);
    objects[6].oldLocation = objects[6].location;
    objects[6].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[6].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[6].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[6].oldVelocity = objects[6].velocity;
    objects[6].isSphere = true;
    objects[6].collision = true;
    objects[6].massNum = 1;
    objects[6].massExp = 10;
    objects[6].mass = 1*pow(10,10);
    objects[6].elasticity = 1.0;
    objects[6].meshDir = "resources/models/Ball_E/Ball_E.obj";
    objects[6].hidden = false;

    // LOAD AND INITIATE SPHERE F
    objects[7].location = glm::vec3 (0.0f,0.0f,3.0f);
    objects[7].oldLocation = objects[7].location;
    objects[7].rotation = glm::vec3 (0.0f,0.0f,0.0f);
    objects[7].scale = glm::vec3 (1.0f,1.0f,1.0f);
    objects[7].velocity = glm::vec3 (0.0f,0.0f,0.0f);
    objects[7].oldVelocity = objects[7].velocity;
    objects[7].isSphere = true;
    objects[7].collision = true;
    objects[7].massNum = 1;
    objects[7].massExp = 10;
    objects[7].mass = 1*pow(10,10);
    objects[7].elasticity = 1.0;
    objects[7].meshDir = "resources/models/Ball_F/Ball_F.obj";
    objects[7].hidden = false;



    // Load all models
    for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
    {
        objects[i].model.LoadModel(objects[i].meshDir);
    }

    Model GUI;
    GUI.LoadModel("resources/models/GUI/GUI.obj");

    Light lights[NUMBER_OF_LIGHTS];// LOL
    lights[0].location = glm::vec3 (10.0f,10.0f,10.0f);
    lights[0].type = POINT;
    lights[0].diffuse = glm::vec3 (1.0f,1.0f,1.0f);
    lights[0].ambient = glm::vec3 (1.0f,1.0f,1.0f);
    lights[0].specular = glm::vec3 (1.0f,1.0f,1.0f);
    lights[0].direction = glm::vec3 (1.0f,1.0f,1.0f);
    lights[0].constant = 0.99;
    lights[0].linear = 0.0;
    lights[0].quadratic = 0.01;
    lights[0].cutOff = 12.5;
    lights[0].outerCutOff = 17.5;
    lights[0].index = "0";

    lights[1].location = glm::vec3 (-10.0f,10.0f,-10.0f);
    lights[1].type = POINT;
    lights[1].diffuse = glm::vec3 (1.0f,1.0f,1.0f);
    lights[1].ambient = glm::vec3 (1.0f,1.0f,1.0f);
    lights[1].specular = glm::vec3 (1.0f,1.0f,1.0f);
    lights[1].direction = glm::vec3 (0.0f,0.0f,0.0f);
    lights[1].constant = 0.99;
    lights[1].linear = 0.0;
    lights[1].quadratic = 0.01;
    lights[1].cutOff = 12.5;
    lights[1].outerCutOff = 17.5;
    lights[1].index = "1";

    // Projection type      //          // Projection Type//Field of view//Aspect ratio        // Near clip // Far clip
    glm::mat4 projection = glm::perspective(camera.GetZoom(), ((GLfloat)SCREEN_WIDTH - SDL_WIDTH)/(GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

    // Create GUI variable
    GUI_C guiBuffer;
    // Initialize the VBO and VAO used for all text in the guiBuffer
    guiBuffer.initTextVerts();
    // Load the characters used for the GUI font
    loadCharacters();
    // Start text input
    SDL_StartTextInput();
//==============================================================================================================

    ifstream fin;                               // Create input file
    ofstream fout;                              // Output to file


    // MAIN LOOP HERE
    while (true)																																		// Loop forever
    {
        // Write to info file
        fout.open("files/info.txt",ios_base::out|ios_base::trunc);
        fout.clear();

        // Write "hidden" information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++)
        {
            if (objects[i].hidden) fout << "no" <<endl;
            else fout << "yes" <<endl;
        }
        // xLoc information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].location.x << endl;
        // yLoc information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].location.y << endl;
        // zLoc information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].location.z << endl;
        // xVelocity information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].velocity.x << endl;
        // yVelocity information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].velocity.y << endl;
        // zVelocity information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].velocity.z << endl;
        //  mass coefficient information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].massNum << endl;
        //  mass exponent information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].massExp << endl;
        //  radius information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].scale.x << endl;
        //  elasticity information
        for (int i = 2; i < NUMBER_OF_OBJECTS; i++) fout << fixed << setprecision(3) << objects[i].elasticity * 100 << endl;
        fout.close();

        // SPHERE TEXTURES
        // WRITE WHILE INPUTTING

        // Set up frame independent time
        GLfloat currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Stop simulation time if simulation is not running
        if (simulate)
        {
            simTime = deltaTime;
            projection = glm::perspective(camera.GetZoom(), ((GLfloat)SCREEN_WIDTH)/(GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
            glViewport(0, 0, WIDTH, HEIGHT);
            guiBuffer.hit = false;
        }
        else
        {
            simTime = 0;
            projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH /(GLfloat)(SCREEN_HEIGHT-SDL_WIDTH), 0.1f, 1000.0f);
            glViewport(0, SDL_WIDTH, WIDTH, HEIGHT - SDL_WIDTH);
        }

        // Poll SDL event
        if (SDL_PollEvent(&windowEvent))																												// Check if something is happening with the window
        {
            if (SDL_QUIT == windowEvent.type)																											// Check if what's happening with the window is the quit event
            {
                break;																																	// If the quit event has been triggered, break the loop, thus exiting the program
            }

            if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                break;
            }
        }

        // Handle the movement of the camera
        DoMovement(windowEvent);
        // Handle GUI
        guiBuffer.checkClick (windowEvent);
        objects[guiBuffer.activeColumn] = guiBuffer.inputValue(objects[guiBuffer.activeColumn], windowEvent);
        guiBuffer.updateInfoFile();
        // RENDER
        //

        //simulation

        // Set up rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // (or whatever buffer you want to clear)
        // Use the shader and set up some ititial values
        shader.Use();
        GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
        glUniform3f (viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition().z );
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32.0f);
        glUniform1i(glGetUniformLocation(shader.Program, "NUMBER_OF_LIGHTS"), NUMBER_OF_LIGHTS);


        // Make all lights work
        for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
        {
            lights[i].Draw(shader);
        }

        // Create camera transformation
        glm::mat4 view;
        view = camera.GetViewMatrix();
        GLint modelLoc = glGetUniformLocation ( shader.Program, "model");
        GLint viewLoc = glGetUniformLocation ( shader.Program, "view");
        GLint projLoc = glGetUniformLocation ( shader.Program, "projection");

        glUniformMatrix4fv ( viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glUniformMatrix4fv ( projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Update velocitie, locations, and masses
        for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
        {
            // Skip if the object is hidden
            if (objects[i].hidden) continue;
            objects[i].oldVelocity = objects[i].velocity;
            objects[i].oldLocation = objects[i].location;
            objects[i].mass = objects[i].massNum * pow(10, objects[i].massExp);
        }

        // Do gravity
        for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
        {
            // Skip if the object is hidden
            if (objects[i].hidden) continue;
            for (int q = 0; q < NUMBER_OF_OBJECTS; q++)
            {
                // Skip if the object is hidden
                if (objects[q].hidden) continue;
                // Skip if the object is being compared to itself
                if (q == i) continue;
                objects[i].Gravity(objects[q], simTime/1000);
            }
        }

        // check collisions
        for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
        {
            // Skip if the object is hidden
            if (objects[i].hidden||!objects[i].collision) continue;
            for (int q = 0; q < NUMBER_OF_OBJECTS; q++)
            {
                // Skip if the object is hidden
                if (objects[q].hidden||!objects[q].collision) continue;
                // Skip if the object is being compared to itself
                if (q == i) continue;
                objects[i].Collide(objects[q]);
            }
        }

        // For loop to set all objects
        for (int i = 0; i < NUMBER_OF_OBJECTS; i++)
        {

            // Skip if the object is hidden
            if (objects[i].hidden||!objects[i].collision) continue;
                objects[i].CalcLoc(simTime/1000);
                glm::mat4 model; // Prepare to apply all transformations to all models
                model = glm::translate(model, objects[i].location); // Apply translations
                model = glm::scale(model, objects[i].scale); // Apply dilation
                model = glm::rotate(model, objects[i].rotation.z, glm::vec3(0.0f,0.0f,1.0f)); // Rotate on z axis
                model = glm::rotate(model, objects[i].rotation.y, glm::vec3(0.0f,1.0f,0.0f)); // Rotate on y axis
                model = glm::rotate(model, objects[i].rotation.x, glm::vec3(1.0f,0.0f,0.0f)); // Rotate on x axis
                glUniformMatrix4fv (glGetUniformLocation (shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model)); // Apply all transformations
                // still need to draw the model
                objects[i].model.Draw(shader);


                // Draw arrows
                if (!simulate && objects[i].isSphere)
                {

                    // find the rotation of the object's velocity
                    //model =glm::orientation(objects[i].velocity, glm::vec3(0.0f,0.0f,0.0f));
                    glm::vec2 temp = glm::normalize(glm::vec2(objects[i].velocity.x, objects[i].velocity.z));

                    GLfloat roll = glm::orientedAngle(temp, glm::vec2(1.0,0.0));

                    // Rotation axis for the pitch
                    glm::vec3 axis = glm::vec3 (objects[i].velocity);
                    axis.y = 0;
                    axis = glm::normalize(axis);
                    axis = glm::rotate(axis, float(PI/2), glm::vec3(0.0f,1.0f,0.0f));

                    GLfloat pitch = glm::orientedAngle(glm::normalize(objects[i].velocity), glm::vec3(0.0f,1.0f,0.0f), axis);

                    // Measure how far down the vector is from positive y
                   // model = glm::rotate(model, pitch, glm::vec3(1.0f,0.0f,0.0f)); // Rotate on z axis
                    model = glm::rotate(model, roll, glm::vec3(0.0f,1.0f,0.0f)); // Rotate on z axis
                    model = glm::rotate(model, pitch, glm::vec3(0.0f,0.0f,1.0f));
                    model = glm::translate(model, glm::vec3(0.0f,objects[i].scale.x,0.0f));
                    model = glm::scale(model, glm::vec3(1.0f, glm::distance(objects[i].velocity, glm::vec3(0.0f,0.0f,0.0f)), 1.0f)); // Apply dilation

                    glUniformMatrix4fv (glGetUniformLocation (shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model)); // Apply all transformations
                    objects[1].model.Draw(shader);
                }
        }

        // GUI Text Stuff
        if (simulate)
        {
            glViewport(0, 0, WIDTH, HEIGHT);
            textShader.Use();
            projection = glm::ortho(0.0f, (GLfloat)WIDTH, 0.0f, (GLfloat)HEIGHT);
            glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            guiBuffer.RenderText(textShader,"Press [P] to pause", 20.0f, 20.0f, 0.5f, glm::vec3(1.0f, 1.0f,1.0f));
        }

        // Open info file
        fin.open("files/info.txt");
        if (!simulate)
        {
            postShader.Use();

            glViewport(0, 0, WIDTH, SDL_WIDTH);
            GUI.Draw(postShader);

            glViewport(0, 0, WIDTH, HEIGHT);
            textShader.Use();

            projection = glm::ortho(0.0f, (GLfloat)WIDTH, 0.0f, (GLfloat)HEIGHT);
            glUniformMatrix4fv(glGetUniformLocation(textShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


            // Tell user how to use the program
            guiBuffer.RenderText(textShader, "Press [P] to unpause", 20.0f, 750.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            guiBuffer.RenderText(textShader, "Use [W][A][S][D] to move", 20.0f, 700.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            guiBuffer.RenderText(textShader, "Use the mouse to look around", 20.0f, 650.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            guiBuffer.RenderText(textShader, "Press [Esc] to quit", 20.0f, 600.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            guiBuffer.RenderText(textShader, "Click on the chart to edit values", 20.0f, 550.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
            // Go through file and print contents
            string text;
            // Read "hidden" information
            for (int i = 2; i < NUMBER_OF_OBJECTS; i++)
            {
                fin >> text;
                guiBuffer.RenderText(textShader, text,(BOX_START_X + (i-2)*BOX_WIDTH + 50), (BOX_START_Y - 0*BOX_HEIGHT -90), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            // Read all other information
            for (int q = 1; q < 11; q++)
            {
                for (int i = 2; i < NUMBER_OF_OBJECTS; i++)
                {
                    fin >> text;
                    guiBuffer.RenderText(textShader, text,(BOX_START_X + (i-2)*BOX_WIDTH + 10), (BOX_START_Y - q*BOX_HEIGHT -93), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }
        }
        fin.close();

        // Swap screen buffers
        SDL_GL_SwapWindow(window);
        // Update the specified window
    }
    // CLEAN UP
    SDL_StopTextInput();

    SDL_DestroyWindow(window);																															// Destroy the window before exiting
    SDL_GL_DeleteContext(context);
    SDL_Quit();																																			// Quit SDL
    return 0;																																			// Quit program

}


// FUNCTIONS

void DoMovement(SDL_Event event)
{
    //--------------------------------------------------------------------------------------------------------------------------------------------
    // IDEA: Make global variables for key presses, that way this function can set the variables, while other functions can use them independently
    //--------------------------------------------------------------------------------------------------------------------------------------------

    // RESET MOUSE POSITION
    if (simulate) SDL_WarpMouseInWindow(NULL, WIDTH/2, HEIGHT/2);

    // IF key was up or w
    if ( (keys [SDL_SCANCODE_UP] ) || (keys [SDL_SCANCODE_W]) )
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    // IF key was right or d
    if ( (keys [SDL_SCANCODE_RIGHT] ) || (keys [SDL_SCANCODE_D]) )
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    // IF key was down or s
    if ( (keys [SDL_SCANCODE_DOWN] ) || (keys [SDL_SCANCODE_S]) )
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    // IF key was left or a
    if ( (keys [SDL_SCANCODE_LEFT] ) || (keys [SDL_SCANCODE_A]) )
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    // Allow the user to pause
    if ( (keys [SDL_SCANCODE_P] ) && (event.type == SDL_KEYDOWN))
    {
        if (simulate == true)
        {
            simulate = false;
            SDL_ShowCursor(SDL_ENABLE);
        }
        else
        {
            simulate = true;
            SDL_ShowCursor(SDL_DISABLE);
        }
        // cout << "P";
    }

    // Check Mouse stuff
    if ((event.type == SDL_MOUSEMOTION) && (simulate))
    {
        GLfloat xOffset = 0, yOffset = 0;

        // Find offset relative to the mouse's rest position at the centre of the screen
        xOffset = event.motion.x - WIDTH/2;
        yOffset = HEIGHT/2 - event.motion.y ;

        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

// Sources
// Sonar Learing UK (openGL video tutorials)
// Learnopengl.com (openGL tutorial articles and segments of code)
// Open.gl (openGL tutorial articles and segments of code)
// c++.com
// Stackoverflow
