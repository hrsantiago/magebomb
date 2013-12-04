#ifndef __POSITION_H
#define __POSITION_H

#include <stdlib.h>
#include <cmath>
#include <iostream>

#include "ConstTibia.h"

class Position{
public:
  	Position() : x(31), y(31), z(7) { };
	~Position() {};

	template<int deltax, int deltay, int deltaz>
	inline static bool areInRange(const Position& p1, const Position& p2){
		if(std::abs(p1.x - p2.x) > deltax ||
			std::abs(p1.y - p2.y) > deltay ||
			std::abs(p1.z - p2.z) > deltaz){
			return false;
		}
		return true;
	}

	template<int deltax, int deltay>
	inline static bool areInRange(const Position& p1, const Position& p2){
		if(std::abs(p1.x - p2.x) > deltax ||
			std::abs(p1.y - p2.y) > deltay){
			return false;
		}
		return true;
	}

	Position(int _x, int _y, int _z)
	: x(_x), y(_y), z(_z) {};

	int x,y,z;

	bool operator<(const Position& p) const {
		if(z < p.z)
			return true;
		if(z > p.z)
			return false;

		if(y < p.y)
			return true;
		if(y > p.y)
			return false;

		if(x < p.x)
			return true;
		if(x > p.x)
			return false;

		return false;
	}

	bool operator>(const Position& p) const {
		return ! (*this < p);
	}

	bool operator==(const Position p)  const {
		if(p.x==x && p.y==y && p.z ==z)
			return true;
		else
			return false;
	}

	bool operator!=(const Position p)  const {
		if(p.x==x && p.y==y && p.z ==z)
			return false;
		else
			return true;
	}

	Position operator-(const Position p1){
		return Position(x-p1.x, y-p1.y,z-p1.z);
	}
};

std::ostream& operator<<(std::ostream&, const Position&);
std::ostream& operator<<(std::ostream&, const Direction&);

class PositionEx : public Position{
public:
	PositionEx(){ };
	~PositionEx(){};

	PositionEx(int _x, int _y, int _z, int _stackpos)
	: Position(_x,_y,_z), stackpos(_stackpos) {};

	PositionEx(int _x, int _y, int _z)
	: Position(_x,_y,_z), stackpos(0) {};

	PositionEx(Position p)
	: Position(p.x,p.y,p.z), stackpos(0) {};

	PositionEx(Position p,int _stackpos)
	: Position(p.x,p.y,p.z), stackpos(_stackpos) {};

	int stackpos;

	bool operator==(const PositionEx p)  const {
		if(p.x==x && p.y==y && p.z ==z && p.stackpos == stackpos)
			return true;
		else
			return false;
	}

	bool operator!=(const PositionEx p)  const {
		if(p.x==x && p.y==y && p.z ==z && p.stackpos != stackpos)
			return false;
		else
			return true;
	}

};

#endif
