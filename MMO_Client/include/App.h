#pragma once
#include "../../MMO_Src/pch/raylibCpp.h"
//========SOME NOTE========//
//Confict: In wingdi.h, Ignore WINAPI HDC Rectangle(), cause previous declaration in raylib.h
//Confict: In wingdi.h, Ignore WINAPI CloseWindow (HWND hWnd), we just use the sane API in raylib.h
//Confict: In wingdi.h, Ignore WINAPI ShowCursor(WINBOOL bShow), we just use the sane API in raylib.h
#include "../../MMO_Src/network/tie_net.h"
//=========================//
#include "GUI.h"
#include "MineField.h"

class App : public tie::net::client_interface<GameMsg>
{
public:
    App(const int fps);
    ~App() noexcept;
    App (const App& another) = delete;
    App& operator = (const App& another) = delete;
    bool AppShouldClose() const;
    void Tick();
private:
    void Update();
    void Draw();

    void CreateField(const int width, const int height, const int nBooms);
    void DestroyField();

    void OnePlayerGame();
    void TwoPlayerGame();

    void ResetPlayer();

    bool IsGameOver = false;
    bool IsWin = false;

    GUI gui;
    MineField* pField = nullptr;
private:
    std::unordered_map< uint32_t, sPlayerDescription > mapObjects;
    uint32_t nPlayerID = 0;
    bool isHost = false;
    sPlayerDescription descPlayer;

    bool bWaitingForConnect = false;
};