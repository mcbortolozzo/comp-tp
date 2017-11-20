#include "func_list.h"

func_decl_t* create_func_decl(hash_node_t *id)
{
  func_decl_t *new_func = calloc(1, sizeof(func_decl_t));
  new_func->id = id;
  return new_func;
}

func_list_t* create_func_list()
{
  func_list_t *list = calloc(1, sizeof(func_list_t));
  list->head = 0;
  list->tail = 0;
  return list;
}

func_list_t* insert_decl(func_list_t *func_list, func_decl_t *func_decl)
{
  if(!func_list->tail)
  {
    func_list->head = func_decl;
    func_list->tail = func_decl;
  }
  else
  {
    func_list->tail->next = func_decl;
    func_list->tail = func_decl;
  }
}

hash_node_t* insert_param(func_decl_t *func_decl, hash_node_t *param)
{
  hash_node_t *p = calloc(1, sizeof(hash_node_t));
  p->text = param->text;
  p->type = param->type;
  hash_node_t *curr = func_decl->params;
  hash_node_t *prev;
  if(!curr)
  {
    func_decl->params = p;
    return p;
  }

  while(curr)
  {
    prev = curr;
    curr = curr->next;
  }

  prev->next = p;
  return p;
}

hash_node_t* find_param(func_decl_t *func_decl, char* text)
{
    hash_node_t *curr = func_decl->params;
    while(curr)
    {
      if(hashString(curr->text) == hashString(text))
        return curr;
      curr = curr->next;
    }
    return 0;
}

func_decl_t* find_decl(func_list_t *func_list, hash_node_t *id)
{
  func_decl_t *curr = func_list->head;
  while(curr)
  {
    if(curr->id == id)
      return curr;
    curr = curr->next;
  }
  return 0;
}
