#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#define SIZE_BUFFER 140

double perevod(char* c)

{
    double a;
    WORD prov = 0;
    WORD i = 0;
    while (c[i] != '\0') //пока не наткнемся на конец - ищем точку или запятую, чтобы отделить целую часть от дробной
    {
        if (c[i] == ',' || c[i] == '.')
        {
            prov = 1; //если находим, то прерываемся и записываем в значение 1, чтобы потом посмотреть дробь ли это
            break;
        }
        i++;
    }
    if (prov == 1) //если дробь
    {
        char* b = strtok(c, ",."); //целая часть отделяется
        a = atoi(b); //переводим целую часть из строки в число
        b = strtok(NULL, ",."); //дробная часть
        if (b != NULL)
        {
            double aa = atoi(b); //переводим дробную часть из строки в число
            WORD kol = 1;
            while (*b != '\0') //смотрим какая степень у дроби и делим на 10 каждый раз, чтобы целое число стало дробным
            {
                b++;
                aa = aa / 10;
            }
            if (a >= 0)
                a = a + aa; //прибавляем дробную часть к целой
            else
                a = a - aa;
        }
    }
    else
        a = atoi(c);//если число целое, то просто присваиваем
    return a;
}

int main()
{
    system("chcp 1251>nul");
    BOOL Zap = TRUE;
    DWORD i = -1;
    HANDLE hNamedPipe; //дескриптор канала
    LPSTR lpszPipeName = L"\\\\.\\pipe\\MyPipe";//имя канала
    BOOL Connected;
    DWORD sizeBuffer = SIZE_BUFFER;
    DWORD actualReaden;
    BOOL SuccessRead;
    LPWSTR buffer = (CHAR*)calloc(sizeBuffer, sizeof(CHAR));
    char* message = calloc(sizeBuffer, sizeof(char));
    while (TRUE)
    {
        hNamedPipe = CreateNamedPipe(lpszPipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 512, 512, INFINITE, NULL); //Инициалиция канала
        if (Zap)
        {
            printf("\nСервер запущен!\n");
            Zap = FALSE;
        }
        if (i == -2)
        {
            printf("Клиент отключен!");
            i++;
        }
        Connected = ConnectNamedPipe(hNamedPipe, NULL); //создаём соеднинение
        if (Connected) //проверяем соединение
        {
            if (i == -1)
            {
                printf("\nКлиент подключился\n");
                i++;
            }
           
            SuccessRead = ReadFile(hNamedPipe, buffer, sizeBuffer, &actualReaden, NULL);
            
            if (!SuccessRead)
            {
                if (i == 30) //если клиент не отвечает около 30 секунд
                {
                    printf("\nКлиент долго не отвечает, повторяем подключение...\n");
                    i = -3;
                }
               Sleep(1000);
                i++;
            }
            if (SuccessRead)
            {
                printf(buffer);
                char* st = malloc(sizeof(char) * 100);
                char* stt = st;
                char* c = buffer;
                while (*c != '\0') //перезаписываем lpswtr в char* для разбивки + проверка на корректность данных
                {
                    c--;
                    char cmin = *c; //предыдущий символ
                    c++;
                    c++;
                    char cmax = *c; //следующий символ
                    c--;
                    *st = *c;
                    if (*st == '\r')
                    {
                        *st = '\n';
                        c++;
                    }//      число                    или пробел   или перенос строки   или ;     или минус      в начале строки    и не после чисел               и перед числом                      или , или .               но только между числами
                    if ((*st >= '0' && *st <= '9') || *st == ' ' || *st == '\n' || *st == ';' || (*st == '-' && (st == stt || (!(cmin >= '0' && cmin <= '9') && (cmax >= '0' && cmax <= '9')))) || ((*st == ',' || *st == '.') && !(cmin == ',' || cmin == '.' || cmin == '-') && (cmax >= '0' && cmax <= '9')))
                    {
                        st++;
                        c++;
                    }
                    else
                    {
                        buffer = L"Неверный формат входных данных!";
                        break;
                    }
                }
                if (st != stt)
                {
                    *st = L'\0';
                    double kv = perevod(stt);
                    kv = kv * kv;
                    char* op = malloc(sizeof(wchar_t) * 150);
                    sprintf(op, "%f", kv); //записываем данные в чар
                    wchar_t* w = calloc(SIZE_BUFFER,sizeof(CHAR));
                    mbstowcs(w, op, sizeBuffer);
                    buffer = w;
                }
                WriteFile(hNamedPipe, buffer, sizeBuffer, &actualReaden, NULL);
                buffer = (CHAR*)calloc(sizeBuffer, sizeof(CHAR));
            }
            CloseHandle(hNamedPipe);
        }
    }
    printf("\nСервер отключен!\n");
    Zap = TRUE;
}
