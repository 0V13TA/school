#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Student {
  char *name;
  uint16_t mark;
  uint32_t id;
} Student;

typedef struct StudentArray {
  Student *students;
  size_t length;
  size_t capacity;
  uint32_t next_id;
} StudentArray;

StudentArray create_array(size_t capacity) {
  StudentArray std_arr = {};
  std_arr.length = 0;
  std_arr.capacity = capacity;
  std_arr.next_id = 1; // IDs start at 1
  std_arr.students = malloc(sizeof(Student) * capacity);
  return std_arr;
}

uint8_t _append_student(StudentArray *std_arr, Student student) {
  if (std_arr->length >= std_arr->capacity) {
    Student *students =
        realloc(std_arr->students, sizeof(Student) * (std_arr->capacity * 2));

    if (students == NULL) {
      return 1;
    }

    std_arr->students = students;
    std_arr->capacity *= 2;
  }

  std_arr->students[std_arr->length] = student;
  std_arr->length++;
  return 0;
}

uint8_t add_student(StudentArray *std_arr, const char *name, uint16_t mark) {
  Student student = {};
  student.id = std_arr->next_id;
  student.mark = mark;

  student.name = malloc(strlen(name) + 1);
  if (student.name == NULL)
    return 1;
  strcpy(student.name, name);

  uint8_t result = _append_student(std_arr, student);

  if (result != 0) {
    free(student.name);
  } else {
    std_arr->next_id++;
  }

  return result;
}

Student *get_student(StudentArray *std_arr, uint32_t id) {
  if (std_arr->length <= 0)
    return NULL;
  Student *student = NULL;
  for (size_t i = 0; i < std_arr->length; i++) {
    Student *curr_student = &std_arr->students[i];
    if (id == curr_student->id) {
      student = curr_student;
      break;
    }
  }
  return student;
}

uint8_t delete_student(StudentArray *std_arr, uint32_t id) {
  Student *student = get_student(std_arr, id);

  if (student == NULL)
    return 1;

  free(student->name);

  if (student != &std_arr->students[std_arr->length - 1]) {
    *student = std_arr->students[std_arr->length - 1];
  }

  std_arr->length--;
  return 0;
}

void sort_students(StudentArray *std_arr, int ascending) {
  for (size_t i = 1; i < std_arr->length; i++) {
    Student student = std_arr->students[i];
    size_t j = i;

    while (j > 0 &&
           (ascending ? std_arr->students[j - 1].mark > student.mark
                      : std_arr->students[j - 1].mark < student.mark)) {
      std_arr->students[j] = std_arr->students[j - 1];
      j--;
    }

    std_arr->students[j] = student;
  }
}

void destroy_array(StudentArray *std_arr) {
  for (size_t i = 0; i < std_arr->length; i++) {
    free(std_arr->students[i].name);
  }

  free(std_arr->students);

  std_arr->students = NULL;
  std_arr->length = 0;
  std_arr->capacity = 0;
  std_arr->next_id = 1;
}

void show_welcome(char *username, size_t username_size) {
  printf("========================================\n");
  printf("        STUDENT RECORD SYSTEM\n");
  printf("========================================\n\n");

  printf("Welcome to the Student Record System.\n");
  printf("Please enter your name: ");

  fgets(username, username_size, stdin);
  username[strcspn(username, "\n")] = '\0';

  printf("\nHello, %s!\n", username);
  printf("Let's manage some student records.\n\n");
}

void show_menu(void) {
  printf("\n");
  printf("========================================\n");
  printf("          STUDENT RECORD SYSTEM\n");
  printf("========================================\n");

  printf("1. Add Student\n");
  printf("2. View Students\n");
  printf("3. Search Student\n");
  printf("4. Modify Student\n");
  printf("5. Delete Student\n");
  printf("6. Calculate Average\n");
  printf("7. Sort Students\n");
  printf("8. Save Records\n");
  printf("9. Load Records\n");
  printf("10. Seed Dummy Data\n");
  printf("0. Exit\n");

  printf("\nEnter your choice: ");
}

void display_student(const Student *student) {
  printf("\n");
  printf("Name:        %s\n", student->name);
  printf("Student ID:  %u\n", student->id);
  printf("Mark:        %u\n", student->mark);
  printf("Status:      %s\n", student->mark >= 40 ? "Passed" : "Failed");
}

