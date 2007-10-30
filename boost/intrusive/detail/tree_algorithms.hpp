/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2007.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_TREE_ALGORITHMS_HPP
#define BOOST_INTRUSIVE_TREE_ALGORITHMS_HPP

#include <boost/intrusive/detail/config_begin.hpp>
#include <boost/intrusive/detail/assert.hpp>
#include <boost/intrusive/intrusive_fwd.hpp>
#include <cstddef>
#include <boost/intrusive/detail/no_exceptions_support.hpp>
#include <boost/intrusive/detail/utilities.hpp>

namespace boost {
namespace intrusive {
namespace detail {

//!   This is an implementation of a binary search tree.
//!   A node in the search tree has references to its children and its parent. This 
//!   is to allow traversal of the whole tree from a given node making the 
//!   implementation of iterator a pointer to a node.
//!   At the top of the tree a node is used specially. This node's parent pointer 
//!   is pointing to the root of the tree. Its left pointer points to the 
//!   leftmost node in the tree and the right pointer to the rightmost one.
//!   This node is used to represent the end-iterator.
//!
//!                                            +---------+ 
//!       header------------------------------>|         | 
//!                                            |         | 
//!                   +----------(left)--------|         |--------(right)---------+ 
//!                   |                        +---------+                        | 
//!                   |                             |                             | 
//!                   |                             | (parent)                    |
//!                   |                             |                             |
//!                   |                             |                             |
//!                   |                        +---------+                        |
//!    root of tree ..|......................> |         |                        |
//!                   |                        |    D    |                        |
//!                   |                        |         |                        |
//!                   |                +-------+---------+-------+                |
//!                   |                |                         |                |
//!                   |                |                         |                |
//!                   |                |                         |                |
//!                   |                |                         |                |
//!                   |                |                         |                |
//!                   |          +---------+                 +---------+          |
//!                   |          |         |                 |         |          |
//!                   |          |    B    |                 |    F    |          |
//!                   |          |         |                 |         |          |
//!                   |       +--+---------+--+           +--+---------+--+       |
//!                   |       |               |           |               |       |
//!                   |       |               |           |               |       |
//!                   |       |               |           |               |       |
//!                   |   +---+-----+   +-----+---+   +---+-----+   +-----+---+   |
//!                   +-->|         |   |         |   |         |   |         |<--+ 
//!                       |    A    |   |    C    |   |    E    |   |    G    | 
//!                       |         |   |         |   |         |   |         | 
//!                       +---------+   +---------+   +---------+   +---------+ 
//!

//! tree_algorithms is configured with a NodeTraits class, which encapsulates the
//! information about the node to be manipulated. NodeTraits must support the
//! following interface:
//!
//! <b>Typedefs</b>:
//!
//! <tt>node</tt>: The type of the node that forms the circular list
//!
//! <tt>node_ptr</tt>: A pointer to a node
//!
//! <tt>const_node_ptr</tt>: A pointer to a const node
//!
//! <b>Static functions</b>:
//!
//! <tt>static node_ptr get_parent(const_node_ptr n);</tt>
//! 
//! <tt>static void set_parent(node_ptr n, node_ptr parent);</tt>
//!
//! <tt>static node_ptr get_left(const_node_ptr n);</tt>
//! 
//! <tt>static void set_left(node_ptr n, node_ptr left);</tt>
//!
//! <tt>static node_ptr get_right(const_node_ptr n);</tt>
//! 
//! <tt>static void set_right(node_ptr n, node_ptr right);</tt>
template<class NodeTraits>
class tree_algorithms
{
   /// @cond
   private:
   typedef typename NodeTraits::node            node;
   /// @endcond

   public:
   typedef NodeTraits                           node_traits;
   typedef typename NodeTraits::node_ptr        node_ptr;
   typedef typename NodeTraits::const_node_ptr  const_node_ptr;

   //! This type is the information that will be filled by insert_unique_check
   struct insert_commit_data
   {
      insert_commit_data()
         :  link_left(false)
         ,  node(0)
      {}
      bool     link_left;
      node_ptr node;
   };

   struct nop_erase_fixup
   {
      void operator()(node_ptr to_erase, node_ptr successor){}
   };

   /// @cond
   private:
   static node_ptr uncast(const_node_ptr ptr)
   {
      return node_ptr(const_cast<node*>(::boost::intrusive::detail::get_pointer(ptr)));
   }
   /// @endcond

   public:
   static node_ptr begin_node(const_node_ptr header)
   {  return node_traits::get_left(header);   }

   static node_ptr end_node(const_node_ptr header)
   {  return uncast(header);   }

   //! <b>Requires</b>: node is a node of the tree or an node initialized
   //!   by init(...).
   //! 
   //! <b>Effects</b>: Returns true if the node is initialized by init().
   //! 
   //! <b>Complexity</b>: Constant time.
   //! 
   //! <b>Throws</b>: Nothing.
   static bool unique(const_node_ptr node)
   { return NodeTraits::get_parent(node) == 0; }

   static node_ptr get_header(const_node_ptr node)
   {
      node_ptr h = uncast(node);
      if(NodeTraits::get_parent(node)){
         h = NodeTraits::get_parent(node);
         while(!is_header(h))
            h = NodeTraits::get_parent(h);
      }
      return h;
   }

   //! <b>Requires</b>: node1 and node2 can't be header nodes
   //!  of two trees.
   //! 
   //! <b>Effects</b>: Swaps two nodes. After the function node1 will be inserted
   //!   in the position node2 before the function. node2 will be inserted in the
   //!   position node1 had before the function.
   //! 
   //! <b>Complexity</b>: Logarithmic. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: This function will break container ordering invariants if
   //!   node1 and node2 are not equivalent according to the ordering rules.
   //!
   //!Experimental function
   static void swap_nodes(node_ptr node1, node_ptr node2)
   {
      if(node1 == node2)
         return;
   
      node_ptr header1(get_header(node1)), header2(get_header(node2));
      swap_nodes(node1, header1, node2, header2);
   }

