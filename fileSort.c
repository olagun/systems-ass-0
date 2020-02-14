#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO:
// - So we're supposed to write quicksort for linked lists?
// - Sorting comparators and testing.
// - Wrap `malloc` with error handling.
// - Add fatal errors.
// - Flags, etc.

// 🌍 Globals
int file_desc;

// 🦾 Typedefs & Prototypes
typedef int (*Comparator)(void*, void*);
typedef struct Node Node;
typedef union Data Data;

typedef struct {
  int is_str;
  Node* ll;
} ReadOut;

Node* insertion_sort(void*, Comparator);
Node* quicksort(void*, Comparator);
void deallocate();

// 👥 Comparators
int int_compare(void* i1, void* i2) { return (*(int*)i1) - *((int*)i2); }

int string_compare(void* s1, void* s2) {
  if (s1 == NULL && s2 == NULL) {
    return 0;
  }

  if (s1 == NULL && s2 != NULL) {
    return *(char*)s2;
  }

  if (s1 != NULL && s2 == NULL) {
    return *(char*)s1;
  }

  while (*(char*)s1 && *(char*)s2) {
    int diff = *(char*)s1 - *(char*)s2;
    if (diff != 0) {
      return diff;
    }
    s1++;
    s2++;
  }

  if (*(char*)s1 && *(char*)s2) {
    return 0;
  }

  return *(char*)s1 ? *(char*)s1 : *(char*)s2;
}

// 🧶 String helper functions
char* concat(char* s1, char* s2) {
  if (s1 != NULL && s2 != NULL) {
    int total_size = strlen(s1) + strlen(s2) + 1;
    char* str = malloc(sizeof(char) * total_size);
    strcpy(str, s1);
    strcat(str, s2);
    return str;
  }

  if (s1 != NULL) {
    return s1;
  }

  if (s2 != NULL) {
    return s2;
  }

  return NULL;
}

int equal(char* s1, char* s2) { return string_compare(s1, s2) == 0; }

// 🔥 Error helper functions
void fatalError(char* err_str) {
  fprintf(stderr, "😔 [Fatal Error]: %s\n", err_str);
  deallocate();
  exit(0);
}

void error(char* err_str) { printf("%s", concat("😟 [Error] ", err_str)); }

void warning(char* warn_str) { printf("%s", concat("😕 [Warning] ", warn_str)); }

// 📔 Linked List declarations
union Data {
  int int_data;
  char* str_data;
  char char_data;
  Node* list;
};

struct Node {
  Data data;
  struct Node* next;
};

// 🧩 Sorting implementations
// TODO: They all must be in-place?
Node* insertion_sort(void* list, Comparator cmpr) {
  Node* curr_node = (Node*)list;
  Node* prev_node = NULL;

  while (curr_node != NULL) {
    Node* ptr = list;
    Node* prev = NULL;

    while (ptr != curr_node) {
      if (cmpr(curr_node, ptr) < 0) {
        prev_node->next = curr_node->next;

        if (prev != NULL) {
          curr_node->next = ptr;
          prev->next = curr_node;
        } else {
          curr_node->next = list;
          list = curr_node;
        }

        curr_node = prev_node->next;
        break;
      }

      prev = ptr;
      ptr = ptr->next;
    }

    prev_node = curr_node;
    curr_node = curr_node->next;
  }

  return list;
}

// // TODO: Remove "appending" etc. Since it is a void function,
// // you must physically move the data.
// // Update: You either need a double pointer or a Node* in the
// // return signature
// void quicksort(void* list, Comparator cmpr) {
//   // 1. Partition
//   Node* pivot = (Node*) list;
//   Node* rest = pivot->next;

//   Node* lte = NULL;
//   Node* gt = NULL;

//   while (rest != null) {
//     Node*next = rest->next;
//     rest->next = NULL;

//     if (cmpr(rest, pivot) <= 0) {
//       lte = append(lte, rest);
//     }  else {
//       gt = append(gt, rest);
//     }

//     rest = rest->next;
//   }

