#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NUM_STUDENTS 10000
#define MAX_LINE_LENGTH 1000
#define MAX_NAME_SIZE 50
typedef struct student_s Student;

struct student_s {
    char name[MAX_NAME_SIZE];
    int age;
    Student* next;
};

Student studentPool[MAX_NUM_STUDENTS]; // Create a pool of student records to be allocated on demand
int firstFree = 0;

// Return a pointer to a new student record from the pool, after filling in the provided name and age fields. Returns NULL if the student pool is exhausted.
Student* newStudent(const char* name, int age)
{
    Student* student = NULL;
    if (firstFree < MAX_NUM_STUDENTS) {
        student = &studentPool[firstFree];
        firstFree += 1;
        strncpy(student->name, name, MAX_NAME_SIZE);
        student->name[MAX_NAME_SIZE - 1] = '\0';  // Ensure termination
        student->age = age;
        student->next = NULL;
    }
    return student;
}

// Read a single student from a csv input file with student name in first column, and student age in second. Returns a pointer to a Student record, or NULL if EOF or a bad student record is read.
Student* readOneStudent(FILE* file)
{
    char buffer[MAX_LINE_LENGTH] = {0}; // Line buffer
    Student* student = NULL;           // Pointer to a student record

    // Read a line, extract name and age
    char* s = fgets(buffer, MAX_LINE_LENGTH, file);
    if (s != NULL) {           // Proceed only if we read something
        char* commaPos = strchr(buffer, ',');
        if (commaPos != NULL && commaPos > buffer) {
            int age = atoi(commaPos + 1);
            *commaPos = '\0';  // null-terminate the name
            student = newStudent(buffer, age);
        }
    }
    return student;
}

// Reads a list of students from a given file. List is terminated by a blank line or EOF or bad data. Returns a pointer to the first student in the list or NULL if no valid student records could be read.
Student* readStudents(FILE* file)
{
    Student* first = NULL;     // Pointer to first student in list
    Student* last = NULL;      // Pointer to last student in list
    Student* student = readOneStudent(file);
    while (student != NULL) {
        if (first == NULL) {
            first = last = student;   // Empty list case
    } else {
            last->next = student;
            last = student;
        }
        student= readOneStudent(file);
    };
    return first;
}

// printOneStudent: prints a single student, passed by value
void printOneStudent(Student student)
{
    printf("%s (%d)\n", student.name, student.age);
}


// printStudents: print all students in a list of students passed by reference
void printStudents(const Student* student)
{
    while (student != NULL) {
        printOneStudent(*student);
        student = student->next;
    };
}

// Main program. Read a linked list of students from a csv file, then display the contents of that list.
int main(void)
{
    FILE* inputFile = fopen("studlist.txt", "r");
    Student* studentList = readStudents(inputFile);
    printStudents(studentList);

    // The program could now do various things that make use of the linked list, like deleting students and adding new ones.

    return EXIT_SUCCESS;
}
