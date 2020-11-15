#include <windows.h>
#include <fstream>

HANDLE   g_hidHandle;

/**
 * Initialize the g_hidHandle global variable
 * It will attempt to read paths from devicepath.dat file if it exists until it finds one which works.
 * If nothing is found, as a last resort it will try the default path from the Leonardo firmware.
 *
 * @return 0 on success, -1 on error
 */
static int controller_init(){
    FILE *file;
    char  path[256];
    
    file = fopen("devicepath.dat", "r");
    if (file == NULL)
    {
        goto last_resort;
    }

    while ( fgets(path,256,file) != NULL )
    {
        path[strcspn(path, "\r\n")] = 0;
        g_hidHandle = CreateFile(path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if ( g_hidHandle != INVALID_HANDLE_VALUE )
            break;
    }
    fclose(file);

    if ( g_hidHandle == INVALID_HANDLE_VALUE )
    {
        goto last_resort;
    }
    return 0;

last_resort:
    g_hidHandle = CreateFile("\\\\?\\HID#VID_2341&PID_8036&MI_02#8&149c8820&1&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}",
                             GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if ( g_hidHandle != INVALID_HANDLE_VALUE )
        return 0;

    printf("Couldn't open device. Make sure devicepath.dat exists and contains the correct path.\r\n");
    return -1;
}

int main(int argc, char* argv[])
{
    char     OutputReport[6];
    int      res;
    int      mode;
    DWORD	 BytesWritten = 0;

    if ( argc < 2 )
    {
        printf("Usage: %s mode\r\n",argv[0]);
        return 1;
    }

    mode = atoi(argv[1]);
    if ( mode < 0 || mode > 6 )
    {
        printf("Invalid mode value %d\r\n", mode);
        return 2;
    }
    
    if ( controller_init() == -1 )
    {
        printf("Couldn't open device. Make sure devicepath.dat contains the correct path.\r\n");
        return 3;
    }

    OutputReport[0] = 0x06;     //report ID
    OutputReport[1] = mode;
    OutputReport[2] = 0x00;
    OutputReport[3] = 0x00;
    OutputReport[4] = 0x00;
    OutputReport[5] = 0x00;
    
    WriteFile(g_hidHandle, OutputReport, 6, &BytesWritten, NULL);
    if ( BytesWritten == 6 )
    {
        res = 0;
        printf("Succesfully switched to mode %u ", mode);
        switch(mode)
        {
            case 0:
                printf("(reactive)\r\n");
                break;
            case 1:
                printf("(HID)\r\n");
                break;
            case 2:
                printf("(mixed)\r\n");
                break;
            case 3:
                printf("(combined)\r\n");
                break;
            case 4:
                printf("(combined invert)\r\n");
				break;
            case 5:
                printf("(rainbow)\r\n");
                break;
            case 6:
                printf("(TC)\r\n");
        }
    }
    else
    {
        printf("Error sending HID report (%u bytes written)\r\n",BytesWritten);
        res = 4;
    }

    CloseHandle(g_hidHandle);
    return res;
}
