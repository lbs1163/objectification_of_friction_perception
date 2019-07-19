//==============================================================================
/*
	Software License Agreement (BSD License)
	Copyright (c) 2003-2016, CHAI3D.
	(www.chai3d.org)

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	* Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above
	copyright notice, this list of conditions and the following
	disclaimer in the documentation and/or other materials provided
	with the distribution.

	* Neither the name of CHAI3D nor the names of its contributors may
	be used to endorse or promote products derived from this software
	without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.

	\author    <http://www.chai3d.org>
	\author    Francois Conti
	\version   3.2.0 $Rev: 1925 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
	C_STEREO_DISABLED:            Stereo is disabled
	C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
	C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
	C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;


//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cSpotLight *light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool;

// a few objects that are placed in the scene
cMesh* base;

// a colored background
cBackground* background;

// a font for rendering text
cFontPtr font;

// a label to display the rate [Hz] at which the simulation is running
cLabel* labelRates;

// a widget panel
cPanel* panel;

// some labels
cLabel* labelZStick;
cLabel* labelZStickValue;
cLabel* labelZStickDecrease;
cLabel* labelZStickIncrease;
cLabel* labelZMax;
cLabel* labelZMaxValue;
cLabel* labelZMaxDecrease;
cLabel* labelZMaxIncrease;
cLabel* labelSigma;
cLabel* labelSigmaValue;
cLabel* labelSigmaDecrease;
cLabel* labelSigmaIncrease;


// a flag that indicates if the haptic simulation is currently running
bool simulationRunning = false;

// a flag that indicates if the haptic simulation has terminated
bool simulationFinished = true;

double mouseX, mouseY;

// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread* hapticsThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;

// root resource path
string resourceRoot;


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// callback to handle mouse click
void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods);

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);


//==============================================================================
/*
	DEMO:   13-primitives.cpp

	This example illustrates how to build simple triangle based mesh primitives
	using the functions provided in file graphics/CPrimitives.h
*/
//==============================================================================

int main(int argc, char* argv[])
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 13-primitives" << endl;
	cout << "Copyright 2003-2016" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[s] - Save copy of shadowmap to file" << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[m] - Enable/Disable vertical mirroring" << endl;
	cout << "[q] - Exit application" << endl;
	cout << endl << endl;


	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLFW library
	if (!glfwInit())
	{
		cout << "failed initialization" << endl;
		cSleepMs(1000);
		return 1;
	}

	// set error callback
	glfwSetErrorCallback(errorCallback);

	// compute desired size of window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int w = 0.8 * mode->height;
	int h = 0.5 * mode->height;
	int x = 0.5 * (mode->width - w);
	int y = 0.5 * (mode->height - h);

	// set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// set active stereo mode
	if (stereoMode == C_STEREO_ACTIVE)
	{
		glfwWindowHint(GLFW_STEREO, GL_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_STEREO, GL_FALSE);
	}

	// create display context
	window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
	if (!window)
	{
		cout << "failed to create window" << endl;
		cSleepMs(1000);
		glfwTerminate();
		return 1;
	}

	// get width and height of window
	glfwGetWindowSize(window, &width, &height);

	// set position of window
	glfwSetWindowPos(window, x, y);

	// set key callback
	glfwSetKeyCallback(window, keyCallback);

	// set resize callback
	glfwSetWindowSizeCallback(window, windowSizeCallback);

	// set mouse button callback
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	// set current display context
	glfwMakeContextCurrent(window);

	// sets the swap interval for the current display context
	glfwSwapInterval(swapInterval);

#ifdef GLEW_VERSION
	// initialize GLEW library
	if (glewInit() != GLEW_OK)
	{
		cout << "failed to initialize GLEW library" << endl;
		glfwTerminate();
		return 1;
	}
