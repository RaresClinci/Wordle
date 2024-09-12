#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#define W_MAX 100

// ecranul jocului
void screen(){
    int i;
    move(1,1);
    printw(":Menu");
    move(2, 5);
    printw("WORDLE\n");
    move(3, 4);
    for (i = 1; i <= 11; i++)
        printw("_");
    for (i = 1; i <= 6; i++)
    {
        move(3 + i, 4);
        printw("|_|_|_|_|_|");
    }
}

// lista de cuvinte
int wordlist(char f[W_MAX][6]){
    FILE *fisier;
    fisier = fopen("words.txt", "r");
    //
    char s[7];
    int n, k = 0;
    while (fgets(s, 6, fisier)){
        n = strlen(s);
        if (s[n - 1] == '\n')
            s[n - 1] = 0;
        strcpy(f[k++], s);
        fgetc(fisier);
    }
    fclose(fisier);
    return k;
}

// alegere nr random < n
int randomized(int n){
    int k;
    srand(time(NULL));
    k = rand();
    return k % n;
}

// subprogram determinare backspace
int isdel(char a){
    if (a == KEY_BACKSPACE)
        return 1;
    if (a == 127)
        return 1;
    if (a == '\b')
        return 1;
    return 0;
}

// functie determinare culori
void colors(char s[], char f[], int c[]){
    int i, n;
    n = strlen(f);
    for (i = 0; i < n; i++)
        if (f[i] == s[i])
            c[i] = 3;
        else if (strchr(s, f[i]) != NULL)
            c[i] = 2;
        else
            c[i] = 1;
}

// functie determinare castig
int win(int s[]){
    int i;
    for (i = 0; i < 5; i++)
        if (s[i] != 3)
            return 0;
    return 1;
}

// functie :
int finish(){
    char op;
    move(5, 19);
    printw("Optiuni:");
    move(6, 19);
    printw("1. Iesire");
    move(7, 19);
    printw("2. Joc nou");
    move(8, 19);
    printw("Apasati tasta 1 sau 2: ");
    op = getch();
    if (op == '1')
        return 0;
    else
        return 1;
}

// functie colorare
void paint(char inp[], int a[], int l){
    int i;
    for (i = 1; i <= 5; i++)
    {
        move(3 + l, 3 + 2 * i);
        attron(COLOR_PAIR(a[i - 1]));
        printw("%c", toupper(inp[i - 1]));
        attroff(COLOR_PAIR(a[i - 1]));
    }
}

//functie majuscule
void strupr(char f[]){
    int i;
    for(i=0;i<5;i++)
        f[i]=toupper(f[i]);
}

//functie procesare linie
int line(char inp[], int *i, int j, int *eg){
    while(j<=5){
        //elimianre caracter din afara tabelului cauzat de backspace
        move(3 + *i, 3);
        printw(" ");
        //
        move(3 + *i, 3 + 2 * j);
        inp[j - 1] = getch();
        // cazul :
        if (inp[j - 1] == ':'){
            *eg = finish();
            break;
        }
        // enter prematur
        if (inp[j - 1] == '\n'){
            move(10, 4);
            printw("Litere insuficiente!");
            move(3 + *i, 4);
            printw("|_|_|_|_|_|");
            *i=*i-1;
            return 0;
        }
        // backspace
        if (isdel(inp[j - 1])){
            move(3 + *i, 3 + 2 * j);
            printw("_|");
            j--;
            if (j > 0){
                move(3 + *i, 3 + 2 * j);
                printw("_");
                j--;
            }
        }
        // validam litera si scriem cu majuscule
        move(3 + *i, 3 + 2 * j);
        if (!isalpha(inp[j - 1])){
            printw("_");
            j--;
        }
        else{
            printw("%c", toupper(inp[j - 1]));
            inp[j - 1] = tolower(inp[j - 1]);
        }
        j++;
    }
    return 1;
}
int main(int argc, char *argv[]){
    char cont = 'y', sol[6], inp[6], c;
    int eg = 2, k, ok = 1, col[6] = {0};
    initscr();
    // setare culori
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    //generare wrdlist
    char f[W_MAX][6];
    int i, n;
    n = wordlist(f);
    while (cont == 'y'){
        // initilizare ecran
        erase();
        screen();
        // alegere solutie
        k = randomized(n);
        strcpy(sol, f[k]);
        // afisare solutie(debugging util)
        /*
        move(1,1);
        printw("%s",sol);
        */
        // procesare input
        for (i = 1; i < 7; i++){
            // literele care vor intra in casuta
            echo();
            ok = line(inp, &i, 1, & eg);
            inp[5] = 0;
            // cazul :
            if (eg != 2)
                break;
            if (ok){
                // literele care nu vor intra in casuta
                noecho();
                c = getch();
                while (c != '\n'){
                    c = getch();
                    //backspace
                    if(isdel(c)){
                        ok = line(inp, &i, 5, & eg);
                        if(ok==0) 
                            break;
                    }
                    // cazul :
                    if (c == ':'){
                        eg = finish();
                        break;
                    }
                }
                //
                if(ok){
                    // cazul :
                    if (eg != 2)
                        break;
                    // stergem mesaj de litere insuficiente
                    move(10, 0);
                    clrtoeol();
                    // culori
                    colors(sol, inp, col);
                    // colararea conform vectorului
                    paint(inp, col, i);
                    // castig
                    if (win(col)){
                        move(5, 19);
                        printw("Felicitari! Ati castigat!");
                        break;
                    }
                }
            }
        }
        if (i == 7){
            move(5, 19);
            strupr(sol);
            printw("Incercarile au expirat! Solutia era %s",sol);
        }
        if (eg == 2){
            move(6, 19);
            printw("Doriti sa mai jucati? (y/n)\n");
            move(7, 19);
            cont = getch();
            while (cont != 'y' && cont != 'n'){
                move(7, 19);
                cont = getch();
            }
        }
        else if (eg == 1){
            eg = 2;
            cont = 'y';
        }
        else
            cont = 'n';
    }
    endwin();
    return 0;
}