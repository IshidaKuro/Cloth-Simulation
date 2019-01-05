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

//declare the number of particles in the simulation
const int numberOfParticles = 100;

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
	
	
	Particle p[numberOfParticles];
	float x = 0.0;
	float z = 0.0;
	int c=0;
	int d=0;

	for(int i = 0; i < numberOfParticles; i++)
	{
		
		
		if (d == 10)
		{
			c++;
			d = 0;
		}
		x = (d*9.8 / sqrt(numberOfParticles) - 4.9);
		z = (c*9.8 / sqrt(numberOfParticles) - 4.9);

		
		//cout << "loading (" << x << ", 8.0, " << z << ")";
		p[i].setPos(glm::vec3(x, 8.0, z));
		
		p[i].getMesh().setShader(Shader("resources/shaders/solid.vert", "resources/shaders/solid_blue.frag"));
		
		Gravity *g = new Gravity;
		p[i].addForce(g);
		//correct
		if (i % 10 != 9)
		{
			Hooke *spring = new Hooke(&p[i + 1], &p[i], 60, 25, .0001);
			p[i].addForce(spring);
		}
		//correct
		if (i % 10 != 0)
		
		{
				Hooke *spring2 = new Hooke(&p[i - 1], &p[i], 60, 25, .0001);
				p[i].addForce(spring2);
				
		}
		//correct
		if (i >9)
		
		{
				Hooke *spring3 = new Hooke(&p[i - 10], &p[i], 60, 25, .0001);
				p[i].addForce(spring3);
				
		}
		//correct
		if (i < 90)
		
		{
				Hooke *spring4 = new Hooke(&p[i + 10], &p[i], 60, 25, .0001);
				p[i].addForce(spring4);
				
		}
		d++;
		
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

		



			//	

			for (int i = 0; i<numberOfParticles; i++)//-1 denotes 2 anchor points
			{

				if (i != 0 && i!=9 && i!=90 && i!=99)
				{
					//calculate the total force being applied to the particle

					p[i].setAcc(p[i].applyForces(p[i].getPos(), p[i].getVel(), t, dt));





					//calculate the particle's new velocity based on the acceleration



					p[i].setVel(p[i].getVel() + (p[i].getAcc()  * dt));

					//set the particle's new position

					p[i].setPos(p[i].getPos() + p[i].getVel()*dt);


					//collisions with room

					{
						//if the particle is outside the y boundaries of the "cube", bounce off the "wall"

						if (p[i].getPos().y < 0.0f)
						{
							p[i].setPos(1, 0); //set the y co ordinate to 0
							p[i].setVel(p[i].getVel() * glm::vec3(1.0f, -0.5f, 1.0f)); //invert the y velocity to simulate a "bounce"

						}


						if (p[i].getPos().y > 10.0f)
						{
							p[i].setPos(1, 10);
							p[i].setVel(p[i].getVel() * glm::vec3(1.0f, -.5f, 1.0f)); //invert the y velocity to simulate a "bounce"
						}

						//if the particle is outside x boundaries of the "cube", bounce off the "wall"
						if (p[i].getPos().x < -5.0f)
						{
							p[i].setPos(0, -5);
							p[i].setVel(p[i].getVel() * glm::vec3(-.5f, 1.0f, 1.0f));

						}

						if (p[i].getPos().x > 5.0f)
						{
							p[i].setPos(0, 5);
							p[i].setVel(p[i].getVel() * glm::vec3(-.5f, 1.0f, 1.0f));

						}

						//if the particle is outside z boundaries of the "cube", bounce off the "wall"
						if (p[i].getPos().z < -5.0f)
						{
							p[i].setPos(2, -5);
							p[i].setVel(p[i].getVel() * glm::vec3(1.0f, 1.0f, -.5f));


						}

						if (p[i].getPos().z > 5.0f)
						{
							p[i].setPos(2, 5);
							p[i].setVel(p[i].getVel() * glm::vec3(1.0f, 1.0f, -.5f));

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

		//for(int i; i<5; i++) //- for loop doesn't draw anything
		for each (Particle(p) in p)
		{
			//app.draw(particles[i].getMesh());
			app.draw(p.getMesh());
		}
		// draw demo objects
	/*	app.draw(cube);
		app.draw(sphere);*/

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

