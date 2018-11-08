/*
===========================================================================
#(Date, Writer) : (18-10-31, Chanhaeng Lee)
This Interface is based on the cyclone demo of GPED book.
I'm not quite used to this structure. For this reason,
This structure is gonna change later.

Let me know The 'Architecture' of This CG Project...
This is an example of the architecture of this project for
the understandings.

Examples:
Application -> Demo(Graphics Experiments) -> Various imgui setting
										  -> Various graphics setting
			-> Demo(Physics Experiments) -> Various imgui setting
										 -> Various graphics setting

Demo means one purpose of demonstration such as graphics, physics, MapTools, GameMaking.
Actually we are gonna develop applications in this order :
graphics experiment application -> physics experiment application ->
MapTool application -> GameMaking application
Therefore, based on the cyclonde demo, 
GetApplication() function will get one purpose demo including lots of sub-demos.


I will state the important interface 
of the class Application structure (virtual keyword)
**** Application ****
getTitle() : The name of window
initGraphics() : literally intialize the graphics setup
initImgui() : initialize imgui library
setView() : Set up the camera properties such as projection, position, and so on.
deinit() : free resources allocated in demo

execute() : execute the mainloop
update() : update in demo
display() : render objects in deomo
key() : process key in demo
mouse() : process mouse in demo
mouseDrag() :not implemented yet, I will implement it later.

renderText() : not implemented yet, I will implement it later.
**** Application ****
===========================================================================
*/
#include "CG_Application.h"
extern CGProj::Application* getApplication();
CGProj::Application* app = getApplication();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	app->resize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	app->mouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	app->scroll(yoffset);
}


int main()
{
	app->initGraphics();
	glfwSetFramebufferSizeCallback(app->app_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(app->app_window, mouse_callback);
	glfwSetScrollCallback(app->app_window, scroll_callback);
	
	app->initImgui();
	
	app->execute();
	
	app->deinit();

	delete app;

	return 0;
}