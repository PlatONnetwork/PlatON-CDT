
#ifndef __SET_H_
#define __SET_H_

#include "rbtree.h"
#include "assert.h"
#include <functional>

namespace platon {
namespace container {


template <class T>
class set_node {
  public:
  struct rb_node node;
  T value;
};

template <class T>
set_node<T>* set_node_entry(rb_node* n){
  return ((set_node<T> *)( (char *)n - __builtin_offsetof(set_node<T>, node) ));
}

template <class T, bool R>
class set_iterator {
  private:
    rb_node* node;
  public:

    set_node<T>* get_set_node(){
      return set_node_entry<T>(node);
    }

    T operator*(){
      return get_set_node()->value;
    }
    T* operator->(){
      set_node<T>* mnode = set_node_entry<T>(node);
      return &get_set_node()->value;
    }

    set_iterator<T,R> next(){
      return set_iterator(rb_next(node));
    } 
    set_iterator<T,R> operator++(){
      node = R? rb_prev(node) : rb_next(node);
      return *this;
    }
    set_iterator<T,R> operator++(int){
      set_iterator<T,R> ret = *this;
      node = R? rb_prev(node) : rb_next(node);
      return ret;
    }

    set_iterator(rb_node* n){node=n;}

    bool operator!=(set_iterator set_iter){
      return node != set_iter.node;
    }

    bool operator==(set_iterator set_iter){
      return node == set_iter.node;
    }
};

template <class T, class Compare = std::less<T>>
class set
{
private:
  rb_root root;

  Compare cmp;
  unsigned long set_size;

public:

  typedef set_iterator<T,false> iterator;
  typedef set_iterator<T,true> riterator;

  ////////////////////

  explicit set(const Compare& comp = Compare()){
    cmp = comp;
    root.rb_node = nullptr;
    set_size = 0;
  }
  
  set (std::initializer_list<T> il,
     const Compare& comp = Compare()){
    cmp = comp;
    root.rb_node = nullptr;
    set_size = 0;
    for(auto p : il)
      insert(p);
  }


  std::pair<iterator,bool> insert (const T& val);

  ~set(){clear();}

  iterator find(const T& value);

  unsigned long count(const T& value);
  iterator lower_bound(const T& value){return find(value);}
  iterator upper_bound(const T& value){return find(value).next();}
  std::pair<iterator,iterator> equal_range (const T& value){
    return make_pair(find(value), find(value).next());
  }

  iterator begin(){return set_iterator<T,false>(rb_first(&root));}
  iterator end(){return set_iterator<T,false>(nullptr);}
  riterator rbegin(){return set_iterator<T,true>(rb_last(&root));}
  riterator rend(){return set_iterator<T,true>(nullptr);}

  unsigned long size(){return set_size;}
  bool empty(){return set_size==0;}

  unsigned long erase(const T& value);
  iterator erase(iterator iter){return erase(iter, iter.next());}
  iterator erase(iterator, iterator);


  void swap(set&);
  void clear(){erase(begin(), end());}

  Compare key_comp() const {return cmp;}
  Compare value_comp() const{return cmp;}
};

template <class T, class Cmp>
unsigned long set<T,Cmp>::count(const T& value){
  auto iter = find(value);
  if(iter==end())return 0;
  else return 1;
}

template <class T, class Cmp>
void set<T,Cmp>::swap(set& x){
  rb_node* rb0 = root.rb_node;
  Cmp cmp0 = cmp;
  unsigned long set_size0 = set_size;

  root.rb_node = x.root.rb_node;
  cmp = x.cmp;
  set_size  = x.set_size;

  x.root.rb_node = rb0;
  x.cmp = cmp0;
  x.set_size = set_size0;
}


template <class T, class Cmp>
typename set<T,Cmp>::iterator set<T,Cmp>::find(const T& value){
  struct rb_node *node = root.rb_node;
  while (node) {
    set_node<T> *mnode = set_node_entry<T>(node);

    if (cmp(value,mnode->value))
      node = node->rb_left;
    else if (cmp(mnode->value, value))
      node = node->rb_right;
    else
      return iterator(node);
  }
  return iterator(nullptr);
}


template <class T, class Cmp>
std::pair<typename set<T,Cmp>::iterator,bool> 
set<T,Cmp>::insert (const T& value){

  struct rb_node **n = &(root.rb_node), *parent = nullptr;

  /* Figure out where to put new node */
  while (*n) {
    set_node<T> *mnode = set_node_entry<T>(*n);
    parent = *n;
    if (cmp(value, mnode->value))
      n = &((*n)->rb_left);
    else if (cmp(mnode->value,value))
      n = &((*n)->rb_right);
    else
      return std::make_pair(iterator(*n), false);
  }

  /* Add new node and rebalance tree. */
  set_node<T> *mnode = new set_node<T>;
  mnode->value = value;
  rb_link_node(&mnode->node, parent, n);
  rb_insert_color(&mnode->node, &root);
  set_size++;
  return std::make_pair(iterator(&mnode->node), true);
}

template <class T, class Cmp>
unsigned long set<T,Cmp>::erase(const T& value){

  auto iter = find(value);

  if(iter == end()){
    return 0;
  } else {
    set_node<T>* mnode = iter.get_set_node();

  	rb_erase(&mnode->node, &root);
  	delete mnode;
    set_size--;
    return 1;
  }
}

template <class T, class Cmp>
typename set<T,Cmp>::iterator set<T,Cmp>::erase(
                                                set<T,Cmp>::iterator iter,
                                                set<T,Cmp>::iterator end){

  while(iter!=end){

    set_node<T> *mnode = iter.get_set_node();

    if(mnode==nullptr){
      return iterator(nullptr);
    } else {
      iterator next = iter.next();
      
    	rb_erase(&mnode->node, &root);
    	delete mnode;
      set_size--;
      iter = next;
    }
  }

  return end;
}

 
}  // namespace container
}  // namespace platon

#endif
