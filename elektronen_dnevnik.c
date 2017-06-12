/*
1. Zapis bez masiv za uchenici
2. Chetene 
3. Dobavqne
4. Redaktirane
5. Smqna na file
6. Izhod. 

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <conio.h>

typedef struct {
	char name[50];
	int num;
	int grades[50];
	int currentGradeIndex;

} Student;

typedef struct {
	int studentNum;
} Options;

Student createStudent();
Student * getStudentArray();
Options createOptions();
Options getOptions();
void addGrade(Student *student, int *grade);
void printStudentInfo(Student student);
float getAverageGrade(int grades[], int size);
void changeWorkingStudentDatabase(char studentFileName[]);
void clear();

int main(void) {
	FILE *studentFile;
	

	char studentFileName[30];
	// Get the file that the user wants to work with and create it if it doesnt exist.
	changeWorkingStudentDatabase(studentFileName);
	
	
	while(1) {
		char command[20];
		

		printf("What would you like to do?\n");
		printf("Type 'read' to display the file information\n");
		printf("Type 'add' to add a student\n");
		printf("Type 'change' to change current active student database.\n");
		printf("Type 'edit' to edit a student\n");
		printf("Type 'average' to get the average of all the students\n");
		printf("Type 'exit' to exit the program.\n");

		gets(command);
		clear();

		if(strcmp(command, "read") == 0) {
			Student students[30];
			studentFile = fopen(studentFileName, "rb");
			
			Student student;
			while((fread(&student, sizeof(Student), 1, studentFile))) {
				printStudentInfo(student);
			}
			fclose(studentFile);

		}
		else if(strcmp(command, "add") == 0) {
			// Get number of students;
			int studentNum = 0;
			Student student;
			studentFile = fopen(studentFileName, "rb");
			while((fread(&student, sizeof(Student), 1, studentFile))) {
				studentNum++;
			}
			fclose(studentFile);


			studentFile = fopen(studentFileName, "ab");

			while(1) {
				Student student = createStudent();
				student.num = studentNum;
				printf("Enter name(Type 'NULL' to exit): ");
				gets(student.name);

				if(strcmp(student.name, "NULL") == 0) {
					break;
				}
				
				printf("\nEnter the stream of student's grades(Type 0 to exit): ");
				while(1) {
					int grade;
					scanf("%d", &grade);
					getchar();
					if(grade <= 0) {
						break;
					}
					addGrade(&student, &grade);
				}
				printStudentInfo(student);
				fwrite(&student, sizeof(Student), 1, studentFile);
				studentNum++;
				
			}
			fclose(studentFile);
		}
		else if(strcmp(command, "exit") == 0) {
			break;
		} 
		else if(strcmp(command, "change") == 0) {
			changeWorkingStudentDatabase(studentFileName);
		}
		else if(strcmp(command, "edit") == 0) {
			int index;
			printf("\nEnter the index of the student: ");
			scanf("%d", &index);
			getchar();


			studentFile = fopen(studentFileName, "r+b");
			fseek(studentFile, index, SEEK_SET); // Set the file's stream position to the index.
			Student student = createStudent();
			student.num = index;
			printf("Enter name(Type 'END' to exit): ");
			gets(student.name);

			if(strcmp(student.name, "END") == 0) {
				break;
			}
			
			printf("\nEnter the stream of student's grades(Type 0 to exit): ");
			while(1) {
				int grade;
				scanf("%d", &grade);
				getchar();
				if(grade <= 0) {
					break;
				}
				addGrade(&student, &grade);
			}
			printStudentInfo(student);
			fwrite(&student, sizeof(Student), 1, studentFile);

			fclose(studentFile);
		}
		else if(strcmp(command, "average") == 0) {
			studentFile = fopen(studentFileName, "rb");
			Student student;
			int numStudents = 0;
			float sum = 0;
			while((fread(&student, sizeof(Student), 1, studentFile))) {
				numStudents++;
				sum += getAverageGrade(student.grades, student.currentGradeIndex);
			}
			printf("Average class grade is: %.2f\n", sum / numStudents);
		}
		else if(strcmp(command, "remove") == 0) {
			int index;
			printf("\nEnter the index of the student you would like to remove: ");
			scanf("%d", &index);
			getchar();

			Student student;
			studentFile = fopen(studentFileName, "r+b");
			FILE * copyTo = fopen("classes/temp.neshto", "wb");	
			
			while(fread(&student, sizeof(Student), 1, studentFile)) {
				if((ftell(studentFile) / sizeof(Student)) == index + 1) {
					continue;
				}
				fwrite(&student, sizeof(Student), 1, copyTo);
			}
			fclose(studentFile);
			int rm = remove(studentFileName);

			if(rm) {
				printf("Error with deleting file!");
				scanf("%s");
			}
			else {
				fclose(copyTo);
				rename("classes/temp.neshto", studentFileName);
			}
			
		}
	}
}

Student createStudent() {
	Student student;
	student.currentGradeIndex = 0;
	return student;
}

Options createOptions() {
	Options options;
	options.studentNum = 0;
	return options;
}


void addGrade(Student *student, int *grade) {
	(*student).grades[(*student).currentGradeIndex] = *grade;
	(*student).currentGradeIndex += 1;
}


void printStudentInfo(Student student) {
	printf("\n-------------------------\n");
	printf("Student Info: \n");
	printf("Name: %s\n", student.name);
	printf("Class number: %d\n", student.num);
	float average = getAverageGrade(student.grades, student.currentGradeIndex);
	if(average == -1) {
		printf("Average Grade: %s does not have any grades.\n", student.name);
	}
	else 
	{
		printf("Average Grade: %.2f\n", average);
	}
	printf("-------------------------\n");
}

float getAverageGrade(int grades[], int size) {
	if(size == 0) {
		return -1;
	}

	float sum = 0;
	for(int i = 0; i < size; i++) {
		sum += grades[i];
	}

	return sum / size;
}

void changeWorkingStudentDatabase(char studentFileName[]) {
	FILE *optionsFile;
	FILE *studentFile;
	DIR *d;
	struct dirent *dir;

	char fileName[30];

	// Print all student databases
	d = opendir("./classes");
	printf("Available classes:\n");
	while((dir = readdir(d)) != NULL) {
		printf("%s\n", dir->d_name);
	}

	// Get the student file name and create the file if it doesnt exist.
	printf("Enter the name of the file you would like to work with.\n(Create new one by entering a name not in the list): ");
	gets(fileName);
	sprintf(studentFileName, "classes/%s.students", fileName);
	studentFile = fopen(studentFileName, "ab");
	fclose(studentFile);
}

void clear() {
	system("cls");
}