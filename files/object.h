#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "model.h"
#include <string>
#include <SDL_opengl.h>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/vector_angle.hpp>
#include <gtx/rotate_vector.hpp>
// Include everything needed for models?
// Include glm for vector math and stuff

using namespace std;

// Object Class
class Object
{
public:
    glm::vec3 location;// Location
    glm::vec3 oldLocation;
    glm::vec3 rotation;// Rotation
    glm::vec3 scale;// Scale
    glm::vec3 oldVelocity;// Velocity in metres per second
    glm::vec3 velocity;// Velocity in metres per second

    double massNum; // First digit of the mass
    double massExp; // Exponent of the mass in scientific notation
    double mass;// Mass
    double elasticity;// Elasticity

    bool hidden;// Whether the object should be drawn
    bool isSphere;// Whether the object is a sphere
    bool collision;// Whether the object should collide

    GLchar * meshDir;// Mesh directory for the model

    Model model;// The object's model

    void CalcLoc(float dTime)
    {
        location += velocity*dTime;
    }

    void Collide (Object obj)
    {
        // If the distance between the objects is smaller than the sum of their radii, and they are both spheres
        if ((glm::distance(location, obj.location) <= (scale.x + obj.scale.x))&&(obj.isSphere)&&(isSphere))
        {
            // Create vectors from the axis of normal alignment in both directions
            glm::vec3 colDir = glm::vec3(obj.location - location);
            glm::vec3 invColDir = glm::vec3(location - obj.location);

            // Calculate the angle of the objects velocity relative to the axis of normal alignment
            double theta = glm::angle(glm::normalize(velocity), glm::normalize(colDir));
            double epsilon = glm::angle(glm::normalize(obj.oldVelocity), glm::normalize(invColDir));

            // Calculate the component of the velocity that should be used in the collision
            glm::vec3 via = (glm::length (velocity) * float(cos(theta)))* glm::normalize(colDir);
            // Calculate the component of the velocity that should NOT be used int the collision
            glm::vec3 viNA = velocity - via;
            glm::vec3 vib = (glm::length(obj.oldVelocity) * float(cos(epsilon)))*glm::normalize(invColDir);
            // Set up variables for the physics calculation
            double ma = mass;
            double el = (elasticity + obj.elasticity)/2;
            double mb = obj.mass;
            glm::vec3 vfa = glm::vec3(0,0,0);

            // Do the momentum calculations in all directions
            vfa.x  = (via.x *((ma - mb)/(ma + mb)) + vib.x*((2*mb)/(ma + mb)))*el + ((ma*via.x+ mb*vib.x)/(ma+mb))*(1 - el);
            vfa.y  = (via.y *((ma - mb)/(ma + mb)) + vib.y*((2*mb)/(ma + mb)))*el + ((ma*via.y+ mb*vib.y)/(ma+mb))*(1 - el);
            vfa.z  = (via.z *((ma - mb)/(ma + mb)) + vib.z*((2*mb)/(ma + mb)))*el + ((ma*via.z+ mb*vib.z)/(ma+mb))*(1 - el);

            // Add back the reflected component of the velocity that was not involved in the collision
            vfa += glm::reflect (viNA,glm::normalize(colDir));

            // Set the object's velocity
            velocity = vfa;
        }


        // If a sphere is colliding with the boundaries
        if (isSphere && !(obj.isSphere))
        {

            bool collision = false;
            glm::vec3 colDir = glm::vec3(0,0,0);

            // If the sphere collides with the +x side of the domain
            if((location.x + scale.x) >= (obj.scale.x))
            {
                // Keep track of the direction of the collision
                // (whether the sphere has collided with multiple walls)
                colDir += glm::vec3(1,0,0);
                location.x = obj.scale.x - scale.x;
                collision = true;

            }

            // If the sphere collides with the -x side of the domain
            if((location.x - scale.x) <= (-obj.scale.x))
            {
                colDir += glm::vec3(-1,0,0);
                location.x = -obj.scale.x + scale.x;
                collision = true;

            }

            // If the sphere collides with the +y side of the domain
            if((location.y + scale.y) >= (obj.scale.y))
            {
                colDir += glm::vec3(0,1,0);
                location.y = obj.scale.y - scale.y;
                collision = true;

            }

            // If the sphere collides with the -y side of the domain
            if((location.y - scale.y) <= (-obj.scale.y))
            {
                colDir += glm::vec3(0,-1,0);
                location.y = -obj.scale.y + scale.y;
                collision = true;

            }

            // If the sphere collides with the +z side of the domain
            if((location.z + scale.z) >= (obj.scale.z))
            {
                colDir += glm::vec3(0,0,1);
                location.z = obj.scale.z - scale.z;
                collision = true;

            }

            // If the sphere collides with the -z side of the domain
            if((location.z - scale.z) <= (-obj.scale.z))
            {
                colDir += glm::vec3(0,0,-1);
                location.z = -obj.scale.z + scale.z;
                collision = true;
            }

            colDir = glm::normalize(colDir);

            // If a collision has occured
            if (collision)
            {
                // Calculate angle that determines the component of the velocity to use in calculations
                double theta = glm::angle(glm::normalize(velocity), colDir);

                // Find the reflected component and non-reflected component
                glm::vec3 via = (glm::length (velocity) * float(cos(theta)))* glm::normalize(colDir);
                glm::vec3 viNA = velocity - via;

                glm::vec3 vfa = glm::vec3(0,0,0);

                // Only the momentum-relevant component is effected by elasticity
                vfa.x  = -(via.x)*elasticity;
                vfa.y  = -(via.y)*elasticity;
                vfa.z  = -(via.z)*elasticity;

                // Add back the non-relevant reflected velocity
                vfa += glm::reflect (viNA,colDir);

                // Set the velocity of the object
                velocity = vfa;
            }

        }
    }

