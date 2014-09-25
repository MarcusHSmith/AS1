#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>
#include <iterator>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>


#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
int  WindowId;

// Colors [r, g, b]
std::vector<float> ka;
std::vector<float> kd;
std::vector<float> ks;

// Roughness
float sp;

// Lights [x, y, z, r, g, b]
std::vector< std::vector<float> > point_lights;
std::vector< std::vector<float> > directional_lights;


//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by looping over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));



  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      // sqr squares floats
      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius - dist*dist);

        // You should assume that the vector from eye to the surface will always be -Z.

        // PseudoCode

        float sum = 0.0f;

        //   N = Normal at this point on surface; //used to be in every iteration of loop

        std::vector< std::vector<float> >::iterator row;
        std::vector<float>::iterator light;
        for( row = directional_lights.begin(); row != directional_lights.end(); row ++){
          for (light = row->begin(); light != row->end(); light++){
            //   Rm = ( 2 * ( normalize(*light.L) dot normalize(N) ) * normalize(N) ) - *light.L;
            //   sum += ( kd * max(0, ( *light.L dot normalize(N) )) * *light.color )
            //          + ks * max(0, pow( normalize(Rm) dot normalize(V), sp));
          }
        }

          
        std::vector< std::vector<float> >::iterator row2;
        std::vector<float>::iterator light2;
        for( row2 = point_lights.begin(); row2 != point_lights.end(); row2 ++){
          for (light2 = row2->begin(); light2 != row2->end(); light2++){
          //   Rm = ( 2 * ( normalize(*light2.L) dot normalize(N) ) * normalize(N) ) - *light2.L;
          //   sum += ( kd * max(0, ( *light2.L dot normalize(N) )) * *light2.color )
          //          + ks * max(0, pow( normalize(Rm) dot normalize(V), sp));
          }
        }

        // setPixel(i, j, Ip.r, Ip.g, Ip.b);

        // THIS IS THE MAGIC
        // TODO: find r,g,b for each pixel
        setPixel(i, j, 1.0, 0.0, 0.0);

        

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
  }


  glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}


// void Update (DWORD milliseconds)                // Perform Motion Updates Here
// {
//     if (g_keys->keyDown [' '] == TRUE)           // Is the Space Bar Being Pressed?
//     {
//         glutDestroyWindow ( Win.id );
//         exit (0);
 
//         g_keys->keyDown [' '] = FALSE;
//     }

// }

bool isColor(string object) {
  return object.compare("-ka") == 0 || object.compare("-kd") == 0
         || object.compare("-ks") == 0;
}

bool isLight(string object) {
  return object.compare("-dl") == 0 || object.compare("-pl") == 0;
}

// TODO FINISH THIS SHIT
bool check_inputs(int inputs, string object) {
  if ( ( inputs == 3 && isColor(object) ) 
       || ( inputs == 6 && isLight(object) ) 
       || ( inputs == 1 && ( object.compare("-sp") == 0 ) ) ) {
    object += " has been created";
    std::cout << object <<std::endl;
    return 1;
  }
  else {
    object += " has an incorrect number of inputs.";
    std::cout << object <<std::endl;
    return 0;
  }
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

    
  std::string s;                    //a standard string
  getline(cin, s);
  int inputs;
  while (s.compare("") != 0) {

    std::stringstream os(s);          //a standard stringstream which parses 's'
    std::string temp;                 //a temporary string
    std::vector<string> v;
    
    // std::cout <<"s is: " <<s <<std::endl;
    
    // while (os >> temp) {                //the stringstream makes temp a token
    os >> temp;
    inputs = 0;
    if (temp.compare("-ka") == 0 && ka.size() == 0) {
      //create the -ka r g b
      while (os >> temp) {
        ka.push_back(std::stof(temp));
        inputs ++;
      }
      if (!check_inputs(inputs, "-ka")) {
        ka.clear();
      }
    }
    if (temp.compare("-kd") == 0) {
      //create the -kd r g b
      while (os >> temp) {
        kd.push_back(std::stof(temp));
        inputs ++;
      }
      if (!check_inputs(inputs, "-kd")) {
        kd.clear();
      }
    }
    if (temp.compare("-ks") == 0) {
      //create the -ks r g b
      while (os >> temp) {
        ks.push_back(std::stof(temp));
        inputs ++;
      }
      if (!check_inputs(inputs, "-ks")) {
        ks.clear();
      }
    }
    if (temp.compare("-sp") == 0) {
      //create the -sp v
      while (os >> temp) {
        sp = std::stof(temp);
        inputs ++;
      }
      if (!check_inputs(inputs, "-sp")) {
        sp = -1.0f;
      }
    }
    if (temp.compare("-pl") == 0) {
      //create the -pl x y z r g b
      std::vector<float> pl;
      while (os >> temp) {
        pl.push_back(std::stof(temp));
        inputs ++;
      }
      if (check_inputs(inputs, "-pl")) {
        point_lights.push_back(pl);
      }
      else {
        pl.clear();
      }
    }
    if (temp.compare("-dl") == 0) {
      //create the -dl x y z r g b
      std::vector<float> dl;
      while (os >> temp) {
        dl.push_back(std::stof(temp));
        inputs ++;
      }
      if (check_inputs(inputs, "-dl")) {
        directional_lights.push_back(dl);
      }
      else {
        dl.clear();
      }
    }
    getline(cin, s);
  }
  // }

    // std::cout << temp <<std::endl;  //and deletes that token from itself 
    //                                 //the token can now be
    //                                 //outputted to console, or put into an array, 
    //                                 //or whatever you choose to do ith it .

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  WindowId = glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}



        // for (int i = 0; i < lights.length; ++i)
        // {
        //   N = Normal at this point on surface;
        //   Rm = ( 2 * ( normalize(lights[i].L) dot normalize(N) ) * normalize(N) ) - lights[i].L;
        //   sum += ( kd * ( lights[i].L dot normalize(N) ) * lights[i].color )
        //          + ks * max(0, pow( normalize(Rm) dot normalize(V), sp));
        // }
        // Ip = (ka * ia) + sum





