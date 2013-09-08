/*
 * Noise generation for PuTTY's cryptographic random number
 * generator.
 */

#include <stdio.h>

#include "putty.h"
#include "ssh.h"
#include "storage.h"

<<<<<<< HEAD
HCRYPTPROV prov;

#define PROV_NOT_TRIED 0
#define PROV_LOADED_SUCCESS 1
#define PROV_ERROR -1
int prov_loaded = PROV_NOT_TRIED;

void noise_get_serious(void (*func) (void *, int)) {
    BYTE buf[256 / 8];
    if (PROV_NOT_TRIED == prov_loaded) {
        prov_loaded = CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, 0) ?
            PROV_LOADED_SUCCESS : PROV_ERROR;
        // what an odd API: http://msdn.microsoft.com/en-us/library/windows/desktop/aa382375(v=vs.85).aspx
        if (PROV_ERROR == prov_loaded && NTE_BAD_KEYSET == GetLastError()) {
            prov_loaded = CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET) ?
                PROV_LOADED_SUCCESS : PROV_ERROR;
        }
    }

    if (PROV_ERROR == prov_loaded)
        return;

    if (CryptGenRandom(prov, sizeof(buf), buf))
        func(buf, sizeof(buf));
}
=======
#include <wincrypt.h>

DECL_WINDOWS_FUNCTION(static, BOOL, CryptAcquireContextA,
                      (HCRYPTPROV *, LPCTSTR, LPCTSTR, DWORD, DWORD));
DECL_WINDOWS_FUNCTION(static, BOOL, CryptGenRandom,
                      (HCRYPTPROV, DWORD, BYTE *));
DECL_WINDOWS_FUNCTION(static, BOOL, CryptReleaseContext,
                      (HCRYPTPROV, DWORD));
static HMODULE wincrypt_module = NULL;
>>>>>>> upstream/master

/*
 * This function is called once, at PuTTY startup.
 */

void noise_get_heavy(void (*func) (void *, int))
{
    HANDLE srch;
    WIN32_FIND_DATA finddata;
    DWORD pid;
    HCRYPTPROV crypt_provider;
    char winpath[MAX_PATH + 3];

    GetWindowsDirectory(winpath, sizeof(winpath));
    strcat(winpath, "\\*");
    srch = FindFirstFile(winpath, &finddata);
    if (srch != INVALID_HANDLE_VALUE) {
	do {
	    func(&finddata, sizeof(finddata));
	} while (FindNextFile(srch, &finddata));
	FindClose(srch);
    }

    pid = GetCurrentProcessId();
    func(&pid, sizeof(pid));

<<<<<<< HEAD
    noise_get_serious(func);
=======
    if (!wincrypt_module) {
        wincrypt_module = load_system32_dll("advapi32.dll");
        GET_WINDOWS_FUNCTION(wincrypt_module, CryptAcquireContextA);
        GET_WINDOWS_FUNCTION(wincrypt_module, CryptGenRandom);
        GET_WINDOWS_FUNCTION(wincrypt_module, CryptReleaseContext);
    }

    if (wincrypt_module && p_CryptAcquireContextA &&
        p_CryptGenRandom && p_CryptReleaseContext &&
        p_CryptAcquireContextA(&crypt_provider, NULL, NULL, PROV_RSA_FULL,
                               CRYPT_VERIFYCONTEXT)) {
        BYTE buf[32];
        if (p_CryptGenRandom(crypt_provider, 32, buf)) {
            func(buf, sizeof(buf));
        }
        p_CryptReleaseContext(crypt_provider, 0);
    }
>>>>>>> upstream/master

    read_random_seed(func);
    /* Update the seed immediately, in case another instance uses it. */
    random_save_seed();
}

void random_save_seed(void)
{
    int len;
    void *data;

    if (random_active) {
	random_get_savedata(&data, &len);
	write_random_seed(data, len);
	sfree(data);
    }
}

/*
 * This function is called every time the random pool needs
 * stirring, and will acquire the system time in all available
 * forms.
 */
void noise_get_light(void (*func) (void *, int))
{
    SYSTEMTIME systime;
    DWORD adjust[2];
    BOOL rubbish;

    GetSystemTime(&systime);
    func(&systime, sizeof(systime));

    GetSystemTimeAdjustment(&adjust[0], &adjust[1], &rubbish);
    func(&adjust, sizeof(adjust));
}

/*
 * This function is called on a timer, and it will monitor
 * frequently changing quantities such as the state of physical and
 * virtual memory, the state of the process's message queue, which
 * window is in the foreground, which owns the clipboard, etc.
 */
void noise_regular(void)
{
    HWND w;
    DWORD z;
    POINT pt;
    MEMORYSTATUS memstat;
    FILETIME times[4];

    w = GetForegroundWindow();
    random_add_noise(&w, sizeof(w));
    w = GetCapture();
    random_add_noise(&w, sizeof(w));
    w = GetClipboardOwner();
    random_add_noise(&w, sizeof(w));
    z = GetQueueStatus(QS_ALLEVENTS);
    random_add_noise(&z, sizeof(z));

    GetCursorPos(&pt);
    random_add_noise(&pt, sizeof(pt));

    GlobalMemoryStatus(&memstat);
    random_add_noise(&memstat, sizeof(memstat));

    GetThreadTimes(GetCurrentThread(), times, times + 1, times + 2,
		   times + 3);
    random_add_noise(&times, sizeof(times));
    GetProcessTimes(GetCurrentProcess(), times, times + 1, times + 2,
		    times + 3);
    random_add_noise(&times, sizeof(times));
}

/*
 * This function is called on every keypress or mouse move, and
 * will add the current Windows time and performance monitor
 * counter to the noise pool. It gets the scan code or mouse
 * position passed in.
 */
void noise_ultralight(unsigned long data)
{
    DWORD wintime;
    LARGE_INTEGER perftime;

    random_add_noise(&data, sizeof(DWORD));

    wintime = GetTickCount();
    random_add_noise(&wintime, sizeof(DWORD));

    if (QueryPerformanceCounter(&perftime))
	random_add_noise(&perftime, sizeof(perftime));
}