#endif


	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();

	// set the background color of the environment
	world->m_backgroundColor.setWhite();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	// position and orient the camera
	camera->set(cVector3d(0.9, 0.0, 0.6),    // camera position (eye)
		cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

// set the near and far clipping planes of the camera
// anything in front or behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 10.0);

	// set stereo mode
	camera->setStereoMode(stereoMode);

	// set stereo eye separation and focal length (applies only if stereo is enabled)
	camera->setStereoEyeSeparation(0.03);
	camera->setStereoFocalLength(1.8);

	// set vertical mirrored display mode
	camera->setMirrorVertical(mirroredDisplay);

	// create a light source
	light = new cSpotLight(world);

	// attach light to camera
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(0.6, 0.6, 0.5);

	// define the direction of the light beam
	light->setDir(-0.5, -0.5, -0.5);

	// enable this light source to generate shadows
	light->setShadowMapEnabled(true);

	// set the resolution of the shadow map
	//light->m_shadowMap->setQualityLow();
	light->m_shadowMap->setQualityMedium();

	// set light cone half angle
	light->setCutOffAngleDeg(30);


	//--------------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();

	// get access to the first available haptic device
	handler->getDevice(hapticDevice, 0);

	// retrieve information about the current haptic device
	cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

	// if the haptic devices carries a gripper, enable it to behave like a user switch
	hapticDevice->setEnableGripperUserSwitch(true);

	// create a tool (cursor) and insert into the world
	tool = new cToolCursor(world);
	world->addChild(tool);

	// connect the haptic device to the tool
	tool->setHapticDevice(hapticDevice);

	// map the physical workspace of the haptic device to a larger virtual workspace.
	tool->setWorkspaceRadius(1.0);

	// define the radius of the tool (sphere)
	double toolRadius = 0.05;

	// define a radius for the tool
	tool->setRadius(toolRadius);

	// hide the device sphere. only show proxy.
	tool->setShowContactPoints(true, false);

	// enable if objects in the scene are going to rotate of translate
	// or possibly collide against the tool. If the environment
	// is entirely static, you can set this parameter to "false"
	tool->enableDynamicObjects(true);

	// haptic forces are enabled only if small forces are first sent to the device;
	// this mode avoids the force spike that occurs when the application starts when 
	// the tool is located inside an object for instance. 
	tool->setWaitForSmallForce(true);

	// start the haptic tool
	tool->start();


	//--------------------------------------------------------------------------
	// CREATE OBJECTS
	//--------------------------------------------------------------------------

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// stiffness properties
	double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;


	/////////////////////////////////////////////////////////////////////////
	// BASE
	/////////////////////////////////////////////////////////////////////////

	// create a mesh
	base = new cMesh();

	// add object to world
	world->addChild(base);

	// build mesh using a cylinder primitive
	cCreateCylinder(base,
		0.01,
		0.5,
		36,
		1,
		10,
		true,
		true,
		cVector3d(0.0, 0.0, -0.01),
		cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ)
	);

	// set material properties
	base->m_material->setGrayGainsboro();
	base->m_material->setStiffness(0.5 * maxStiffness);

	// build collision detection tree
	base->createAABBCollisionDetector(toolRadius);

	// use display list to optimize graphic rendering performance
	base->setUseDisplayList(true);


	//--------------------------------------------------------------------------
	// CREATE SHADERS
	//--------------------------------------------------------------------------

	// create program shader
	cShaderProgramPtr shaderProgram = cShaderProgram::create(C_SHADER_FONG_VERT, C_SHADER_FONG_FRAG);

	// set uniforms
	shaderProgram->setUniformi("uShadowMap", C_TU_SHADOWMAP);

	// assign shader to mesh objects in the world
	tool->setShaderProgram(shaderProgram);
	base->setShaderProgram(shaderProgram);


	//--------------------------------------------------------------------------
	// WIDGETS
	//--------------------------------------------------------------------------

	// create a font
	font = NEW_CFONTCALIBRI20();

	// create a label to display the haptic and graphic rate of the simulation
	labelRates = new cLabel(font);
	labelRates->m_fontColor.setBlack();
	camera->m_frontLayer->addChild(labelRates);

	// create a background
	background = new cBackground();
	camera->m_backLayer->addChild(background);

	// set background properties
	background->setCornerColors(cColorf(1.0f, 1.0f, 1.0f),
		cColorf(1.0f, 1.0f, 1.0f),
		cColorf(0.8f, 0.8f, 0.8f),
		cColorf(0.8f, 0.8f, 0.8f));

	// a widget panel
	panel = new cPanel();
	camera->m_frontLayer->addChild(panel);
	panel->setSize(300, 50);
	panel->m_material->setGrayDim();
	panel->setTransparencyLevel(0.8);


	// create some labels

	std::ostringstream stream;
	string zStickValueString;
	string zMaxValueString;
	string sigmaValueString;

	stream << std::fixed;
	stream.precision(1);

	stream << base->m_material->getZstick();
	zStickValueString = stream.str();
	stream.str(string());

	stream << base->m_material->getZmax();
	zMaxValueString = stream.str();
	stream.str(string());

	stream << base->m_material->getSigma();
	sigmaValueString = stream.str();
	stream.str(string());
	
	labelZStick = new cLabel(font);
	panel->addChild(labelZStick);
	labelZStick->setText("z_stick");
	labelZStick->setLocalPos(27, 30, 0.1);
	labelZStick->m_fontColor.setWhite();

	labelZStickValue = new cLabel(font);
	panel->addChild(labelZStickValue);
	labelZStickValue->setText(zStickValueString);
	labelZStickValue->setLocalPos(42, 10, 0.1);
	labelZStickValue->m_fontColor.setWhite();

	labelZStickDecrease = new cLabel(font);
	panel->addChild(labelZStickDecrease);
	labelZStickDecrease->setText("<");
	labelZStickDecrease->setLocalPos(10, 10, 0.1);
	labelZStickDecrease->m_fontColor.setWhite();

	labelZStickIncrease = new cLabel(font);
	panel->addChild(labelZStickIncrease);
	labelZStickIncrease->setText(">");
	labelZStickIncrease->setLocalPos(80, 10, 0.1);
	labelZStickIncrease->m_fontColor.setWhite();


	labelZMax = new cLabel(font);
	panel->addChild(labelZMax);
	labelZMax->setText("z_max");
	labelZMax->setLocalPos(127, 30, 0.1);
	labelZMax->m_fontColor.setWhite();

	labelZMaxValue = new cLabel(font);
	panel->addChild(labelZMaxValue);
	labelZMaxValue->setText(zMaxValueString);
	labelZMaxValue->setLocalPos(135, 10, 0.1);
	labelZMaxValue->m_fontColor.setWhite();

	labelZMaxDecrease = new cLabel(font);
	panel->addChild(labelZMaxDecrease);
	labelZMaxDecrease->setText("<");
	labelZMaxDecrease->setLocalPos(110, 10, 0.1);
	labelZMaxDecrease->m_fontColor.setWhite();

	labelZMaxIncrease = new cLabel(font);
	panel->addChild(labelZMaxIncrease);
	labelZMaxIncrease->setText(">");
	labelZMaxIncrease->setLocalPos(180, 10, 0.1);
	labelZMaxIncrease->m_fontColor.setWhite();


	labelSigma = new cLabel(font);
	panel->addChild(labelSigma);
	labelSigma->setText("sigma");
	labelSigma->setLocalPos(227, 30, 0.1);
	labelSigma->m_fontColor.setWhite();

	labelSigmaValue = new cLabel(font);
	panel->addChild(labelSigmaValue);
	labelSigmaValue->setText(sigmaValueString);
	labelSigmaValue->setLocalPos(235, 10, 0.1);
	labelSigmaValue->m_fontColor.setWhite();

	labelSigmaDecrease = new cLabel(font);
	panel->addChild(labelSigmaDecrease);
	labelSigmaDecrease->setText("<");
	labelSigmaDecrease->setLocalPos(210, 10, 0.1);
	labelSigmaDecrease->m_fontColor.setWhite();

	labelSigmaIncrease = new cLabel(font);
	panel->addChild(labelSigmaIncrease);
	labelSigmaIncrease->setText(">");
	labelSigmaIncrease->setLocalPos(280, 10, 0.1);
	labelSigmaIncrease->m_fontColor.setWhite();


	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);


	//--------------------------------------------------------------------------
	// MAIN GRAPHIC LOOP
	//--------------------------------------------------------------------------

	// call window size callback at initialization
	windowSizeCallback(window, width, height);

	// main graphic loop
	while (!glfwWindowShouldClose(window))
	{
		// get width and height of window
		glfwGetWindowSize(window, &width, &height);

		// render graphics
		updateGraphics();

		// swap buffers
		glfwSwapBuffers(window);

		// process events
		glfwPollEvents();

		// signal frequency counter
		freqCounterGraphics.signal(1);
	}

	// close window
	glfwDestroyWindow(window);

	// terminate GLFW library
	glfwTerminate();

	// exit
	return (0);
}

