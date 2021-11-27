#include <stdio.h>
#include <Windows.h>
#define SIZE_BUFFER 140

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
    char message[SIZE_BUFFER];
    while (TRUE)
    {
        hNamedPipe = CreateNamedPipe(lpszPipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 512, 512, INFINITE, NULL); //Инициалиция канала
        if (Zap)
        {
            printf("\nСервер запущен!\n");
            Zap = FALSE;
        }
        Connected = ConnectNamedPipe(hNamedPipe, NULL); //создаём соеднинение
        if (Connected) //проверяем соединение
        {
            if (i == -1)
            {
                printf("\nКлиент подключился\n");
                i = 0;
            }
           
            SuccessRead = ReadFile(hNamedPipe, buffer, sizeBuffer, &actualReaden, NULL);
            
            while (!SuccessRead)
            {
                if (i > 100)
                {
                    printf("\nКлиент отключился!\n");
                    i = -1;
                    break;
                }
                i++;
                SuccessRead = ReadFile(hNamedPipe, buffer, sizeBuffer, &actualReaden, NULL);
            }
            if (SuccessRead)
            {
                printf("Клиент пишет: ");
                printf(buffer);
                printf("\n");

                printf("Введите сообщение для клиента:\n");
                gets(message);
                buffer = &message;
                WriteFile(hNamedPipe, buffer, sizeBuffer, &actualReaden, NULL);
            }
            CloseHandle(hNamedPipe);
        }
        else 
        {
            buffer = "";
            printf("\nКлиент отключился!\n");
            i = -1;
        }
    }
    printf("\nСервер отключен!\n");
    Zap = TRUE;
}
