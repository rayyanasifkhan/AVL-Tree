/*avl.h*/
// Author: Rayyan Khan
// This is the file containing the class of AVL trees. 

#pragma once

#include <iostream>
#include <algorithm>  // std::max
#include <cstdlib>    // std::abs
#include <stack>
#include <vector>
#include <cassert>

using namespace std;

template<typename TKey, typename TValue>
class avltree
{
private:
  struct NODE
  {
    TKey   Key;
    TValue Value;
    int    Height;
    NODE*  Left;
    NODE*  Right;
  };

  NODE* Root;  // pointer to root node of tree (nullptr if empty)
  int   Size;  // # of nodes in the tree (0 if empty)

  // 
  // _inorder does the actual inorder traversal and output 
  // to console.  Each key is output to the console followed
  // by " ", including the last key.
  //
  void _inorder(NODE* cur)
  {
    if (cur == nullptr)
      return;
    else
    {
      _inorder(cur->Left);
      cout << "(" << cur->Key << "," << cur->Value << "," << cur->Height << ")" << " ";
      _inorder(cur->Right);
    }
  }

  void _inorder_keys(NODE* cur, std::vector<TKey>& V)
  {
    if (cur == nullptr)
      return;
    else
    {
      _inorder_keys(cur->Left, V);
      V.push_back(cur->Key);
      _inorder_keys(cur->Right, V);
    }
  }

  void _inorder_values(NODE* cur, std::vector<TValue>& V)
  {
    if (cur == nullptr)
      return;
    else
    {
      _inorder_values(cur->Left, V);
      V.push_back(cur->Value);
      _inorder_values(cur->Right, V);
    }
  }

  void _inorder_heights(NODE* cur, std::vector<int>& V)
  {
    if (cur == nullptr)
      return;
    else
    {
      _inorder_heights(cur->Left, V);
      V.push_back(cur->Height);
      _inorder_heights(cur->Right, V);
    }
  }

  //
  // _copytree:
  //
  // Given a tree, inserts the (key, value) pairs into *this* tree.
  //
  void _copytree(NODE* cur)
  {
    if (cur == nullptr)
      return;
    else {
      //
      // to copy and build an exact copy, we need to "visit" cur
      // first and insert that (key, value) pair first.  That way
      // all the nodes to the left will properly go to the left,
      // all the node to the right will properly go to the right.
      //
      insert(cur->Key, cur->Value);

      _copytree(cur->Left);
      _copytree(cur->Right);
    }
  }

  //
  // _RightRotate
  // _LeftRotate
  //
  // Rotates the tree around the node N, where Parent is N's parent.  Note that
  // Parent could be null, which means N is the root of the entire tree.  If 
  // Parent denotes a node, note that N could be to the left of Parent, or to
  // the right.  You'll need to take all this into account when linking in the
  // new root after the rotation.  Don't forget to update the heights as well.
  //
  void _RightRotate(NODE* Parent, NODE* N)
  {
   assert(N != nullptr);  // pre-conditions: N and L must exist
   assert(N->Left != nullptr);
    
   NODE* L = N->Left;
   NODE* B = L->Right;
   int HL, HR;

   if (Parent != nullptr) {
      if (Parent->Left == N) 
         Parent->Left = L;
      else 
         Parent->Right = L;
	 }
   
   else 
      Root = L;

   L->Right = N;
   N->Left = B;

   HL = (B == nullptr) ? -1 : B->Height;
   HR = (N->Right== nullptr) ? -1 : N->Right->Height;
   N->Height = 1 + max(HL, HR);

   HR = N->Height;
   HL = (L->Left == nullptr) ? -1 : L->Left->Height;
   L->Height = 1 + max(HL, HR);
  }

