#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#define TimeInterval      0.5
#define maxSpeed          3.0
#define maxAccelleration  1.0
#define targetRadious     5.0

////////////////////  LOG CHARACHTER POSITION   ///////////////////
#define LOG_OBJECT(X,K,S)                                     \
    myfile << std::setw(10) << X;                             \
	myfile << std::fixed << std::setprecision(2);             \
    myfile << std::setw(10) << std::left << time;             \
    myfile << std::fixed << std::setprecision(5);             \
    myfile << std::setw(15) << std::left << K.xPosition;      \
    myfile << std::setw(15) << std::left << K.zPosition;      \
    myfile << std::setw(15) << std::left << K.xVelocity;      \
    myfile << std::setw(15) << std::left << K.zVelocity;      \
    myfile << std::setw(15) << std::left << S.xLinear;        \
    myfile << std::setw(15) << std::left << S.zLinear;        \
    myfile << std::setw(15) << std::left << K.orientation;    \
    myfile << std::setw(15) << std::left << K.rotation;       \
    myfile << std::setw(15) << std::left << S.angular;        \
    myfile << std::endl
///////////////////////////////////////////////////////////////////

struct SteeringOutput
{
	float xLinear, zLinear;    // ACCELLERATION
	float angular;             // 
};

struct Kinematic 
{
	float  xPosition, zPosition;   // X and Z Position
	float  orientation;            // direction object is facing  (RADIANS)
	float  xVelocity, zVelocity;   // X and Z Velocity
	float  rotation;               // rate of change of orientation

	void HSupdate(SteeringOutput& steering)
	{
		//Update the position and orientation
		xPosition += xVelocity * TimeInterval + 0.5f * steering.xLinear * TimeInterval * TimeInterval;
		zPosition += zVelocity * TimeInterval + 0.5f * steering.zLinear * TimeInterval * TimeInterval;
		orientation += rotation * TimeInterval + 0.5f * steering.angular * TimeInterval * TimeInterval;
		//Update the velocity and rotation
		xVelocity += steering.xLinear * TimeInterval;
		zVelocity += steering.zLinear * TimeInterval;
		rotation += steering.angular * TimeInterval;
		//Normalize Velocity if its greater than predefined velocity
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
	}

	void NEupdate(SteeringOutput& steering)
	{
		//Update the position and orientation
		xPosition += xVelocity * TimeInterval;
		zPosition += zVelocity * TimeInterval;
		orientation += rotation * TimeInterval;
		//Update the velocity and rotation
		xVelocity += steering.xLinear * TimeInterval;
		zVelocity += steering.zLinear * TimeInterval;
		rotation += steering.angular * TimeInterval;
		//Normalize Velocity if its greater than predefined velocity
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
	}
};

void Seek(SteeringOutput& steering, Kinematic& character, float targetX, float targetZ)
{
	steering.xLinear = targetX - character.xPosition;
	steering.zLinear = targetZ - character.zPosition;
	////////////// NORMALIZE //////////////
    float length = sqrt((steering.xLinear * steering.xLinear) + (steering.zLinear * steering.zLinear));
	steering.xLinear /= length;
	steering.zLinear /= length;
	steering.xLinear *= maxAccelleration;
	steering.zLinear *= maxAccelleration;
	///////////////////////////////////////
}

void arrive(SteeringOutput& steering, Kinematic& character1, int* target, float Path1[], int size)
{
	float a = character1.xPosition + targetRadious;
	float b = character1.xPosition - targetRadious;
	float c = character1.zPosition + targetRadious;
	float d = character1.zPosition - targetRadious;
	

	if ((Path1[*(target)] > b && Path1[*(target)] < a) && (Path1[*(target + 1)] > d && Path1[*(target + 1)] < c) && (*(target + 1) + 2 < size))
	{
		*(target)  += 2;
		*(target+1) += 2;
	}
}

