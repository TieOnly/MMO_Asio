#pragma once
#include "Entity.h"

class GUI
{   
//==================BUTTONS==================//
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
//==================INPUT==================//
public:
    class Input: public Entity
    {
    public:
        Input( const RectF& dest );
        void Update();
        const std::string& GetValue() const;
        void Forcus();
        void ClearValue();
        void Draw() const;
    private:
        void PutData();
        void UpdateValueAndCursor();
    private:
        std::string value;
        std::string draw_value = "";
        bool isTyping = false;
        
        int measure;
        Vec2 pos_cursor;
        float padding = 6.0f;
        bool blink = false;
        float counterBlink = 0.0f;
        float duraBlink = 0.4f;
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
        Change_Name,
        Chat,
        Back,
        Non
    };
    enum class Input_SeqID
    {
        Line_1
    };
    struct GameMod
    {
        GUI::Btn_SeqID amount_player = Btn_SeqID::Non;
        GUI::Btn_SeqID game_size = Btn_SeqID::Non;
        std::string input_value = "";
        
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
    void ClearInputValue();

private:
    FileIO data;
    GameMod gameMod;

    //Button variables
    int btn_amount = 0;
    std::vector<std::string> btn_titles;
    std::vector<RectF> btn_dests;

    std::vector<Button> buttons;

    //Input variables
    int input_amount = 0;
    std::vector<RectF> input_dests;
    std::vector<Input> inputs;
};