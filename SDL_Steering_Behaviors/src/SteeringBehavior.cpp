#include "SteeringBehavior.h"
#include "SDL_SimpleApp.h"

#include <vector>




SteeringBehavior::SteeringBehavior()
{
}


SteeringBehavior::~SteeringBehavior()
{
}

//Dummy Seek
Vector2D SteeringBehavior::KinematicSeek(Agent *agent, Vector2D target, float dtime)
{
	Vector2D steering = target - agent->position;
	steering.Normalize();
	return steering * agent->max_velocity;
}

Vector2D SteeringBehavior::KinematicSeek(Agent *agent, Agent *target, float dtime)
{
	return KinematicSeek(agent, target->position, dtime);
}

// Dummy Flee
Vector2D SteeringBehavior::KinematicFlee(Agent *agent, Vector2D target, float dtime)
{
	Vector2D steering = agent->position - target;
	steering.Normalize();
	return steering * agent->max_velocity;
}

Vector2D SteeringBehavior::KinematicFlee(Agent *agent, Agent *target, float dtime)
{
	return KinematicFlee(agent, target->position, dtime);
}

/* Add here your own Steering Behavior functions definitions */

//Seek
Vector2D SteeringBehavior::Seek(Agent *agent, Vector2D target, float dtime)
{
	Vector2D DesiredVelocity = target - agent->position;
	DesiredVelocity.Normalize();
	DesiredVelocity *= agent->max_velocity;
	Vector2D SteeringForce = (DesiredVelocity - agent->velocity);
	SteeringForce /= agent->max_velocity;
	return SteeringForce * agent->max_force;
}

Vector2D SteeringBehavior::Seek(Agent *agent, Agent *target, float dtime)
{
	return Seek(agent, target->position, dtime);
}

//Flee
Vector2D SteeringBehavior::Flee(Agent *agent, Vector2D target, float dtime)
{
	Vector2D DesiredVelocity = agent->position - target;
	DesiredVelocity.Normalize();
	DesiredVelocity *= agent->max_velocity;
	Vector2D SteeringForce = (DesiredVelocity - agent->velocity);
	SteeringForce /= agent->max_velocity;
	return SteeringForce * agent->max_force;
}

Vector2D SteeringBehavior::Flee(Agent *agent, Agent *target, float dtime)
{
	return Flee(agent, target->position, dtime);
}

//Arrive
Vector2D SteeringBehavior::Arrive(Agent *agent, Vector2D target, float dtime)
{
	// Radius to start slowing down
#ifndef SlowingRadius
	const int SlowingRadius = 200;
#endif // !SlowingRadius

	//Radius circle
	draw_circle(TheApp::Instance()->getRenderer(), (int)target.x, (int)target.y, SlowingRadius, 255, 255, 255, 255);

	float distanceToTarget = Vector2D::Distance(target, agent->position);
	Vector2D ArriveSpeed = agent->max_velocity;
	float speedFactor = 1.0f;

	//Seek
	Vector2D DesiredVelocity = target - agent->position;
	DesiredVelocity.Normalize();

	if (distanceToTarget > SlowingRadius)
	{
		DesiredVelocity *= agent->max_velocity;
	}
	else {
		DesiredVelocity *= agent->max_velocity * (distanceToTarget / SlowingRadius);
	}
	
	Vector2D SteeringForce = (DesiredVelocity - agent->velocity);
	SteeringForce /= agent->max_velocity;
	
	return SteeringForce * agent->max_force;
}

Vector2D SteeringBehavior::Arrive(Agent *agent, Agent *target, float dtime)
{
	return Arrive(agent, target->position, dtime);
}

//Pursue
Vector2D SteeringBehavior::Pursue(Agent *agent, Vector2D target, float dtime)
{

	return Vector2D(0, 0);
}

Vector2D SteeringBehavior::Pursue(Agent *agent, Agent *target, float dtime)
{
	Vector2D DistanceToTarget = target->position - agent->position;
	if (agent->velocity.Length() == 0)
	{
		return Seek(agent, target->position, dtime);
	}
	float t = hypot(DistanceToTarget.x, DistanceToTarget.y) / agent->velocity.Length();
	Vector2D TargetPosition = target->position + (target->getVelocity() * t);
	return Seek(agent, TargetPosition, dtime);
}

//Evade
Vector2D SteeringBehavior::Evade(Agent *agent, Vector2D target, float dtime)
{
	return Vector2D(0, 0);
}

Vector2D SteeringBehavior::Evade(Agent *agent, Agent *target, float dtime)
{
	Vector2D DistanceToTarget = target->position - agent->position;
	if (agent->velocity.Length() == 0)
	{
		return Seek(agent, target->position, dtime);
	}
	float t = hypot(DistanceToTarget.x, DistanceToTarget.y) / agent->velocity.Length();
	Vector2D TargetPosition = target->position + (target->getVelocity() * t);
	return Flee(agent, TargetPosition, dtime);
}