    void Gravity (Object obj, float dTime)
    {
        if ((obj.isSphere)&&(isSphere))
        {
            glm::vec3 colDir = glm::vec3(obj.location - location);
            double fG;
            double r = glm::distance(location, obj.location);
            fG = (6.67)*pow(10,-11)*(mass*obj.mass)/(r*r);

            glm::vec3 a;
            a = glm::normalize(colDir)* float(fG/mass);

            velocity += a*(dTime);
        }
    }

//    void takeInput
};

// Light
class Light
{
public:
    glm::vec3 location;// Location
    glm::vec3 diffuse;// RGB of diffuse
    glm::vec3 ambient;// RGB of ambient
    glm::vec3 specular;// RGE of specular
    glm::vec3 direction;// Components of direction
    float constant;// Amount of constant light
    float linear;// Amount of linear falloff
    float quadratic;// Amount of quadratic falloff
    float cutOff;
    float outerCutOff;
    int type;// The type of light: 0 for point light, 1 for directional light, 2 for spot light
    string index;//The "birth number" of the light


    void Draw( Shader shader )// A function meant to be used in a loop to automate the process of passing all uniform information to the fragment shader
    {
        // OpenGL is weird. I need to specify the exact name of the uniform I want to find the location of, but in a GLchar
        // This means that (the way it is intended to be done) you have to explicity declare every single uniform affected
        // by every single light, individually. This is a problem if you have many lights because, even if they were identical
        // and you stored them neatly in an array, you would have to specifically type out the index of each array when activating the lights,
        // defeating the purpose of having them in an array.

        // To remedy this situation, I thought I could have a function (this function) that does the necessary uniform setting
        // for any given light in the light array, and cycle through this function with each different index of light. I thought
        // I could concatinate the string representing the name of the uniform with the index of the light (which is passed in as
        // string) automating the process, but alas, I needed a GLchar *.

        // GLchar *'s don't concatinate, and (while the entire array of characters can be changed) individual indices cannot

        // For instance:
        // GLchar * word = "light[i].position" ;//works
        // word = "light[0].position";// Still works
        // cout << word [6]; //Outputs "0" as expected
        // word [6] = "1";// Gives an error
        // word [6] = '1';// Gives no error *******BUT CRASHES AT RUNTIME*********
        // So simply changing the [index] part of the GLchar * to match the index of the light is impossible

        // However, through some witchcraft, I managed to concatinate some strings, then somehow convert them to a GLchar const * (it only works for const)

        // This is a lot of code, but it automates the process

        string startPath = "light[" + index;// A string for the beginning part of the uniform name (will be constant, except for the index)
        string endPath = "].position";// A string for the end part of the uniform name (will be changed)
        string wholePath = startPath + endPath;// Concatinating the strings into one
        GLchar const * whichUniform = wholePath.c_str();// Setting th GLchar const * to the name of the uniform (concatination cannot be done in this step)
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), location.x, location.y, location.z); // Setting the uniform based on the light's parameters

        endPath = "].ambient"; // Changing the end of the uniform name to affect the ambient light
        wholePath = startPath + endPath; // Re-concatinating
        whichUniform = wholePath.c_str(); // Changing the GLchar const * to the new re-concatinated variable
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), ambient.r, ambient.g, ambient.b);

        endPath = "].diffuse"; // The rest is all the same
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), diffuse.r, diffuse.g, diffuse.b);

        endPath = "].specular";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), specular.r, specular.g, specular.b );

        endPath = "].direction";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform3f (glGetUniformLocation(shader.Program, whichUniform), direction.x, direction.y, direction.z);


        endPath = "].constant";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation(shader.Program, whichUniform), constant);

        endPath = "].linear";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation(shader.Program, whichUniform), linear);

        endPath = "].quadratic";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation(shader.Program, whichUniform), quadratic);

        endPath = "].cutOff";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation (shader.Program, whichUniform), glm::cos(glm::radians(cutOff)));

        endPath = "].outerCutOff";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation (shader.Program, whichUniform), glm::cos(glm::radians(outerCutOff)));

        endPath = "].type";
        wholePath = startPath + endPath;
        whichUniform = wholePath.c_str();
        glUniform1f (glGetUniformLocation (shader.Program, whichUniform), type);

        // For reference; the non-automated version of the code would look like this
        // glUniform3f (glGetUniformLocation(shader.Program, "light[0].position"), location.x, location.y, location.z);
    }

};


#endif // OBJECT_H_INCLUDED
