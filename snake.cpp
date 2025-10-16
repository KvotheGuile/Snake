#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime> 
  
#ifdef _WIN32
  #include <curses.h>
  #include <windows.h>  
#else
  #include <ncurses.h>  
  #include <unistd.h>   
#endif

struct Point {
    int x, y;

    void print(char end='\n')
    {
        std::cout << "(" << x << ", " << y << ")" << end;
    }
};

class Node {
public:
    Point data;
    Node* next;
    Node(Point p) : data(p), next(nullptr) {}
};


//INSTRUCCIONES: Completa la implementación de los siguientes métodos CRUD, y añade la complejidad temporal y espacial.
class LinkedList {
private:
    Node* head;

public:
    LinkedList(); 
    // Constructor: inicializa la lista enlazada estableciendo el puntero head en nullptr.

    ~LinkedList(); 
    // Destructor: libera toda la memoria dinámica utilizada por los nodos de la lista.

    // CREATE
    void insertAtHead(Point p)
    {
        head = new Node(p);
    }
    // Inserta un nuevo nodo al inicio de la lista que contenga el punto especificado.

    // READ
    Node* getHead() const
    {
        return head;
    }
    // Devuelve un puntero al nodo cabeza de la lista.

    int countNodes() const
    {
        int c = 0;
        Node* temp = head;

        while (temp != nullptr)
        {
            c++;
            temp = temp -> next;
        }

        return c;
    }
    // Cuenta y devuelve el número total de nodos presentes en la lista.

    void printNodes() const
    {
        Node* temp = head;

        while (temp != nullptr)
        {
            temp->data.print();
            temp = temp -> next;
        }
    }
    // Imprime por consola las coordenadas (x, y) de cada nodo en la lista, en orden.

    // UPDATE
    void updatePositions(const Point& newHead, bool grow)
    {
        Node* newHeadNode = new Node(newHead);
        if (head != nullptr)
            newHeadNode = head->next;
        head = newHeadNode;

        if (!grow) removeTail();
    }
    // Actualiza las posiciones de todos los nodos desplazando los valores a lo largo de la lista.
    // Si el parámetro grow es verdadero, se añade un nuevo nodo al final de la lista con la posición anterior de la cola.

    // REMOVE
    void removeTail()
    {
        int s = countNodes();
        if (s == 0) return;
        removeNodeAt(s - 1);
    }
    // Elimina el último nodo (cola) de la lista, liberando su memoria.

    void removeNodeAt(int index)
    {
        if (head == nullptr) return;
        
        int c = 0;
        Node* back = head;
        Node* front = head;
        
        if (index == 0) head = head -> next;

        while (front != nullptr)
        {
            if (c == index){
                back->next = front->next;

                delete front;
                break;
            }

            back = front;
            front = front -> next;
            c++;
        }
    }
    // Elimina el nodo que se encuentra en la posición indicada por el índice (empezando en 0).
    // Si el índice no es válido o la lista está vacía, no realiza ninguna acción.


};


class Game {
private:
    int width, height;
    Point fruit;
    Point badFruit;
    int score;
    bool gameOver;
    enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
    eDirection dir;
    LinkedList snake;

    void setup() {
        initscr();            
        clear();              
        noecho();             
        cbreak();             
        curs_set(0);          
        getmaxyx(stdscr, height, width); 
        width -= 5; 
        height -= 5;

        score = 0;
        gameOver = false;
        dir = STOP;
        snake.insertAtHead({ width / 2, height / 2 });
        generateFruit();
        badFruit = {-1, -1};
        
        nodelay(stdscr, TRUE);
        keypad(stdscr, TRUE); 
    }

    bool isOnSnake(Point p) const {
        Node* cur = snake.getHead();
        while (cur != nullptr) {
            if (cur->data.x == p.x && cur->data.y == p.y) return true;
            cur = cur->next;
        }
        return false;
    }

