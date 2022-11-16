#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);
    //Helper functions
    virtual void insertFix(AVLNode<Key,Value>*parent, AVLNode<Key,Value>* current);
    virtual void removeFix(AVLNode<Key,Value>* current, int diff);
    virtual void rotateRight(AVLNode<Key,Value>* current);
    virtual void rotateLeft(AVLNode<Key,Value>* current);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key,Value>* current = (AVLNode<Key,Value>*)this->root_;
    if(current == nullptr){
      current = new AVLNode<Key, Value> (new_item.first, new_item.second, nullptr);
      this->root_ = current;
    }
  
    else{
        while(current != nullptr){
            if(current->getKey() > new_item.first){
              //Insertion at the left
                if(current->getLeft() == nullptr){
                    AVLNode<Key,Value>* inserted = new AVLNode<Key, Value> (new_item.first, new_item.second, current);
                    current->setLeft(inserted);
                    //simple case, no rebalancing needed
                    if(current->getBalance() == 1){ 
                      current->setBalance(0);
                    }
                    //Rebalance
                    else if(current->getBalance() == 0){
                      current->setBalance(-1);
                      insertFix(current, inserted);                    
                      }
                    break;
                }
                //Keep travering left 
                else{
                    current = current->getLeft();
                }
            }
            else if(current->getKey() < new_item.first){
              //Insertion at the right
                if(current->getRight() == nullptr){
                    AVLNode<Key,Value> *temp = new AVLNode<Key,Value>(new_item.first,new_item.second, current);
                    current->setRight(temp);
                    //No rebalancing needed
                    if(current->getBalance() == -1){
                      current->setBalance(0);
                    }
                    //Balancing is needed
                    else if(current->getBalance() == 0){
                      current->setBalance(1);
                      insertFix(current, temp);                    
                      }
                    break;
                }
                //Keep traversing to the right
                else{
                    current = current->getRight();
                }
            }
            else{
                current->setValue(new_item.second);
                break;
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix (AVLNode<Key,Value>*parent, AVLNode<Key,Value>* current)
{
  //Following psuedocode from CSCI104 slides
  if(parent == nullptr || parent->getParent() == nullptr){
    return;
  }

  AVLNode<Key,Value>*grandp = parent->getParent();//grandparent

  //parent is left of grandparent
  if(grandp->getLeft() == parent){
    grandp->updateBalance(-1);
    //Case 1
    if(grandp->getBalance() == 0){return;}
    //Case 2
    else if(grandp->getBalance() == -1){insertFix(grandp,parent);}
    //Case 3
    else if(grandp->getBalance() == -2){
      //zigzig to left 
      if(parent->getLeft() == current ){
        rotateRight(grandp);
        parent->setBalance(0);
        grandp->setBalance(0);
        return;
      }
      //zig zag 
      else if(parent->getLeft() == nullptr && parent->getRight() == current){
        rotateLeft(parent);
        rotateRight(grandp);
        //3a
        if(current->getBalance() == -1){
          parent->setBalance(0);
          grandp->setBalance(1);
          current->setBalance(0);
        }
        //3b
        else if(current->getBalance() == 0){
          parent->setBalance(0);
          grandp->setBalance(0);
          current->setBalance(0);
        }
        //3c
        else if(current->getBalance() == 1){
          parent->setBalance(-1);
          grandp->setBalance(0);
          current->setBalance(0);
        }
        return;
      } 
    }
  }

  //parent is right of grandparent
    else if(grandp->getRight() == parent){
      grandp->updateBalance(1);
      //Case 1
      if(grandp->getBalance() == 0){return;}
      //Case 2
      else if(grandp->getBalance() == 1){insertFix(grandp,parent);}
      //Case 3
      else if(grandp->getBalance() == 2){
        //zigzig to right 
        if(parent->getRight() == current ){
          rotateLeft(grandp);
          parent->setBalance(0);
          grandp->setBalance(0);
        }
        //zig zag 
        else if(parent->getRight() == nullptr && parent->getLeft() == current){
          rotateRight(parent);
          rotateLeft(grandp);
           //3a
          if(current->getBalance() == 1){
          parent->setBalance(0);
          grandp->setBalance(-1);
          current->setBalance(0);
        }
          //3b
          else if(current->getBalance() == 0){
            parent->setBalance(0);
            grandp->setBalance(0);
            current->setBalance(0);
          }
          //3c
          else if(current->getBalance() == -1){
            parent->setBalance(1);
            grandp->setBalance(0);
            current->setBalance(0);
          } 
        } 
      }
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* current){
  AVLNode<Key,Value>*parent = current->getParent();
  AVLNode<Key,Value>* LC = current->getLeft();
  //has a parent 
  if(parent != nullptr){
    if(parent->getLeft() == current){
      parent->setLeft(LC);
    }
    else{
      parent->setRight(LC);
    }
  }
  else{
    this->root_ = LC;
  }
  LC->setParent(parent);
  current->setParent(LC);
  AVLNode<Key,Value>* RC = LC->getRight();
  if(RC != nullptr){
    RC->setParent(current);
  }
  //readjust pointers 
  current->setLeft(RC);
  LC->setRight(current);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* current){
  AVLNode<Key,Value>* parent = current->getParent();
  AVLNode<Key,Value>* RC = current->getRight();
  //has parent
  if(parent != nullptr){
    if(parent->getRight() == current){
      parent->setRight(RC);
    }
    else{
      parent->setLeft(RC);
    }
  }
  else{
    this->root_ = RC;
  }
  RC->setParent(parent);
  current->setParent(RC);
  AVLNode<Key,Value>* LC = RC->getLeft();
  if(LC != nullptr){
    LC->setParent(current);
  }
  //readjust pointers 
  current->setRight(LC);
  RC->setLeft(current);
}

 template<class Key, class Value>
 void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* current, int diff){
   //Following pseudocode from CSCI104 slides
   int nextdiff =0;
   if(current == nullptr){
     return;
   }
   if(current->getParent() == nullptr){
     return;
   }
   AVLNode<Key,Value>* parent = current->getParent();
   //left child +1
   if(parent->getLeft() == current){
     nextdiff +=1;
   }
  //right child, -1
   else if(parent->getRight() == current){
     nextdiff+=-1;
   }
   //assume diff is =-1
   if(diff == -1){
     //Case1
     if((current->getBalance() + diff) == -2){
      //make left child variable 
      AVLNode<Key,Value>* leftC = current->getLeft();
      //1a
      if(leftC->getBalance() == -1){
        rotateRight(current);
        current->setBalance(0);
        leftC->setBalance(0);
      }
      //1b
      else if(leftC->getBalance() == 0){
        rotateRight(current);
        current->setBalance(-1);
        leftC->setBalance(1);
      }
      //1c
      else if(leftC->getBalance() == 1){
        AVLNode <Key,Value>* grandC = leftC->getRight();
        rotateLeft(leftC);
        rotateRight(current);
        if(grandC->getBalance() == 1){
          current->setBalance(0);
          leftC->setBalance(-1);
          grandC->setBalance(0);
        }
        else if(grandC->getBalance() == 0){
          current->setBalance(0);
          leftC->setBalance(0);
          grandC->setBalance(0);
        }
        else if(grandC->getBalance() == -1){
          current->setBalance(1);
          leftC->setBalance(0);
          grandC->setBalance(0);
        }
        removeFix(parent,nextdiff);
      }
     }
     //Case 2
     else if((current->getBalance())+ diff == -1){
       current->setBalance(-1);
     }
     //Case 3
     else if((current->getBalance())+ diff == 0){
       current->setBalance(0);
       removeFix(parent, nextdiff);
     }
   }

    //assume diff is =1
    else if(diff == 1){
      if((current->getBalance()+diff) == 2){
        AVLNode<Key,Value>* rightC = current->getRight();
      //1a
        if(rightC->getBalance() == 1){
          rotateLeft(current);
          current->setBalance(0);
          rightC->setBalance(0);
        }
      //1b
      else if(rightC->getBalance() == 0){
        rotateLeft(current);
        current->setBalance(1);
        rightC->setBalance(-1);
      }
      //1c
      else if(rightC->getBalance() == -1){//
        AVLNode <Key,Value>* grandC = rightC->getLeft();
        rotateRight(rightC);
        rotateLeft(current);
        if(grandC->getBalance() == 1){
          current->setBalance(0);
          rightC->setBalance(-1);
          grandC->setBalance(0);
        }
        else if(grandC->getBalance() == 0){
          current->setBalance(0);
          rightC->setBalance(0);
          grandC->setBalance(0);
        }
        else if(grandC->getBalance() == -1){
          current->setBalance(1);
          rightC->setBalance(0);
          grandC->setBalance(0);
        }
        removeFix(parent,nextdiff);
      }
      }
    }
 }

// /*
//  * Recall: The writeup specifies that if a node has 2 children you
//  * should swap with the predecessor and then remove.
//  */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
     AVLNode<Key, Value>* current = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    //empty tree
    if(current == nullptr){
        return;
    }
    //two children 
    if(current->getLeft() != nullptr && current->getRight()!= nullptr){
      nodeSwap(current, static_cast<AVLNode<Key,Value>*>(this->predecessor(current)));
    }
    
    //leaf node 
    if(current->getLeft() == nullptr && current->getRight() == nullptr){
      //leaf child is the root!
      if(current->getParent() == nullptr){
        this->root_ = nullptr;
        delete current;
        return;
      }
      //left lead node
      else if(current->getParent()->getLeft() == current){
        current->getParent()->setLeft(nullptr);
      }
      //right leaf node
      else if(current->getParent()->getRight() == current){
        current->getParent()->setRight(nullptr);
      }
      delete current;
      return;
    }

    //if current has one right lead child 
    else if(current->getRight() != nullptr && current->getLeft() == nullptr){
      AVLNode <Key, Value>* rightC = current->getRight();
      AVLNode <Key, Value>* parent = current->getParent();

      //casse that it is the root 
      if(current == this->root_){
        current->setRight(nullptr);
        rightC->setParent(nullptr);
        this->root_ = rightC;
        delete current;
        return;
      }
      //it is a left child of a parent 
      else if(current->getParent()->getLeft() == current){
        parent->setLeft(rightC);
        rightC->setParent(parent);
        current->setParent(nullptr);
        current->setRight(nullptr);
        delete current;
        return;
      }
      //node is right child of a parent
      else if(current->getParent()->getRight() == current){
        parent->setRight(rightC);
        rightC->setParent(parent);
        current->setParent(nullptr);
        current->setLeft(nullptr);
        delete current;
        return;
      }
    }

// current has one left child 
  else if(current->getRight() == nullptr && current->getLeft() != nullptr){
    AVLNode <Key, Value>* leftC = current->getLeft();
    AVLNode <Key, Value>* parent = current->getParent();
    //root to be removed has a child 
    if(current == this->root_){
      current->setLeft(nullptr);
      leftC->setParent(nullptr);
      this->root_ = leftC;
      delete current;
      return;
      }
      //is left of its parent 
      else if(current->getParent()->getLeft() == current){
        parent->setLeft(leftC);
        leftC->setParent(parent);
        current->setParent(nullptr);
        current->setLeft(nullptr);
        delete current;
        return;
      }
      //is right of its parent 
      else if(current->getParent()->getRight() == current){
        parent->setRight(leftC);
        leftC->setParent(parent);
        current->setParent(nullptr);
        current->setRight(nullptr);
        delete current;
        return;
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