   //! <b>Requires</b>: node1 and node2 can't be header nodes
   //!  of two trees with header header1 and header2.
   //! 
   //! <b>Effects</b>: Swaps two nodes. After the function node1 will be inserted
   //!   in the position node2 before the function. node2 will be inserted in the
   //!   position node1 had before the function.
   //! 
   //! <b>Complexity</b>: Constant. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: This function will break container ordering invariants if
   //!   node1 and node2 are not equivalent according to the ordering rules.
   //!
   //!Experimental function
   static void swap_nodes(node_ptr node1, node_ptr header1, node_ptr node2, node_ptr header2)
   {
      if(node1 == node2)
         return;
   
      //node1 and node2 must not be header nodes 
      //BOOST_INTRUSIVE_INVARIANT_ASSERT((header1 != node1 && header2 != node2));
      if(header1 != header2){
         //Update header1 if necessary
         if(node1 == NodeTraits::get_left(header1)){
            NodeTraits::set_left(header1, node2);
         }

         if(node1 == NodeTraits::get_right(header1)){
            NodeTraits::set_right(header1, node2);
         }

         if(node1 == NodeTraits::get_parent(header1)){
            NodeTraits::set_parent(header1, node2);
         }

         //Update header2 if necessary
         if(node2 == NodeTraits::get_left(header2)){
            NodeTraits::set_left(header2, node1);
         }

         if(node2 == NodeTraits::get_right(header2)){
            NodeTraits::set_right(header2, node1);
         }

         if(node2 == NodeTraits::get_parent(header2)){
            NodeTraits::set_parent(header2, node1);
         }
      }
      else{
         //If both nodes are from the same tree
         //Update header if necessary
         if(node1 == NodeTraits::get_left(header1)){
            NodeTraits::set_left(header1, node2);
         }
         else if(node2 == NodeTraits::get_left(header2)){
            NodeTraits::set_left(header2, node1);
         }

         if(node1 == NodeTraits::get_right(header1)){
            NodeTraits::set_right(header1, node2);
         }
         else if(node2 == NodeTraits::get_right(header2)){
            NodeTraits::set_right(header2, node1);
         }

         if(node1 == NodeTraits::get_parent(header1)){
            NodeTraits::set_parent(header1, node2);
         }
         else if(node2 == NodeTraits::get_parent(header2)){
            NodeTraits::set_parent(header2, node1);
         }

         //Adjust data in nodes to be swapped
         //so that final link swap works as expected
         if(node1 == NodeTraits::get_parent(node2)){
            NodeTraits::set_parent(node2, node2);

            if(node2 == NodeTraits::get_right(node1)){
               NodeTraits::set_right(node1, node1);
            }
            else{
               NodeTraits::set_left(node1, node1);
            }
         }
         else if(node2 == NodeTraits::get_parent(node1)){
            NodeTraits::set_parent(node1, node1);

            if(node1 == NodeTraits::get_right(node2)){
               NodeTraits::set_right(node2, node2);
            }
            else{
               NodeTraits::set_left(node2, node2);
            }
         }
      }

      //Now swap all the links
      node_ptr temp;
      //swap left link
      temp = NodeTraits::get_left(node1);
      NodeTraits::set_left(node1, NodeTraits::get_left(node2));
      NodeTraits::set_left(node2, temp);
      //swap right link
      temp = NodeTraits::get_right(node1);
      NodeTraits::set_right(node1, NodeTraits::get_right(node2));
      NodeTraits::set_right(node2, temp);
      //swap parent link
      temp = NodeTraits::get_parent(node1);
      NodeTraits::set_parent(node1, NodeTraits::get_parent(node2));
      NodeTraits::set_parent(node2, temp);

      //Now adjust adjacent nodes for newly inserted node 1
      if((temp = NodeTraits::get_left(node1))){
         NodeTraits::set_parent(temp, node1);
      }
      if((temp = NodeTraits::get_right(node1))){
         NodeTraits::set_parent(temp, node1);
      }
      if((temp = NodeTraits::get_parent(node1)) &&
         //The header has been already updated so avoid it
         temp != header2){
         if(NodeTraits::get_left(temp) == node2){
            NodeTraits::set_left(temp, node1);
         }
         if(NodeTraits::get_right(temp) == node2){
            NodeTraits::set_right(temp, node1);
         }
      }
      //Now adjust adjacent nodes for newly inserted node 2
      if((temp = NodeTraits::get_left(node2))){
         NodeTraits::set_parent(temp, node2);
      }
      if((temp = NodeTraits::get_right(node2))){
         NodeTraits::set_parent(temp, node2);
      }
      if((temp = NodeTraits::get_parent(node2)) &&
         //The header has been already updated so avoid it
         temp != header1){
         if(NodeTraits::get_left(temp) == node1){
            NodeTraits::set_left(temp, node2);
         }
         if(NodeTraits::get_right(temp) == node1){
            NodeTraits::set_right(temp, node2);
         }
      }
   }

   //! <b>Requires</b>: node_to_be_replaced must be inserted in a tree
   //!   and new_node must not be inserted in a tree.
   //! 
   //! <b>Effects</b>: Replaces node_to_be_replaced in its position in the
   //!   tree with new_node. The tree does not need to be rebalanced
   //! 
   //! <b>Complexity</b>: Logarithmic. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: This function will break container ordering invariants if
   //!   new_node is not equivalent to node_to_be_replaced according to the
   //!   ordering rules. This function is faster than erasing and inserting
   //!   the node, since no rebalancing and comparison is needed.
   //!
   //!Experimental function
   static void replace_node(node_ptr node_to_be_replaced, node_ptr new_node)
   {
      if(node_to_be_replaced == new_node)
         return;
      replace_node(node_to_be_replaced, get_header(node_to_be_replaced), new_node);
   }

