#include <stdio.h>
#include <Windows.h>

int main()
{
    system("chcp 1251>nul");
    BOOL Zap = TRUE;
    DWORD i = -1;
    HANDLE hNamedPipe; //дескриптор канала
    LPSTR lpszPipeName = L"\\\\.\\pipe\\MyPipe";//имя канала
    BOOL Connected;
    DWORD sizeBuffer = 100;
    DWORD actualReaden;
    BOOL SuccessRead;
    LPWSTR buffer;
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
            
            buffer = (CHAR*)calloc(sizeBuffer, sizeof(CHAR));
           
            SuccessRead = ReadFile(hNamedPipe, buffer, sizeBuffer, &actualReaden, NULL);
            printf(buffer);
            if (!SuccessRead)
            {
                //if (i > 1)
                {
                    printf("\nКлиент отключился!\n");
                    i = -2;
                }
                i++;
            }
            else
            {
                i = 0;
            }
            
            CloseHandle(hNamedPipe);
        }
    }
    printf("\nСервер отключен!\n");
    Zap = TRUE;
}
