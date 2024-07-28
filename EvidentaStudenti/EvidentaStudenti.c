#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_CURSURI 10
#define MAX_NUME 50
#define DATA_FILE "Studenti.txt"
#define DATA_CURS "Cursuri.txt"

typedef struct Cursuri {
    char name[MAX_NUME];
} Cursuri;

typedef struct Student {
    char nume[MAX_NUME];
    int note[MAX_CURSURI];
    int prezente[MAX_CURSURI];
    int participare_curs[MAX_CURSURI];
    struct Student* next;
} Student;

Cursuri cursuri[MAX_CURSURI];
Student* head = NULL;
int nr_curs = 0;

Student* creare_student(char* name) {
    Student* new_student = (Student*)malloc(sizeof(Student));
    strcpy(new_student->nume, name);
    new_student->next = NULL;
    for (int i = 0; i < MAX_CURSURI; i++) {
        new_student->note[i] = 0;
        new_student->prezente[i] = 0;
        new_student->participare_curs[i] = 0;
    }
    return new_student;
}

char* strtrim(char* str) {
    while (isspace((unsigned char)(*str))) {
        str++;
    }
    if (*str == '\0') {
        return str;
    }
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)(*end))) {
        end--;
    }
    *(end + 1) = '\0';

    return str;
}
void citire_fisier_cursuri() {
    char buffer[MAX_NUME];
    FILE* file = fopen(DATA_CURS, "r");
    if (file == NULL) {
        perror("Nu se poate deschide fisierul");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL && nr_curs < MAX_CURSURI) {
        buffer[strcspn(buffer, "\n")] = '\0';
        strcpy(cursuri[nr_curs].name, buffer);
        nr_curs++;
    }

    fclose(file);
}

void scriere_fisier_cursuri(char* name) {
    FILE* file = fopen(DATA_CURS, "a");
    if (file == NULL) {
        perror("Nu se poate deschide fisierul");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\n",(name));
    fclose(file);
}

void citire_fisier_studenti() {
    char buffer[300];
    char nume[MAX_NUME];
    char nume_curs[MAX_NUME];
    int nota, prezenta;

    FILE* file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        perror("Nu se poate deschide fisierul");
        exit(EXIT_FAILURE);
    }

    Student* student_nou = NULL;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strncmp(buffer, "Nume:", 5) == 0) {
            sscanf(buffer + 5, "%s", nume);
            student_nou = creare_student(nume);

            if (head == NULL) {
                head = student_nou;
            }
            else {
                Student* s = head;
                while (s->next != NULL) {
                    s = s->next;
                }
                s->next = student_nou;
            }
        }
        else if (strncmp(buffer, "Curs:", 5) == 0 && student_nou != NULL) {
            sscanf(buffer, "Curs:%s Nota:%d Prezenta:%d", nume_curs, &nota, &prezenta);
            for (int i = 0; i < nr_curs; i++) {
                if (strcmp(strtrim(nume_curs), strtrim(cursuri[i].name)) == 0) {
                    student_nou->note[i] = nota;
                    student_nou->prezente[i] = prezenta;
                    student_nou->participare_curs[i]++;
                    break;
                }
            }
        }
    }

    fclose(file);
}

void scriere_fisier_student() {
    FILE* file = fopen(DATA_FILE, "w");
    if (file == NULL) {
        perror("Nu se poate deschide fisierul");
        exit(EXIT_FAILURE);
    }

    Student* s = head;
    while (s != NULL) {
        int k = 0;
        for (int i = 0; i < nr_curs; i++) {
            if (s->participare_curs[i] > 0) {
                k = 1;
                break;
            }
        }
        if (k == 1) {
            fprintf(file, "Nume:%s\n", s->nume);
            for (int i = 0; i < nr_curs; i++) {
                if (s->participare_curs[i] > 0) {
                    fprintf(file, "Curs:%s Nota:%d Prezenta:%d\n", cursuri[i].name, s->note[i], s->prezente[i]);
                }
            }
        }
        s = s->next;
    }

    fclose(file);
}