void display_students(const StudentArray *std_arr) {
  if (std_arr->length == 0) {
    printf("\nNo students found.\n");
    return;
  }

  printf("\n========================================\n");
  printf("              STUDENTS\n");
  printf("========================================\n");

  for (size_t i = 0; i < std_arr->length; i++) {
    display_student(&std_arr->students[i]);
  }
}

void handle_add_student(StudentArray *std_arr) {
  char name[100];
  unsigned int mark;

  printf("\nEnter student name: ");
  scanf(" %99[^\n]", name);

  printf("Enter student mark: ");
  scanf("%u", &mark);

  if (mark > UINT16_MAX) {
    printf("Mark is too large.\n");
    return;
  }

  if (add_student(std_arr, name, (uint16_t)mark) != 0) {
    printf("Failed to add student.\n");
    return;
  }

  printf("\nStudent added successfully. Assigned ID: %u\n",
         std_arr->next_id - 1);
}

void handle_search_student(StudentArray *std_arr) {
  unsigned int id;

  printf("\nEnter student ID: ");
  scanf("%u", &id);

  Student *student = get_student(std_arr, (uint32_t)id);

  if (student == NULL) {
    printf("\nStudent not found.\n");
    return;
  }

  display_student(student);
}

void handle_modify_student(StudentArray *std_arr) {
  unsigned int id;
  char name[100];
  unsigned int mark;

  printf("\nEnter ID of student to modify: ");
  scanf("%u", &id);

  Student *student = get_student(std_arr, (uint32_t)id);

  if (student == NULL) {
    printf("\nStudent not found.\n");
    return;
  }

  printf("Current name: %s\n", student->name);
  printf("Enter new name: ");
  scanf(" %99[^\n]", name);

  printf("Current mark: %u\n", student->mark);
  printf("Enter new mark: ");
  scanf("%u", &mark);

  if (mark > UINT16_MAX) {
    printf("Mark is too large.\n");
    return;
  }

  char *new_name = malloc(strlen(name) + 1);

  if (new_name == NULL) {
    printf("Failed to allocate memory.\n");
    return;
  }

  strcpy(new_name, name);

  free(student->name);
  student->name = new_name;
  student->mark = (uint16_t)mark;

  printf("\nStudent modified successfully.\n");
}

void handle_delete_student(StudentArray *std_arr) {
  unsigned int id;

  printf("\nEnter ID of student to delete: ");
  scanf("%u", &id);

  if (delete_student(std_arr, (uint32_t)id) != 0) {
    printf("\nStudent not found.\n");
    return;
  }

  printf("\nStudent deleted successfully.\n");
}

void display_average_marks(const StudentArray *std_arr) {
  if (std_arr->length == 0) {
    printf("\nNo students available.\n");
    return;
  }

  uint32_t total = 0;

  for (size_t i = 0; i < std_arr->length; i++) {
    total += std_arr->students[i].mark;
  }

  double average = (double)total / std_arr->length;

  printf("\nAverage mark: %.2f\n", average);
}

void handle_sort_students(StudentArray *std_arr) {
  if (std_arr->length < 2) {
    printf("\nNot enough students to sort.\n");
    return;
  }

  int choice;

  printf("\nSort students by mark:\n");
  printf("1. Ascending\n");
  printf("2. Descending\n");
  printf("Enter your choice: ");

  scanf("%d", &choice);

  switch (choice) {
  case 1:
    sort_students(std_arr, 1);
    printf("\nStudents sorted in ascending order.\n");
    break;

  case 2:
    sort_students(std_arr, 0);
    printf("\nStudents sorted in descending order.\n");
    break;

  default:
    printf("\nInvalid option.\n");
    break;
  }
}

void save_students(const StudentArray *std_arr) {
  FILE *file = fopen("students.txt", "w");

  if (file == NULL) {
    printf("\nFailed to open students.txt for writing.\n");
    return;
  }

  for (size_t i = 0; i < std_arr->length; i++) {
    Student *student = &std_arr->students[i];

    fprintf(file, "%u|%u|%s\n", student->id, student->mark, student->name);
  }

  fclose(file);

  printf("\nStudents saved successfully.\n");
}