//Wander
Vector2D SteeringBehavior::Wander(Agent *agent, Vector2D target, float dtime)
{
	//Maximum Change of Angle in one frame
	const float WanderMaxAngleChange = 30;

	//Distance from Agent to Wander Circle Center
	const float WanderOffset = 200;

	//Radius of Wander Circle
	const float WanderRadius = 100;

	float RandomBinomial = 0.0f;
	float TargetAngle = agent->wanderAngle;

	//Update Wander Angle
	if (agent->wanderCounter > 900.0f) {
		RandomBinomial = ((float)rand() / (RAND_MAX)) - ((float)rand() / (RAND_MAX));
		agent->wanderAngle += RandomBinomial * WanderMaxAngleChange;
		TargetAngle = agent->orientation + agent->wanderAngle;

		agent->wanderCounter = 0.0f;
	}
	agent->wanderCounter += 1.0f;
	//crear un contador aqui para frenar la ejecución medio segundo, por ejemplo
	//el contador se debe alojar en agent o en algun otro lado, porque sino la ejecución se encalla aqui

	//Calculate new wander Position
	Vector2D CircleCenter = agent->position + Vector2D::Normalize(agent->velocity) * WanderOffset;

	Vector2D TargetPosition = (0.0f, 0.0f);

	TargetPosition.x = CircleCenter.x + WanderRadius * cos(TargetAngle);
	TargetPosition.y = CircleCenter.y + WanderRadius * sin(TargetAngle);

	//CircleCenter
	draw_circle(TheApp::Instance()->getRenderer(), (int)CircleCenter.x, (int)CircleCenter.y, WanderRadius, 255, 255, 255, 255);
	//TargetPosition
	draw_circle(TheApp::Instance()->getRenderer(), (int)TargetPosition.x, (int)TargetPosition.y, 15, 0, 255, 0, 0);

	return TargetPosition;
	//return Vector2D(0, 0);
}

Vector2D SteeringBehavior::Wander(Agent *agent, Agent *target, float dtime)
{
	return Wander(agent, target->position, dtime);
}

//Flocking
Vector2D SteeringBehavior::Flocking(Agent *agent, Vector2D target, float dtime, std::vector<Agent*> agents)
{

#define NEIGHBORHOOD_RADIUS 100.0f
#define K_SEPARATION_FORCE 60.0f
#define K_COHESION_FORCE 40.0f
#define K_ALIGNMENT_FORCE 20.0f
#define K_MAX_FLOCKING_FORCE 100.0f


	int neighborCount = 0;
	Vector2D separationVector = {};
	Vector2D averagePosition = {};
	Vector2D averageVelocity = {};

	//Neighborhood radius
	draw_circle(TheApp::Instance()->getRenderer(), (int)agent->position.x, (int)agent->position.y, NEIGHBORHOOD_RADIUS, 0, 255, 0, 255);

	for (int i = 0; i < agents.size(); i++) {
		//calculo la distancia antes para no repetir el calculo en el if
		int distance = Vector2D::Distance(agents[i]->position, agent->position);
		if (distance != 0 && distance < NEIGHBORHOOD_RADIUS) {
			
			//Separation
			separationVector += (agent->position - agents[i]->position);

			//Cohesion
			averagePosition += agent->position;

			//Alignment
			averageVelocity += agent->velocity;

			++neighborCount;
		}
	}

	//Separation
	separationVector /= neighborCount;
	Vector2D separationDirection = Vector2D::Normalize(separationVector);

	//Cohesion
	averagePosition /= neighborCount;
	averagePosition -= agent->position;
	Vector2D cohesionDirection = Vector2D::Normalize(averagePosition);

	//Alignment
	averageVelocity /= neighborCount;
	Vector2D alignmentDirection = Vector2D::Normalize(averageVelocity);

	//flocking
	Vector2D flockingForce = separationDirection * K_SEPARATION_FORCE
						   + cohesionDirection * K_COHESION_FORCE
						   + alignmentDirection * K_ALIGNMENT_FORCE;

	flockingForce = Vector2D::Normalize(flockingForce);

	flockingForce *= K_MAX_FLOCKING_FORCE;
	//flockingForce *= agent->max_force;
	
	return flockingForce + Arrive(agent, target, dtime);

	//return flockingForce;
}


Vector2D SteeringBehavior::Flocking(Agent *agent, Agent *target, float dtime)
{
	return (agent, target->position, dtime);
}

//Perimeter Avoidance
Vector2D SteeringBehavior::PerimeterAvoidance(Agent *agent, Vector2D target, float dtime) {
	
#define PERIMETER_BORDER 50
	
	Vector2D desiredVelocity;
	Vector2D steeringForce;

	//pared izquierda
	if (agent->position.x < PERIMETER_BORDER)
		desiredVelocity.x = agent->max_velocity;

	//pared derecha
	else if (agent->position.x > WIN_WIDTH - PERIMETER_BORDER)
		desiredVelocity.x = -agent->max_velocity;
	
	//pared superior
	if (agent->position.y < PERIMETER_BORDER)
		desiredVelocity.y = agent->max_velocity;

	//pared inferior
	else if (agent->position.y > WIN_HEIGHT - PERIMETER_BORDER)
		desiredVelocity.y = -agent->max_velocity;
	
	if (desiredVelocity.Length() > 0.0f) {

		steeringForce = desiredVelocity - agent->velocity;
		steeringForce /= agent->max_velocity;
		steeringForce *= agent->max_force;

	}
	
	return steeringForce + Arrive(agent, target, dtime);

	//return Vector2D(0, 0);
}