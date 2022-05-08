#include <hackpro_ext.h>
#include <fmt/core.h>
#include <gd.h>

DWORD WINAPI MainThread(LPVOID lpParam)
{
  if (InitialiseHackpro())
  {
    fmt::print("Oh hell naw");
    return S_FALSE;
  }

  return S_OK;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH)
    CreateThread(0, 0x1000, MainThread, NULL, 0, NULL);
  return TRUE;
}