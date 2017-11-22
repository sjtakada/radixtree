#ifndef _RADIXTREE_LIB_TREE_HPP_
#define _RADIXTREE_LIB_TREE_HPP_

#include <memory>

using namespace std;

namespace librt {

//
// Radix Tree template.
//
template<typename P, typename D>
class Tree
{
public:
  // Constructor.
  Tree()
    : top_(), count_() {
  }

  // Destructor.
  ~Tree() {
  }

  using Ptr = shared_ptr<Tree>;

  //
  // Tree Node
  //
  class Node : public std::enable_shared_from_this<Node>
  {
  public:
    using Ptr = shared_ptr<Node>;

    // Constructors.
    Node(const P& prefix)
      : parent_(), prefix_(prefix),
        children_(), data_(), has_data_() {
    }
    Node(const P& prefix1, const P& prefix2)
      : parent_(), prefix_(prefix1, prefix2),
        children_(), data_(), has_data_() {
    }

    // Destructor.
    ~Node() {
    }

    // Return prefix for the node.
    const P& prefix() {
      return prefix_;
    }

    // Return child at.
    const Ptr child(u_char bit) {
      return children_[bit];
    }

    // Return parent.
    const Ptr parent() {
      return parent_;
    }

    // Set given node as a child at left or right.
    void set_child(Ptr child) {
      u_char bit = (u_char)child->prefix().bit_at(prefix_.len());
      set_child_at(child, bit);
      child->parent_ = enable_shared_from_this<Node>::shared_from_this();
    }

    // Set give node as a child at.
    void set_child_at(Ptr child, u_char bit) {
      children_[bit] = child;
    }

    // Set parent.
    void set_parent(Ptr parent) {
      parent_ = parent;
    }

    // Set data.
    void set_data(D& data) {
      data_ = data;
      has_data_ = true;
    }

    // Unset data.
    void unset_data() {
      has_data_ = false;
    }

    // Return self pointer.
    const Ptr self() {
      return enable_shared_from_this<Node>::shared_from_this();
    }

    // Return data reference.
    const D& data() {
      return data_;
    }

    // Return true if node has data.
    bool has_data() {
      return has_data_;
    }

    // Return true if this node is locked.
    const bool is_locked() {
      return children_[0] || children_[1] || has_data_;
    }

    // Return next Node.
    const Ptr next() {
      shared_ptr<Node> curr =
        enable_shared_from_this<Node>::shared_from_this();

      if (curr->children_[0]) {
        return curr->children_[0];
      }
      if (curr->children_[1]) {
        return curr->children_[1];
      }

      while (curr->parent_) {
        if (curr->parent_->children_[0] == curr
            && curr->parent_->children_[1]) {
          return curr->parent_->children_[1];
        }
        curr = curr->parent_;
      }

      return nullptr;
    }

  private:
    // Pointer to parent node.
    Ptr parent_;

    // Prefix of this node.
    P prefix_;

    // Children nodes.
    Ptr children_[2];

    // Data.
    D data_;

    // True if node has data.
    bool has_data_;
  };

  using NodePtr = shared_ptr<Node>;

  // Tree::Node iterator.
  class iterator {
  public:
    typedef iterator self_type;
    typedef Node value_type;
    typedef std::pair<P, D> reference;
    typedef NodePtr pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(pointer ptr) : ptr_(ptr) { }
    self_type operator++() {
      self_type i = *this;
      do {
        ptr_ = ptr_->next();
        if (ptr_ && ptr_->has_data()) {
          break;
        }
      } while (ptr_);
      return i;
    }
    self_type operator++(int) {
      do {
        ptr_ = ptr_->next();
        if (ptr_ && ptr_->has_data()) {
          break;
        }
      } while (ptr_);
      return *this;
    }
    //    reference operator*() {
    //      return std::make_pair<P, D>(ptr_->prefix(), ptr_->data());
    //    }

    self_type next() {
      ptr_ = ptr_->next();
      return *this;
    }

    const P& prefix() { return ptr_->prefix(); }
    const D& data() { return ptr_->data(); }

    pointer operator->() { return ptr_; }
    bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
    bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }

