#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  nil_node->color = RBTREE_BLACK;
  t->nil = nil_node;
  nil_node->left = nil_node->right = nil_node->parent = nil_node;
  t->root = t->nil;
  return t;
}

void right_rotate(rbtree *t, node_t *cur){
  node_t * left_child = cur->left;
  cur->left = left_child->right; // left_child 노드에 right 값이 있다면 연결 해주는 코드
  if(left_child->right != t->nil) left_child->right->parent = cur; //만약 값이 있다면 자식의 노드를 부모가 누군지 알수있게 연결

  left_child->parent = cur->parent;

  if(cur->parent == t->nil){ // cur이 만약 루트 노드이면 left_child 를 루트노드로 만들면 됨
    t->root = left_child;
  }
  else if (cur == cur->parent->left){ //cur 의 부모 노드가 있다면 cur이 부모의 어느 쪽에 연결되어 있는지 확인하고 부모노드와 left_child 노드를 연결
    cur->parent->left = left_child;
  }
  else {
    cur->parent->right = left_child;
  }

  left_child->right = cur; // 회전후 left_child와 cur을 연결
  cur -> parent = left_child; // cur의 부모노드가 left_child

}

void left_rotate(rbtree *t, node_t *cur){
  node_t* right_child = cur->right;
  cur -> right = right_child->left;
  if(right_child->left != t->nil) right_child->left->parent = cur;

  right_child->parent = cur -> parent;

  if(cur -> parent == t->nil){
    t->root = right_child;
  }
  else if (cur == cur->parent->right){
    cur->parent->right = right_child;
  }
  else{
    cur->parent->left = right_child;
  }
  right_child->left = cur;
  cur->parent = right_child;

}


static node_t *subtree_min(const rbtree *t, node_t *x) {
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

void RB_Delete_Fixup(rbtree *t, node_t *x){
  node_t *w;
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left){ //왼쪽 자식일때
      w = x->parent->right; // w는 x의 형재
      if (w -> color == RBTREE_RED){ //case 1 형제가 red 일때
        w->color = RBTREE_BLACK;
        x -> parent-> color = RBTREE_RED;
        left_rotate(t,x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ //case 2 형재가 모두 블랙일때
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->right->color == RBTREE_BLACK){ //case 3 가까운 자식이 red, 먼 자식이 black
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t,w);
          w = x->parent->right;
        }
        w->color = x->parent->color; // case 4 가까운 자식이 black , 먼 자식이 red
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t,x->parent);
        x = t->root;
      }
    }
    else { //오른쪽 자식일때
      w = x->parent->left; //형제
      if (w -> color == RBTREE_RED){ // 형제가 red 일때
        w->color = RBTREE_BLACK;
        x -> parent-> color = RBTREE_RED;
        right_rotate(t,x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){ //형재의 두 자식이 black일때
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->left->color == RBTREE_BLACK){ //가까운 자식이 black, 먼 자식이 red
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t,w);
          w = x->parent->left;
        }
        w->color = x->parent->color; //case 4
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}



void RB_Transplant(rbtree* t, node_t* u, node_t* v){
  
  if (u->parent == t->nil){
    //u가 루트면, v를 새 루트로
    t->root = v;
  }
  else if(u == u->parent->left){ 
    //u가 부모의 왼쪽 자식일 때
    u->parent->left = v;
  }
  else{
    //u가 부모의 오른쪽 자식일 때
    u->parent->right = v;
  }
  //v가 이 자리에 연결되었으니, v의 부모 포인터 갱신
  v->parent = u->parent;
}

