//General
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

// Custom
#include "geofence.h"
#include "path_planner.h"

// ROS
#include <ros/ros.h>
#include "gcs/planPath.h"
#include "gcs/waypoint.h"
#include "gcs/path.h"

#define TEST 1

#define NAME_AS_STRING(macro) #macro
#define VALUE_AS_STRING(macro) NAME_AS_STRING(macro)

#define PATH_PLANNER_CLASS path_planner
#define CRUISE_HEIGHT 5
#define TAKEOFF 0
#define WAYPOINT 1
#define LAND 2

class PATH_PLANNER_CLASS
{
public:
	PATH_PLANNER_CLASS(ros::NodeHandle n);
	~PATH_PLANNER_CLASS();
	bool isRunning();
	void planPath();
private:
	// Methods


	// Attributes
	ros::NodeHandle nh;
	bool isPlanning = false;
	gcs::waypoint origin;
	gcs::waypoint goal;


	// Services
	ros::ServiceServer planPathService;
	bool callbackPlanPathService(gcs::planPath::Request &req, gcs::planPath::Response &res);

	// Published topics
	ros::Publisher pathPublisher;

};

PATH_PLANNER_CLASS::PATH_PLANNER_CLASS(ros::NodeHandle n)
{
	nh = n;
	planPathService = nh.advertiseService("/path_planner/plan",&PATH_PLANNER_CLASS::callbackPlanPathService, this);
	pathPublisher = nh.advertise<gcs::path>("/path_planner/path",1);
}

PATH_PLANNER_CLASS::~PATH_PLANNER_CLASS()
{}

bool PATH_PLANNER_CLASS::callbackPlanPathService(gcs::planPath::Request &req, gcs::planPath::Response &res)
{
	if(isPlanning)
	{
		res.result = 1;
	}
	else
	{
		origin = req.origin;
		goal = req.goal;
		isPlanning = true;
		res.result = 0;
	}
}

void PATH_PLANNER_CLASS::planPath()
{
	// NOTE: Implement a better path planner and use geofence
	gcs::path plannedPath;
	gcs::waypoint origin_temp = origin;
	origin_temp.alt = CRUISE_HEIGHT;
	origin_temp.type = TAKEOFF;
	plannedPath.path.push_back(origin_temp);
	gcs::waypoint goal_temp = goal;
	goal.alt = CRUISE_HEIGHT;
	goal.type = WAYPOINT;
	plannedPath.path.push_back(goal);
	gcs::waypoint goal_land = goal;
	goal_land.alt = 0;
	goal_land.type = LAND;
	plannedPath.path.push_back(goal_land);
	pathPublisher.publish(plannedPath);
	isPlanning = false;
}

bool PATH_PLANNER_CLASS::isRunning()
{
	return isPlanning;
}




