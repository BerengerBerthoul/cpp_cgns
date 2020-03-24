#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/exception.hpp"
#include "std_e/utils/string.hpp"


namespace cpp_cgns {


// tree search {
/// by predicate {
template<class Unary_predicate>
bool has_child_by_predicate(const tree& t, Unary_predicate p) {
  auto pos = std::find_if(begin(t.children),end(t.children),p);
  if (pos==end(t.children)) return false;
  return true;
}

template<class Unary_predicate>
tree_range get_children_by_predicate(tree& t, Unary_predicate p) {
  tree_range sub_ts;
  std::copy_if(begin(t.children),end(t.children),std::back_inserter(sub_ts),p);
  return sub_ts;
}
template<class Unary_predicate>
tree& get_child_by_predicate(tree& t, Unary_predicate p, const cgns_exception& e) {
  auto pos = std::find_if(begin(t.children),end(t.children),p);
  if (pos==end(t.children)) {
    throw e;
  } else {
    return *pos;
  }
}

//// const versions {
template<class Unary_predicate>
const_tree_range get_children_by_predicate(const tree& t, Unary_predicate p) {
  const_tree_range sub_ts;
  std::copy_if(begin(t.children),end(t.children),std::back_inserter(sub_ts),p);
  return sub_ts;
}
template<class Unary_predicate>
const tree& get_child_by_predicate(const tree& t, Unary_predicate p, const cgns_exception& e) {
  auto pos = std::find_if(begin(t.children),end(t.children),p);
  if (pos==end(t.children)) {
    throw e;
  } else {
    return *pos;
  }
}
//// const versions }
/// by predicate }


/// common predicates {
bool is_of_name(const tree& tree, const std::string& name) {
  return tree.name == name;
}
bool is_of_type(const tree& tree, const std::string& type) {
  return tree.type == type;
}
bool is_one_of_types(const tree& tree, const std::vector<std::string>& types) {
  return std::any_of(begin(types),end(types),[&tree](const std::string& type){ return is_of_type(tree,type); });
}

bool has_child_of_name(const tree& t, const std::string& name) {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  return has_child_by_predicate(t,predicate);
}
bool has_child_of_type(const tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  return has_child_by_predicate(t,predicate);
}
/// common predicates }


/// common searches {
tree_range get_children_by_type(tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  return get_children_by_predicate(t,predicate);
}
tree_range get_children_by_types(tree& t, const std::vector<std::string>& types) {
  auto predicate = [&](const tree& child){ return is_one_of_types(child,types); };
  return get_children_by_predicate(t,predicate);
}
tree_range get_children_by_name_or_type(tree& t, const std::string& s) {
  auto predicate = [&](const tree& child){ return is_of_name(child,s) || is_of_type(child,s); };
  return get_children_by_predicate(t,predicate);
}

tree& get_child_by_name(tree& t, const std::string& name) {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  cgns_exception e("Child of name \""+name+"\" not found in tree \""+t.name+"\"");
  return get_child_by_predicate(t,predicate,e);
}
tree& get_child_by_type(tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  cgns_exception e("Child of type \""+type+"\" not found in tree \""+t.name+"\"");
  return get_child_by_predicate(t,predicate,e);
}

//// const versions {
const_tree_range get_children_by_type(const tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  return get_children_by_predicate(t,predicate);
}
const_tree_range get_children_by_types(const tree& t, const std::vector<std::string>& types) {
  auto predicate = [&](const tree& child){ return is_one_of_types(child,types); };
  return get_children_by_predicate(t,predicate);
}
const_tree_range get_children_by_name_or_type(const tree& t, const std::string& s) {
  auto predicate = [&](const tree& child){ return is_of_name(child,s) || is_of_type(child,s); };
  return get_children_by_predicate(t,predicate);
}

const tree& get_child_by_name(const tree& t, const std::string& name) {
  auto predicate = [&](const tree& child){ return is_of_name(child,name); };
  cgns_exception e("Child of name \""+name+"\" not found in tree \""+t.name+"\"");
  return get_child_by_predicate(t,predicate,e);
}
const tree& get_child_by_type(const tree& t, const std::string& type) {
  auto predicate = [&](const tree& child){ return is_of_type(child,type); };
  cgns_exception e("Child of type \""+type+"\" not found in tree \""+t.name+"\"");
  return get_child_by_predicate(t,predicate,e);
}
//// const versions }


// TODO get_nodes_by_matching: this implementation is ugly and slow (memory allocations and copies all the way) and should be replaced by a proper DFS
template<class Array> constexpr auto
append(Array& x, const Array& y)  {
  for (const auto& e : y) {
    x.push_back(e);
  }
}
tree_range get_nodes_by_matching(tree& t, std::vector<std::string> identifiers_stack) {
  STD_E_ASSERT(identifiers_stack.size()>0);

  auto current_id = identifiers_stack.back();
  tree_range nodes_matching_current_id = get_children_by_name_or_type(t,current_id);

  if (identifiers_stack.size()==1) {
    return nodes_matching_current_id;
  } else {
    identifiers_stack.pop_back();

    tree_range matching_nodes;
    for (auto& node : nodes_matching_current_id) {
      append(matching_nodes,get_nodes_by_matching(node,identifiers_stack));
    }
    return matching_nodes;
  }
}

tree_range get_nodes_by_matching(tree& t, const std::string& gen_path) {
  auto identifiers = std_e::split(gen_path,'/');
  std::reverse(begin(identifiers),end(identifiers));
  return get_nodes_by_matching(t,identifiers);
}

tree& get_node_by_matching(tree& t, const std::string& gen_path) {
  tree_range ts = get_nodes_by_matching(t,gen_path);
  if (ts.size() == 0) {
    throw cgns_exception("No sub-tree matching \""+gen_path+"\" in tree \""+t.name+"\"");
  } else {
    return ts[0];
  }
}
/// common searches }
// tree search }


} // cpp_cgns