  void _LeftRotate(NODE* Parent, NODE* N)
  {
   assert(N != nullptr);  // pre-conditions: N and R must exist
   assert(N->Right != nullptr);
    
   NODE* R = N->Right;
   NODE* B = R->Left;
   int HL, HR;
   
   if (Parent != nullptr) {
      if (Parent->Left == N)
         Parent->Left = R;
      else 
         Parent->Right = R;
   }
   
   else 
      Root = R;

   R->Left = N;
   N->Right = B;
   
   HR = (B == nullptr) ? -1 : B->Height;
   HL = (N->Left == nullptr) ? -1 : N->Left->Height;
   N->Height = 1 + max(HL, HR);
   
   HL = N->Height;
   HR = (R->Right == nullptr) ? -1 : R->Right->Height;
   R->Height = 1 + max(HL, HR);
  }
	
	// decides which case of rotation is required and calls the rotate functions accordingly
	void _RotateToFix(NODE* N, stack<NODE*> nodes, int hLofN, int hRofN) 
	{
	
		int hL, hR;
		NODE* Parent = (!nodes.empty()) ? nodes.top() : nullptr;

		if (hLofN > hRofN) { // leans to the left

			hL = (N->Left->Left == nullptr) ? -1 : N->Left->Left->Height;
			hR = (N->Left->Right == nullptr) ? -1 : N->Left->Right->Height;

			if (hL > hR) { // case 1
				_RightRotate(Parent, N);
			}

			else { // case 2
				_LeftRotate(N, N->Left);
				_RightRotate(Parent, N);
			}
		}

		else { // leans to the right

			hL = (N->Right->Left == nullptr) ? -1 : N->Right->Left->Height;
			hR = (N->Right->Right == nullptr) ? -1 : N->Right->Right->Height;

			if (hR > hL) { // case 3
				_LeftRotate(Parent, N);
			}

			else { // case 4
				_RightRotate(N, N->Right);
				_LeftRotate(Parent, N);
			}
		}
	}
	
	// destructor helper function
	void _Destructor(NODE* Root)
	{
		NODE* cur = Root;
		if (cur == nullptr) {
			return;
		}
		_Destructor(cur->Left);
		_Destructor(cur->Right);
		delete cur;
	}
	
	// distance helper function
	int _Distance(TKey k1, TKey k2, NODE* Root) 
	{
		NODE* cur = Root;
		TKey small = (k1 > k2) ? k2 : k1;
		TKey big = (small == k2) ? k1 : k2; 

		if (_Search(cur, small) == -1 || _Search(cur, big) == -1) { // if one of the keys DNE
			return -1;
		}
		
		else if (k1 == k2) { // if the keys are equal
			return 0;
		}

		if (small < cur->Key && big > cur->Key) { // if the keys are on either sides of 'cur'
			int d1 = _Search(cur, small);
			int d2 = _Search(cur, big);
			return  d1 + d2;
		}
		
		else if (small < cur->Key && big < cur->Key) { // if the keys are both left of 'cur'
			_Distance(small, big, cur->Left);
		}

		else if (small > cur->Key && big > cur->Key) { // if the keys are both right of 'cur'
			_Distance(small, big, cur->Right);
		}

		else { // if 'cur' contains one of the keys 
			if (small == cur->Key)
				return _Search(cur, big);
			else
				return _Search(cur, small);
		}
	}
	
	// returns the distance of key from cur; a helper function for distance
	int _Search(NODE* cur, TKey key) 
	{
		int distance = 0;

		while (cur != nullptr) {
			if (key == cur->Key) {
				return distance;
			}
			else if (key < cur->Key) {
				distance++;
				cur = cur->Left;
			}
			else {
				distance++;
				cur = cur->Right;
			}
		}
		return -1;
	}

public:
  //
  // default constructor:
  //
  // Creates an empty tree.
  //
  avltree()
  {
    Root = nullptr;
    Size = 0;
  }

  //
  // copy constructor:
  //
  avltree(avltree& other)
  {
    Root = nullptr;
    Size = 0;

    _copytree(other.Root);
  }

  // 
  // size:
  //
  // Returns the # of nodes in the tree, 0 if empty.
  //
  int size()
  {
    return Size;
  }

  //
  // height
  //
  // Returns the height of tree; the height of an empty tree is
  // defined as -1.
  //
  int height()
  {
    if (Root == nullptr)
      return -1;
    else
      return Root->Height;
  }

