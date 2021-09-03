#ifndef lane_H
#define lane_H

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <json.hpp>
using namespace std;
using json = nlohmann::json;


class lane{
	private:
		
		string id;
		int index;
		string type;
		string direction;
		double length;
		double speed_limit;
		
		vector<double> left_boundary;
		vector<double> right_boundary;
		vector<double> up_boundary;
		vector<double> down_boundary;

		/*
		unordered_map<string, vector<double>> left_boundary;
		unordered_map<string, vector<double>> right_boundary;
		unordered_map<string, vector<double>> up_boundary;
		unordered_map<string, vector<double>> down_boundary;
		*/

		vector<string> from_id;
		vector<string> to_id;

	public:

		lane(json json_lane);

		string getLaneletID(){return id;}
		int getIndex(){return index;}
		string getType(){return type;}
		string getDirection(){return direction;}
		double getLength(){return length;}
		double getSpeedLimit(){return speed_limit;}
		
};

lane::lane(json json_lane){
	

	id = json_lane["lanelet_id"];
	index = json_lane["index"];
	type = json_lane["type"];
	direction = json_lane["direction"];
	length = json_lane["direction"];
	speed_limit = json_lane["speed_limit"];

	left_boundary.insert(left_boundary.begin(), begin(json_lane["left_boundary"]), end(json_lane["left_boundary"]));
	right_boundary.insert(right_boundary.begin(), begin(json_lane["right_boundary"]), end(json_lane["right_boundary"]));
	up_boundary.insert(up_boundary.begin(), begin(json_lane["up_boundary"]), end(json_lane["up_boundary"]));
	down_boundary.insert(down_boundary.begin(), begin(json_lane["down_boundary"]), end(json_lane["down_boundary"]));

	from_id.insert(from_id.begin(), begin(json_lane["from_id"]), end(json_lane["from_id"]));
	to_id.insert(to_id.begin(), begin(json_lane["to_id"]), end(json_lane["to_id"]));

	cout << "success" << endl;

}

#endif