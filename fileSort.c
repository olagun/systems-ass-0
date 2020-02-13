#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// TODO:
// - Sorting comparators and testing.
// - Wrap `malloc` with error handling.

// 🦾 Sorting prototypes
typedef int (*Comparator)(void*, void*);

void insertion_sort(void*, Comparator);
void selection_sort(void*, Comparator);
void quicksort(void*, Comparator);

// 📔 Linked List declarations
typedef struct Node Node;
typedef union Data Data;

typedef union Data {
  int int_data;
  char* str_data;
  char char_data;
  Node* list;
} Data;

typedef struct Node {
  Data data;
  struct Node* next;
} Node;

// 🧩 Sorting implementations
void insertion_sort(void* list, Comparator cmpr) {}
void selection_sort(void* list, Comparator cmpr) {}
void quicksort(void* list, Comparator cmpr) {}

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

int is_whitespace(char c) { return c == '\n' || c == ' ' || c == '\t'; }

Node* read_file_to_list(int file_descriptor) {
  int total_bytes_read = 0;

  Node* list_of_items = NULL;
  Node* item_tmp_chars = NULL;

  int bytes_read;
  do {
    char* c = malloc(sizeof(char));
    bytes_read = read(file_descriptor, c, 1);

    if (bytes_read > 0 && !is_whitespace(*c)) {
      if (isdigit(*c)) {
        item_tmp_chars = append_list(item_tmp_chars, create_int_node(atoi(c)));
      }

      if (isalpha(*c)) {
        item_tmp_chars = append_list(item_tmp_chars, create_char_node(*c));
      }

      if (*c == ',') {
        // TODO: Check length and don't add if length is 0.
        char* str = str_from_list(item_tmp_chars);
        printf("%s\n", str);
        list_of_items = append_list(list_of_items, create_str_node(str));
        item_tmp_chars = NULL;
      }
    }

    total_bytes_read += bytes_read;
  } while (bytes_read > 0);

  char* str = str_from_list(item_tmp_chars);
  list_of_items = append_list(list_of_items, create_str_node(str));
  item_tmp_chars = NULL;
  printf("%s\n", str);

  return list_of_items;
}

void print_list(Node* head) {
  while (head != NULL) {
    printf("%s\n", head->data.str_data);
    head = head->next;
  }
}

int main(int argc, char* argv[]) {
  char* sort_flag = argv[0];
  char* file_name = argv[1];

  int file_descriptor = open(file_name, O_RDONLY | O_CREAT);

  Node* list = read_file_to_list(file_descriptor);
  // print_list(list);

  return 0;
}
