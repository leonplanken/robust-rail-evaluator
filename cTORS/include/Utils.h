#pragma once
#include<iostream>

#ifndef DEBUG
#define DEBUG 0
#endif // !DEBUG
#ifndef debug_out
#define debug_out(s) \
if(DEBUG) { cout << s << endl; }
#endif

#ifndef DELETE_LIST
#define DELETE_LIST(l) l.remove_if([](auto e) { delete e; return true; });
#endif

#ifndef DELETE_VECTOR
#define DELETE_VECTOR(l) l.erase(remove_if(l.begin(), l.end(), [](auto e) {delete e; return true;}), l.end());
#endif