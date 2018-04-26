#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>
#include <limits>
#include "Material.h"

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;


// #define SCREEN_WIDTH 208
#define SCREEN_HEIGHT 100
#define SCREEN_WIDTH 100
// #define SCREEN_HEIGHT 164
#define FULLSCREEN_MODE false
vector<Triangle> triangles;
vector<Sphere> spheres;
float focalLength = SCREEN_HEIGHT/2;
vec4 cameraPos( 0, 0, -2.5, 1.0);
// Intersection intersection;
float m = std::numeric_limits<float>::max();
vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );

vec4 lightPos( 0, -0.5, -0.7, 1.0 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );

struct Intersection
{
  vec4 position;
  float distance;
  int triangleIndex;
  Material material;
};

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw(screen* screen);
bool ClosestIntersection(vec4 start, vec4 dir,
                         const vector<Triangle>& triangles,
                         Intersection& closestIntersection );
vec3 DirectLight( const Intersection& i );
bool SphereIntersection(vec4 start, vec4 dir,
                         const vector<Sphere>& spheres,
                         Intersection& closestIntersection );
vec4 ReflectedRay(vec4 normal, vec4 dir);
vec3 getReflectedColour(Intersection& intersectionReflection , vec4 dir );

int main( int argc, char* argv[] )
{

  LoadTestModel( triangles );
  spheres.push_back(Sphere(vec4(400,50,100, 1),50,vec3(1,0.5,0), Diffuse));
  float L = 555;
  spheres[0].centre *= 2/L;
  spheres[0].centre -= vec4(1,1,1,1);
  spheres[0].centre.x *= -1;
  spheres[0].centre.y *= -1;

  spheres[0].radius *= 2/L;

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  while( NoQuitMessageSDL() )
    {
      Update();
      Draw(screen);
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen)
{
  /* Clear buffer */
  // memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  for(int x=0; x<SCREEN_WIDTH; x++){
      for(int y=0; y<SCREEN_HEIGHT; y++){
        vec4 d = vec4(x-SCREEN_WIDTH/2, y - SCREEN_WIDTH/2, focalLength, 1);
        // d = d - cameraPos;
        Intersection intersection;
        if(ClosestIntersection(cameraPos, d, triangles, intersection)){

          int i = intersection.triangleIndex;
          Intersection intersection2 = intersection;
					// vec3 illumination =DirectLight(intersection);
          vec3 colour = triangles[i].color;

          if(intersection.material == Specular){ // && maxBounce > 0
            vec4 reflection = ReflectedRay(triangles[i].normal, d);
            // std::cout << "colour: ( "<<illumination.x
            //  <<", "<<illumination.y<<", "<<illumination.z<<")"<< std::endl;
            //  std::cout<< "i index: " << i<< std::endl;

            colour = getReflectedColour(intersection2 , reflection );
            // vec3 illumination =DirectLight(intersection);

          }
          vec3 illumination =DirectLight(intersection2);
          vec3 finalColour = illumination+colour;
          if(colour == vec3(0,0,0)) finalColour = colour;
          // if(colour == vec3(0,0,0)) finalColour = colour;
					PutPixelSDL(screen, x, y, finalColour*indirectLight);
          }

        else PutPixelSDL(screen, x, y, vec3(0,0,0));

        if(SphereIntersection(cameraPos, d, spheres, intersection)){
          vec3 illumination =float(80)*(DirectLight(intersection));
          vec3 colour = illumination+spheres[0].color;
          PutPixelSDL(screen, x, y, colour*indirectLight);
          }

      }
  }
}

/*Place updates of parameters here*/
void Update()
{
  static int t = SDL_GetTicks();
  /* Compute frame time */
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  /*Good idea to remove this*/
  // std::cout << "Render time: " << dt << " ms." << std::endl;
  /* Update variables*/
	// uint8_t* keystate = SDL_GetKeyState( 0 );
	// 		if( keystate[SDLK_UP] )
	// 		{
	// 		// Move camera forward
	// 		}
	// 		if( keystate[SDLK_DOWN] )
	// 		{
	// 		// Move camera backward
	// 		}
	// 		if( keystate[SDLK_LEFT] )
	// 		{
	// 		// Move camera to the left
	// 		}
	// 		if( keystate[SDLK_RIGHT] )
	// 		{
	// 		// Move camera to the right
	// 		}
}


bool ClosestIntersection(vec4 start,
                         vec4 dir,
                         const vector<Triangle>& triangles,
                         Intersection& closestIntersection ){
 bool doesIntersect = false;
 closestIntersection.distance = m;
 for(int i=0; i<triangles.size(); i++){

    vec4 v0 = triangles[i].v0;
    vec4 v1 = triangles[i].v1;
    vec4 v2 = triangles[i].v2;

    vec3 e1 = vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
    vec3 e2 = vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
    vec3 b = vec3(start.x-v0.x,start.y-v0.y,start.z-v0.z);

    vec3 d = vec3(dir.x, dir.y, dir.z);
    mat3 A( -d, e1, e2 );
    vec3 x = glm::inverse( A ) * b;

    float t,u,v;
    t = x.x;
    u = x.y;
    v = x.z;

    vec4 r = (start+t*dir);
    float distance = glm::distance(start,r);
    // if(t > 0){ // for Specular
    //Check if intersection occured with triangle
    // if((0<=u) && (0<=v) && (u+v<=1)){
    if((0<=u) && (0<=v) && (u+v<=1) && (0 <= t)){
      doesIntersect = true;
      //Check if intersection is new min
      float currentMin = closestIntersection.distance;
      // float distance = glm::distance(start,r);
      if(distance < currentMin){
        closestIntersection.position = r;
        closestIntersection.distance = distance;
        closestIntersection.triangleIndex = i;
        closestIntersection.material = triangles[i].material;
      }
    }
  // }
  }
  return doesIntersect;

}