void insert_up(rbtree* t, node_t* cur ){
  node_t * uncle;
  node_t *par = cur->parent;
  node_t *g_par = par->parent;

  if (cur == t->root){ // 삽입되는 노드가 root 노드 이면 블랙으로 만들고 삽입
    cur -> color = RBTREE_BLACK;
    return;
  }

  if(par -> color == RBTREE_RED){ // 부모의 색깔이 레드일 경우
    uncle = (par == g_par->left) ? g_par->right : g_par->left; // 삼촌의 위치 확인
    if(uncle->color == RBTREE_RED){ // 삼촌의 색깔이 부모랑 같다면(삼촌의 색깔이 레드) 색 바꾸고 조부모를 확인
      uncle -> color = RBTREE_BLACK;
      par -> color = RBTREE_BLACK;
      g_par -> color = RBTREE_RED;
      insert_up(t, g_par);
      return;
    }
    else{ // 삼촌의 색깔이 부모랑 다르다면 삼촌 == black 이라면
      if(par == g_par->left){ // LL 의 경우 
        if(cur == par->left){
          par->color = RBTREE_BLACK; // 부모와 조부모의 색을 바꾼뒤 로테이션
          g_par->color = RBTREE_RED;
          right_rotate(t, g_par);
        }
        else{ // LR의 경우 펴준뒤 바꾼다
          g_par->color = RBTREE_RED;
          cur -> color = RBTREE_BLACK;
          left_rotate(t,par);
          right_rotate(t, g_par);
        }
      }
      else{ 
        if(cur == par->right){ // RR의 경우
          par->color = RBTREE_BLACK;
          g_par -> color = RBTREE_RED;
          left_rotate(t, g_par);
        }
        else{ //RL 의 경우
          g_par->color = RBTREE_RED;
          cur->color = RBTREE_BLACK;
          right_rotate(t, par);
          left_rotate(t, g_par);
        }
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

void free_node(rbtree *t, node_t *del){ 
  if(del == t->nil){
    return;
  }
  free_node(t, del->left);
  free_node(t, del->right);
  free(del);
}

void delete_rbtree(rbtree *t) { //재귀적으로 삭제함
  // TODO: reclaim the tree nodes's memory
  free_node(t, t->root);
  free(t->nil);
  free(t);
}


node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *new_node = malloc(sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node -> left = new_node->right = t->nil;
  new_node -> parent = t->nil;
  
  if(t->root == t->nil){ // 트리가 비어있을때
    t->root = new_node;
    new_node -> color = RBTREE_BLACK;
    return t->root;
  }
  else{
    node_t* cur = t->root;
    node_t *par = NULL;
    while (cur != t->nil)
    {
      if(key >= cur->key){
        par = cur;
        cur = cur->right;
      }
      else{
        par = cur;
        cur = cur->left;
      }
    }
    if(par->key < key){
      par -> right = new_node;
      new_node->parent = par;
    }
    else{
      par -> left = new_node;
      new_node -> parent = par;
    }
    if(par->color == RBTREE_BLACK){
      return new_node;
    }
    else{
      insert_up(t, new_node);
      t->root->color =RBTREE_BLACK;
    }
  }

  return new_node;
}

node_t* rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;
  while (cur != t->nil)
  {
    if (cur ->key == key){
      return cur;
    }
    else if(cur->key < key){
      cur = cur -> right;
    }
    else{
      cur = cur -> left;
    }
  }


  return NULL;
}


node_t *rbtree_min(const rbtree *t) { // left로 쭉 내려가면 됨 nil이 나올때까지
  // TODO: implement find
  node_t *cur = t->root;
  while (cur->left != t->nil)
  {
    cur = cur->left;
  }
  
  return cur;
}

node_t *rbtree_max(const rbtree *t) { // right로 쭉 내려간다.
  // TODO: implement find
  node_t *cur = t->root;
  while (cur->right != t->nil)
  {
    cur = cur->right;
  }
  return cur;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  // p : 삭제 대상 노드, y : 실제로 트리에서 꺼내질 노드, x : y 빠진 위치에 대체될 노드
  node_t* y = p;
  node_t* x;
  int y_original_color = y->color; // 색 저장해 블랙노드 삭제 여부 판단
  if (p->left == t->nil){
    // ---case3 : 왼쪽 자식이 없을 때---
    x = p->right;
    RB_Transplant(t,p,p->right); // p를 오른쪽 자식으로 바꾼다
  }
  else if (p->right == t->nil){
    // ---case2 : 오른쪽 자식이 없을 때 ---
    x = p->left;
    RB_Transplant(t,p,p->left); // p를 왼쪽 자식으로 바꾼다
  }
  else{
    // ---case 3 : 두 자식이 모두 있을 때---
    y = subtree_min(t, p->right);
    y_original_color = y->color;
    x = y->right;

    if (y != p->right){
      RB_Transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    else{
      
        x->parent = y;
      
    }

    RB_Transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  // 삭제된 노드가 블랙이면 보정 진행
  if (y_original_color == RBTREE_BLACK){
    RB_Delete_Fixup(t,x);
  }

  //메모리 해제
  free(p);
  return 0;
}



void to_array(const rbtree* t, key_t *arr, const size_t n,  node_t* cur, int* count){
  if(*count >= n) return;
  if(cur == t->nil) return;

  to_array(t, arr, n, cur->left, count);
  if(*count < n){
    arr[*count] = cur->key;
    *count += 1;
  } 
  to_array(t, arr, n, cur->right, count);

}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int count = 0;
  to_array(t, arr, n, t->root, &count);

  return 0;
}


