#include <stdio.h>
#include <stdlib.h>
 
// Дисперсия датчика gps (взял случайным образом)
#define GPS_DISP 16.0
// Дисперсия окружающей среды (взял случайным образом)
#define ENV_DISP 3.0

// Глобальный массив для записи координат поступающих с gps
float moving[3] = { 0 };

/* Структура для записи состояния фильтра */
typedef struct KalmanState
{
    // Координата x
    float x;
    // Координата у
    float y;
    // Координата z
    float z;
    // Скорость
    float v;
    // Ускорение
    float a;
    // Оценка положения
    float disp;
}State;

/* Функция которая симулирует показатели датчика gps */
static void emul_gps()
{
    // Увеличиваем показатели, которые были даны датчику объектом на случайную величину
    // Это симулирует дисперсию gps
    moving[0] += rand() % 15;
    moving[1] += rand() % 12;
    moving[2] += rand() % 16;
}

/*
* Функция которая симулирует движение объекта
* Формула x = x0 + vt + at^2/2
*/
static void emul_moving(State* state)
{
    // Запиминаю предыдущую скорость
    float pred_v = state->v;

    // Увеличиваю скорость на рандомное число
    state->v = state->v + rand() % 5;
    // Уменьшаю скорость на рандомное число
    state->v = state->v - rand() % 4;
    // Высчитываю ускорение по формуле (v- v0)/t , t = 1
    state->a = state->v - pred_v;

    // Меняем местоположение объекта, движение которого следует формуле
    // Увеличиваем показатели - это симулирует дисперсию окружающей среды
    moving[0] = state->x + state->v * 1 + (state->a * 1 / 2) + rand() % 3;
    moving[1] = state->y + state->v * 1 + (state->a * 1 / 2) + rand() % 3;
    moving[2] = state->z + state->v * 1 + (state->a * 1 / 2) + rand() % 3;

    // Передаем полученное местоположение датчику
    emul_gps();
}

/* 
* Предсказание по формуле x = x0 + vt + at ^ 2 / 2
* Каждый шаг t равен 1 секунде
*/
void predict(State* state)
{
    // Меняем значение x на новое значение по формуле
    state->x = state->x + state->v * 1 + (state->a * 1 / 2);
    // Меняем значение y на новое значение по формуле
    state->y = state->y + state->v * 1 + (state->a * 1 / 2);
    // Меняем значение z на новое значение по формуле
    state->z = state->z + state->v * 1 + (state->a * 1 / 2);
    // Увеличиваем дисперсию на дисперсию окружающей среды
    state->disp = state->disp + ENV_DISP;
}

/* Функция для обновления положения, относительно полученных данных */
void update(State* state)
{
    // Находим разницу между полученным значением и предсказанным
    float dif_x = moving[0] - state->x;
    float dif_y = moving[1] - state->y;
    float dif_z = moving[2] - state->z;

    // Находим максимальную дисперсию
    float max_disp = state->disp + GPS_DISP;
    // Находим коэффициент Калмана
    float koef_kalm = state->disp / max_disp;

    // Находим максимально близкое к настоящему положение
    state->x = state->x + koef_kalm * dif_x;
    state->y = state->y + koef_kalm * dif_y;
    state->z = state->z + koef_kalm * dif_z;

    // Уменьшаем неопределенность, учитывая новую информацию
    state->disp = (1 - koef_kalm) * state->disp;
}

