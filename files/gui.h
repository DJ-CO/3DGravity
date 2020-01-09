#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <iostream>
#include <string>
#include <fstream>

#define BOX_START_X 197
#define BOX_START_Y 434
#define BOX_WIDTH 133.3
#define BOX_HEIGHT 33

using namespace std;

void checkClick (int mouseX, int mouseY);
double inputValue (int row, int column);
void loadCharacters (void);

// Structure for the characters used in text
struct Character
{
    // Texture ID of the texture
    GLuint TextureID;
    // Size of the texture
    glm::ivec2 Size;
    // Bearing of the character in the texture
    glm::ivec2 Bearing;
    // Offset to advance to next texture
    GLuint Advance;
};

// Don't know what a map is, but it was used by the function (at the bottom) that was
// Taken from a tutorial
map <GLchar, Character> Characters;

// Check if a click has been clicked
// Find out where the click was
// Check the location of the click for the domains of the boxes
class GUI_C
{
    // Write each relevant character to the input file
    ifstream fin;                               // Create input file
    ofstream fout;                              // Output to file

public:
// Text vertex array object and vertex buffer object
    GLuint VAO, VBO;
// Variable to keep track of if user has navigated to a new box
    bool newHit = false;
// Give class buffer variables
    bool hit = false;
// Used to determine when input is ready to use
    bool inputReady = false;
// Which parameter is being affected
    int activeRow = 0;
// Which sphere is being affected
    int activeColumn = 0;
// a buffer value used for any type of input
    double bufferValue = 0;
// Variable to keep track of whether the mouse is down or up: needed to make "isHidden" function properly
// since it has the properties of a button, rather than a text box
    bool clickDown = false;
// Variable to keep track of whether to keep changing string
    bool newKey = false;
    double inputBuffer = 0;
    string inStringBuffer = "";
    double input = 0;
    string inString = "";

    void checkClick (SDL_Event event)// Function that takes the position of the mouse (after it has clicked) and determines which value to get
    {
        newHit = false;
        // Check click
        if ((event.button.type == SDL_MOUSEBUTTONDOWN) && (event.button.button == SDL_BUTTON_LEFT))
        {
            // Set variables to hold the x and y position of the mouse
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;

            hit = false;
            clickDown = true;

            // Loop through the columns to find which ball was specified
            for (int column = 0; column < 6; column++)
            {
                // Check x boundaries
                if ( (mouseX > (column * BOX_WIDTH + BOX_START_X) ) && (mouseX < (column * BOX_WIDTH + BOX_START_X + BOX_WIDTH) ) )
                {
                    // Loop through the rows to find which parameter was specified
                    for (int row = 0; row < 11; row ++)
                    {
                        // Check y boundaries
                        if( (mouseY > (row * BOX_HEIGHT + BOX_START_Y) ) && (mouseY < (row * BOX_HEIGHT + BOX_START_Y + BOX_HEIGHT) ) )
                        {
                            // Determine if the hit is a new hit
                            if (activeColumn != column || activeRow != row) newHit = true;
                            else newHit = false;
                            // Set flag that somthing useful has been clicked at all
                            hit = true;
                            // Update active rows and columns
                            activeRow = row;
                            // Column + 2 because the index of spheres starts at [2] in the object array
                            activeColumn = column + 2;
                        }
                    }
                }
            }
        }

        // If the mouse click is released, set a flag
        if ((event.button.type == SDL_MOUSEBUTTONUP) && (event.button.button == SDL_BUTTON_LEFT))
        {
            clickDown = false;
        }
    }

    Object inputValue (Object sphere, SDL_Event event)
    {
        // Stop the users input from carrying over across text boxes
        if (newHit)
        {
            inStringBuffer = "";
            inString = "";
            newHit = false;
        }

        if (hit)
        {
            // If the user hits enter or navigates to a different box write
            if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE))
            {
                newKey = true;
            }

            if (event.type == SDL_KEYUP && (event.key.keysym.sym == SDLK_RETURN|| event.key.keysym.sym == SDLK_SPACE) && newKey)
            {
                newKey = false;
                inputReady = true;
                input = writeToFile(inString);
                inString = "";
            }