void adaugare_student() {
    char name[MAX_NUME];
    printf("Introdu numele studentului: ");
    scanf(" %[^\n]", name);

    Student* student_nou = creare_student(name);


    if (head == NULL || strcmp(student_nou->nume, head->nume) < 0) {
        student_nou->next = head;
        head = student_nou;
    }
    else {
       
        Student* student_curent = head;
        while (student_curent->next != NULL && strcmp(student_curent->next->nume, student_nou->nume) < 0) {
            student_curent = student_curent->next;
        }
        student_nou->next = student_curent->next;
        student_curent->next = student_nou;
    }
}


void adauga_curs() {
    if (nr_curs >= MAX_CURSURI) {
        printf("Numar maxim de cursuri atins.\n");
        return;
    }

    printf("Introdu numele cursului: ");
    scanf(" %[^\n]", cursuri[nr_curs].name);
    scriere_fisier_cursuri(cursuri[nr_curs].name);
    nr_curs++;
    printf("Curs adaugat cu succes.\n");
}

void afisare_cursuri() {
    printf("Lista cursurilor:\n");
    for (int i = 0; i < nr_curs; i++) {
        printf("%d. %s\n", i, cursuri[i].name);
    }
}

void afisare_studenti() {
    int i = 0;
    Student* s = head;

    printf("Lista studentilor:\n");
    while (s != NULL) {
        printf("%d. %s\n", i, s->nume);
        s = s->next;
        i++;
    }
}
int verificare_index_student_curs(Student** student, int* curs_index) {
    Student* s = head;
    int student_index, i = 0;
    afisare_studenti();
    printf("Introdu indexul studentului (0...): ");
    scanf("%d", &student_index);

    while (s != NULL && i < student_index) {
        s = s->next;
        i++;
    }
    if (s == NULL) {
        printf("Index invalid.\n");
        return 0;
    }
    afisare_cursuri();
    printf("Introdu indexul cursului (0-%d): ", nr_curs - 1);
    scanf("%d", curs_index);
    if (*curs_index < 0 || *curs_index >= nr_curs) {
        printf("Index invalid.\n");
        return 0;
    }

    *student = s;
    return 1;
}

void adaugare_student_la_curs() {
    int curs_index;
    Student* s;

    if (verificare_index_student_curs(&s, &curs_index) == 1) {
        s->note[curs_index] = 0;
        s->prezente[curs_index] = 0;
        s->participare_curs[curs_index]++;

        printf("Studentul a fost inscris la curs.\n");
    }
}

void adaugare_nota() {
    int curs_index, nota;
    Student* s = head;

    if (verificare_index_student_curs(&s, &curs_index) == 1) {
        printf("Introdu nota: ");
        scanf("%d", &nota);
        s->note[curs_index] = nota;

        printf("Nota adaugata cu succes.\n");
    }
}

void adauga_prezenta() {
    int curs_index, prezenta;
    Student* s = head;

    if (verificare_index_student_curs(&s, &curs_index) == 1) {
        printf("Introdu prezenta (1 pentru prezent, 0 pentru absent): ");
        scanf("%d", &prezenta);
        s->prezente[curs_index] = prezenta;

        printf("Prezenta adaugata cu succes.\n");
    }
}

void informatii_student() {
    int student_index;
    Student* s = head;
    int i = 0, contor = 0;


    printf("Lista studentilor:\n");
    while (s != NULL) 
    {
        contor = 0;
        for (int j = 0; j < nr_curs; j++) 
            if (s->participare_curs[j] > 0)
                contor = 1;
        if (contor == 1)
        {
            printf("%d. %s\n", i, s->nume);
            for (int j = 0; j < nr_curs; j++)
                if (s->participare_curs[j] > 0)
                    printf("    Curs: %s, Nota: %d, Prezenta: %d\n", cursuri[j].name, s->note[j], s->prezente[j]);


        }
            s = s->next;
            i++;
        
    }
}

