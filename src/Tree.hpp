#ifndef _RADIXTREE_LIB_TREE_HPP_
#define _RADIXTREE_LIB_TREE_HPP_

#include <memory>

using namespace std;

namespace librt {

template<typename P, typename D>
class Tree;

template<typename P, typename D>
class Node : public std::enable_shared_from_this<Node<P, D> >
{
public:
  using NodePtr = shared_ptr<Node<P, D> >;

  Node(Tree<P, D> *tree, const P& prefix)
    : tree_(tree), parent_(nullptr), prefix_(prefix) {
  }
  Node(Tree<P, D> *tree, const P& prefix1, const P& prefix2)
    : tree_(tree), parent_(nullptr), prefix_(prefix1, prefix2) {
  }

  const P& prefix() { return prefix_; }

  //  const shared_ptr<Node<P, D> > child(u_char bit) {
  const NodePtr child(u_char bit) {
    return children_[bit];
  }

  void setChild(u_char bit, NodePtr node) {
    children_[bit] = node;
    node->parent_ = enable_shared_from_this<Node<P, D> >::shared_from_this();
  }

  void setParent(NodePtr parent) {
    parent_ = parent;
  }

  const NodePtr nextNode() {
    shared_ptr<Node<P, D> > curr =
      enable_shared_from_this<Node<P, D> >::shared_from_this();

    if (curr->children_[0]) {
      return curr->children_[0];
    }
    if (curr->children_[1]) {
      return curr->children_[1];
    }

    while (curr->parent_) {
      if (curr->parent_->children_[0] == curr && curr->parent_->children_[1]) {
        return curr->parent_->children_[1];
      }
      curr = curr->parent_;
    }

    return nullptr;
  }

private:
  // Pointer to the tree.
  Tree<P, D> *tree_;

  // Pointer to parent node.
  NodePtr parent_;

  // Prefix of this node.
  P prefix_;

  // Children nodes.
  NodePtr children_[2];

  // Data.
  shared_ptr<D> data_;
};

template<typename P, typename D>
class Tree
{
public:
  Tree() {}
  ~Tree() {}

  shared_ptr<Node<P, D> > setNode(const P& prefix) {
    return make_shared<Node<P, D> >(this, prefix);
  }

  void setChildNode(shared_ptr<Node<P, D> > parent,
                    shared_ptr<Node<P, D> > child) {
    u_char bit = (u_char)child->prefix().bit_at(parent->prefix().len());
    parent->setChild(bit, child);
  }

  // Return a node if it exists, otherwise create one.
  shared_ptr<Node<P, D> > getNode(const P& prefix) {
    shared_ptr<Node<P, D> > new_node;
    shared_ptr<Node<P, D> > curr = top_;
    shared_ptr<Node<P, D> > matched = nullptr;

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
      new_node = make_shared<Node<P, D> >(this, curr->prefix(), prefix);
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
        // count??
      }
    }
    // count??
    // lock ??

    return new_node;
  }

  shared_ptr<Node<P, D>> AddNode(P p, D d) {

  }

  shared_ptr<Node<P, D> > top() { return top_; }

private:
  // top node of this tree.
  shared_ptr<Node<P, D> > top_;

  // number of nodes in this tree.
  uint32_t count_;
};

} // namespace librt

#endif // _RADIXTREE_LIB_TREE_HPP_