   //! <b>Requires</b>: node_to_be_replaced must be inserted in a tree
   //!   with header "header" and new_node must not be inserted in a tree.
   //! 
   //! <b>Effects</b>: Replaces node_to_be_replaced in its position in the
   //!   tree with new_node. The tree does not need to be rebalanced
   //! 
   //! <b>Complexity</b>: Constant. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: This function will break container ordering invariants if
   //!   new_node is not equivalent to node_to_be_replaced according to the
   //!   ordering rules. This function is faster than erasing and inserting
   //!   the node, since no rebalancing or comparison is needed.
   //!
   //!Experimental function
   static void replace_node(node_ptr node_to_be_replaced, node_ptr header, node_ptr new_node)
   {
      if(node_to_be_replaced == new_node)
         return;
   
      //Update header if necessary
      if(node_to_be_replaced == NodeTraits::get_left(header)){
         NodeTraits::set_left(header, new_node);
      }

      if(node_to_be_replaced == NodeTraits::get_right(header)){
         NodeTraits::set_right(header, new_node);
      }

      if(node_to_be_replaced == NodeTraits::get_parent(header)){
         NodeTraits::set_parent(header, new_node);
      }

      //Now set data from the original node
      node_ptr temp;
      NodeTraits::set_left(new_node, NodeTraits::get_left(node_to_be_replaced));
      NodeTraits::set_right(new_node, NodeTraits::get_right(node_to_be_replaced));
      NodeTraits::set_parent(new_node, NodeTraits::get_parent(node_to_be_replaced));

      //Now adjust adjacent nodes for newly inserted node
      if((temp = NodeTraits::get_left(new_node))){
         NodeTraits::set_parent(temp, new_node);
      }
      if((temp = NodeTraits::get_right(new_node))){
         NodeTraits::set_parent(temp, new_node);
      }
      if((temp = NodeTraits::get_parent(new_node)) &&
         //The header has been already updated so avoid it
         temp != header){
         if(NodeTraits::get_left(temp) == node_to_be_replaced){
            NodeTraits::set_left(temp, new_node);
         }
         if(NodeTraits::get_right(temp) == node_to_be_replaced){
            NodeTraits::set_right(temp, new_node);
         }
      }
   }

   //! <b>Requires</b>: p is a node from the tree except the header.
   //! 
   //! <b>Effects</b>: Returns the next node of the tree.
   //! 
   //! <b>Complexity</b>: Average constant time.
   //! 
   //! <b>Throws</b>: Nothing.
   static node_ptr next_node(node_ptr p)
   {
      node_ptr p_right(NodeTraits::get_right(p));
      if(p_right){
         return minimum(p_right);
      }
      else {
         node_ptr x = NodeTraits::get_parent(p);
         while(p == NodeTraits::get_right(x)){
            p = x;
            x = NodeTraits::get_parent(x);
         }
         return NodeTraits::get_right(p) != x ? x : uncast(p);
      }
   }

   //! <b>Requires</b>: p is a node from the tree except the leftmost node.
   //! 
   //! <b>Effects</b>: Returns the previous node of the tree.
   //! 
   //! <b>Complexity</b>: Average constant time.
   //! 
   //! <b>Throws</b>: Nothing.
   static node_ptr prev_node(node_ptr p)
   {
      if(is_header(p)){
         return maximum(NodeTraits::get_parent(p));
      }
      else if(NodeTraits::get_left(p)){
         return maximum(NodeTraits::get_left(p));
      }
      else {
         node_ptr x = NodeTraits::get_parent(p);
         while(p == NodeTraits::get_left(x)){
            p = x;
            x = NodeTraits::get_parent(x);
         }
         return x;
      }
   }

   //! <b>Requires</b>: p is a node of a tree but not the header.
   //! 
   //! <b>Effects</b>: Returns the minimum node of the subtree starting at p.
   //! 
   //! <b>Complexity</b>: Logarithmic to the size of the subtree.
   //! 
   //! <b>Throws</b>: Nothing.
   static node_ptr minimum (node_ptr p)
   {
      for(node_ptr p_left = NodeTraits::get_left(p)
         ;p_left
         ;p_left = NodeTraits::get_left(p)){
         p = p_left;
      }
      return p;
   }

   //! <b>Requires</b>: p is a node of a tree but not the header.
   //! 
   //! <b>Effects</b>: Returns the maximum node of the subtree starting at p.
   //! 
   //! <b>Complexity</b>: Logarithmic to the size of the subtree.
   //! 
   //! <b>Throws</b>: Nothing.
   static node_ptr maximum(node_ptr p)
   {
      for(node_ptr p_right = NodeTraits::get_right(p)
         ;p_right
         ;p_right = NodeTraits::get_right(p)){
         p = p_right;
      }
      return p;
   }

   //! <b>Requires</b>: node must not be part of any tree.
   //!
   //! <b>Effects</b>: After the function unique(node) == true.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Nodes</b>: If node is inserted in a tree, this function corrupts the tree.
   static void init(node_ptr node)
   {
      NodeTraits::set_parent(node, 0);
      NodeTraits::set_left(node, 0);
      NodeTraits::set_right(node, 0); 
   };

   //! <b>Requires</b>: node must not be part of any tree.
   //!
   //! <b>Effects</b>: Initializes the header to represent an empty tree.
   //!   unique(header) == true.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Nodes</b>: If node is inserted in a tree, this function corrupts the tree.
   static void init_header(node_ptr header)
   {
      NodeTraits::set_parent(header, 0);
      NodeTraits::set_left(header, header);
      NodeTraits::set_right(header, header); 
   }

