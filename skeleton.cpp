#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <stdint.h>

using namespace std;
using glm::vec3;
using glm::mat3;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 256
#define FULLSCREEN_MODE false


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
int t;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw(screen* screen);
void Interpolate(float a, float b, vector<float>& result);
void Interpolate(vec3 a, vec3 b, vector<vec3>& result);

int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );
  t = SDL_GetTicks();	/*Set start value for timer.*/

  while( NoQuitMessageSDL() )
    {
      // Draw(screen);
      // Update();
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  // vector<float> result(10);
  // Interpolate(14, 5,result);
  // for (int i=0; i<result.size(); ++i)
  //     cout << result[i] << " ";

  vector<vec3> result(4);
  vec3 a(1,4,9.2);
  vec3 b(4,1,9.8);
  Interpolate(a, b, result);
  for( int i=0; i<result.size(); ++i )
    {
      cout << "( "
      << result[i].x << ", "
      << result[i].y << ", "
      << result[i].z << " ) ";
    }
  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen)
{
  vec3 topLeft(1,0,0); //red
  vec3 topRight(0,0,1); //blue
  vec3 bottomRight(0,1,0); //green
  vec3 bottomLeft(1,1,0); //yellow

  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));


  vector<vec3> leftSide( SCREEN_HEIGHT );
  vector<vec3> rightSide( SCREEN_HEIGHT );
  Interpolate (topLeft, bottomLeft, leftSide );
  Interpolate (topRight, bottomRight, rightSide );

  // vector<vec3> rows( SCREEN_HEIGHT );
  // Interpolate (leftSide, rightSide, rows);
  // vec3 colour(1.0,0.0,0.0);
  vec3 colour(0.0,0.6,1.0);
  // for(int i=0; i<1000; i++)
  for(int i=0; i<SCREEN_HEIGHT; i++)
    {
      vector<vec3> rows( SCREEN_WIDTH );
      Interpolate (leftSide[i], rightSide[i], rows );
      for
      // uint32_t x = rand() % screen->width;
      // uint32_t y = rand() % screen->height;
      // PutPixelSDL(screen, x, y, colour);
    }
}

/*Place updates of parameters here*/
void Update()
{
  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  /*Good idea to remove this*/
  std::cout << "Render time: " << dt << " ms." << std::endl;
  /* Update variables*/
}


void Interpolate(vec3 a, vec3 b, vector<vec3>& result){
  float length = result.size();
  vec3 dif = b-a;
  float numSteps = length -1;
  vec3 step = dif / numSteps;

  for(int i=0; i <=numSteps; i++){
      result[i] = a;
      a += step;
  }
}

void Interpolate(float a, float b, vector<float>& result){
  float length = result.size();
  float dif = b - a;

  if(length == 1) result[0] = a + (dif/2); // placeholder val, change pls

  else{
    float numSteps = length - 1;
    float step = dif / numSteps;

    for(int i=0; i <=numSteps; i++){
        result[i] = a;
        a += step;
    }
  }
}























//
