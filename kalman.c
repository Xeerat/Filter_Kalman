#include <stdio.h>
#include <stdlib.h>
 
// ��������� ������� gps (���� ��������� �������)
#define GPS_DISP 16.0
// ��������� ���������� ����� (���� ��������� �������)
#define ENV_DISP 3.0

// ���������� ������ ��� ������ ��������� ����������� � gps
float moving[3] = { 0 };

/* ��������� ��� ������ ��������� ������� */
typedef struct KalmanState
{
    // ���������� x
    float x;
    // ���������� �
    float y;
    // ���������� z
    float z;
    // ��������
    float v;
    // ���������
    float a;
    // ������ ���������
    float disp;
}State;

/* ������� ������� ���������� ���������� ������� gps */
static void emul_gps()
{
    // ����������� ����������, ������� ���� ���� ������� �������� �� ��������� ��������
    // ��� ���������� ��������� gps
    moving[0] += rand() % 15;
    moving[1] += rand() % 12;
    moving[2] += rand() % 16;
}

/*
* ������� ������� ���������� �������� �������
* ������� x = x0 + vt + at^2/2
*/
static void emul_moving(State* state)
{
    // ��������� ���������� ��������
    float pred_v = state->v;

    // ���������� �������� �� ��������� �����
    state->v = state->v + rand() % 5;
    // �������� �������� �� ��������� �����
    state->v = state->v - rand() % 4;
    // ���������� ��������� �� ������� (v- v0)/t , t = 1
    state->a = state->v - pred_v;

    // ������ �������������� �������, �������� �������� ������� �������
    // ����������� ���������� - ��� ���������� ��������� ���������� �����
    moving[0] = state->x + state->v * 1 + (state->a * 1 / 2) + rand() % 3;
    moving[1] = state->y + state->v * 1 + (state->a * 1 / 2) + rand() % 3;
    moving[2] = state->z + state->v * 1 + (state->a * 1 / 2) + rand() % 3;

    // �������� ���������� �������������� �������
    emul_gps();
}

/* 
* ������������ �� ������� x = x0 + vt + at ^ 2 / 2
* ������ ��� t ����� 1 �������
*/
void predict(State* state)
{
    // ������ �������� x �� ����� �������� �� �������
    state->x = state->x + state->v * 1 + (state->a * 1 / 2);
    // ������ �������� y �� ����� �������� �� �������
    state->y = state->y + state->v * 1 + (state->a * 1 / 2);
    // ������ �������� z �� ����� �������� �� �������
    state->z = state->z + state->v * 1 + (state->a * 1 / 2);
    // ����������� ��������� �� ��������� ���������� �����
    state->disp = state->disp + ENV_DISP;
}

/* ������� ��� ���������� ���������, ������������ ���������� ������ */
void update(State* state)
{
    // ������� ������� ����� ���������� ��������� � �������������
    float dif_x = moving[0] - state->x;
    float dif_y = moving[1] - state->y;
    float dif_z = moving[2] - state->z;

    // ������� ������������ ���������
    float max_disp = state->disp + GPS_DISP;
    // ������� ����������� �������
    float koef_kalm = state->disp / max_disp;

    // ������� ����������� ������� � ���������� ���������
    state->x = state->x + koef_kalm * dif_x;
    state->y = state->y + koef_kalm * dif_y;
    state->z = state->z + koef_kalm * dif_z;

    // ��������� ����������������, �������� ����� ����������
    state->disp = (1 - koef_kalm) * state->disp;
}