//------------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
	// update window size
	width = a_width;
	height = a_height;

	panel->setLocalPos(10, (height - panel->getHeight()) - 10);
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
	cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
		return;
	}

	// option - exit
	else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
	{
		glfwSetWindowShouldClose(a_window, GLFW_TRUE);
	}

	// option - save shadow map to file
	else if (a_key == GLFW_KEY_S)
	{
		cImagePtr image = cImage::create();
		light->m_shadowMap->copyDepthBuffer(image);
		image->saveToFile("shadowmapshot.png");
		cout << "> Saved screenshot of shadowmap to file.       \r";
	}

	// option - toggle fullscreen
	else if (a_key == GLFW_KEY_F)
	{
		// toggle state variable
		fullscreen = !fullscreen;

		// get handle to monitor
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		// get information about monitor
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// set fullscreen or window mode
		if (fullscreen)
		{
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			glfwSwapInterval(swapInterval);
		}
		else
		{
			int w = 0.8 * mode->height;
			int h = 0.5 * mode->height;
			int x = 0.5 * (mode->width - w);
			int y = 0.5 * (mode->height - h);
			glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
			glfwSwapInterval(swapInterval);
		}
	}

	// option - toggle vertical mirroring
	else if (a_key == GLFW_KEY_M)
	{
		mirroredDisplay = !mirroredDisplay;
		camera->setMirrorVertical(mirroredDisplay);
	}
}