////////////// UPDATE ORIENTATION ////////////// 
float getNewOrientation(float& currentOrientation, float &xVelocity, float &zVelocity)
{
	if (xVelocity != 0 || zVelocity != 0) return atan2(xVelocity, zVelocity);
	else return currentOrientation;
}
////////////////////////////////////////////////


void findClosestPointOnPath(Kinematic& character, float* Path, int* target,  int size)
{
	float closestPointX = *(Path);
	float closestPointZ = *(Path + 1);
	int closestTarget = 0;
	float minDistance = sqrt(pow(character.xPosition - (*(Path)), 2) + pow(character.zPosition - (*(Path + 1)), 2));
	

	// FIND THE CLOSEST POINT ON THE PATH
	for (int T = 0; T < size; T += 2)
	{
		float slope = (   *(Path +(T + 1))   -   *(Path + (T + 2) + 1)    )   /   (   *(Path + T)    -    *(Path + (T + 2))   );
		float b = *(Path + T + 1) - (slope *  *(Path + T) );
		float xpos, zpos;

		for (int g = *(Path + T); g <= *(Path + (T + 2)); g++)
		{
			xpos = g;
			zpos = slope * (xpos)+b;
			float currentDisrance = sqrt(pow(character.xPosition - xpos, 2) + pow(character.zPosition - zpos, 2));
			if (currentDisrance < minDistance)
			{
				minDistance = currentDisrance;
				closestPointX = xpos;
				closestPointZ = zpos;
				closestTarget = T;
			}
		}
	}
	*(target) = closestTarget;
	*(target + 1) = closestTarget + 1;
	Path[closestTarget] = closestPointX;
	Path[closestTarget + 1] = closestPointZ;
}

int main()
{
	std::ofstream myfile;
	myfile.open("data.txt");

	float time = 0.0f;
	int target1[2] = {0,1};
	int target2[2] = { 0,1 };

	float Path1[] = {-80,40,   -40,70,    40,10,    80,40};
	int size1 = sizeof(Path1) / sizeof(Path1[0]);
	float Path2[] = {70,-25,    25,-20,    45,-80,    -55,-80,    -35,-20,    -80,-25};
	int size2 = sizeof(Path2) / sizeof(Path2[0]);

	Kinematic character1 = { -90.0f, 60.0f, 0.0f, 0.0f, 0.0f, 0.0f };  // Initial conditions [xPosition   zPosition   orientation   xVelocity   zVelocity   rotation]
	SteeringOutput steering1 = { 0.0f,0.0f,0.0 };                       // Initial conditions   [xlinear   zlinear  angle]

	Kinematic character2= { 75.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f };  // Initial conditions [xPosition   zPosition   orientation   xVelocity   zVelocity   rotation]
	SteeringOutput steering2 = { 0.0f,0.0f,0.0 };                       // Initial conditions   [xlinear   zlinear  angle]

	findClosestPointOnPath(character1, &Path1[0], &target1[0], size1);
	findClosestPointOnPath(character2, &Path2[0], &target2[0], size2);

	myfile << std::setw(10) << std::left << "PATH";
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
	LOG_OBJECT(1,character1,steering1);
	LOG_OBJECT(2,character2, steering2);

	for (int x = 0; x < 600; x++)
	{
		time += TimeInterval;
		character1.NEupdate(steering1);
		character1.orientation = getNewOrientation(character1.orientation, steering1.xLinear, steering1.zLinear);
		Seek(steering1, character1, Path1[target1[0]], Path1[target1[1]]);
	    arrive(steering1, character1, &target1[0], Path1, size1);
		LOG_OBJECT(1,character1,steering1);
	
		character2.NEupdate(steering2);
		character2.orientation = getNewOrientation(character2.orientation, steering2.xLinear, steering2.zLinear);
		Seek(steering2, character2, Path2[target2[0]], Path2[target2[1]]);
		arrive(steering2, character2, &target2[0], Path2, size2);
		LOG_OBJECT(2,character2, steering2);
	}
	myfile.close();

	//std::cin.get();
}