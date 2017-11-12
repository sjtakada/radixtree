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
      : tree_(tree), parent_(nullptr), prefix_(prefix) {
    }
    Node(Tree::Ptr tree, const P& prefix1, const P& prefix2)
      : tree_(tree), parent_(nullptr), prefix_(prefix1, prefix2) {
    }

    // 
    const P& prefix() {
      return prefix_;
    }

    // 
    const Ptr child(u_char bit) {
      return children_[bit];
    }

    //
    void setChild(u_char bit, Ptr node) {
      children_[bit] = node;
      node->parent_ = enable_shared_from_this<Node>::shared_from_this();
    }

    //
    const Ptr nextNode() {
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

  // Node iterator
  class iterator {
  public:
    typedef iterator self_type;
    typedef Node value_type;
    typedef Node& reference;
    typedef NodePtr pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef int difference_type;
    iterator(pointer ptr) : ptr_(ptr) { }
    self_type operator++() { self_type i = *this;
      ptr_ = ptr_->nextNode();
      return i;
    }
    self_type operator++(int) {
      ptr_ = ptr_->nextNode();
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

  // Tree member functions.
  NodePtr top() {
    return top_;
  }

  NodePtr setNode(const P& prefix) {
    return make_shared<Node>(enable_shared_from_this<Tree>::shared_from_this(),
                             prefix);
  }

  void setChildNode(NodePtr parent, NodePtr child) {
    u_char bit = (u_char)child->prefix().bit_at(parent->prefix().len());
    parent->setChild(bit, child);
  }

  // Return a node if it exists, otherwise create one.
  NodePtr getNode(const P& prefix) {
    NodePtr new_node;
    NodePtr curr = top_;
    NodePtr matched = nullptr;

    while (curr
           && curr->prefix().len() <= prefix.len()
           && curr->prefix().match(prefix)) {
      // Found the exact node.
      if (curr->prefix().len() == prefix.len()) {
        return curr; // TBD: need lock?
      }

      matched = curr;
      curr = curr->child(prefix.bit_at(curr->prefix().len()));
    }

    if (curr == NULL) {
      new_node = setNode(prefix);
      if (matched) {
        setChildNode(matched, new_node);
      }
      else {
        top_ = new_node;
      }
    }
    else {
      new_node = make_shared<Node>(enable_shared_from_this<Tree>::shared_from_this(),
                                   curr->prefix(), prefix);
      setChildNode(new_node, curr);

      if (matched) {
        setChildNode(matched, new_node);
      }
      else {
        top_ = new_node;
      }

      if (new_node->prefix().len() != prefix.len()) {
        matched = new_node;
        new_node = setNode(prefix);
        setChildNode(matched, new_node);
      }
    }
    // lock ??

    return new_node;
  }

private:
  // top node of this tree.
  NodePtr top_;

  // number of nodes in this tree.
  uint32_t count_;
};

} // namespace librt

#endif // _RADIXTREE_LIB_TREE_HPP_