void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	if (a_button == GLFW_MOUSE_BUTTON_LEFT && a_action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &mouseX, &mouseY);

		cCollisionRecorder recorder;
		cCollisionSettings settings;

		std::ostringstream stream;
		stream << std::fixed;
		stream.precision(1);		

		bool hit = camera->selectFrontLayer(mouseX, (height - mouseY), width, height, recorder, settings);
		if (hit)
		{
			if (recorder.m_nearestCollision.m_object == labelZStickDecrease)
			{
				double zStick = base->m_material->getZstick();
				zStick = zStick - 1;

				base->m_material->setZstick(zStick);
				zStick = base->m_material->getZstick();
				stream << zStick;
				labelZStickValue->setText(stream.str());
			}
			if (recorder.m_nearestCollision.m_object == labelZStickIncrease)
			{
				double zStick = base->m_material->getZstick();
				zStick = zStick + 1;
				stream << zStick;
				labelZStickValue->setText(stream.str());

				base->m_material->setZstick(zStick);

			}

			if (recorder.m_nearestCollision.m_object == labelZMaxDecrease)
			{
				double zMax = base->m_material->getZmax();
				zMax = zMax - 1;
				
				base->m_material->setZmax(zMax);
				zMax = base->m_material->getZmax();
				stream << zMax;
				labelZMaxValue->setText(stream.str());
			}
			if (recorder.m_nearestCollision.m_object == labelZMaxIncrease)
			{
				double zMax = base->m_material->getZmax();
				zMax = zMax + 1;
				stream << zMax;
				labelZMaxValue->setText(stream.str());

				base->m_material->setZmax(zMax);
			}

			if (recorder.m_nearestCollision.m_object == labelSigmaDecrease)
			{
				double sigma = base->m_material->getSigma();
				sigma = sigma - 1;

				base->m_material->setSigma(sigma);
				sigma = base->m_material->getSigma();
				stream << sigma;
				labelSigmaValue->setText(stream.str());
			}
			if (recorder.m_nearestCollision.m_object == labelSigmaIncrease)
			{
				double sigma = base->m_material->getSigma();
				sigma = sigma + 1;
				stream << sigma;
				labelSigmaValue->setText(stream.str());

				base->m_material->setSigma(sigma);

			}
		}
	}


}