            // Let user use backspace
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE)
            {
                newKey = true;
            }
            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_BACKSPACE && newKey && inString.length() > 0)
            {
                inString.erase(inString.length()-1,1);
                newKey = false;
            }

            // Determine which parameters to affect and which sphere to apply it to based on the row and column
            // Toggle the existence of the sphere
            if ((activeRow == 0)&&(!clickDown))
            {
                // Swith the sphere's hidden value
                if (sphere.hidden) sphere.hidden = false;
                else sphere.hidden = true;
                // Reset hit to zero
                hit = false;
            }

            // Affect the x location of the sphere
            if (activeRow == 1)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.location.x = input;
                    hit = false;
                }
            }

            // Affect the y location of the sphere
            if (activeRow == 2)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.location.y = input;
                    hit = false;
                }
            }

            // Affect the z location of the sphere
            if (activeRow == 3)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.location.z = input;
                    hit = false;
                }
            }

            // Affect the x velocity of the sphere
            if (activeRow == 4)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.velocity.x = input;
                    hit = false;
                }
            }

            // Affect the y velocity of the sphere
            if (activeRow == 5)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.velocity.y = input;
                    hit = false;
                }
            }

            // Affect the z velocity of the sphere
            if(activeRow == 6)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.velocity.z = input;
                    hit = false;
                }
            }

            // Affect the mass coefficient of the sphere
            if (activeRow == 7)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady && input > 0)
                {
                    inputReady = false;
                    sphere.massNum = input;
                    hit = false;
                }
            }

            // Affect the mass exponent of the sphere
            if (activeRow == 8)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady)
                {
                    inputReady = false;
                    sphere.massExp = input;
                    hit = false;
                }
            }

            // Affect the radius of the sphere
            if (activeRow == 9)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady && input > 0)
                {
                    inputReady = false;
                    sphere.scale.x = input;
                    sphere.scale.y = input;
                    sphere.scale.z = input;
                    hit = false;
                }
            }

            // Affect the elasticity of the sphere
            if (activeRow == 10)
            {
                if (!clickDown && event.type == SDL_TEXTINPUT)
                {
                    newKey = true;
                    inStringBuffer = event.text.text[0];
                }

                if (inputReady && input >= 0 && input <= 100)
                {
                    inputReady = false;
                    sphere.elasticity = input/100.0;
                    hit = false;
                }
            }

            // Update the flag to determine whether a new key has been pressed
            if ((event.type == SDL_KEYUP)&&(newKey))
            {
                inString += inStringBuffer;
                newKey = false;
                //cout << inString <<endl;
            }
        }
        // Retrun the sphere
        return sphere;
    }

    double writeToFile (string input)
    {

        fout.open ("files/input.txt",ios_base::out|ios_base::trunc);   // Output file AND append to file
        fout.clear();
        double getValue = 0;
        double value = 0;
        // Write input to file
        fout << input;
        fout.close();

        fin.open ("files/input.txt");                  // Open the file
        fin.clear();
        // Get read input back from file
        fin >> getValue;
        // Catch invalid inputs
        while (fin.fail())
        {
            getValue = 0;
            fin.clear();
            fin.ignore(1000,'\n');
            cout << "Okay" << endl;
            inputReady = false;


        }
        fin.ignore(1000,'\n');
        fin.close();
        value = getValue;
        return value;
    }

    void updateInfoFile ()
    {
        if (hit && activeRow != 0)
        {
            ifstream fin;
            // Open file
            fin.open("files/info.txt");
            // Go through the text file line by line until the desired line is reached, then save that position
            int lines = 0;
            do
            {
                string q;
                getline(fin, q);
                lines++;
            }
            while (!fin.eof());
            lines--;
            // Create variable to hold the file's original contents
            string fileText [lines];
            // Go to the begining of the file
            fin.close();
            fin.open("files/info.txt");
            // Copy the file's contents into the string
            for (int i = 0; i < lines; i ++)
            {
                getline(fin, fileText[i]);
                // cout << fileText[i];
            }
            fin.close();
            // Edit the desired text in the string
            fileText[activeRow*6 + activeColumn-2] = inString + "|";
            // Open file and replace text
            fout.open("files/info.txt", ios_base::out|ios_base::trunc);
            for (int i = 0; i < lines; i ++)fout << fileText[i]<<endl;
            fout.close();
        }

    }

    void initTextVerts (void)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void RenderText(Shader &shader, string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour)
    {
        // Activate corresponding render state
        shader.Use();
        glUniform3f(glGetUniformLocation(shader.Program, "textColour"), colour.x, colour.y, colour.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // Iterate through all characters
        string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];

            GLfloat xpos = x + ch.Bearing.x * scale;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;
            // Update VBO for each character
            GLfloat vertices[6][4] =
            {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};


// THIS FUNCTION WAS TAKEN FROM A TUTORIAL ON
// LEARNOPENGL.COM
void loadCharacters (void)
{
    // Initialize Freetype
    FT_Library ftLib;
    if (FT_Init_FreeType(&ftLib))
    {
        cout << "ERROR::FREETYPE: Could not init FreeType Library" << endl;
    }

    FT_Face ftFace;
    if (FT_New_Face(ftLib, "resources/fonts/arial.ttf", 0, &ftFace))
    {
        cout << "ERROR::FREETYPE: Failed to load font" << endl;
    }

    // Set size of the font
    FT_Set_Pixel_Sizes(ftFace, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
        {
            cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            ftFace->glyph->bitmap.width,
            ftFace->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            ftFace->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character =
        {
            texture,
            glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows),
            glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top),
            ftFace->glyph->advance.x
        };
        Characters.insert(pair<GLchar, Character>(c, character));
    }

    // Clean up
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);
}

#endif // GUI_H_INCLUDED
