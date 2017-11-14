#ifndef _RADIXTREE_LIB_TREE_HPP_
#define _RADIXTREE_LIB_TREE_HPP_

#include <memory>

using namespace std;

namespace librt {

//
// Radix Tree template
//
template<typename P, typename D>
class Tree : public std::enable_shared_from_this<Tree<P, D>>
{
public:
  Tree()
    : top_(), count_() { }
  ~Tree() { }

  using Ptr = shared_ptr<Tree>;

  //
  // Tree Node
  //
  class Node : public std::enable_shared_from_this<Node>
  {
  public:
    using Ptr = shared_ptr<Node>;

    // Constructors.
    Node(Tree::Ptr tree, const P& prefix)
      : tree_(tree), parent_(), prefix_(prefix),
        children_(), data_() {
    }
    Node(Tree::Ptr tree, const P& prefix1, const P& prefix2)
      : tree_(tree), parent_(), prefix_(prefix1, prefix2),
        children_(), data_() {
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
    void set_data(D* data) {
      data_ = shared_ptr<D>(data);
    }

    // Return self pointer.
    const Ptr self() {
      return enable_shared_from_this<Node>::shared_from_this();
    }

    // Return true if this node is locked.
    const bool is_locked() {
      return children_[0] || children_[1] || data_;
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
    // Pointer to the tree.
    Tree::Ptr tree_;

    // Pointer to parent node.
    Ptr parent_;

    // Prefix of this node.
    P prefix_;

    // Children nodes.
    Ptr children_[2];

    // Data.
    shared_ptr<D> data_;
  };

  using NodePtr = shared_ptr<Node>;

  // Tree::Node iterator.
  class iterator {
  public:
    typedef iterator self_type;
    //    typedef Node value_type;
    typedef Node& reference;
    typedef NodePtr pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(pointer ptr) : ptr_(ptr) { }
    self_type operator++() {
      self_type i = *this;
      ptr_ = ptr_->next();
      return i;
    }
    self_type operator++(int) {
      ptr_ = ptr_->next();
      return *this;
    }
    reference operator*() { return *ptr_; }
    pointer operator->() { return ptr_; }
    bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
    bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }

  private:
    pointer ptr_;
  };

  iterator begin() {
    return iterator(top());
  }

  iterator end() {
    return iterator(nullptr);
  }

  /// Tree member functions.

  //
  NodePtr top() {
    return top_;
  }

  NodePtr get_node_for(const P& prefix) {
    return make_shared<Node>(enable_shared_from_this<Tree>::shared_from_this(),
                             prefix);
  }

  // XXX/TODO
  iterator insert(const P& prefix, D* data) {
    NodePtr new_node;
    NodePtr curr = top_;
    NodePtr matched = nullptr;

    while (curr
           && curr->prefix().len() <= prefix.len()
           && curr->prefix().match(prefix)) {
      // Found the exact node.
      if (curr->prefix().len() == prefix.len()) {
        curr->set_data(data);
        return iterator(curr); // TBD: need lock?
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
      new_node =
        make_shared<Node>(enable_shared_from_this<Tree>::shared_from_this(),
                          curr->prefix(), prefix);
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
    // lock ??

    new_node->set_data(data);
    return iterator(new_node);
  }

  // Lookup node with given prefix.
  iterator find(const P& prefix) {
    NodePtr node = top_;

    while (node
           && node->prefix().len() <= prefix.len()
           && node->prefix().match(prefix)) {
      if (node->prefix().len() == prefix.len())
        return iterator(node);

      node = node->child(prefix.bit_at(node->prefix().len()));
    }

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
};

} // namespace librt

#endif // _RADIXTREE_LIB_TREE_HPP_
