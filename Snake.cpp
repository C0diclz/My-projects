#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <chrono>
#include <conio.h>
#include <windows.h>

using namespace std;

bool fineGioco;
char nome[30];
int x, y, ciboX, ciboY, lunghezzaCoda, punteggio;
int codaX[100], codaY[100]; 
const int maxY= 30;
const int maxX= 30;
enum Direzione {
	STOP = 0, SU, GIU, DX, SX
};
Direzione dir;

struct nodo {
    char nome[30];
    int punteggio;
    struct nodo* prossimo;
};

// Funzioni dichiarate
void setup();
void draw();
void input();
void logic();
struct nodo* creaNodo(char nome[30], int punteggio);
int contaNodi(nodo* testa);
void aggiungiInOrdine(nodo** testa, char nome[30], int punteggio);
void stampaLista(nodo* testa);

// Funzione per calcolare l'offset per centrare il testo
int calcolaOffsetCentro(int larghezzaMassima, int larghezzaTesto) {
    return (larghezzaMassima - larghezzaTesto) / 2;
}

int main() {
    struct nodo* lista = NULL;
    int menu;
    do {
        system("cls"); // Pulisce lo schermo
        cout << "********** Menu': ***********\n";
        cout << "* 1 - Nuova partita.        *\n";
        cout << "* 2 - 10 Punteggi migliori  *\n";
        cout << "* 0 - Esci dal gioco.       *\n";
        cout << "*****************************\n";
        cin >> menu;

        switch (menu) {
            case 1: {
                setup(); // Impostazioni iniziali del gioco
                using clock = chrono::steady_clock;
                auto lastLogicUpdate = clock::now();
                auto lastDrawUpdate = clock::now();
                int logicDelay = 150;
                int drawDelay = 50;

                while (!fineGioco) {
                    auto now = clock::now();

                    // Aggiorna la logica del gioco
                    if (chrono::duration_cast<chrono::milliseconds>(now - lastLogicUpdate).count() >= logicDelay) {
                        input();
                        logic();
                        lastLogicUpdate = now;
                    }

                    // Ridisegna la scena
                    if (chrono::duration_cast<chrono::milliseconds>(now - lastDrawUpdate).count() >= drawDelay) {
                        draw();
                        lastDrawUpdate = now;
                    }
                }

                // Al termine della partita, memorizza il punteggio
                cout << "Inserisci il tuo nome per memorizzare il tuo punteggio in classifica\n";
                cin >> nome;
                aggiungiInOrdine(&lista, nome, punteggio);
                break;
            }
            case 2: {
                // Stampa la classifica dei punteggi
                stampaLista(lista);
                cout << "Premi un tasto qualsiasi per continuare\n";
                while (!_kbhit()) {
                    Sleep(10);
                }
                break;
            }
            case 0: {
                // Esce dal gioco
                cout << "Arrivederci.\n";
                Sleep(1000);
                break;
            }
            default: {
                // Gestione input non valido
                cout << "Scelta non valida, inserire un valore numerico dalla lista.\n";
                cout << "Premi un tasto qualsiasi per continuare\n";
                while (!_kbhit()) {
                    Sleep(10);
                }
                break;
            }
        }
    } while (menu != 0);
    return 0;
}

// Funzione di setup iniziale per il gioco
void setup() {
    srand(time(0)); // Inizializza il generatore di numeri casuali
    fineGioco = false;
    x = maxX / 2;
    y = maxY / 2;
    ciboX = rand() % maxX;
    ciboY = rand() % maxY;
    punteggio = 0;
    lunghezzaCoda = 0;
    dir = STOP;
}