int main(int argc, char** argv)
{
	ros::init(argc, argv, VALUE_AS_STRING(PATH_PLANNER_CLASS));
	ros::NodeHandle n;
    PATH_PLANNER_CLASS planner(n);

// std::ifstream geofence_file ("/home/mathias/Desktop/geofence_test.csv");
// std::cout << geofence_file.good() << geofence_file.eof() << geofence_file.fail() << geofence_file.bad() << std::endl;

// //ifstream file ( "file.csv" ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
//  std::string fieldValue, lat, lon;
// std::getline  (geofence_file,fieldValue,'\n'); // Skip first line
// int pointCount = 1;
// std::vector<point> fence_points;
// while ( geofence_file.good() && std::getline ( geofence_file, fieldValue, ',' ) )
// {
// 	point temp_point;
// 	// Skip first field in if statement (to ensure it does not repeat last line)
// 	std::getline ( geofence_file, fieldValue, ',' );	// Skip second field
// 	std::getline ( geofence_file, lat, ',' ); 			// Read Lat
// 	std::getline ( geofence_file, lon, ',' ); 			// Read lon
// 	std::getline ( geofence_file, fieldValue, '\n' ); // Skip the rest of the line
// 	//std::cout << std::string( field_value, 1, field_value.length()-2 ) << ";"; // display value removing the first and the last character from it
// 	std::cout << pointCount++ << ": "  << lat << " " << lon << std::endl;
// 	temp_point.lat = std::atof(lat.c_str());
// 	temp_point.lon = std::atof(lon.c_str());
// 	fence_points.push_back(temp_point);
// }
//  geofence_file.close();

// std::cout << "Fence points from vector" << std::endl;
// for( int i = 0; i < fence_points.size(); i++)
// {
// 	std::cout << (i+1) << ": " << fence_points[i].lat << " " << fence_points[i].lon << std::endl;
// }

// std::ifstream obstacle_file ("/home/mathias/Desktop/obstacle_test.csv");
// std::string height, pointsInObstacleStr;
// if(obstacle_file.good())
// 	std::cout << "good" << std::endl;
// else
// 	std::cout << "bad" << std::endl;
// std::cout << obstacle_file.good() << obstacle_file.eof() << obstacle_file.fail() << obstacle_file.bad() << std::endl;

// std::vector<obstacle> obstacles;
// while( obstacle_file.good() && std::getline( obstacle_file, height, ',' ) )
// {
// 	std::getline( obstacle_file, pointsInObstacleStr, '\n' );
// 	int pointsInObstacle = std::atoi(pointsInObstacleStr.c_str());
// 	std::cout << "PointsInObstacle: " << pointsInObstacleStr << " " << pointsInObstacle << std::endl;
// 	obstacle temp_obstacle;
// 	temp_obstacle.height = std::atof(height.c_str());
// 	for( int i = 0; i < pointsInObstacle; i++) // For all points in this obstacle
// 	{
// 		point temp_point;
// 		std::getline ( obstacle_file, fieldValue, ',' );	// Skip first field (until delimiter ,)
// 		std::getline ( obstacle_file, fieldValue, ',' );	// Skip second field (until delimiter ,)
// 		std::getline ( obstacle_file, lat, ',' ); 			// Read Lat (until delimiter ,)
// 		std::getline ( obstacle_file, lon, ',' ); 			// Read lon(until delimiter ,)
// 		std::getline ( obstacle_file, fieldValue, '\n' ); // Skip the rest of the line	(until delimiter \n)
// 		temp_point.lat = std::atof(lat.c_str());
// 		temp_point.lon = std::atof(lon.c_str());
// 		temp_obstacle.points.push_back(temp_point);
// 	}
// 	obstacles.push_back(temp_obstacle);
// }

// std::cout << "Obstacles from vector:" << std::endl;
// for( int i = 0; i < obstacles.size(); i++)
// {
// 	std::cout << "Obstacle" << i << ":" << std::endl;
// 	std::cout << "\tHeight: " << obstacles[i].height << std::endl;
// 	for( int j = 0; j < obstacles[i].points.size(); j++)
// 	{
// 		std::cout  << '\t' << obstacles[i].points[j].lat << " " << obstacles[i].points[j].lon << std::endl;
// 	}
// }

// std::cout << "Finished" << std::endl;

if(TEST)
{
    geofence fence_test;
    fence_test.set_max_altitude(200.0);
    //fence_test.set_fence_csv("/home/tobias/drone_ws/src/UASD_GCS/src/fences/google_fence1.csv");
    fence_test.set_fence_csv("/home/tobias/drone_ws/src/UASD_GCS/src/fences/geofence.txt", true);
    Path_planner plan(fence_test);
    plan.set_nodes(fence_test.get_fence());

    
    plan.export_as_csv("/home/tobias/drone_ws/src/UASD_GCS/src/fences/testfile.txt");   // Debugging

    point start, goal, ok_point, p_inside, p_outside, fence_point;
    /*start.lat = 55.558944;
    start.lon = 10.109823;
    goal.lat = 55.559004; //55.559056;
    goal.lon = 10.109658; //10.109683;
    ok_point.lat = 55.558889;
    ok_point.lon = 10.109714;
    p_inside.lat = 55.05;
    p_inside.lon = 10.05;
    p_outside.lat =  55.559113;
    p_outside.lon =  10.110014;
    fence_point.lat = 55.558841;
    fence_point.lon = 10.109575;
    fence_test.geodetic_to_UTM(start);
    fence_test.geodetic_to_UTM(goal);
    fence_test.geodetic_to_UTM(ok_point);
    fence_test.geodetic_to_UTM(fence_point);*/
    //std::cout << "HER " <<  start.lat << " " << start.lon << std::endl;
    //fence_test.UTM_to_geodetic(start);
    //std::cout << start.lat << " " << start.lon << std::endl;
    /*
    std::cout << "DEBUGGING edge legal: " << fence_test.edge_legal(start, ok_point) << std::endl;
    std::cout << "Point legal: " << fence_test.point_legal(ok_point, UTM);*/

    // Start and goal on the real geofence
    start.lat = 55.560531;
    start.lon = 10.113121;
    goal.lat =  55.565000;
    goal.lon =  10.118309;
    fence_test.geodetic_to_UTM(start);
    fence_test.geodetic_to_UTM(goal);


    gcs::path thepath = plan.plan_path(start,goal);
    std::cout << "\nPath planning has finished!" << std::endl;

    /*
    for(unsigned int i = 0; thepath.path.size(); i++)
    {
        std::cout << thepath.path[i].lat << std::endl;
    }*/
}


//fence_test.set_obstacles("/home/mathias/Desktop/obstacle_test.csv");


// // GEOFENCE TESTING!
// 	geofence fence;
// 	//fence.set("home/mathias/Downloads/Geofence.kml","...");
// 	point p1, p2, p3, p4, ptest;
// 	p1.lon = 1.0;
// 	p1.lat = 1.0;
// 	p2.lon = 2.0;
// 	p2.lat = 1.0;
// 	p3.lon = 2.0;
// 	p3.lat = 2.0;
// 	p4.lon = 1.0;
// 	p4.lat = 2.0;
// 	std::vector<point> points;
// 	points.push_back(p1);
// 	points.push_back(p2);
// 	points.push_back(p3);
// 	points.push_back(p4);
// 	ptest.lon = 1.5;
// 	ptest.lat = 1.5;
// 	ptest.alt = 10;

// 	//bool result = fence.inside_polygon(points,ptest);
// 	obstacle o1;
// 	std::vector<point> opoints;
// 	point op;
// 	op.lon = 1.2;
// 	op.lat = 1.4;
// 	o1.points.push_back(op);
// 	op.lon = 1.2;
// 	op.lat = 1.6;
// 	o1.points.push_back(op);
// 	op.lon = 1.8;
// 	op.lat = 1.4;
// 	o1.points.push_back(op);
// 	op.lon = 1.8;
// 	op.lat = 1.6;
// 	o1.points.push_back(op);
// 	o1.height = 10;
// 	std::vector<obstacle> obstacles;
// 	obstacles.push_back(o1);
// 	fence.set_fence(points);
// 	fence.set_obstacles(obstacles);
// 	fence.set_max_altitude(30);


// 	point ptest2, ptest3;
// 	ptest2.lon = 1.5;
// 	ptest2.lat = 1.2;
// 	ptest2.alt = 11;
// 	ptest3.lon = 1.5;
// 	ptest3.lat = 1.8;
// 	ptest3.alt = 11;
// 	bool result = fence.edge_legal(ptest2,ptest3);

// 	//bool result = fence.point_legal(ptest);
// 	//ROS_INFO("%i",points.size());

// 	ROS_INFO("main result: %i",result);

// 	point geodetic_p;
// 	geodetic_p.lat = 55.0000000000;
// 	geodetic_p.lon = 009.0000000000;
// 	//geodetic_p.lat = 55.403756;
// 	//geodetic_p.lon = 10.40237;
// 	//ROS_INFO("%f,%f",geodetic_p.lat,geodetic_p.lon);
// 	//fence.geodetic_to_UTM(geodetic_p);
// 	//ROS_INFO("%f,%f",geodetic_p.lat,geodetic_p.lon);
// 	//fence.test_UTM();
// 	fence.self_test();
	ros::Rate rate(20);
	while(ros::ok())
	{
		if(planner.isRunning())
			planner.planPath();
		ros::spinOnce();
		rate.sleep();
	}
  return 0;
}
