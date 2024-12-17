#include <iostream>
#include <conio.h>  // Para kbhit() y getch()
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

using namespace std;

void run();
void printMap();
void initMap();
void move(int dx, int dy);
void update();
void changeDirection(char key);
void clearScreen();
void generateFood();

char getValueMap(int value);

const int mapWidth = 30;
const int mapHeight = 20;
const int mapSize = mapWidth * mapHeight;
int map[mapSize];

int headxpos;
int headypos;
int direction; 

int food = 4;

atomic<bool> running(true);  
mutex mtx;  

int main()
{
    run();
    return 0;
}

void run()
{
    initMap();  // Inicializa el mapa y la serpiente
    direction = 1;  // Establecemos una dirección inicial (por ejemplo, derecha)

    while (running)
    {
        if (_kbhit())  // Si se ha presionado una tecla
        {
            char key = _getch();  // Lee la tecla presionada
            changeDirection(key);  // Cambia la dirección de la serpiente
        }

        update();  // Actualiza el estado del juego

        clearScreen();  // Limpia la pantalla

        printMap();  // Imprime el mapa actualizado

        // Ajusta la velocidad de la actualización del juego (más rápido)
        this_thread::sleep_for(chrono::milliseconds(200));  // Espera 400 ms entre cada ciclo
    }

    cout << "\t\tGame Over!" << endl << "\t\tYour score is: " << food << endl;
    cin.ignore();
}

void changeDirection(char key)
{
    lock_guard<mutex> lock(mtx);  // Protege la dirección con un lock para evitar problemas de concurrencia

    // Cambia la dirección según la tecla presionada
    switch (key)
    {
    case 'w':  // Arriba
        if (direction != 2) direction = 0;  // No se puede ir hacia abajo si ya se va hacia arriba
        break;
    case 'd':  // Derecha
        if (direction != 3) direction = 1;  // No se puede ir hacia la izquierda si ya se va hacia la derecha
        break;
    case 's':  // Abajo
        if (direction != 0) direction = 2;  // No se puede ir hacia arriba si ya se va hacia abajo
        break;
    case 'a':  // Izquierda
        if (direction != 1) direction = 3;  // No se puede ir hacia la derecha si ya se va hacia la izquierda
        break;
    }
}

// Mueve la cabeza del jugador y actualiza el cuerpo
void move(int dx, int dy)
{
    int newx = headxpos + dx;
    int newy = headypos + dy;

    // Verifica si la nueva posición está fuera del mapa o si la serpiente colisiona con su cuerpo
    if (newx < 0 || newy < 0 || newx >= mapWidth || newy >= mapHeight || map[newx + newy * mapWidth] > 0)
    {
        running = false;  // Termina el juego si hay colisión
    }
    else
    {
        // Si la nueva posición tiene comida (-2), incrementa la puntuación y genera nueva comida
        if (map[newx + newy * mapWidth] == -2)
        {
            food++;
            generateFood();  // Genera nueva comida
        }
        else {
         
            for (int i = 0; i < mapSize; i++)
            {

                if (map[i] > 0) map[i]--;  // Aumenta el valor de las partes del cuerpo de la serpiente

            }
        }
        headxpos = newx;
        headypos = newy;
        map[headxpos + headypos * mapWidth] = food + 1;  // Marca la cabeza en el mapa


        
    }
}

// Limpia la pantalla
void clearScreen()
{
    system("cls");  // Comando para limpiar la pantalla en Windows
}

// Genera la comida en una posición aleatoria
void generateFood()
{
    int x = 0, y = 0;
    do
    {
        // Genera una posición aleatoria para la comida
        x = rand() % (mapWidth - 2) + 1;
        y = rand() % (mapHeight - 2) + 1;
    } while (map[x + y * mapWidth] != 0);  // Asegúrate de que el espacio esté vacío

    // Coloca la comida en el mapa
    map[x + y * mapWidth] = -2;
}

// Actualiza el estado del juego
void update()
{
    lock_guard<mutex> lock(mtx);  // Protege la dirección durante la actualización

    // Mueve la serpiente en función de la dirección actual
    switch (direction)
    {
    case 0: move(0, -1); break;  // Arriba (dy = -1)
    case 1: move(1, 0); break;   // Derecha (dx = +1)
    case 2: move(0, 1); break;   // Abajo (dy = +1)
    case 3: move(-1, 0); break;  // Izquierda (dx = -1)
    }
}

// Inicializa el mapa y coloca la serpiente en el centro
void initMap()
{
    headxpos = mapWidth / 2;
    headypos = mapHeight / 2;
    map[headxpos + headypos * mapWidth] = 1;  // Coloca la cabeza de la serpiente en el centro

    // Coloca las paredes
    for (int x = 0; x < mapWidth; ++x)
    {
        map[x] = -1;
        map[x + (mapHeight - 1) * mapWidth] = -1;
    }

    for (int y = 0; y < mapHeight; y++)
    {
        map[0 + y * mapWidth] = -1;
        map[(mapWidth - 1) + y * mapWidth] = -1;
    }

    generateFood();  // Genera la primera comida
}

// Retorna el valor del mapa como un caracter para representar cada elemento
char getValueMap(int value)
{
    if (value > 0) return 'o';  // Cuerpo de la serpiente
    if (value == -1) return 'X';  // Pared
    if (value == -2) return 'O';  // Comida
    return ' ';
}

// Imprime el mapa en pantalla
void printMap()
{
    for (int y = 0; y < mapHeight; ++y)
    {
        for (int x = 0; x < mapWidth; ++x)
        {
            cout << getValueMap(map[x + y * mapWidth]);  // Imprime cada valor en el mapa
        }
        cout << endl;  // Nueva línea después de cada fila
    }
}
