/*
 * Author:         Austin Brennan
 * University:     Clemson University
 * Course:         2D Fluid Simulation
 * Professor:      Dr. Jerry Tessendorf
 * Due Date:       4/7/2016
 */


#include <GL/gl.h>
#include <GL/glut.h>
#include <OpenImageIO/imageio.h>
#include <omp.h>
#include "FLIPSolver.h"
#include "CmdLineFind.h"

using namespace std;
using namespace lux;
OIIO_NAMESPACE_USING

#define MAX_TIME_STEP 0.04166667f // cap the maximum time step to 24 frames per second

struct point {
  float x, y, z;
};

FLIPSolver *fluid;
unsigned int iwidth = 512;
unsigned int iheight = 512;
unsigned int frame_count = 0;
timespec start_time;
string output_path;
bool write_to_output;
bool simulation_paused = false;


//----------------------------------------------------
//
//  Error handler
//
//----------------------------------------------------


void handleError(const char* error_message, int kill)
{
  fprintf(stderr, "Error: %s\n\n", error_message);

  if (kill == 1)
    exit(-1);
}


//----------------------------------------------------
//
//  Write images
//
//----------------------------------------------------


void writeImage() {
  char buffer[256];

  if (output_path.back() != '/') { output_path.push_back('/'); }
  if (sprintf(buffer, "%sfluid_simulator_%04d.jpg", output_path.c_str(), frame_count++) < 0) {
    handleError((const char *) "creating filename in writeImage() failed", 0);
    return;
  }
  const char *filename = buffer;
  const unsigned int channels = 3; // RGB
  float *write_pixels = new float[iwidth * iheight * channels];
  float *window_pixels = new float[iwidth * iheight * channels];
  ImageOutput *out = ImageOutput::create(filename);
  if (!out) {
    handleError((const char *) "creating output file in writeImage() failed", 0);
    return;
  }

  glReadPixels(0, 0, iwidth, iheight, GL_RGB, GL_FLOAT, window_pixels);
  long index = 0;
  for (unsigned int j = 0; j < iheight; j++) {
    for (unsigned int i = 0; i < iwidth; i++) {
      for (unsigned int c = 0; c < channels; c++) {
        write_pixels[(i + iwidth * (iheight - j - 1)) * channels + c] = window_pixels[index++]; //color[index++];
      }
    }
  }

  ImageSpec spec (iwidth, iheight, channels, TypeDesc::FLOAT);
  out->open (filename, spec);
  out->write_image (TypeDesc::FLOAT, write_pixels);
  out->close ();
  delete out;
  delete write_pixels;
  delete window_pixels;
}


//----------------------------------------------------
//
//  Setting up camera for OpenGL
//
//----------------------------------------------------


void setupTheViewVolume()
{
  struct point eye, view, up;

  // specify size and shape of view volume
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0,-1.0,1.0,0.0,20.0);

  // specify position for view volume
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  eye.x = 1.0; eye.y = 1.0; eye.z = 1.0;
  view.x = 1.0; view.y = 1.0; view.z = 0.0;
  up.x = 0.0; up.y = 1.0; up.z = 0.0;

  gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}


//----------------------------------------------------
//
//  OpenGL drawing commands
//
//----------------------------------------------------


void drawParticles() {
  // draw particles
  glPointSize(2.5f);
  glBegin(GL_POINTS);
  vector<FLIPParticle>::iterator pi = fluid->particles.begin();
  while(pi != fluid->particles.end()) {
    vector3 color = pi->color;
    glColor3f(color.x, color.y, color.z);
    vector2 position = pi->position;
    glVertex3f(position.x, position.y, 0.0f);
    ++pi;
  }
  glEnd();
  glFlush();
}


void drawScene()
{
  struct point front[4] = {
      {0.0f,0.0f,0.0f}, {0.0f,2.0f,0.0f}, {2.0f,2.0f,0.0f}, {2.0f,0.0f,0.0f} };


  // call for wireframe
  glPolygonMode(GL_FRONT,GL_LINE);
  glPolygonMode(GL_BACK,GL_LINE);

  // fat yellow lines
  glColor3f(1.0,1.0,0.0);
  glLineWidth(2.0);

  glBegin(GL_QUADS);
  for(int i=0; i<4; i++) glVertex3f(front[i].x,front[i].y,front[i].z);
  glEnd();

  drawParticles();
}


//----------------------------------------------------
//
//  Initialize Particles
//
//----------------------------------------------------


void initParticleSim(UPDATE_FUNCTION update_function, bool party_mode, const float h,
                     const float dx, const int nloops, const int oploops)
{

  srand (static_cast <unsigned> (time(0)));

  fluid = new FLIPSolver(1000, 0.0f, 2.0f, h, dx, nloops, oploops);
  fluid->update_function = update_function;
  fluid->party_mode = party_mode;
}


