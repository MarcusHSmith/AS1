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

#define KEY_SPC 32 // SPACE BAR
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
int WindowId;



// Colors [r, g, b]
std::vector<float> ka(3);
std::vector<float> kd(3);
std::vector<float> ks(3);

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

vector<float> multiply_vectors( vector<float> a, vector<float> b ) {
  std::vector<float> add(3);
  add.at(0) = a.at(0) * b.at(0);
  add.at(1) = a.at(1) * b.at(1);
  add.at(2) = a.at(2) * b.at(2);
  return add;
}

float dot_vectors(vector<float> a, vector<float> b ) {
  return ((a.at(0) * b.at(0)) + (a.at(1) * b.at(1)) + (a.at(2) * b.at(2)));
}

vector<float> add_vectors(vector<float> a, vector<float> b ) {
  std::vector<float> add(3);
  add.at(0) = a.at(0) + b.at(0);
  add.at(1) = a.at(1) + b.at(1);
  add.at(2) = a.at(2) + b.at(2);
  return add;
}

vector<float> subtract_vectors(vector<float> a, vector<float> b ) {
  std::vector<float> sub(3);
  sub.at(0) = a.at(0) - b.at(0);
  sub.at(1) = a.at(1) - b.at(1);
  sub.at(2) = a.at(2) - b.at(2);
  return sub;
}

vector<float> scale_vector(float a, vector<float> v ) {
  std::vector<float> scale(3);
  scale.at(0) = v.at(0) * a;
  scale.at(1) = v.at(1) * a;
  scale.at(2) = v.at(2) * a;
  return scale;
}

vector<float> normalize(vector<float> v){
  float mag = abs( sqrt( (v.at(0) * v.at(0)) + (v.at(1) * v.at(1)) + (v.at(2) * v.at(2)) ) );
  v.at(0) = v.at(0) / mag;
  v.at(1) = v.at(1) / mag;
  v.at(2) = v.at(2) / mag;
  return v;
}

/* @DEBUG */
void print_vector(std::vector<float> v) {
  for( std::vector<float>::const_iterator i = v.begin(); i != v.end(); ++i )
     std::cout << to_string(*i) << ' ';
  std::cout << "\n" << ' ';
}


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

  std::vector<float> view_vector(3);

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

        std::vector<float> rgb(3);
        std::vector<float> Rm(3);
        std::vector<float> surface_normal(3);

        // This is right
        surface_normal.at(0) = x;
        surface_normal.at(1) = y;
        surface_normal.at(2) = z;

        // This works better, but @vv2 might also work
        view_vector.at(0) = 0.0;
        view_vector.at(1) = 0.0;
        view_vector.at(2) = 1.0;
     
        // Comment back in for directional Lights
        std::vector< std::vector<float> >::const_iterator light;
        std::vector<float> L(3);
        std::vector<float> Ia(3);
        std::vector<float> light_color(3);
        for ( light = directional_lights.begin(); light != directional_lights.end(); light++ ) {
          // Creates light vector from light source to surface point.
          // Adjusts for light and everything, YO!!!
          L.at(0) = -(*light).at(0);
          L.at(1) = -(*light).at(1);
          L.at(2) = -(*light).at(2);

          // REFLECTION: ALREADY NORMALIZED
          Rm =  subtract_vectors( scale_vector( 2.0 * dot_vectors( normalize(L), normalize(surface_normal) ),
                                                normalize(surface_normal) ),
                                  normalize(L) );

          Ia.at(0) += (*light).at(3);
          Ia.at(1) += (*light).at(4);
          Ia.at(2) += (*light).at(5);

          light_color.at(0) = (*light).at(3);
          light_color.at(1) = (*light).at(4);
          light_color.at(2) = (*light).at(5);

          rgb = add_vectors( rgb, add_vectors( multiply_vectors( scale_vector( fmax( 0.0, dot_vectors( normalize(L), normalize(surface_normal) ) ), kd ),
                                                                  light_color ),
                                               multiply_vectors( scale_vector( pow( fmax( 0.0, dot_vectors( Rm,  normalize(view_vector) ) ), sp ), ks ),
                                                                  light_color ) ) );
        }

        std::vector< std::vector<float> >::const_iterator light2;
        for ( light2 = point_lights.begin(); light2 != point_lights.end(); ++light2 ) {
          // Creates light vector from light source to surface point.
          // Adjusts for light and everything, YO!!!
          L.at(0) = ( ( (*light2).at(0) * radius )) - x;
          L.at(1) = ( ( (*light2).at(1) * radius )) - y;
          L.at(2) = ( (*light2).at(2) * radius ) - z;

          // REFLECTION: ALREADY NORMALIZED
          Rm =  subtract_vectors( scale_vector( 2.0 * dot_vectors( normalize(L), normalize(surface_normal) ),
                                                normalize(surface_normal) ),
                                  normalize(L) );

          Ia.at(0) += (*light2).at(3);
          Ia.at(1) += (*light2).at(4);
          Ia.at(2) += (*light2).at(5);

          light_color.at(0) = (*light2).at(3);
          light_color.at(1) = (*light2).at(4);
          light_color.at(2) = (*light2).at(5);

          rgb = add_vectors( rgb, add_vectors( multiply_vectors( scale_vector( fmax( 0.0, dot_vectors( normalize(L), normalize(surface_normal) ) ), kd ),
                                                                  light_color ),
                                               multiply_vectors( scale_vector( pow( fmax( 0.0, dot_vectors( Rm,  normalize(view_vector) ) ), sp ), ks ),
                                                                  light_color ) ) );

        }

        rgb = add_vectors( rgb, multiply_vectors( ka, Ia ) ); // Add the ambient term

        // THIS IS THE MAGIC
        setPixel(i, j, rgb.at(0), rgb.at(1), rgb.at(2));

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

void keyboarding (unsigned char key, int x, int y){
    switch (key){
        case 32:
            glutDestroyWindow ( WindowId );
            break;
        default:
            break;
    }
}


bool isColor(string object) {
  return object.compare("-ka") == 0 || object.compare("-kd") == 0
         || object.compare("-ks") == 0;
}

bool isLight(string object) {
  return object.compare("-dl") == 0 || object.compare("-pl") == 0;
}

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
    if (temp.compare("-ka") == 0) {
      //create the -ka r g b
      while (os >> temp) {
        ka.at(inputs) = std::stof(temp);
        inputs ++;
      }
      if (!check_inputs(inputs, "-ka")) {
        ka.clear();
      } 
    }
    if (temp.compare("-kd") == 0) {
      //create the -kd r g b
      while (os >> temp) {
        kd.at(inputs) = std::stof(temp);
        inputs ++;
      }
      if (!check_inputs(inputs, "-kd")) {
        kd.clear();
      }
    }
    if (temp.compare("-ks") == 0) {
      //create the -ks r g b
      while (os >> temp) {
        ks.at(inputs) = std::stof(temp);
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
      std::vector<float> pl(6);
      while (os >> temp) {
        pl.at(inputs) = std::stof(temp);
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
      std::vector<float> dl(6);
      while (os >> temp) {
        dl.at(inputs) = std::stof(temp);
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

  glutKeyboardFunc(keyboarding);
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  // glutDestroyWindow ( WindowId );

  return 0;
}