   //! <b>Requires</b>: "disposer" must be an object function
   //!   taking a node_ptr parameter and shouldn't throw.
   //!
   //! <b>Effects</b>: Empties the target tree calling 
   //!   <tt>void disposer::operator()(node_ptr)</tt> for every node of the tree
   //!    except the header.
   //! 
   //! <b>Complexity</b>: Linear to the number of element of the source tree plus the.
   //!   number of elements of tree target tree when calling this function.
   //! 
   //! <b>Throws</b>: If cloner functor throws. If this happens target nodes are disposed.
   template<class Disposer>
   static void clear_and_dispose(node_ptr header, Disposer disposer)
   {
      node_ptr source_root = NodeTraits::get_parent(header);
      if(!source_root)
         return;
      dispose_subtree(source_root, disposer);
      init_header(header);
   }

   //! <b>Requires</b>: header is the header of a tree.
   //! 
   //! <b>Effects</b>: Unlinks the leftmost node from the tree, and
   //!   updates the header link to the new leftmost node.
   //! 
   //! <b>Complexity</b>: Average complexity is constant time.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Notes</b>: This function breaks the tree and the tree can
   //!   only be used for more unlink_leftmost_without_rebalance calls.
   //!   This function is normally used to achieve a step by step
   //!   controlled destruction of the tree.
   static node_ptr unlink_leftmost_without_rebalance(node_ptr header)
   {
      node_ptr leftmost = NodeTraits::get_left(header);
      if (leftmost == header)
         return 0;
      node_ptr leftmost_parent(NodeTraits::get_parent(leftmost));
      node_ptr leftmost_right (NodeTraits::get_right(leftmost));
      bool is_root = leftmost_parent == header;

      if (leftmost_right){
         NodeTraits::set_parent(leftmost_right, leftmost_parent);
         NodeTraits::set_left(header, tree_algorithms::minimum(leftmost_right));

         if (is_root)
            NodeTraits::set_parent(header, leftmost_right);
         else
            NodeTraits::set_left(NodeTraits::get_parent(header), leftmost_right);
      }
      else if (is_root){
         NodeTraits::set_parent(header, 0);
         NodeTraits::set_left(header,  header);
         NodeTraits::set_right(header, header);
      }
      else{
         NodeTraits::set_left(leftmost_parent, 0);
         NodeTraits::set_left(header, leftmost_parent);
      }
      return leftmost;
   }

   //! <b>Requires</b>: node is a node of the tree but it's not the header.
   //! 
   //! <b>Effects</b>: Returns the number of nodes of the subtree.
   //! 
   //! <b>Complexity</b>: Linear time.
   //! 
   //! <b>Throws</b>: Nothing.
   static std::size_t count(const_node_ptr node)
   {
      std::size_t result = 1;
      if(NodeTraits::get_left(node))
         result += count(NodeTraits::get_left(node));
      if(NodeTraits::get_right(node))
         result += count(NodeTraits::get_right(node));
      return result;
   }

   //! <b>Requires</b>: header1 and header2 must be the header nodes
   //!  of two trees.
   //! 
   //! <b>Effects</b>: Swaps two trees. After the function header1 will contain 
   //!   links to the second tree and header2 will have links to the first tree.
   //! 
   //! <b>Complexity</b>: Constant. 
   //! 
   //! <b>Throws</b>: Nothing.
   static void swap_tree(node_ptr header1, node_ptr header2)
   {
      if(header1 == header2)
         return;
   
      node_ptr tmp;

      //Parent swap
      tmp = NodeTraits::get_parent(header1);
      NodeTraits::set_parent(header1, NodeTraits::get_parent(header2));
      NodeTraits::set_parent(header2, tmp);
      //Left swap
      tmp = NodeTraits::get_left(header1);
      NodeTraits::set_left(header1, NodeTraits::get_left(header2));
      NodeTraits::set_left(header2, tmp);
      //Right swap
      tmp = NodeTraits::get_right(header1);
      NodeTraits::set_right(header1, NodeTraits::get_right(header2));
      NodeTraits::set_right(header2, tmp);

      //Now test parent
      node_ptr h1_parent(NodeTraits::get_parent(header1));
      if(h1_parent){
         NodeTraits::set_parent(h1_parent, header1);
      }
      else{
         NodeTraits::set_left(header1, header1);
         NodeTraits::set_right(header1, header1);
      }

      node_ptr h2_parent(NodeTraits::get_parent(header2));
      if(h2_parent){
         NodeTraits::set_parent(h2_parent, header2);
      }
      else{
         NodeTraits::set_left(header2, header2);
         NodeTraits::set_right(header2, header2);
      }
   }