//------------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	// close haptic device
	tool->stop();

	// delete resources
	delete hapticsThread;
	delete world;
	delete handler;
}

//------------------------------------------------------------------------------

void updateGraphics(void)
{
	/////////////////////////////////////////////////////////////////////
	// UPDATE WIDGETS
	/////////////////////////////////////////////////////////////////////

	// update haptic and graphic rate data
	labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
		cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");

	// update position of label
	labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);


	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	world->updateShadowMaps(false, mirroredDisplay);

	// render world
	camera->renderView(width, height);

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------

enum cMode
{
	IDLE,
	SELECTION
};

void updateHaptics(void)
{
	cMode state = IDLE;
	cGenericObject* object = NULL;
	cTransform tool_T_object;

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////////
		// HAPTIC RENDERING
		/////////////////////////////////////////////////////////////////////////

		// signal frequency counter
		freqCounterHaptics.signal(1);

		// compute global reference frames for each object
		world->computeGlobalPositions(true);

		// update position and orientation of tool
		tool->updateFromDevice();

		// compute interaction forces
		tool->computeInteractionForces();


		/////////////////////////////////////////////////////////////////////////
		// HAPTIC MANIPULATION
		/////////////////////////////////////////////////////////////////////////

		// compute transformation from world to tool (haptic device)
		cTransform world_T_tool = tool->getDeviceGlobalTransform();

		// get status of user switch
		bool button = tool->getUserSwitch(0);

		//
		// STATE 1:
		// Idle mode - user presses the user switch
		//
		if ((state == IDLE) && (button == true))
		{
			// check if at least one contact has occurred
			if (tool->m_hapticPoint->getNumCollisionEvents() > 0)
			{
				// get contact event
				cCollisionEvent* collisionEvent = tool->m_hapticPoint->getCollisionEvent(0);

				// get object from contact event
				object = collisionEvent->m_object;

				// get transformation from object
				cTransform world_T_object = object->getGlobalTransform();

				// compute inverse transformation from contact point to object 
				cTransform tool_T_world = world_T_tool;
				tool_T_world.invert();

				// store current transformation tool
				tool_T_object = tool_T_world * world_T_object;

				// update state
				state = SELECTION;
			}
		}

		//
		// STATE 2:
		// Selection mode - operator maintains user switch enabled and moves object
		//
		else if ((state == SELECTION) && (button == true))
		{
			// compute new tranformation of object in global coordinates
			cTransform world_T_object = world_T_tool * tool_T_object;

			// compute new tranformation of object in local coordinates
			cTransform parent_T_world = object->getParent()->getLocalTransform();
			parent_T_world.invert();
			cTransform parent_T_object = parent_T_world * world_T_object;

			// assign new local transformation to object
			object->setLocalTransform(parent_T_object);

			// set zero forces when manipulating objects
			tool->setDeviceGlobalForce(0.0, 0.0, 0.0);
		}

		//
		// STATE 3:
		// Finalize Selection mode - operator releases user switch.
		//
		else
		{
			state = IDLE;
		}


		/////////////////////////////////////////////////////////////////////////
		// FINALIZE
		/////////////////////////////////////////////////////////////////////////

		// send forces to haptic device
		tool->applyToDevice();
	}

	// exit haptics thread
	simulationFinished = true;
}

//------------------------------------------------------------------------------
