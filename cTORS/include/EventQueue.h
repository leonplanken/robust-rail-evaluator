#pragma once
#include <queue>
#include "Event.h"
using namespace std;

struct EventCompare
{
    bool operator()(const Event* left, const Event* right) { return *left > *right; }
};


class EventQueue : public priority_queue < Event*, vector<Event*>, EventCompare> {
private:

//TODO constructor, destructor, copy operator
	

};