    void generateFruit() {
        do {
            fruit.x = rand() % (width - 2) + 1;
            fruit.y = rand() % (height - 2) + 1;
        } while (isOnSnake(fruit) || (badFruit.x != -1 && fruit.x == badFruit.x && fruit.y == badFruit.y));
    }

    void generateBadFruit() {
        do {
            badFruit.x = rand() % (width - 2) + 1;
            badFruit.y = rand() % (height - 2) + 1;
        } while (isOnSnake(badFruit) || (badFruit.x == fruit.x && badFruit.y == fruit.y));
    }

    void draw() {
        clear();

        for (int i = 0; i < width + 2; i++) {
            mvprintw(0, i, "#");
            mvprintw(height + 1, i, "#");
        }

        for (int i = 0; i < height; i++) {
            mvprintw(i + 1, 0, "#");
            for (int j = 0; j < width; j++) {
                bool isSnakePart = false;
                Node* current = snake.getHead();
                while (current != nullptr) {
                    if (current->data.x == j && current->data.y == i) {
                        isSnakePart = true;
                        break;
                    }
                    current = current->next;
                }

                if (snake.getHead() && i == snake.getHead()->data.y && j == snake.getHead()->data.x)
                    mvprintw(i + 1, j + 1, "O");
                else if (isSnakePart)
                    mvprintw(i + 1, j + 1, "o");
                else if (i == fruit.y && j == fruit.x)
                    mvprintw(i + 1, j + 1, "+"); 
                else if (badFruit.x != -1 && i == badFruit.y && j == badFruit.x)
                    mvprintw(i + 1, j + 1, "x"); 
            }
            mvprintw(i + 1, width + 1, "#");
        }

        mvprintw(height + 3, 0, "Score: %d", score);
        refresh();
    }

    void input() {
        int ch = getch();
        switch (ch) {
            case 'a': case KEY_LEFT:  if (dir != RIGHT) dir = LEFT; break;
            case 'd': case KEY_RIGHT: if (dir != LEFT)  dir = RIGHT; break;
            case 'w': case KEY_UP:    if (dir != DOWN)  dir = UP; break;
            case 's': case KEY_DOWN:  if (dir != UP)    dir = DOWN; break;
            case 'x': gameOver = true; break;
        }
    }

    bool checkSelfCollision(Point headPos) {
        Node* current = snake.getHead(); 
        if (current) current = current->next; 
        while (current != nullptr) {
            if (current->data.x == headPos.x && current->data.y == headPos.y) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void logic() {
        if (!snake.getHead()) return;

        Point newHeadPos = snake.getHead()->data;

        switch (dir) {
            case LEFT:  newHeadPos.x--; break;
            case RIGHT: newHeadPos.x++; break;
            case UP:    newHeadPos.y--; break;
            case DOWN:  newHeadPos.y++; break;
            default:    return;
        }

        if (newHeadPos.x >= width || newHeadPos.x < 0 || newHeadPos.y >= height || newHeadPos.y < 0) {
            gameOver = true;
            return;
        }

        if (checkSelfCollision(newHeadPos)) {
            gameOver = true;
            return;
        }

        bool grow = false;

        if (newHeadPos.x == fruit.x && newHeadPos.y == fruit.y) {
            score += 10;
            generateFruit();
            generateBadFruit();

            grow = true;
        }

        if (newHeadPos.x == badFruit.x && newHeadPos.y == badFruit.y) {
            int size = snake.countNodes(); 
            if (size > 1) {
                int removeIndex = (rand() % (size - 1)) + 1;
                snake.removeNodeAt(removeIndex);
            }
            generateFruit();
            generateBadFruit();
        }

        snake.updatePositions(newHeadPos, grow);
    }

public:
    void run() {
        setup();
        while (!gameOver) {
            draw();
            input();
            logic();
            #ifdef _WIN32
            Sleep(100);
            #else
            usleep(100000);
            #endif
        }
        endwin();
        std::cout << "Game Over! Final Score: " << score << std::endl;
    }
};

int main() {
    srand(time(0));
    Game game;
    game.run();
    return 0;
}