  // 
  // search:
  //
  // Searches the tree for the given key, returning a pointer to the 
  // value if found and nullptr if not.  We return a pointer so the caller
  // may update the underlying value if needed.
  //
  TValue* search(TKey key)
  {
    NODE* cur = Root;
    while (cur != nullptr)
    {
      if (key == cur->Key)  // already in tree
      {
        return &cur->Value;
      }

      if (key < cur->Key)  // search left:
      {
        cur = cur->Left;
      }
      else
      {
        cur = cur->Right;
      }
    }//while  

    // if get here, not found
    return nullptr;
  }

  //
  // insert
  //
  // Inserts the given (key, value) pair into the tree; if the key has 
  // already been inserted then the function returns without changing 
  // the tree.
  //
  void insert(TKey key, TValue value)
  {
    NODE* prev = nullptr;
    NODE* cur = Root;

    //
    // stack the nodes we visit so we can walk back up
    // the search path later, adjusting heights:
    //
    stack<NODE*> nodes;

    //
    // 1. Search to see if tree already contains key:
    //
    while (cur != nullptr)
    {
      if (key == cur->Key)  // already in tree
        return;

      nodes.push(cur);  // stack so we can return later:

      if (key < cur->Key)  // search left:
      {
        prev = cur;
        cur = cur->Left;
      }
      else
      {
        prev = cur;
        cur = cur->Right;
      }
    }//while

    //
    // 2. if we get here, key is not in tree, so allocate
    // a new node to insert:
    // 
    NODE* newNode;
    newNode = new NODE();
    newNode->Key = key;
    newNode->Value = value;
    newNode->Height = 0;  // leaf node -> sub-tree of height 0:
    newNode->Left = nullptr;
    newNode->Right = nullptr;

    //
    // 3. link in the new node:
    //
    // NOTE: cur is null, and prev denotes node where
    // we fell out of the tree.  if prev is null, then
    // the tree is empty and the Root pointer needs 
    // to be updated.
    //
    if (prev == nullptr)
      Root = newNode;
    else if (key < prev->Key)
      prev->Left = newNode;
    else
      prev->Right = newNode;

    // 
    // 4. update size:
    //
    Size++;

    //
    // 5. walk back up tree using stack and update heights.
    //
    while (!nodes.empty())
    {
      cur = nodes.top();
      nodes.pop();

      int hL = (cur->Left == nullptr) ? -1 : cur->Left->Height;
      int hR = (cur->Right == nullptr) ? -1 : cur->Right->Height;
			
			// AVL Tree condition check; if it's not met, rotation(s) are needed
      if (abs(hL - hR) > 1) {
      	_RotateToFix(cur, nodes, hL, hR);
				continue; 
			}
			
			int hCur = 1 + std::max(hL, hR);
			if (cur->Height == hCur)  // didn't change, so no need to go further:
				break;
			else  // height changed, update and keep going:
				cur->Height = hCur;
			
 
    }//while

    //
    // done!
    //
    return;
  }
	
	// clears the tree; ready for next use
	void clear() 
	{
		_Destructor(Root);
		Root = nullptr;
		Size = 0;
	}
	
	// destructor
	virtual ~avltree() 
	{
		_Destructor(Root);
	}
	
	// returns the shortest distance between nodes with keys k1 and k2
	int distance(TKey k1, TKey k2) {
		return _Distance(k1, k2, Root);
	}

  //
  // inorder:
  //
  // Performs an inorder traversal of the tree, outputting
  // the keys to the console.
  //
  void inorder()
  {
    cout << "Inorder: ";

    _inorder(Root);

    cout << endl;
  }

  std::vector<TKey> inorder_keys()
  {
    std::vector<TKey>  V;

    _inorder_keys(Root, V);

    return V;
  }

  std::vector<TValue> inorder_values()
  {
    std::vector<TValue>  V;

    _inorder_values(Root, V);

    return V;
  }

  std::vector<int> inorder_heights()
  {
    std::vector<int>  V;

    _inorder_heights(Root, V);

    return V;
  }
};
