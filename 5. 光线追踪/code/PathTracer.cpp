#include "PathTracer.h"
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include "float.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

hitable *random_scene(){
	int n = 100;
	hitable **list = new hitable*[n+1];
	list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.0,0.8,0.8)));
	int i=1;
	for(int a=-4; a<4; a++){
		for(int b=-4; b<4; b++){
			float choose_mat = (rand()%1000)/1000.0;
			vec3 center(a+0.9*(rand()%1000)/1000.0, 0.2, b+0.9*(rand()%1000)/1000.0);
			if((center-vec3(4,0.2,0)).length()>0,9){
				if(choose_mat<0.5){
					list[i++] = new sphere(center, 0.2, new lambertian(vec3((rand()%1000)/1000.0*(rand()%1000)/1000.0,
					(rand()%1000)/1000.0*(rand()%1000)/1000.0,(rand()%1000)/1000.0*(rand()%1000)/1000.0)));
				}
				else if(choose_mat < 0.75){
					list[i++] = new sphere(center, 0.2,new metal(vec3(0.5*(1+rand()%1000)/1000.0,
					0.5*(1+rand()%1000)/1000.0,0.5*(1+rand()%1000)/1000.0),0.5*(rand()%1000)/1000.0));
				}
				else{
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(vec3(0.4,0.2,0.1)));
	list[i++] = new sphere(vec3(4,1,0), 1.0, new metal(vec3(0.5,0.6,0.7),0.0));
	return new hitable_list(list,i);
}

vec3 calcColor(const ray& r, hitable *world, int depth){
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec)) {
    	ray scattered;
    	vec3 attenuation;
    	if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
    		return attenuation*calcColor(scattered, world, depth+1);
		}
    } 
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(1.0,0.8,0.4);;
    }
}

PathTracer::PathTracer()
	: m_channel(4), m_width(800), m_height(600), m_image(nullptr) {}

PathTracer::~PathTracer()
{
	if (m_image != nullptr)
		m_image;
	m_image = nullptr;
}

void PathTracer::initialize(int width, int height)
{
	m_width = width;
	m_height = height;
	if (m_image != nullptr)
		delete m_image;

	// allocate pixel buffer, RGBA format.
	m_image = new unsigned char[width * height * m_channel];
}

unsigned char * PathTracer::render(double & timeConsuming)
{
	if (m_image == nullptr)
	{
		std::cout << "Must call initialize() before rendering.\n";
		return nullptr;
	}

	// record start time.
	double startFrame = clock();
    
    hitable *world = random_scene();
    int ns = 100;
    vec3 lookfrom = vec3(2,2,-5);
	vec3 lookat = vec3(0,0,-1); 
	float dist_to_focus = (lookat-lookfrom).length();
	float aperture = 0.1;
    camera cam(lookfrom,lookat,vec3(0,1,0),90,float(m_width)/float(m_height),aperture, dist_to_focus);
	// render the image pixel by pixel.
	for (int row = m_height - 1; row >= 0; --row)
	{
		for (int col = 0; col < m_width; ++col)
		{
            vec3 color(0,0,0);
			 for (int s=0; s < ns; s++) {
                float u = float(static_cast<float>(col) + (rand()%1000)/1000.0) / static_cast<float>(m_width);
                float v = float(static_cast<float>(row) + (rand()%1000)/1000.0) / static_cast<float>(m_height);
                ray r = cam.get_ray(u, v);
                color += calcColor(r, world, 0);
            }
            color /= float(ns);
            color = vec3(sqrt(color[0]),sqrt(color[1]),sqrt(color[2]));
			drawPixel(col, row, color);
		}
	}

	// record end time.
	double endFrame = clock();

	// calculate time consuming.
	timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	return m_image;
}

void PathTracer::drawPixel(unsigned int x, unsigned int y, const vec3 & color)
{
	// Check out 
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return;
	// x is column's index, y is row's index.
	unsigned int index = (y * m_width + x) * m_channel;
	// store the pixel.
	// red component.
	m_image[index + 0] = static_cast<unsigned char>(255 * color.x());
	// green component.
	m_image[index + 1] = static_cast<unsigned char>(255 * color.y());
	// blue component.
	m_image[index + 2] = static_cast<unsigned char>(255 * color.z());
	// alpha component.
	m_image[index + 3] = static_cast<unsigned char>(255);
}