/* �������� ������� */
int main(void)
{
    State state;
    // �������������� ���������
    // ������ ��������� ��������� 
    state.x = 0;
    state.y = 0;
    state.z = 0;
    // ��������� ��������
    state.v = 2.0;
    // ��������� ���������
    state.a = 2.0;
    // ��������� �������� ���������
    state.disp = 1.0;

    // ���������� ��� ��������� �������� ��������
    int size = 1;
    // ������ ��� ������ � �������
    int idx = 0;

    // ������� ��� ������ ��������������� ��������
    float* graphic_x = malloc(sizeof(float) * size);
    float* graphic_y = malloc(sizeof(float) * size);
    float* graphic_z = malloc(sizeof(float) * size);

    // ������� ��� ������ ����������������� ��������
    float* moving_x = malloc(sizeof(float) * size);
    float* moving_y = malloc(sizeof(float) * size);
    float* moving_z = malloc(sizeof(float) * size);

    // ����������� ���� 
    while (1)
    {
        // ���� ������ �� ������
        if (idx != 0)
        {
            // ���������� ������ ���������� ��� ��� ������� � ��������� ����������
            float* x = realloc(graphic_x, sizeof(float) * size);
            float* y = realloc(graphic_y, sizeof(float) * size);
            float* z = realloc(graphic_z, sizeof(float) * size);

            float* mx = realloc(moving_x, sizeof(float) * size);
            float* my = realloc(moving_y, sizeof(float) * size);
            float* mz = realloc(moving_z, sizeof(float) * size);

            // ���� ��������� ������ ��������� ������, �� ����������� ������ ���������
            if (!x || !y || !z || !mx || !my || !mz)
            {
                fprintf(stderr, "������ ��������� ������");
                // ������� ����� ������
                free(x);
                free(y);
                free(z);
                free(mx);
                free(my);
                free(mz);

                // ������� ������ ������
                free(graphic_x);
                free(graphic_y);
                free(graphic_z);
                free(moving_x);
                free(moving_y);
                free(moving_z);

                // ����� ��������� � �������
                return 1;
            }
            // ���� ������ �� ���������, �� �������������� �������� �������
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

        // �������� �������� �������
        emul_moving(&state);
        // ���������� ��� ���������� ������� � �������
        moving_x[idx] = moving[0];
        moving_y[idx] = moving[1];
        moving_z[idx] = moving[2];

        // ���� ��������� ���������� ������ 200, �� ���������� ����
        if (moving_x[idx] >= 200 || moving_y[idx] >= 200 || moving_z[idx] >= 200)
        {
            break;
        }

        // ������������ ����������� ���������
        predict(&state);

        // ���������� ��������� ������������ �����������
        update(&state);
        // ���������� ��� ��������������� ���������� ������� � �������
        graphic_x[idx] = state.x;
        graphic_y[idx] = state.y;
        graphic_z[idx] = state.z;

        // �������� ������� ��� ������ �������
        FILE* gp = _popen("gnuplot -persist", "w");

        // ����� ��������� ������� ���� ���������
        fprintf(gp, "set xrange [0:200]\n");
        fprintf(gp, "set yrange [0:200]\n");
        fprintf(gp, "set zrange [0:200]\n");

        // �������� 3D ������� � �������������� ��������������� �� ����������������� ��������� 
        fprintf(gp, "splot '-' using 1:2:3 with lines title 'Real', '-' using 1:2:3 with lines title 'GPS'\n");

        // ���� ��� �������� ������� ��������������� ���������
        for (int k = 0; k <= idx; k++)
        {
            fprintf(gp, "%0.6f\t%0.6f\t%0.6f\n", graphic_x[k], graphic_y[k], graphic_z[k]);
        }
        // ����� ������� ��������������� ���������
        fprintf(gp, "%s\n", "e");

        // ���� ��� �������� ������� ����������������� ���������
        for (int k = 0; k <= idx; k++)
        {
            fprintf(gp, "%0.6f\t%0.6f\t%0.6f\n", moving_x[k], moving_y[k], moving_z[k]);
        }
        // ����� ������� ����������������� ���������
        fprintf(gp, "%s\n", "e");

        // ����� �������� �������
        _pclose(gp);
        
        // ���������� ������� ��� ������ ��������� 
        idx++;

        // ���������� ������� ��������
        size++;
    }
    
    // ������������ ������ 
    free(graphic_x);
    free(graphic_y);
    free(graphic_z);
    free(moving_x);
    free(moving_y);
    free(moving_z);

    return 0;
}