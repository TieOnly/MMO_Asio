#pragma once
#include "Entity.h"

class GUI
{   
public:
    class Button : public Entity
    {
    public:
        Button( const RectF& dest, const std::string& title = "Button" );
        void Update();
        void Draw() const;
    private:
        std::string title = "";
    };
////////////////////////////////////////////////////////////////
public:
    enum class Layer
    {
        ChoseMode,
        ChoseSize,
        GameMode
    };
    enum class Btn_SeqID
    {
        One_Player = 0,
        Two_Player,
        Small,
        Medium,
        Large,
        Back,
        Non
    };
    struct GameMod
    {
        GUI::Btn_SeqID amount_player = Btn_SeqID::Non;
        GUI::Btn_SeqID game_size = Btn_SeqID::Non;

        bool isReset = false;
    };

    static void SetLayer( const Layer& l ) { CallLayer() = l; }
    static const Layer& GetLayer() { return CallLayer(); }
private:
    static Layer& CallLayer()
    {
        static Layer layer;
        return layer;
    }
public:

    void LoadAsset();

    void Update();
    
    void Draw() const;

    const GameMod& GetGameMod() const { return gameMod; }
    void ResetGameMode();

private:
    FileIO data;
    GameMod gameMod;

    int btn_amount = 0;
    std::vector<std::string> btn_titles;
    std::vector<RectF> btn_dests;

    std::vector<Button> buttons;
};