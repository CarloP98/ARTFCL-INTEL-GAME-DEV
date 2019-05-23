#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#define TimeInterval      0.25
#define maxSpeed          8.0
#define maxAccelleration  1.0

////////////////////  LOG CHARACHTER POSITION   ///////////////////
#define LOG_OBJECT(K)                                         \
	myfile << std::fixed << std::setprecision(2);             \
    myfile << std::setw(10) << std::left << time;             \
    myfile << std::fixed << std::setprecision(5);             \
    myfile << std::setw(15) << std::left << K.xPosition;      \
    myfile << std::setw(15) << std::left << K.zPosition;      \
    myfile << std::setw(15) << std::left << K.xVelocity;      \
    myfile << std::setw(15) << std::left << K.zVelocity;      \
    myfile << std::setw(15) << std::left << steering.xLinear; \
    myfile << std::setw(15) << std::left << steering.zLinear; \
    myfile << std::setw(15) << std::left << K.orientation;    \
    myfile << std::setw(15) << std::left << K.rotation;       \
    myfile << std::setw(15) << std::left << steering.angular; \
    myfile << std::endl
///////////////////////////////////////////////////////////////////

struct SteeringOutput
{
	float xLinear, zLinear;    // ACCELLERATION
	float angular;             //
};

struct Kinematic      //  [Accelleration] --> [Velocity] --> [Position]
{
	float  xPosition, zPosition;   // X and Z Position
	float  orientation;            // direction object is facing  (RADIANS)
	float  xVelocity, zVelocity;   // X and Z Velocity
	float  rotation;               // rate of change of orientation

	void HSupdate(SteeringOutput& steering)
	{
		//Update the position and orientation
		xPosition   += xVelocity * TimeInterval + 0.5f * steering.xLinear * TimeInterval * TimeInterval;
		zPosition   += zVelocity * TimeInterval + 0.5f * steering.zLinear * TimeInterval * TimeInterval;
		orientation += rotation  * TimeInterval + 0.5f * steering.angular * TimeInterval * TimeInterval;
		//Update the velocity and rotation
		xVelocity   += steering.xLinear * TimeInterval;
		zVelocity   += steering.zLinear * TimeInterval;
		rotation    += steering.angular * TimeInterval;
		//Normalize Velocity if its greater than predefined velocity
		/*
		if (abs(xVelocity) > maxSpeed  || abs(zVelocity) > maxSpeed)
		{
			float length = sqrt((xVelocity * xVelocity) + (zVelocity * zVelocity));
			// normalize vector
			xVelocity /= length;
			zVelocity /= length;
			// increase vector size
			xVelocity *= maxSpeed;
			zVelocity *= maxSpeed;
		}
		*/
	}

	void NEupdate(SteeringOutput& steering)
	{
		//Update the position and orientation
		xPosition   += xVelocity * TimeInterval;
		zPosition   += zVelocity * TimeInterval;
		orientation += rotation  * TimeInterval;
		//Update the velocity and rotation
		xVelocity   += steering.xLinear * TimeInterval;
		zVelocity   += steering.zLinear * TimeInterval;
		rotation    += steering.angular * TimeInterval;
        //Normalize Velocity if its greater than predefined velocity
		/*
		if (abs(xVelocity) > maxSpeed  || abs(zVelocity) > maxSpeed)   
		{
			float length = sqrt((xVelocity * xVelocity) + (zVelocity * zVelocity));
			// normalize vector
			xVelocity /= length;
			zVelocity /= length;
			// increase vector size
			xVelocity *= maxSpeed;
			zVelocity *= maxSpeed;
		}
		*/
	}
};

////////////// UPDATE ORIENTATION ////////////// 
float getNewOrientation(float& currentOrientation, float& xVelocity, float& zVelocity)
{
	if (xVelocity != 0 || zVelocity != 0) return atan2(xVelocity, zVelocity);
	else return currentOrientation;
}
////////////////////////////////////////////////

int main() 
{
	std::ofstream myfile;
	myfile.open("data.txt");

	float time = 0.0f;

	Kinematic character;
	SteeringOutput steering;

	character = { 0.0f, 60.0f, 0.0f, 8.0f, 2.0f, 0.0f };  // Initial conditions
	steering  = { 0.0f,1.0f,0.1 };                       // Initial conditions

	myfile << std::setw(10) << std::left << "TIME";
	myfile << std::setw(15) << std::left << "XPOSITION";
	myfile << std::setw(15) << std::left << "ZPOSITION";
	myfile << std::setw(15) << std::left << "XVELOCITIY";
	myfile << std::setw(15) << std::left << "ZVELOCITIY";
	myfile << std::setw(15) << std::left << "XLINEAR";
	myfile << std::setw(15) << std::left << "ZLINEAR";
	myfile << std::setw(15) << std::left << "ORIENTATION";
	myfile << std::setw(15) << std::left << "ROTATION";
	myfile << std::setw(15) << std::left << "ANGULAR";
	myfile << std::endl;
	LOG_OBJECT(character);

	for (int x = 0; x < 400; x++)
	{
		time += TimeInterval;

		character.HSupdate(steering);
		character.orientation = getNewOrientation(character.orientation, steering.xLinear, steering.zLinear);

		steering.xLinear = (character.xPosition / -100 * TimeInterval) * 9;
		steering.zLinear =  (character.zPosition / -100 * TimeInterval) * 3;
		steering.angular = steering.angular;

		////////////// NORMALIZE //////////////
		//float length = sqrt((steering.xLinear * steering.xLinear) + (steering.zLinear * steering.zLinear));
		// normalize vector
		//steering.xLinear /= length;
		//steering.zLinear /= length;
		// increase vector size
		//steering.xLinear *= maxAccelleration;
		//steering.zLinear *= maxAccelleration;
		///////////////////////////////////////

		LOG_OBJECT(character);
	}
	myfile.close();

	//std::cin.get();
}