  private:
    pointer ptr_;
  };

  // Iterator begin.
  iterator begin() {
    NodePtr node = top_;
    while (!node->has_data()) {
      node = node->next();
    }

    return iterator(node);
  }

  // Iterator end.
  iterator end() {
    return iterator(nullptr);
  }

  // Iterator for top node.
  iterator top() {
    return iterator(top_);
  }

  // Insert data to the node with given prefix.
  iterator insert(const P& prefix, D data) {
    iterator it = get_node(prefix);

    it->set_data(data);

    return it;
  }

  // Delete date from the node with given prefix.
  //  void delete(const P& prefix, D data) {
  //    auto it = find(prefix);
  //    if (it != end()) {
  //      
  //    }
  //  }

  // Add a node for given prefix, and place it in the tree.
  iterator get_node(const P& prefix) {
    NodePtr new_node;
    NodePtr curr = top_;
    NodePtr matched = nullptr;

    while (curr
           && curr->prefix().len() <= prefix.len()
           && curr->prefix().match(prefix)) {
      // Found the exact node.
      if (curr->prefix().len() == prefix.len()) {
        return iterator(curr);
      }

      matched = curr;
      curr = curr->child(prefix.bit_at(curr->prefix().len()));
    }

    if (curr == NULL) {
      new_node = get_node_for(prefix);
      if (matched) {
        matched->set_child(new_node);
      }
      else {
        top_ = new_node;
      }
    }
    else {
      new_node = make_shared<Node>(curr->prefix(), prefix);
      new_node->set_child(curr);

      if (matched) {
        matched->set_child(new_node);
      }
      else {
        top_ = new_node;
      }

      if (new_node->prefix().len() != prefix.len()) {
        matched = new_node;
        new_node = get_node_for(prefix);
        matched->set_child(new_node);
      }
    }

    return iterator(new_node);
  }

  // Perform exact match lookup.
  iterator find(const P& prefix) {
    NodePtr node = top_;

    while (node
           && node->prefix().len() <= prefix.len()
           && node->prefix().match(prefix)) {
      if (node->prefix().len() == prefix.len()) {
        if (node->has_data()) {
          return iterator(node);
        }
        else {
          break;
        }
      }

      node = node->child(prefix.bit_at(node->prefix().len()));
    }

    return iterator(nullptr);
  }

  // Perform longest match lookup.
  iterator match(const P& prefix) {
    NodePtr node = top_;
    NodePtr matched = nullptr;

    while (node
           && node->prefix().len() <= prefix.len()
           && node->prefix().match(prefix)) {
      if (node->has_data()) {
        matched = node;
      }

      if (node->prefix().len() == prefix.len()) {
        break;
      }

      node = node->child(prefix.bit_at(node->prefix().len()));
    }

    if (matched)
      return iterator(matched);

    return iterator(nullptr);
  }

  // Erase the node from tree, and return iterator for next node.
  iterator erase(iterator it) {
    NodePtr child = nullptr;
    NodePtr parent;
    NodePtr next = it->next();

    if (it->child(0) && it->child(1)) {
      return iterator(nullptr);
    }

    if (it->child(0)) {
      child = it->child(0);
    }
    else {
      child = it->child(1);
    }

    parent = it->parent();

    if (child) {
      child->set_parent(parent);
    }

    if (parent) {
      if (parent->child(0) == it->self()) {
        parent->set_child_at(child, 0);
      }
      else {
        parent->set_child_at(child, 1);
      }
    }
    else {
      top_ = child;
    }

    if (parent && !parent->is_locked()) {
      erase(parent);
    }

    return iterator(next);
  }

  iterator erase_at(const P& prefix) {
    auto it = find(prefix);
    if (it != end()) {
      return erase(it);
    }

    return iterator(nullptr);
  }

private:
  // top node of this tree.
  NodePtr top_;

  // number of nodes in this tree.
  uint32_t count_;

  // Create a node for given prefix.
  NodePtr get_node_for(const P& prefix) {
    return make_shared<Node>(prefix);
  }
};

} // namespace librt

#endif // _RADIXTREE_LIB_TREE_HPP_