vec3 DirectLight( const Intersection& i ){
  vec3 colour;
  vec4 n;
  float distance = glm::distance(lightPos,i.position);
  float A = 4*M_PI*distance*distance;
  vec4 r = (lightPos-i.position)/distance;
	vec3 B = lightColor/A;
  Intersection intersection;

  if(i.triangleIndex == 100){ //if sphere
    Sphere s = spheres[0];
    n = glm::normalize((i.position-s.centre)/s.radius);
    spheres[0].normal = n;
    colour = s.color;
    // std::cout << "n " <<n.y << std::endl;
    if(ClosestIntersection(i.position+vec4(0.1,0.1,0.1,0.1)*r,r,triangles,intersection)){
  			if(distance>intersection.distance){
          // std::cout << "Entering"  << std::endl;
  				return vec3(0,0,0);
  			   }
         }

  }

  else{
	Triangle t = triangles[i.triangleIndex];
	n = t.normal;
  colour = t.color;
	if(ClosestIntersection(i.position+vec4(0.1,0.1,0.1,0.1)*r,r,triangles,intersection)){
			if(distance>intersection.distance){
				return vec3(0,0,0);
			   }
	   }
     if(SphereIntersection(i.position+vec4(0.001,0.001,0.001,0.001)*r,r,spheres,intersection)){
   			if(distance>intersection.distance){
   				return vec3(0,0,0);
   			   }
   	   }

  }


	float x = glm::dot(r,n);
	if(x<0){
		x =0;
	}
	vec3 D = B*x;
	// vec3 illumination = D*

	return D*colour;


}


bool SphereIntersection(vec4 start, vec4 dir,
                         const vector<Sphere>& spheres,
                         Intersection& closestIntersection ){

  bool doesIntersect = false;
  for(int i=0; i<spheres.size(); i++){
    // std::cout << "Length: " << spheres.size() << std::endl;

    Sphere s = spheres[i];
    vec4 center = s.centre;
    float rad = s.radius;
    // vec4 sminc = start-centre;

    // float d = 4*(glm::pow((glm::dot(dir,sminc)),2))-(4*(glm::dot(sminc,sminc)-(rad*rad)));

    float dx = dir.x;
    float dy = dir.y;
    float dz = dir.z;
    //
    float a = (dx*dx) + (dy*dy) + (dz*dz);
    float b = (2*dx*(start.x-s.centre.x)) +
              (2*dy*(start.y-s.centre.y)) +
              (2*dz*(start.z-s.centre.z));
    float c = center.x*center.x + center.y*center.y + center.z*center.z + start.x*start.x + start.y*start.y + start.z*start.z
              - 2 * (center.x*start.x + center.y*start.y + center.z*start.z) - rad*rad;

    float d = b*b - 4 * a*c;
    // std::cout << "B " << b*b << std::endl;
    // std::cout << "4ac " << 4*a*c << std::endl;

    if (d< 0) {
			return false;
		}
		// Ray tangent or intersects in two points
		float t = (-b - sqrt(d)) / (2 * a);
		if (t < 0) {
			return false;
		}

    doesIntersect = true;
    // std::cout << "D out loop: " << d << std::endl;

      vec4 iPoint = start+t*dir;
      float distance = glm::distance(start,iPoint);
      float currentMin = closestIntersection.distance;
      if(distance < currentMin){
        closestIntersection.position = iPoint;
        closestIntersection.distance = distance;
        closestIntersection.triangleIndex = 100;  // large value indicating sphere
        closestIntersection.material = s.material;
      }
  }

  return doesIntersect;
}

vec4 ReflectedRay(vec4 normal, vec4 dir){
  vec4 reflected = dir - (float(2)*(normal*dir)*normal);

  return reflected;
}


vec3 getReflectedColour(Intersection& iReflection , vec4 dir ){
    vec3 colour;
    vec4 start = iReflection.position;
    bool doesIntersect = false;
    int p = iReflection.triangleIndex;
    for(int i=0; i<triangles.size(); i++){
      if((i != iReflection.triangleIndex) && (i != p) ){
       vec4 v0 = triangles[i].v0;
       vec4 v1 = triangles[i].v1;
       vec4 v2 = triangles[i].v2;

       vec3 e1 = vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
       vec3 e2 = vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
       vec3 b = vec3(start.x-v0.x,start.y-v0.y,start.z-v0.z);

       vec3 d = vec3(dir.x, dir.y, dir.z);
       mat3 A( -d, e1, e2 );
       vec3 x = glm::inverse( A ) * b;

       float t,u,v;
       t = x.x;
       u = x.y;
       v = x.z;

       vec4 r = (start+t*dir);
       float distance = glm::distance(start,r);
       if((0<=u) && (0<=v) && (u+v<=1) && (0 < t)){
         //Check if intersection is new min
         float currentMin = iReflection.distance;

         if(distance < currentMin){
           iReflection.position = r;
           iReflection.distance = distance;
           iReflection.triangleIndex = i;
           // std::cout << "Index Colour: "<< i << endl;
           // iReflection.material = triangles[i].material;
           colour = triangles[i].color;
           // colour = vec3(1,0,0);
           doesIntersect = true;
         }
       }
     }
   }

  if(SphereIntersection(start,  dir, spheres, iReflection )){
    colour = spheres[0].color;
  }




   if(!doesIntersect) colour = vec3(0,0,0);
    return colour;
}














//