void cautare_dupa_nume() {
    char nume[MAX_NUME];

   int contor = 0;
    printf("Introdu numele studentului:");
    scanf("%s", nume);
   for(Student *s=head;s!=NULL;s=s->next)

       if (strcmp(strtrim(s->nume), strtrim(nume)) == 0) {
           for (int j = 0; j < nr_curs; j++)
               if (s->participare_curs[j] > 0)
                   printf("Curs: %s, Nota: %d, Prezenta: %d\n", cursuri[j].name, s->note[j], s->prezente[j]);
           contor = 1;
       }

   
   
   if (contor == 0) {
       printf("STudentul nu a fost gasit!!!\n");
   }
}
void afisare_student_cu_restante() {  
    for (Student* s = head; s != NULL; s = s->next) 
        for (int j = 0; j < nr_curs; j++) {
            if (s->participare_curs[j] > 0 && s->note[j] <= 4 && s->note[j] >= 1)
                printf("NUME: %s\n Curs: %s, Nota: %d, Prezenta: %d\n",s->nume, cursuri[j].name, s->note[j], s->prezente[j]);
        }
    
}
void afisarea_studentilor_integralisti(){
    int k, contor = 0;
    for (Student* s = head; s != NULL; s = s->next) {
        k = 0;
        for (int j = 0; j < nr_curs; j++) {
            if (s->participare_curs[j] > 0 && s->note[j] <= 4 && s->note[j] >= 1)
                k++;
               }
        if (k == 0)
        {
            printf("Studentul %s este integralist\n",s->nume);
            contor = 1;
        }
        }
    if (contor == 0)
        printf("Nu exista niciun student integralist!\n");
}

void afisare_medie_pe_materie() {

    int suma[MAX_CURSURI], contor[MAX_CURSURI];
    for (int j = 0; j < nr_curs; j++)
    {
        suma[j] = 0;
        contor[j] = 0;
    }
    for (Student* s = head; s != NULL; s = s->next) 
        for (int j = 0; j < nr_curs; j++) 
            if (s->participare_curs[j] > 0) {
                suma[j] = suma[j] + s->note[j];
                contor[j]++;
            }
             
    for (int j = 0; j < nr_curs; j++)
        if (suma[j] > 0)
            printf("La cursul %s, media este %.2f\n", cursuri[j],((float)suma[j] / contor[j]));
    
}

int main() {
    int optiune;
    citire_fisier_cursuri();
    citire_fisier_studenti();

    while (1) {
        printf("0. Iesire\n");
        printf("1. Adauga student\n");
        printf("2. Adauga curs\n");
        printf("3. Inscrie student la curs\n");
        printf("4. Adauga nota\n");
        printf("5. Adauga prezenta\n");
        printf("6. Afiseaza informatii student\n");
        printf("7. Afiseaza toti studentii\n");
        printf("8. Afiseaza toate cursurile\n");
        printf("9. Cautare student dupa nume\n");
        printf("10. Afisare studenti cu restante\n");
        printf("11. Afiseaza  studentii integralisti\n");
        printf("12. Afiseaza medii pe materii\n");
        printf("Alege o optiune: ");
        scanf("%d", &optiune);

        switch (optiune) {
        case 0:
            scriere_fisier_student();
            exit(0);
            break;
        case 1:
            adaugare_student();
            break;
        case 2:
            adauga_curs();
            break;
        case 3:
            adaugare_student_la_curs();
            break;
        case 4:
            adaugare_nota();
            break;
        case 5:
            adauga_prezenta();
            break;
        case 6:
            informatii_student();
            break;
        case 7:
            afisare_studenti();
            break;
        case 8:
            afisare_cursuri();
            break;
        case 9:
            cautare_dupa_nume();
            break;
        case 10:
            afisare_student_cu_restante();
            break;
        case 11:
            afisarea_studentilor_integralisti();
            break;
        case 12:
            afisare_medie_pe_materie();
            break;
        default:
            printf("Ati ales o optiune invalida!!\n");
            break;
        }
    }

    return 0;
}