void load_students(StudentArray *std_arr) {
  FILE *file = fopen("students.txt", "r");

  if (file == NULL) {
    printf("\nNo saved student records found.\n");
    return;
  }

  StudentArray loaded = create_array(10);

  if (loaded.students == NULL) {
    fclose(file);
    printf("\nFailed to allocate memory.\n");
    return;
  }

  char name[100];
  unsigned int id;
  unsigned int mark;

  while (fscanf(file, "%u|%u|%99[^\n]\n", &id, &mark, name) == 3) {

    if (mark > UINT16_MAX)
      continue;

    Student student = {};

    student.id = (uint32_t)id;
    student.mark = (uint16_t)mark;

    student.name = malloc(strlen(name) + 1);

    if (student.name == NULL) {
      destroy_array(&loaded);
      fclose(file);
      printf("\nFailed to allocate memory.\n");
      return;
    }

    strcpy(student.name, name);

    if (_append_student(&loaded, student) != 0) {
      free(student.name);
      destroy_array(&loaded);
      fclose(file);
      printf("\nFailed to load students.\n");
      return;
    }

    if (student.id >= loaded.next_id) {
      loaded.next_id = student.id + 1;
    }
  }

  fclose(file);

  destroy_array(std_arr);
  *std_arr = loaded;

  printf("\nStudents loaded successfully.\n");
}

void handle_seed_students(StudentArray *std_arr) {
  int count;
  printf("\nEnter number of dummy students to generate: ");
  scanf("%d", &count);

  if (count <= 0) {
    printf("Invalid count.\n");
    return;
  }

  const char *first_names[] = {"Alex",  "Bella",  "Charlie", "Diana", "Ethan",
                               "Fiona", "George", "Hannah",  "Ian",   "Julia",
                               "Kevin", "Luna",   "Mason",   "Nora",  "Oliver",
                               "Penny", "Quinn",  "Riley",   "Sam",   "Tara"};
  const char *last_names[] = {"Smith",     "Johnson",  "Williams",  "Brown",
                              "Jones",     "Garcia",   "Miller",    "Davis",
                              "Rodriguez", "Martinez", "Hernandez", "Lopez",
                              "Gonzalez",  "Wilson",   "Anderson",  "Thomas",
                              "Taylor",    "Moore",    "Jackson",   "Martin"};

  int first_name_count = sizeof(first_names) / sizeof(first_names[0]);
  int last_name_count = sizeof(last_names) / sizeof(last_names[0]);

  int success_count = 0;

  for (int i = 0; i < count; i++) {
    char full_name[100];

    // Pick a random first and last name combination
    snprintf(full_name, sizeof(full_name), "%s %s",
             first_names[rand() % first_name_count],
             last_names[rand() % last_name_count]);

    // Generate a random mark between 0 and 100
    uint16_t random_mark = rand() % 101;

    if (add_student(std_arr, full_name, random_mark) == 0) {
      success_count++;
    } else {
      printf("Error adding dummy student %d. Stopping early.\n", i + 1);
      break;
    }
  }

  printf("\nSuccessfully generated %d dummy student records.\n", success_count);
}

int main(void) {
  // Seed the random number generator
  srand((unsigned int)time(NULL));

  char username[100];

  show_welcome(username, sizeof(username));

  StudentArray students = create_array(10);

  int running = 1;

  while (running) {
    show_menu();

    int choice;
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      handle_add_student(&students);
      break;

    case 2:
      display_students(&students);
      break;

    case 3:
      handle_search_student(&students);
      break;

    case 4:
      handle_modify_student(&students);
      break;

    case 5:
      handle_delete_student(&students);
      break;

    case 6:
      display_average_marks(&students);
      break;

    case 7:
      handle_sort_students(&students);
      break;

    case 8:
      save_students(&students);
      break;

    case 9:
      load_students(&students);
      break;

    case 10:
      handle_seed_students(&students);
      break;

    case 0:
      running = 0;
      break;

    default:
      printf("Invalid option.\n");
      break;
    }
  }

  destroy_array(&students);

  return EXIT_SUCCESS;
}
