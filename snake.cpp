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

#define MAX_FRUIT_AMOUNT 10

struct Point {
    int x, y;

    void print(char end='\n')
    {
        std::cout << "(" << x << ", " << y << ")" << end;
    }

    const bool operator==(const Point &other)
    {
        return this->x == other.x && this->y == other.y;
    }
};

struct Fruit {
    Point pos;
    bool isBad;
    bool active;

    Fruit()
    {
        pos.x = 0;
        pos.y = 0;
        isBad = false;
        active = false;
    }
};

class Node {
public:
    Point data;
    Node* next;
    Node(Point p) : data(p), next(nullptr) {}
};

class FruitManager {
    private:
    Fruit *fruits[MAX_FRUIT_AMOUNT];

    public:
    FruitManager()
    {
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            fruits[i] = new Fruit();
        }
    }

    ~FruitManager()
    {
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            delete fruits[i];
        }
    }

    bool fruitInPoint(Point p, bool isBad=false) const
    {
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            if (!fruits[i]->active) continue;
            if (!(fruits[i]->pos == p)) continue;
            if (fruits[i]->isBad == isBad) continue;
            return true;
        }

        return false;
    }

    void killFruits()
    {
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            fruits[i]->active = false;
        }
    }

    void spawnFruit(Point p, bool isBad)
    {
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            if (fruits[i]->active) continue;

            fruits[i]->active = true;
            fruits[i]->pos = p;
            fruits[i]->isBad = isBad;
        }
    }

    int fruitAmount()
    {
        int c=0;
        
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            if (fruits[i]->active) c++;
        }
        return c;
    }
};

//INSTRUCCIONES: Completa la implementación de los siguientes métodos CRUD, y añade la complejidad temporal y espacial.
class LinkedList {
private:
    Node* head;

public:

    //Prof: Faltaba el constructor
    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

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
        Node *tempNode = head;
        Point newPoint = newHead;

        while (tempNode)
        {
            Point lastPoint = tempNode->data;
            tempNode->data = newPoint;
            newPoint = lastPoint;
            
            if (grow && tempNode->next == nullptr)
            {
                Node* newNode = new Node(newPoint);
                tempNode->next = newNode;
                break;
            }

            tempNode = tempNode->next;
        }
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
    //Point badFruit;
    FruitManager fruitManager;
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
        generateFruitsAll();
        
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

    void generateFruitSingle(bool isBad=false) {
        do {
            fruit.x = rand() % (width - 2) + 1;
            fruit.y = rand() % (height - 2) + 1;
        } while (
            isOnSnake(fruit) || 
            fruitManager.fruitInPoint(fruit) ||
            fruitManager.fruitInPoint(fruit, false)
            );
        fruitManager.spawnFruit(fruit, isBad);
    }

    void generateFruitsAll()
    {
        fruitManager.killFruits();
        for (int i = 0; i < MAX_FRUIT_AMOUNT; i++)
        {
            generateFruitSingle(score > (i + 3) * 5);
        }
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
                else if (fruitManager.fruitInPoint(Point{j, i}))
                    mvprintw(i + 1, j + 1, "+"); 
                else if (fruitManager.fruitInPoint(Point{j, i}, true))
                    mvprintw(i + 1, j + 1, "x"); 
            }
            mvprintw(i + 1, width + 1, "#");
        }

        mvprintw(height + 3, 0, "Score: %d", score);
        mvprintw(height + 5, 0, "Fruits: %d", fruitManager.fruitAmount());
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

        if (fruitManager.fruitInPoint(newHeadPos)) {
            score += 10;
            generateFruitsAll();

            grow = true;
        }

        if (fruitManager.fruitInPoint(newHeadPos, true)) {
            int size = snake.countNodes(); 
            if (size > 1) {
                int removeIndex = (rand() % (size - 1)) + 1;
                snake.removeNodeAt(removeIndex);
            }
            generateFruitsAll();
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
            Sleep(300);
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