// Funzione per disegnare il campo di gioco centrato
void draw() {
    system("cls"); // Pulisce la console

    // Ottieni le dimensioni della console
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int larghezzaConsole;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        larghezzaConsole = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    } else {
        larghezzaConsole = 80; // Valore di fallback se non riesce a ottenere le dimensioni
    }

    string bordo(maxX + 2, '#');
    int offsetCentro = calcolaOffsetCentro(larghezzaConsole, bordo.length() * 2);

    // Visualizza le istruzioni per il giocatore
    cout << string(offsetCentro, ' ') << "Usa W, A, S e D per controllare il serpente\n";
    cout << string(offsetCentro, ' ') << "Premi X per terminare la partita\n";

    // Stampa il bordo superiore del campo
    cout << string(offsetCentro, ' ');
    for (int j = 0; j < maxX + 2; j++) {
        cout << "# ";
    }
    cout << "\n";

    // Stampa il campo di gioco
    for (int j = 0; j < maxY; j++) {
        cout << string(offsetCentro, ' ') << "# "; // Bordo sinistro

        for (int k = 0; k < maxX; k++) {
            if (j == y && k == x) {
                cout << "0 "; // Testa del serpente
            } else if (j == ciboY && k == ciboX) {
                cout << "Q "; // Cibo
            } else {
                bool stampa = false;
                for (int h = 0; h < lunghezzaCoda; h++) {
                    if (codaX[h] == k && codaY[h] == j) {
                        cout << "o "; // Corpo del serpente
                        stampa = true;
                    }
                }
                if (!stampa) {
                    cout << "  "; // Spazio vuoto
                }
            }
        }
        cout << "#\n"; // Bordo destro
    }

    // Stampa il bordo inferiore
    cout << string(offsetCentro, ' ');
    for (int j = 0; j < maxX + 2; j++) {
        cout << "# ";
    }
    cout << "\n";

    // Visualizza il punteggio
    cout << string(offsetCentro, ' ') << "Punteggio: " << punteggio << "\n";
}

// Funzione per gestire l'input del giocatore
void input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a': if (dir != DX) dir = SX; break;
            case 'd': if (dir != SX) dir = DX; break;
            case 'w': if (dir != GIU) dir = SU; break;
            case 's': if (dir != SU) dir = GIU; break;
            case 'x': fineGioco = true; break;
        }
    }
}

// Funzione per aggiornare la logica del gioco
void logic() {
    int prevX = codaX[0], prevY = codaY[0], prev2X, prev2Y;
    codaX[0] = x;
    codaY[0] = y;

    for (int i = 1; i < lunghezzaCoda; i++) {
        prev2X = codaX[i];
        prev2Y = codaY[i];
        codaX[i] = prevX;
        codaY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
        case SX: x--; break;
        case DX: x++; break;
        case SU: y--; break;
        case GIU: y++; break;
        default: break;
    }

    if (x >= maxX || x < 0 || y >= maxY || y < 0) fineGioco = true;

    for (int i = 0; i < lunghezzaCoda; i++) {
        if (codaX[i] == x && codaY[i] == y) fineGioco = true;
    }

    if (x == ciboX && y == ciboY) {
        punteggio += 10;
        ciboX = rand() % maxX;
        ciboY = rand() % maxY;
        lunghezzaCoda++;
    }
}

// Funzione per creare un nuovo nodo
nodo* creaNodo(char nome[30], int punteggio) {
    nodo* nuovoNodo = (nodo*)malloc(sizeof(nodo));
    if (nuovoNodo == NULL) {
        cout << "Errore di allocazione memoria\n";
        exit(1);
    }
    strcpy(nuovoNodo->nome, nome);
    nuovoNodo->punteggio = punteggio;
    nuovoNodo->prossimo = NULL;
    return nuovoNodo;
}

// Funzione per contare i nodi nella lista
int contaNodi(nodo* testa) {
    int conta = 0;
    while (testa != NULL) {
        conta++;
        testa = testa->prossimo;
    }
    return conta;
}

// Funzione per aggiungere un nodo in ordine nella lista
void aggiungiInOrdine(nodo** testa, char nome[30], int punteggio) {
    nodo* nuovoNodo = creaNodo(nome, punteggio);
    if (*testa == NULL || (*testa)->punteggio < punteggio) {
        nuovoNodo->prossimo = *testa;
        *testa = nuovoNodo;
    } else {
        nodo* temp = *testa;
        while (temp->prossimo != NULL && temp->prossimo->punteggio >= punteggio) {
            temp = temp->prossimo;
        }
        nuovoNodo->prossimo = temp->prossimo;
        temp->prossimo = nuovoNodo;
    }
}

// Funzione per stampare la lista dei punteggi
void stampaLista(nodo* testa) {
    cout << "Classifica punteggi:\n";
    int posizione = 1;
    while (testa != NULL && posizione <= 10) {
        cout << posizione << ". " << testa->nome << " - " << testa->punteggio << "\n";
        testa = testa->prossimo;
        posizione++;
    }
}
