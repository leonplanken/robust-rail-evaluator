#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Proto.h"
namespace fs = std::filesystem;

#ifndef DEBUG
#define DEBUG 0
#endif // !DEBUG
#ifndef debug_out
#define debug_out(s) \
if(DEBUG) { std::cout << s << std::endl; }
#endif

#ifndef DELETE_LIST
#define DELETE_LIST(l) l.remove_if([](auto e) { delete e; return true; });
#endif

#ifndef DELETE_VECTOR
#define DELETE_VECTOR(l) l.erase(remove_if(l.begin(), l.end(), [](auto e) {delete e; return true;}), l.end());
#endif

template<typename Base, typename T>
inline bool instanceof(const T* ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

template<typename T>
std::vector<const T*> copy_of(const std::vector<const T*>& objs) {
    std::vector<const T*> res(objs.size());
    for(size_t i=0; i<objs.size(); i++)
        res[i] = new T(*objs[i]);
    return res;
}

template<class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

template <class T>
struct std::hash<std::pair<T, T>> {
    std::size_t operator() (const std::pair<T, T> &pair) const {
        auto x = std::hash<T>{}(pair.first);
        hash_combine(x, pair.second);
        return x;
    }
};

inline void parse_json_to_pb(const fs::path& file_path, google::protobuf::Message* message) {
    std::ifstream fileInput;
    fileInput.open(file_path);
    if (!fileInput.good())
        throw std::runtime_error("The file " + file_path.string() + " does not exist.");
    std::stringstream buffer;
    buffer << fileInput.rdbuf();
    auto status = google::protobuf::util::JsonStringToMessage(buffer.str(), message);
    debug_out("Parse JSON " << file_path.string() << " / Status: " << status.ToString());
}

inline void parse_json_to_pb(const std::string& filename, google::protobuf::Message* message) {
    parse_json_to_pb(fs::path(filename), message);
}

#endif