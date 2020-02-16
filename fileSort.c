#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// TODO:
// - Testing.
// - Wrap `malloc` with error handling.

// 🌍 Globals
int file_desc;

// 🦾 Typedefs & Prototypes
typedef int (*Comparator)(void*, void*);
typedef struct Node Node;
typedef union Data Data;

void insertion_sort(void*, Comparator);
void quicksort(void*, Comparator);
void deallocate();
Node* append_list(Node*, Node*);
void print_list(Node*, int);

// 📔 Linked List declarations
union Data {
  int int_data;
  char* str_data;
  char char_data;
};

struct Node {
  Data data;
  struct Node* next;
};

// 👥 Comparators
int int_compare_node(void* s1, void* s2) {
  return ((Node*)s1)->data.int_data - ((Node*)s2)->data.int_data;
}

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

int string_compare_node(void* s1, void* s2) {
  return string_compare(((Node*)s1)->data.str_data, ((Node*)s2)->data.str_data);
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

void error(char* err_str) { printf("%s", concat("😟 [Error]: ", err_str)); }

void warning(char* warn_str) {
  printf("%s", concat("😕 [Warning]: ", warn_str));
}

// 🧩 Sorting implementations
void insertion_sort(void* out, Comparator cmpr) {
  Node** _out = (Node**)out;
  Node* list = *_out;

  Node* curr_node = (Node*)list;
  Node* prev_node = NULL;

  while (curr_node != NULL) {
    Node* ptr = list;
    Node* prev = NULL;

    while (ptr != curr_node) {
      if (cmpr(curr_node, ptr) <= 0) {
        prev_node->next = curr_node->next;

        if (prev != NULL) {
          curr_node->next = ptr;
          prev->next = curr_node;
        } else {
          curr_node->next = list;
          list = curr_node;
        }

        curr_node = prev_node;
        break;
      }

      prev = ptr;
      ptr = ptr->next;
    }

    prev_node = curr_node;
    curr_node = curr_node->next;
  }

  *_out = list;
}

void quicksort(void* out, Comparator cmpr) {
  Node** _out = (Node**)out;
  Node* list = *_out;

  if (list == NULL || list->next == NULL) {
    return;
  }

  // 1. Divide
  Node* pivot = (Node*)list;
  Node* rest = pivot->next;

  pivot->next = NULL;
  Node* lte = NULL;
  Node* gt = NULL;

  while (rest != NULL) {
    Node* next = rest->next;
    rest->next = NULL;

    if (cmpr(rest, pivot) <= 0) {
      lte = append_list(lte, rest);
    } else {
      gt = append_list(gt, rest);
    }

    rest = next;
  }

  // 2. Conquer
  quicksort(&lte, cmpr);
  quicksort(&gt, cmpr);

  *_out = append_list(lte, append_list(pivot, gt));

  // 3. Combine
  // Nothing
}

// 🔧 Linked List utilities
Node* create_str_node(char* str_data) {
  Node* node = malloc(sizeof(struct Node));
  node->data = (Data)str_data;
  node->next = NULL;
  return node;
};

Node* create_char_node(int char_data) {
  Node* node = malloc(sizeof(struct Node));
  node->data.char_data = char_data;
  node->next = NULL;
  return node;
};

Node* create_int_node(int int_data) {
  Node* node = malloc(sizeof(struct Node));
  node->data.int_data = int_data;
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

void print_list(Node* head, int is_str) {
  while (head != NULL) {
    if (is_str) {
      printf("%s", head->data.str_data);
    } else {
      printf("%d", head->data.int_data);
    }

    head = head->next;
    if (head != NULL) {
      printf(" => ");
    }
  }
  printf("\n");
}

// 🛹 Convenience functions
void deallocate() { close(file_desc); }

int is_whitespace(char c) { return c == '\n' || c == ' ' || c == '\t'; }

int is_separator(char c) { return c == ','; }

char* str_from_list(Node* head) {
  int size = get_list_size(head);
  char* str = malloc(sizeof(char) * size + 1);

  int curr = 0;
  while (head != NULL) {
    str[curr] = head->data.char_data;
    head = head->next;
    curr++;
  }

  return str;
}

typedef struct {
  int is_str;
  Node* list;
} ReadFileOutput;

ReadFileOutput* read_file_to_list(int file_descriptor) {
  int total_bytes_read = 0;

  Node* list_of_items = NULL;
  Node* item_tmp_chars = NULL;

  int is_all_str = 1;
  int is_first = 1;

  int bytes_read = 0;
  do {
    int is_str = 0;

    char* token = malloc(sizeof(char));
    bytes_read = read(file_descriptor, token, 1);

    if (bytes_read > 0 && !is_whitespace(*token)) {
      if (isdigit(*token)) {
        item_tmp_chars = append_list(item_tmp_chars, create_char_node(*token));
      }

      if (isalpha(*token)) {
        item_tmp_chars = append_list(item_tmp_chars, create_char_node(*token));
        is_str = 1;
      }

      if ((isdigit(*token) || isalpha(*token)) && is_first) {
        is_all_str = is_str;
        is_first = 0;
      }

      if (is_separator(*token)) {
        if (item_tmp_chars != NULL) {
          if (is_all_str) {
            char* str = str_from_list(item_tmp_chars);
            list_of_items = append_list(list_of_items, create_str_node(str));
          } else {
            char* str = str_from_list(item_tmp_chars);
            list_of_items =
                append_list(list_of_items, create_int_node(atoi(str)));
          }

          item_tmp_chars = NULL;
        }
      }

      total_bytes_read += bytes_read;
    }

  } while (bytes_read > 0);

  if (item_tmp_chars != NULL) {
    if (is_all_str) {
      char* str = str_from_list(item_tmp_chars);
      list_of_items = append_list(list_of_items, create_str_node(str));
    } else {
      char* str = str_from_list(item_tmp_chars);
      list_of_items = append_list(list_of_items, create_int_node(atoi(str)));
    }

    item_tmp_chars = NULL;
  }

  ReadFileOutput* out = malloc(sizeof(ReadFileOutput));
  out->is_str = is_all_str;
  out->list = list_of_items;

  return out;
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

  ReadFileOutput* out = read_file_to_list(file_desc);
  Node* list = out->list;

  if (list == NULL) {
    warning("File was empty.");
  }

  Comparator fn = out->is_str ? string_compare_node : int_compare_node;

  if (equal(sort_flag, "-q")) {
    quicksort(&list, fn);
  }

  if (equal(sort_flag, "-i")) {
    insertion_sort(&list, fn);
  }

  print_list(list, out->is_str);
}