#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Body.h"
#include "Particle.h"
#include "Force.h"

using namespace std;

// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


const int gridX = 16;
const int gridY = 16;


//declare the number of particles in the simulation
constexpr int numberOfParticles = gridX * gridY;


const float particleMass = 0.1f;

//spring values
const float springStiffness = 1.65f;
const float springDamping = 1.5f;
const float springRestingLength = 0.01f;

Gravity* g = new Gravity;

const int offsets[4][2] = {  {0, 1}, {1,1},{-1, 1}, {1,0} };

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 15.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);

	
	
	// create particles
	
	vector<vector<Particle>> particles(gridX, vector<Particle>(gridY));


	float x = 0.0;
	float z = 0.0;
	

	for (int i = 0; i < gridX; i++)
	{
		for (int j = 0; j < gridY; j++)
		{
			x = ((i * 9.8 / gridX) - 4.9);
			z = ((j * 9.8 / gridY) - 4.9);
			particles[i][j].setMass(particleMass);
			particles[i][j].setPos(glm::vec3(x, 8.0, z));
			particles[i][j].getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
			particles[i][j].addForce(g);
		}
	}

	for (int i = 0; i < gridX; i++)
	{
		for (int j = 0; j < gridY; j++)
		{
			//attach springs to neigboring particles
			for (int k = 0; k < 4; k++) {

				int neighborX =  i + offsets[k][0];
				int neighborY = j + offsets[k][1];

				// Check if the neighbor is within the grid bounds
				if ( 0 <= neighborX && neighborX < gridX && neighborY >= 0 && neighborY < gridY) {
					
					// Connect the current point with the neighbor
					Hooke* spring = new Hooke(&particles[i][j], &particles[neighborX][neighborY], springStiffness, springDamping, springRestingLength);
					Hooke* spring2 = new Hooke(&particles[neighborX][neighborY], &particles[i][j], springStiffness, springDamping, springRestingLength);
					particles[neighborX][neighborY].addForce(spring);
					particles[i][j].addForce(spring2);
				}
			}

		
		}
	}	
	/////
	double t = 0.0;
	double dt = 0.01;
	double initialTime = glfwGetTime();
	double currentTime = 0.0;
	double accumulator = 0.0;
	double newTime;

	

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		
		
		// time stuff
		newTime = glfwGetTime()- initialTime;
		t = newTime - currentTime;
		currentTime = newTime;

		accumulator += t;

		while (accumulator >= dt)
		{
			
			for (int i = 0; i < gridX; i++)
			{
				for (int j = 0; j < gridY; j++)
				{
					if(i == 0 || i == gridX-1  || j == 0 || j == gridX-1)
					{
						continue;
					}
					
					//calculate the total force being applied to the particle

					particles[i][j].setAcc(particles[i][j].applyForces(particles[i][j].getPos(), particles[i][j].getVel(), t, dt));


					//calculate the particle's new velocity based on the acceleration

					particles[i][j].setVel(particles[i][j].getVel() + (particles[i][j].getAcc()  * dt));

					//set the particle's new position

					particles[i][j].setPos(particles[i][j].getPos() + particles[i][j].getVel()*dt);

					//collisions with room

					{
						//if the particle is outside the y boundaries of the "cube", bounce off the "wall"

						if (particles[i][j].getPos().y < 0.0f)
						{
							particles[i][j].setPos(1, 0); //set the y co ordinate to 0
							particles[i][j].setVel(particles[i][j].getVel() * glm::vec3(1.0f, -0.5f, 1.0f)); //invert the y velocity to simulate a "bounce"

						}


						if (particles[i][j].getPos().y > 10.0f)
						{
							particles[i][j].setPos(1, 10);
							particles[i][j].setVel(particles[i][j].getVel() * glm::vec3(1.0f, -0.5f, 1.0f)); //invert the y velocity to simulate a "bounce"
						}

						//if the particle is outside x boundaries of the "cube", bounce off the "wall"
						if (particles[i][j].getPos().x < -5.0f)
						{
							particles[i][j].setPos(0, -5);
							particles[i][j].setVel(particles[i][j].getVel() * glm::vec3(-.5f, 1.0f, 1.0f));

						}

						if (particles[i][j].getPos().x > 5.0f)
						{
							particles[i][j].setPos(0, 5);
							particles[i][j].setVel(particles[i][j].getVel() * glm::vec3(-.5f, 1.0f, 1.0f));

						}

						//if the particle is outside z boundaries of the "cube", bounce off the "wall"
						if (particles[i][j].getPos().z < -5.0f)
						{
							particles[i][j].setPos(2, -5);
							particles[i][j].setVel(particles[i][j].getVel() * glm::vec3(1.0f, 1.0f, -.5f));


						}

						if (particles[i][j].getPos().z > 5.0f)
						{
							particles[i][j].setPos(2, 5);
							particles[i][j].setVel(particles[i][j].getVel() * glm::vec3(1.0f, 1.0f, -.5f));

						}
					}

					//inter particle collisions
					{

					}
				}
			}
			accumulator -= dt;
			
		}
			/*
			**	INTERACTION
			*/
			// Manage interaction
			app.doMovement(t);
		

		/*
		**	SIMULATION
		*/
		


		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw ground plane
		app.draw(plane);
		// draw particles

		for (int i = 0; i < gridX; i++)
		{
			for (int j = 0; j < gridY; j++)
			{
				app.draw(particles[i][j].getMesh());
			}
		}
		// draw demo objects
	/*	app.draw(cube);
		app.draw(sphere);*/

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