/* Основная функция */
int main(void)
{
    State state;
    // Инициаллизация структуры
    // Задаем начальное положение 
    state.x = 0;
    state.y = 0;
    state.z = 0;
    // Начальная скорость
    state.v = 2.0;
    // Начальное ускорение
    state.a = 2.0;
    // Начальное значение дисперсии
    state.disp = 1.0;

    // Переменная для изменения размеров массивов
    int size = 1;
    // Индекс для записи в массивы
    int idx = 0;

    // Массивы для записи отфильтрованных значений
    float* graphic_x = malloc(sizeof(float) * size);
    float* graphic_y = malloc(sizeof(float) * size);
    float* graphic_z = malloc(sizeof(float) * size);

    // Массивы для записи неотфильтрованныз значений
    float* moving_x = malloc(sizeof(float) * size);
    float* moving_y = malloc(sizeof(float) * size);
    float* moving_z = malloc(sizeof(float) * size);

    // Бесконечный цикл 
    while (1)
    {
        // Если индекс не первый
        if (idx != 0)
        {
            // Увеличиваю память выделенную под все массивы в отдельные переменные
            float* x = realloc(graphic_x, sizeof(float) * size);
            float* y = realloc(graphic_y, sizeof(float) * size);
            float* z = realloc(graphic_z, sizeof(float) * size);

            float* mx = realloc(moving_x, sizeof(float) * size);
            float* my = realloc(moving_y, sizeof(float) * size);
            float* mz = realloc(moving_z, sizeof(float) * size);

            // Если произошла ошибка выделения памяти, то прекращение работы программы
            if (!x || !y || !z || !mx || !my || !mz)
            {
                fprintf(stderr, "Ошибка выделения памяти");
                // Очистка новой памяти
                free(x);
                free(y);
                free(z);
                free(mx);
                free(my);
                free(mz);

                // Очистка старой памяти
                free(graphic_x);
                free(graphic_y);
                free(graphic_z);
                free(moving_x);
                free(moving_y);
                free(moving_z);

                // Конец программы с ошибкой
                return 1;
            }
            // Если ошибки не произошло, то перезаписываем основные массивы
            else
            {
                graphic_x = x;
                graphic_y = y;
                graphic_z = z;

                moving_x = mx;
                moving_y = my;
                moving_z = mz;
            }
        }

        // Эмуляция движения объекта
        emul_moving(&state);
        // Записываем все координаты объекта в массивы
        moving_x[idx] = moving[0];
        moving_y[idx] = moving[1];
        moving_z[idx] = moving[2];

        // Если последние координаты больше 200, то заканчиваю цикл
        if (moving_x[idx] >= 200 || moving_y[idx] >= 200 || moving_z[idx] >= 200)
        {
            break;
        }

        // Предсказание дальнейшего положения
        predict(&state);

        // Обновление положения относительно полученного
        update(&state);
        // Записываем все отфильтрованные координаты объекта в массивы
        graphic_x[idx] = state.x;
        graphic_y[idx] = state.y;
        graphic_z[idx] = state.z;

        // Открываю процесс для вывода графика
        FILE* gp = _popen("gnuplot -persist", "w");

        // Задаю начальные размеры осей координат
        fprintf(gp, "set xrange [0:200]\n");
        fprintf(gp, "set yrange [0:200]\n");
        fprintf(gp, "set zrange [0:200]\n");

        // Создание 3D графика с использованием отфильтрованных не неотфильтрованных координат 
        fprintf(gp, "splot '-' using 1:2:3 with lines title 'Real', '-' using 1:2:3 with lines title 'GPS'\n");

        // Цикл для создания графика отфильтрованных координат
        for (int k = 0; k <= idx; k++)
        {
            fprintf(gp, "%0.6f\t%0.6f\t%0.6f\n", graphic_x[k], graphic_y[k], graphic_z[k]);
        }
        // Конец графика отфильтрованных координат
        fprintf(gp, "%s\n", "e");

        // Цикл для создания графика неотфильтрованных координат
        for (int k = 0; k <= idx; k++)
        {
            fprintf(gp, "%0.6f\t%0.6f\t%0.6f\n", moving_x[k], moving_y[k], moving_z[k]);
        }
        // Конец графика неотфильтрованных координат
        fprintf(gp, "%s\n", "e");

        // Конец создания графика
        _pclose(gp);
        
        // Увеличение индекса для записи координат 
        idx++;

        // Увеличение размера массивов
        size++;
    }
    
    // Освобождение памяти 
    free(graphic_x);
    free(graphic_y);
    free(graphic_z);
    free(moving_x);
    free(moving_y);
    free(moving_z);

    return 0;
}