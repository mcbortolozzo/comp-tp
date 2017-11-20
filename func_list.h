#include "hash.h"


typedef struct func_decl {
  hash_node_t *id;
  hash_node_t *params;
  struct func_decl *next;
} func_decl_t;

typedef struct func_list {
  func_decl_t *head;
  func_decl_t *tail;
} func_list_t;

func_decl_t* create_func_decl(hash_node_t *id);
func_list_t* create_func_list();
func_list_t* insert_decl(func_list_t *func_list, func_decl_t *func_decl);
hash_node_t* insert_param(func_decl_t *func_decl, hash_node_t *param);
hash_node_t* find_param(func_decl_t *func_declm, char* text);
func_decl_t* find_decl(func_list_t *func_list, hash_node_t *id);
int compare_param(hash_node_t* p1, hash_node_t *p2);