//   // lte
//   quicksort(lte, cmpr);

//   // gt
//   quicksort(gt, cmpr);
// }

// 🔧 Linked List utilities
Node* create_str_node(char* str_data) {
  Node* node = malloc(sizeof(struct Node));
  node->data = (Data)str_data;
  node->next = NULL;
  return node;
};

Node* create_char_node(int char_data) {
  Node* node = malloc(sizeof(struct Node));
  node->data = (Data)char_data;
  node->next = NULL;
  return node;
};

Node* create_int_node(int int_data) {
  Node* node = malloc(sizeof(struct Node));
  node->data = (Data)int_data;
  node->next = NULL;
  return node;
};

Node* append_list(Node* head, Node* node) {
  if (head == NULL) return node;
  head->next = append_list(head->next, node);
  return head;
}

int get_list_size(Node* head) {
  int count = 0;
  while (head != NULL) {
    count++;
    head = head->next;
  }
  return count;
}

void print_list(Node* head) {
  while (head != NULL) {
    printf("%s\n", head->data.str_data);
    head = head->next;
  }
}

// 🛹 Convenience functions
void deallocate() { close(file_desc); }

int is_whitespace(char c) { return c == '\n' || c == ' ' || c == '\t'; }

int is_separator(char c) { return c == ','; }

char* str_from_list(Node* head) {
  int size = get_list_size(head);
  char* str = malloc(sizeof(char) * size);

  int curr = 0;
  while (head != NULL) {
    str[curr] = head->data.char_data;
    head = head->next;
    curr++;
  }

  return str;
}

ReadOut* read_file_to_list(int file_descriptor) {
  int total_bytes_read = 0;

  Node* list_of_items = NULL;
  Node* item_tmp_chars = NULL;

  int bytes_read = 0;
  int is_str = 0;
  do {
    char* token = malloc(sizeof(char));
    bytes_read = read(file_descriptor, token, 1);

    if (bytes_read > 0 && !is_whitespace(*token)) {
      if (isdigit(*token)) {
        item_tmp_chars =
            append_list(item_tmp_chars, create_int_node(atoi(token)));
        is_str = 0;
      }

      if (isalpha(*token)) {
        item_tmp_chars = append_list(item_tmp_chars, create_char_node(*token));
        is_str = 1;
      }

      if (is_separator(*token)) {
        // TODO: Check length and don't add if length is 0.
        char* str = str_from_list(item_tmp_chars);
        // printf("%s\n", str);
        list_of_items = append_list(list_of_items, create_str_node(str));
        item_tmp_chars = NULL;
      }

      total_bytes_read += bytes_read;
    }
  } while (bytes_read > 0);

  char* str = str_from_list(item_tmp_chars);
  list_of_items = append_list(list_of_items, create_str_node(str));
  item_tmp_chars = NULL;
  // printf("%s\n", str);

  ReadOut* ro = malloc(sizeof(ReadOut));
  ro->ll = list_of_items;
  ro->is_str = is_str;
  return ro;
}

int main(int argc, char* argv[]) {
  char* sort_flag = argv[1];

  if (!equal(sort_flag, "-q") && !equal(sort_flag, "-i")) {
    fatalError("Sort flag is missing. Try adding the flags `-q` or `-i`.");
  }

  char* file_name = argv[2];

  file_desc = open(file_name, O_RDONLY | O_CREAT);

  if (file_desc < 0) {
    fatalError(
        concat(concat("Please enter a file name after `", sort_flag), "`."));
  }

  ReadOut* ro = read_file_to_list(file_desc);

  Node* list = ro->ll;
  int is_str = 1;

  // TODO: Determine if string or int.

  Comparator fn = is_str ? string_compare : int_compare;

  printf("%d\n", string_compare("real", "f"));

  // if (equal(sort_flag, "-q")) {
  //   quicksort(list, fn);
  // }

  // if (equal(sort_flag, "-i")) {
  list = insertion_sort(list, fn);
  // }

  print_list(list);

  return 0;
}