   static bool is_header(const_node_ptr p)
   {
      bool is_header = false;
      if(NodeTraits::get_parent(p) == p){
         is_header = true;
      }
      else if(NodeTraits::get_parent(NodeTraits::get_parent(p)) == p){
         if(NodeTraits::get_left(p) != 0){
            if(NodeTraits::get_parent(NodeTraits::get_left(p)) != p){
               is_header = true;
            }
            if(NodeTraits::get_parent(p) == NodeTraits::get_left(p)){
               is_header = true;
            }
         }
      }
      return is_header;
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   KeyNodePtrCompare is a function object that induces a strict weak
   //!   ordering compatible with the strict weak ordering used to create the
   //!   the tree. KeyNodePtrCompare can compare KeyType with tree's node_ptrs.
   //!
   //! <b>Effects</b>: Returns an node_ptr to the element that is equivalent to
   //!   "key" according to "comp" or "header" if that element does not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: If "comp" throws.
   template<class KeyType, class KeyNodePtrCompare>
   static node_ptr find
      (const_node_ptr header, const KeyType &key, KeyNodePtrCompare comp)
   {
      node_ptr end = uncast(header);
      node_ptr y = lower_bound(header, key, comp);
      return (y == end || comp(key, y)) ? end : y;
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   KeyNodePtrCompare is a function object that induces a strict weak
   //!   ordering compatible with the strict weak ordering used to create the
   //!   the tree. KeyNodePtrCompare can compare KeyType with tree's node_ptrs.
   //!
   //! <b>Effects</b>: Returns an a pair of node_ptr delimiting a range containing
   //!   all elements that are equivalent to "key" according to "comp" or an
   //!   empty range that indicates the position where those elements would be
   //!   if they there are no equivalent elements.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: If "comp" throws.
   template<class KeyType, class KeyNodePtrCompare>
   static std::pair<node_ptr, node_ptr> equal_range
      (const_node_ptr header, const KeyType &key, KeyNodePtrCompare comp)
   {
      node_ptr y = uncast(header);
      node_ptr x = NodeTraits::get_parent(header);

      while(x){
         if(comp(x, key)){
            x = NodeTraits::get_right(x);
         }
         else if(comp(key, x)){
            y = x;
            x = NodeTraits::get_left(x);
         }
         else{
            node_ptr xu(x), yu(y);
            y = x, x = NodeTraits::get_left(x);
            xu = NodeTraits::get_right(xu);

            while(x){
               if(comp(x, key)){
                  x = NodeTraits::get_right(x);
               }
               else {
                  y = x;
                  x = NodeTraits::get_left(x);
               }
            }

            while(xu){
               if(comp(key, xu)){
                  yu = xu;
                  xu = NodeTraits::get_left(xu);
               }
               else {
                  xu = NodeTraits::get_right(xu);
               }
            }
            return std::pair<node_ptr,node_ptr> (y, yu);
         }
      }
      return std::pair<node_ptr,node_ptr> (y, y);
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   KeyNodePtrCompare is a function object that induces a strict weak
   //!   ordering compatible with the strict weak ordering used to create the
   //!   the tree. KeyNodePtrCompare can compare KeyType with tree's node_ptrs.
   //!
   //! <b>Effects</b>: Returns an node_ptr to the first element that is
   //!   not less than "key" according to "comp" or "header" if that element does
   //!   not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: If "comp" throws.
   template<class KeyType, class KeyNodePtrCompare>
   static node_ptr lower_bound
      (const_node_ptr header, const KeyType &key, KeyNodePtrCompare comp)
   {
      node_ptr y = uncast(header);
      node_ptr x = NodeTraits::get_parent(header);
      while(x){
         if(comp(x, key)){
            x = NodeTraits::get_right(x);
         }
         else {
            y = x;
            x = NodeTraits::get_left(x);
         }
      }
      return y;
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   KeyNodePtrCompare is a function object that induces a strict weak
   //!   ordering compatible with the strict weak ordering used to create the
   //!   the tree. KeyNodePtrCompare can compare KeyType with tree's node_ptrs.
   //!
   //! <b>Effects</b>: Returns an node_ptr to the first element that is greater
   //!   than "key" according to "comp" or "header" if that element does not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: If "comp" throws.
   template<class KeyType, class KeyNodePtrCompare>
   static node_ptr upper_bound
      (const_node_ptr header, const KeyType &key, KeyNodePtrCompare comp)
   {
      node_ptr y = uncast(header);
      node_ptr x = NodeTraits::get_parent(header);
      while(x){
         if(comp(key, x)){
            y = x;
            x = NodeTraits::get_left(x);
         }
         else {
            x = NodeTraits::get_right(x);
         }
      }
      return y;
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   "commit_data" must have been obtained from a previous call to
   //!   "insert_unique_check". No objects should have been inserted or erased
   //!   from the set between the "insert_unique_check" that filled "commit_data"
   //!   and the call to "insert_commit". 
   //! 
   //! 
   //! <b>Effects</b>: Inserts new_node in the set using the information obtained
   //!   from the "commit_data" that a previous "insert_check" filled.
   //!
   //! <b>Complexity</b>: Constant time.
   //!
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Notes</b>: This function has only sense if a "insert_unique_check" has been
   //!   previously executed to fill "commit_data". No value should be inserted or
   //!   erased between the "insert_check" and "insert_commit" calls.
   static void insert_unique_commit
      (node_ptr header, node_ptr new_value, const insert_commit_data &commit_data)
   {
      //Check if commit_data has not been initialized by a insert_unique_check call.
      BOOST_INTRUSIVE_INVARIANT_ASSERT(commit_data.node != 0);
      link(header, new_value, commit_data.node, commit_data.link_left);
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   KeyNodePtrCompare is a function object that induces a strict weak
   //!   ordering compatible with the strict weak ordering used to create the
   //!   the tree. NodePtrCompare compares KeyType with a node_ptr.
   //! 
   //! <b>Effects</b>: Checks if there is an equivalent node to "key" in the
   //!   tree according to "comp" and obtains the needed information to realize
   //!   a constant-time node insertion if there is no equivalent node.
   //!
   //! <b>Returns</b>: If there is an equivalent value
   //!   returns a pair containing a node_ptr to the already present node
   //!   and false. If there is not equivalent key can be inserted returns true
   //!   in the returned pair's boolean and fills "commit_data" that is meant to
   //!   be used with the "insert_commit" function to achieve a constant-time
   //!   insertion function.
   //! 
   //! <b>Complexity</b>: Average complexity is at most logarithmic.
   //!
   //! <b>Throws</b>: If "comp" throws.
   //! 
   //! <b>Notes</b>: This function is used to improve performance when constructing
   //!   a node is expensive and the user does not want to have two equivalent nodes
   //!   in the tree: if there is an equivalent value
   //!   the constructed object must be discarded. Many times, the part of the
   //!   node that is used to impose the order is much cheaper to construct
   //!   than the node and this function offers the possibility to use that part
   //!   to check if the insertion will be successful.
   //!
   //!   If the check is successful, the user can construct the node and use
   //!   "insert_commit" to insert the node in constant-time. This gives a total
   //!   logarithmic complexity to the insertion: check(O(log(N)) + commit(O(1)).
   //!
   //!   "commit_data" remains valid for a subsequent "insert_unique_commit" only
   //!   if no more objects are inserted or erased from the set.
   template<class KeyType, class KeyNodePtrCompare>
   static std::pair<node_ptr, bool> insert_unique_check
      (const_node_ptr header,  const KeyType &key
      ,KeyNodePtrCompare comp, insert_commit_data &commit_data)
   {
      node_ptr h(uncast(header));
      node_ptr y(h);
      node_ptr x(NodeTraits::get_parent(y));
      node_ptr prev(0);

      //Find the upper bound, cache the previous value and if we should
      //store it in the left or right node
      bool left_child = true;
      while(x){
         y = x;
         x = (left_child = comp(key, x)) ? 
               NodeTraits::get_left(x) : (prev = y, NodeTraits::get_right(x));
      }

      //Since we've found the upper bound there is no other value with the same key if:
      //    - There is no previous node
      //    - The previous node is less than the key
      if(!prev || comp(prev, key)){
         commit_data.link_left = left_child;
         commit_data.node      = y;
         return std::pair<node_ptr, bool>(node_ptr(), true);
      }
      //If the previous value was not less than key, it means that it's equal
      //(because we've checked the upper bound)
      else{
         return std::pair<node_ptr, bool>(prev, false);
      }
   }

   template<class KeyType, class KeyNodePtrCompare>
   static std::pair<node_ptr, bool> insert_unique_check
      (const_node_ptr header,  node_ptr hint, const KeyType &key
      ,KeyNodePtrCompare comp, insert_commit_data &commit_data)
   {
      //hint must be bigger than the key
      if(hint == header || comp(key, hint)){
         node_ptr prev = hint;
         //The previous value should be less than the key
         if(prev == NodeTraits::get_left(header) || comp((prev = prev_node(hint)), key)){
            commit_data.link_left = unique(header) || !NodeTraits::get_left(hint);
            commit_data.node      = commit_data.link_left ? hint : prev;
            return std::pair<node_ptr, bool>(node_ptr(), true);
         }
         else{
            return insert_unique_check(header, key, comp, commit_data);
         }
      }
      //The hint was wrong, use hintless insert
      else{
         return insert_unique_check(header, key, comp, commit_data);
      }
   }

   //! <b>Requires</b>: "header" must be the header node of a tree.
   //!   NodePtrCompare is a function object that induces a strict weak
   //!   ordering compatible with the strict weak ordering used to create the
   //!   the tree. NodePtrCompare compares two node_ptrs. "hint" is node from
   //!   the "header"'s tree.
   //!   
   //! <b>Effects</b>: Inserts new_node into the tree, using "hint" as a hint to
   //!   where it will be inserted. If "hint" is the upper_bound
   //!   the insertion takes constant time (two comparisons in the worst case).
   //!
   //! <b>Complexity</b>: Logarithmic in general, but it is amortized
   //!   constant time if new_node is inserted immediately before "hint".
   //! 
   //! <b>Throws</b>: If "comp" throws.
   template<class NodePtrCompare>
   static node_ptr insert_equal
      (node_ptr header, node_ptr hint, node_ptr new_node, NodePtrCompare comp)
   {
      if(hint == header || !comp(hint, new_node)){
         node_ptr prev(hint);
         if(hint == NodeTraits::get_left(header) || 
            !comp(new_node, (prev = prev_node(hint)))){
            bool link_left = unique(header) || !NodeTraits::get_left(hint);
            link(header, new_node, link_left ? hint : prev, link_left);
            return new_node;
         }
         else{
            return insert_equal_upper_bound(header, new_node, comp);
         }
      }
      else{
         return insert_equal_lower_bound(header, new_node, comp);
      }
   }

   template<class NodePtrCompare>
   static node_ptr insert_equal_upper_bound
      (node_ptr h, node_ptr new_node, NodePtrCompare comp)
   {
      node_ptr y(h);
      node_ptr x(NodeTraits::get_parent(y));

      while(x){
         y = x;
         x = comp(new_node, x) ? 
               NodeTraits::get_left(x) : NodeTraits::get_right(x);
      }

      bool link_left = (y == h) || comp(new_node, y);
      link(h, new_node, y, link_left);
      return new_node;
   }

   template<class NodePtrCompare>
   static node_ptr insert_equal_lower_bound
      (node_ptr h, node_ptr new_node, NodePtrCompare comp)
   {
      node_ptr y(h);
      node_ptr x(NodeTraits::get_parent(y));

      while(x){
         y = x;
         x = !comp(x, new_node) ? 
               NodeTraits::get_left(x) : NodeTraits::get_right(x);
      }

      bool link_left = (y == h) || !comp(y, new_node);
      link(h, new_node, y, link_left);
      return new_node;
   }

   //! <b>Requires</b>: "cloner" must be a function
   //!   object taking a node_ptr and returning a new cloned node of it. "disposer" must
   //!   take a node_ptr and shouldn't throw.
   //!
   //! <b>Effects</b>: First empties target tree calling 
   //!   <tt>void disposer::operator()(node_ptr)</tt> for every node of the tree
   //!    except the header.
   //!    
   //!   Then, duplicates the entire tree pointed by "source_header" cloning each
   //!   source node with <tt>node_ptr Cloner::operator()(node_ptr)</tt> to obtain 
   //!   the nodes of the target tree. If "cloner" throws, the cloned target nodes
   //!   are disposed using <tt>void disposer(node_ptr)</tt>.
   //! 
   //! <b>Complexity</b>: Linear to the number of element of the source tree plus the.
   //!   number of elements of tree target tree when calling this function.
   //! 
   //! <b>Throws</b>: If cloner functor throws. If this happens target nodes are disposed.
   template <class Cloner, class Disposer>
   static void clone
      (const_node_ptr source_header, node_ptr target_header, Cloner cloner, Disposer disposer)
   {
      if(!unique(target_header)){
         clear_and_dispose(target_header, disposer);
      }

      node_ptr leftmost, rightmost;
      node_ptr new_root = clone_subtree
         (source_header, target_header, cloner, disposer, leftmost, rightmost);

      //Now update header node
      NodeTraits::set_parent(target_header, new_root);
      NodeTraits::set_left  (target_header, leftmost);
      NodeTraits::set_right (target_header, rightmost);
   }

   template <class Cloner, class Disposer>
   static node_ptr clone_subtree
      ( const_node_ptr source_parent,  node_ptr target_parent
      , Cloner cloner,                 Disposer disposer
      , node_ptr &leftmost_out,        node_ptr &rightmost_out
      )
   {
      node_ptr target_sub_root = target_parent;
      node_ptr source_root = NodeTraits::get_parent(source_parent);
      if(!source_root){
         leftmost_out = rightmost_out = source_root;
      }
      else{
         //We'll calculate leftmost and rightmost nodes while iterating
         node_ptr current = source_root;
         node_ptr insertion_point = target_sub_root = cloner(current);

         //We'll calculate leftmost and rightmost nodes while iterating
         node_ptr leftmost  = target_sub_root;
         node_ptr rightmost = target_sub_root;

         //First set the subroot
         NodeTraits::set_left(target_sub_root, 0);
         NodeTraits::set_right(target_sub_root, 0);
         NodeTraits::set_parent(target_sub_root, target_parent);

         try {
            while(true) {
               //First clone left nodes
               if( NodeTraits::get_left(current) &&
                  !NodeTraits::get_left(insertion_point)) {
                  current = NodeTraits::get_left(current);
                  node_ptr temp = insertion_point;
                  //Clone and mark as leaf
                  insertion_point = cloner(current);
                  NodeTraits::set_left  (insertion_point, 0);
                  NodeTraits::set_right (insertion_point, 0);
                  //Insert left
                  NodeTraits::set_parent(insertion_point, temp);
                  NodeTraits::set_left  (temp, insertion_point);
                  //Update leftmost
                  if(rightmost == target_sub_root)
                     leftmost = insertion_point;
               }
               //Then clone right nodes
               else if( NodeTraits::get_right(current) && 
                       !NodeTraits::get_right(insertion_point)){
                  current = NodeTraits::get_right(current);
                  node_ptr temp = insertion_point;
                  //Clone and mark as leaf
                  insertion_point = cloner(current);
                  NodeTraits::set_left  (insertion_point, 0);
                  NodeTraits::set_right (insertion_point, 0);
                  //Insert right
                  NodeTraits::set_parent(insertion_point, temp);
                  NodeTraits::set_right (temp, insertion_point);
                  //Update rightmost
                  rightmost = insertion_point;
               }
               //If not, go up
               else if(current == source_root){
                  break;
               }
               else{
                  //Branch completed, go up searching more nodes to clone
                  current = NodeTraits::get_parent(current);
                  insertion_point = NodeTraits::get_parent(insertion_point);
               }
            }
         }
         catch(...) {
            dispose_subtree(target_sub_root, disposer);
            throw;
         }
         leftmost_out   = leftmost;
         rightmost_out  = rightmost;
      }
      return target_sub_root;
   }

   template<class Disposer>
   static void dispose_subtree(node_ptr x, Disposer disposer)
   {
      node_ptr save;
      while (x){
         save = NodeTraits::get_left(x);
         if (save) {
            // Right rotation
            NodeTraits::set_left(x, NodeTraits::get_right(save));
            NodeTraits::set_right(save, x);
         }
         else {
            save = NodeTraits::get_right(x);
            init(x);
            disposer(x);
         }
         x = save;
      }
   }

   //! <b>Requires</b>: p is a node of a tree.
   //! 
   //! <b>Effects</b>: Returns true if p is a left child.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   static bool is_left_child(node_ptr p)
   {  return NodeTraits::get_left(NodeTraits::get_parent(p)) == p;  }

   //! <b>Requires</b>: p is a node of a tree.
   //! 
   //! <b>Effects</b>: Returns true if p is a right child.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   static bool is_right_child (node_ptr p)
   {  return NodeTraits::get_right(NodeTraits::get_parent(p)) == p;  }

   static void replace_own (node_ptr own, node_ptr x, node_ptr header)
   {
      if(NodeTraits::get_parent(header) == own)
         NodeTraits::set_parent(header, x);
      else if(is_left_child(own))
         NodeTraits::set_left(NodeTraits::get_parent(own), x);
      else
         NodeTraits::set_right(NodeTraits::get_parent(own), x);
   }

   static void rotate_left(node_ptr p, node_ptr header)
   {
      node_ptr x = NodeTraits::get_right(p);
      NodeTraits::set_right(p, NodeTraits::get_left(x));
      if(NodeTraits::get_left(x) != 0)
         NodeTraits::set_parent(NodeTraits::get_left(x), p);
      NodeTraits::set_parent(x, NodeTraits::get_parent(p));
      replace_own (p, x, header);
      NodeTraits::set_left(x, p);
      NodeTraits::set_parent(p, x);
   }

   static void rotate_right(node_ptr p, node_ptr header)
   {
      node_ptr x(NodeTraits::get_left(p));
      node_ptr x_right(NodeTraits::get_right(x));
      NodeTraits::set_left(p, x_right);
      if(x_right)
         NodeTraits::set_parent(x_right, p);
      NodeTraits::set_parent(x, NodeTraits::get_parent(p));
      replace_own (p, x, header);
      NodeTraits::set_right(x, p);
      NodeTraits::set_parent(p, x);
   }

   // rotate node t with left child            | complexity : constant        | exception : nothrow
   static node_ptr rotate_left(node_ptr t)
   {
      node_ptr x = NodeTraits::get_right(t);
      NodeTraits::set_right(t, NodeTraits::get_left(x));

      if( NodeTraits::get_right(t) != 0 ){
         NodeTraits::set_parent(NodeTraits::get_right(t), t );
      }
      NodeTraits::set_left(x, t);
      NodeTraits::set_parent(t, x);
      return x;
   }

   // rotate node t with right child            | complexity : constant        | exception : nothrow
   static node_ptr rotate_right(node_ptr t)
   {
      node_ptr x = NodeTraits::get_left(t);
      NodeTraits::set_left(t, NodeTraits::get_right(x));
      if( NodeTraits::get_left(t) != 0 ){
         NodeTraits::set_parent(NodeTraits::get_left(t), t);
      }
      NodeTraits::set_right(x, t);
      NodeTraits::set_parent(t, x);
      return x;
   }

   static void link(node_ptr header, node_ptr z, node_ptr par, bool left)
   {
      if(par == header){
         NodeTraits::set_parent(header, z);
         NodeTraits::set_right(header, z);
         NodeTraits::set_left(header, z);
      }
      else if(left){
         NodeTraits::set_left(par, z);
         if(par == NodeTraits::get_left(header))
             NodeTraits::set_left(header, z);
      }
      else{
         NodeTraits::set_right(par, z);
         if(par == NodeTraits::get_right(header))
             NodeTraits::set_right(header, z);
      }
      NodeTraits::set_parent(z, par);
      NodeTraits::set_right(z, 0);
      NodeTraits::set_left(z, 0);
   }

   // delete node                        | complexity : constant        | exception : nothrow
   static void erase(node_ptr header, node_ptr z)
   {
      data_for_rebalance ignored;
      erase(header, z, nop_erase_fixup(), ignored);
   }

   struct data_for_rebalance
   {
      node_ptr x;
      node_ptr x_parent;
      node_ptr y;
   };

   template<class F>
   static void erase(node_ptr header, node_ptr z, F z_and_successor_fixup, data_for_rebalance &info)
   {
      erase_impl(header, z, info);
      if(info.y != z){
         z_and_successor_fixup(z, info.y);
      }
   }

   static void unlink(node_ptr node)
   {
      node_ptr x = NodeTraits::get_parent(node);
      if(x){
         while(!is_header(x))
            x = NodeTraits::get_parent(x);
         erase(x, node);
      }
   }

   private:
   static void erase_impl(node_ptr header, node_ptr z, data_for_rebalance &info)
   {
      node_ptr y(z);
      node_ptr x;
      node_ptr x_parent(0);
      node_ptr z_left(NodeTraits::get_left(z));
      node_ptr z_right(NodeTraits::get_right(z));
      if(!z_left){
         x = z_right;    // x might be null.
      }
      else if(!z_right){ // z has exactly one non-null child. y == z.
         x = z_left;       // x is not null.
      }
      else{
         // find z's successor
         y = tree_algorithms::minimum (z_right);
         x = NodeTraits::get_right(y);     // x might be null.
      }

      if(y != z){
         // relink y in place of z.  y is z's successor
         NodeTraits::set_parent(NodeTraits::get_left(z), y);
         NodeTraits::set_left(y, NodeTraits::get_left(z));
         if(y != NodeTraits::get_right(z)){
            x_parent = NodeTraits::get_parent(y);
            if(x)
               NodeTraits::set_parent(x, x_parent);
            NodeTraits::set_left(x_parent, x);   // y must be a child of left_
            NodeTraits::set_right(y, NodeTraits::get_right(z));
            NodeTraits::set_parent(NodeTraits::get_right(z), y);
         }
         else
            x_parent = y;
         tree_algorithms::replace_own (z, y, header);
         NodeTraits::set_parent(y, NodeTraits::get_parent(z));
      }
      else {   // y == z --> z has only one child, or none
         x_parent = NodeTraits::get_parent(z);
         if(x)
            NodeTraits::set_parent(x, x_parent);
         tree_algorithms::replace_own (z, x, header);
         if(NodeTraits::get_left(header) == z){
            NodeTraits::set_left(header, NodeTraits::get_right(z) == 0 ?        // z->get_left() must be null also
               NodeTraits::get_parent(z) :  // makes leftmost == header if z == root
               tree_algorithms::minimum (x));
         }
         if(NodeTraits::get_right(header) == z){
            NodeTraits::set_right(header, NodeTraits::get_left(z) == 0 ?        // z->get_right() must be null also
                              NodeTraits::get_parent(z) :  // makes rightmost == header if z == root
                              tree_algorithms::maximum(x));
         }
      }

      info.x = x;
      info.x_parent = x_parent;
      info.y = y;
   }

};

}  //namespace detail {
}  //namespace intrusive 
}  //namespace boost 

#include <boost/intrusive/detail/config_end.hpp>

#endif //BOOST_INTRUSIVE_TREE_ALGORITHMS_HPP