//----------------------------------------------------
//
//  OpenGL Callbacks
//
//----------------------------------------------------


void callbackDisplay( void )
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawScene();
  glutSwapBuffers();
}


// animate and display new result
void callbackIdle() {
  float delta_time = (1.0f / 24.0f);
  if (!simulation_paused) { fluid->update(delta_time); }
  if (write_to_output) { writeImage(); }
  glutPostRedisplay();
}


void callbackKeyboard( unsigned char key, int x, int y )
{
  float new_dampening;
  switch (key)
  {
    case ',' : case '<':
      new_dampening = fluid->dampening - 0.01f;
      fluid->dampening = new_dampening >= 0.0f ? new_dampening : 0.0f;
      cout << "Setting dampening. Bounce energy is " << fluid->dampening * 100 << "% of original energy" << endl;
      break;

    case '.': case '>':
      fluid->dampening += 0.01f;
    cout << "Setting dampening. Bounce energy is " << fluid->dampening * 100 << "% of original energy" << endl;
      break;

    case 'q':
    cout << "Exiting Program" << endl;
    exit(0);

    case 'p':
      fluid->party_mode = !fluid->party_mode;
      cout << (fluid->party_mode ? "Party mode is on" : "Party mode is off") << endl;
      break;

    case ' ':
      simulation_paused = !simulation_paused;
      cout << (simulation_paused ? "Simulation paused" : "Simulation un-paused") << endl;
      break;

    case 'o':
      write_to_output = !write_to_output;
      cout << (write_to_output ? "Starting writing to output" : "Ending writing to output") << endl;
      break;

    default:
    break;
  }
}


//----------------------------------------------------
//
//  Program usage
//
//----------------------------------------------------


void PrintUsage()
{
  cout << "FLIP_fluid_simulator keyboard choices\n";
  cout << "./,      increase/decrease % of energy retained after bounce\n";
  cout << "p        turn on party mode. Randomizes particle color on collison\n";
  cout << "w/a/s/d  switches gravity to point up/left/down/right\n";
  cout << "spacebar paused the simulation. pressing it again un-pauses the simulation\n";
  cout << "o        toggle writing to output_path\n";
  cout << "q        exits the program\n";
  cout << endl;
}


//----------------------------------------------------
//
//  Main
//
//----------------------------------------------------

void setNbCores( int nb )
{
  omp_set_num_threads( nb );
}

int main(int argc, char** argv) {
  CmdLineFind clf(argc, argv);
  output_path = clf.find("-output_path", "./output_images", "Output path for writing image sequence");
  int write_on_start = clf.find("-write_on_start", 0, "Flag for whether to start writing output images at the start of "
                                "the program or not");
  int party_mode = clf.find("-party_mode", 0, "Flag for starting in party mode or not");
  string update_function_str = clf.find("-update_function", "S", "Function in update (options 'LF' for leap frog or 'S'"
                                        " for sixth)");

  float h = clf.find("-radius", 0.15f, "Radius of influence of each particle");
  float dx = clf.find("-grid_cell_size", 0.05f, "Size of each grid cell on velocity grid");
  int nloops = clf.find("-nloops", 1, "Number of loops over pressure calculation");
  int oploops = clf.find("-oploops", 1, "NUber of orthogonal projections");

  // validate flags
  if (party_mode != 0 && party_mode != 1) { handleError("Invalid usage of party mode flag", true); }
  if (write_on_start != 0 && write_on_start != 1) { handleError("Invalid usage of write on start flag", true); }
  if (update_function_str.compare("S") != 0 && update_function_str.compare("LF") != 0)
    { handleError("Invalid usage of update_function flag", true); }

  // print key control usage
  PrintUsage();
  clf.usage("-h");
  clf.printFinds();

  // initialize particle simulation
  UPDATE_FUNCTION update_function;
  if (update_function_str.compare("S") == 0) { update_function = SIXTH; }
  else {update_function = LEAP_FROG; }

  initParticleSim(update_function, party_mode != 0, h, dx, nloops, oploops);

  // decide whether to write to output or not
  write_to_output = write_on_start != 0;

  // get starting clock time for dynamic timestep in callbackIdle func
  clock_gettime(CLOCK_REALTIME, &start_time);

  setNbCores(4);



  // initialize glut window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
  glutInitWindowSize(iwidth, iheight);
  glutInitWindowPosition(100, 50);
  glutCreateWindow("2D FLIP Particle Simulation");
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_MULTISAMPLE_ARB);
  setupTheViewVolume();
  glutDisplayFunc(callbackDisplay);
  glutKeyboardFunc(&callbackKeyboard);
  glutIdleFunc(&callbackIdle);
  glutMainLoop();
  return 0;
}