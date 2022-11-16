#include "equal-paths.h"
#include <iostream>

using namespace std;


// You may add any prototypes of helper functions here
int helperF(Node *root);
//Helper 
int helperF(Node * root)
{
 if(root == nullptr)
 {
   return 0;
   }
	int left  = helperF(root->left);
	int right = helperF(root->right);
	
  return 1+ std::max(left,right);
}

bool equalPaths(Node *root) {
	// TODO: use your helper function (calculateHeightIfBalanced) well!
  if (root == NULL)
        {return 1;}
 if(((root->right) == NULL) && ((root->left)!= NULL)){
	  return equalPaths(root->left);
  } 
  if(((root->left) == NULL) && ((root->right)!= NULL)){
	  return equalPaths(root->right);
  }

  int left  = helperF(root->left);
	int right = helperF(root->right);

    // if (abs(left - right) <= 1 && equalPaths(root->left)
    //      && equalPaths(root->right)){ return 1;}
        
  if(left!=right){
		return false;
	}
	return true;
  
}