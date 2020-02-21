#pragma once


#include <string>
#include <vector>
#include <functional> // for std::reference_wrapper


namespace cpp_cgns {


// core {
// from http://cgns.github.io/CGNS_docs_current/filemap/general.html
using C1 = char;
using I4 = std::int32_t;
using I8 = std::int64_t;
using R4 = float;
using R8 = double;

using g_idx_t = I8;
using l_idx_t = I4;


struct node_value {
  std::string data_type;
  std::vector<I8> dims;
  void* data;
};

struct tree {
  std::string name;
  node_value value;
  std::vector<tree> children;
  std::string type;
};
// core }


// utility {

/// basic type to_string {
template<class T> std::string to_string();

template<> inline std::string to_string<C1>() { return "C1"; }
template<> inline std::string to_string<I4>() { return "I4"; }
template<> inline std::string to_string<I8>() { return "I8"; }
template<> inline std::string to_string<R4>() { return "R4"; }
template<> inline std::string to_string<R8>() { return "R8"; }
/// basic type to_string }


/// empty node {
const node_value MT = {"MT",{0},nullptr};
/// empty node }


/// access functions {
inline
const std::string& name(const tree& t) {
  return t.name;
}
inline
node_value& value(tree& t) {
  return t.value;
}
inline
std::vector<tree>& children(tree& t) {
  return t.children;
}
inline
const std::string& type(const tree& t) {
  return t.type;
}
/// access functions }


/// range {
using tree_ref = std::reference_wrapper<tree>;
using tree_range = std::vector<tree_ref>;
/// range }


/// children {
inline 
tree& emplace_child(tree& t, tree&& c) {
  t.children.emplace_back(std::move(c));
  return t.children.back();
}

inline
void emplace_children(tree& t, std::vector<tree>&& cs) {
  for (size_t i=0; i<cs.size(); ++i) {
    emplace_child(t,std::move(cs[i]));
  }
}
/// children }

// utility }


} // cpp_cgns