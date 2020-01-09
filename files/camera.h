#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <vector>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 0.01f;
GLfloat SENSITIVITY = 0.25f;
GLfloat ZOOM = 45.0f;

class Camera
{
public:
    // Creating a bunch of camera variables and giving them default values???
    Camera (glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 up = glm::vec3 (0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH):
        front (glm::vec3 (0.0f, 0.0f,-1.0f)), movementSpeed (SPEED), mouseSensitivity (SENSITIVITY), zoom (ZOOM)
        // Constructor for vector values
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this-> updateCameraVectors( );
    }

    // Scalar values
    Camera (GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
        front (glm::vec3(0.0f,0.0f,-1.0f)), movementSpeed (SPEED), mouseSensitivity(SENSITIVITY), zoom (ZOOM)
        // Constructor for scalar values
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this-> updateCameraVectors( );
    }

    // Create getter for the view matrix
    glm::mat4 GetViewMatrix ( )
    {
        return glm::lookAt(this->position, this-> position + this-> front, this-> up);
    }

    // Process keyboard input
    // delta time creates smooth frame-independent movement
    void ProcessKeyboard ( Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this-> movementSpeed * deltaTime;
        // If statements to control movement
        // If is used instead of elses because we want to be able to move in two directions at once
        if (FORWARD == direction)
        {
            this-> position += this -> front *velocity;
        }
        if (BACKWARD == direction)
        {
            this-> position -= this -> front *velocity;
        }
        if (LEFT == direction)
        {
            this-> position -= this -> right *velocity;
        }
        if (RIGHT == direction)
        {
            this-> position += this -> right *velocity;
        }
    }

    // Process the mouse movement
    // Offsets for the mouse cursor
    void ProcessMouseMovement (GLfloat xOffset,GLfloat yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= this-> mouseSensitivity ;
        yOffset *= this-> mouseSensitivity ;

        this -> yaw += xOffset;
        this ->pitch += yOffset;

        // You can turn around completely, but cannot look up and down completely, because you'd flip
        if (constrainPitch)
        {
            if (this->pitch >89.0f)
            {
                this-> pitch = 89.0f;
            }
            if (this->pitch <-89.0f)
            {
                this-> pitch = -89.0f;
            }
        }

        // Update Camera Vectors
        this->updateCameraVectors( );
    }

    // Process mouse scroll
    void ProcessMouseScroll (GLfloat yOffset)
    {

    }

    // Getter for zoom
    GLfloat GetZoom ( )
    {
        return this -> zoom;
    }

    // Getter for position
    glm::vec3 GetPosition ( )
    {
        return this->position;
    }

    glm::vec3 GetFront()
    {
        return this->front;
    }

private:
    // Delare all of the variables we've been using
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    GLfloat yaw;
    GLfloat pitch;
    GLfloat movementSpeed;
    GLfloat mouseSensitivity;
    GLfloat zoom;

    void updateCameraVectors ( )
    {
        // Calculate the new camera vectors
        // Some cool math I should probably look into
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        // Normalize the vector
        this ->front = glm::normalize(front);
        this ->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this ->up = glm::normalize(glm::cross(this->right, this->front));
    }
};



#endif // CAMERA_H_INCLUDED
