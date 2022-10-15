#pragma once
#include "Entity.h"

class GUI
{ 
public:
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
        ReadyState,
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
        GUI::Btn_SeqID input_owner = Btn_SeqID::Non;
        std::string input_value = "";

        bool isTypeChating = false;
        bool isReset = false;
    };
//==================BUTTONS==================//
public:
    class Button : public Entity
    {
    public:
        Button( const RectF& dest, const std::string& title = "Button" );
        void Update();
        void Draw() const;

        void SetTitle( const std::string& title );
        bool isChoosen = false;
        bool isAbleActive = true;
        bool isHighLight = false;
        bool AbleDrawOutline = true;
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
        void OnDisable( GUI::GameMod& gameMod );
        void ClearValue();
        void SetDest( const RectF& dest ) override;
        void Draw() const;
        bool isTyping = false;
    private:
        void PutData();
        void UpdateValueAndCursor();
        void OnDisable();
    private:
        std::string value;
        std::string draw_value = "";
        
        int measure;
        Vec2 pos_cursor;
        float padding = 6.0f;
        bool blink = false;
        float counterBlink = 0.0f;
        float duraBlink = 0.4f;
    };
//==================RPSGame==================//
public:
    class RPSGame
    {
    public:
        enum class SeqID_Btn
        {
            Rock, Paper, Scissor
        };
        enum class TYPE_CHOOSE_BTN
        {
            Radio,
            Tick
        };
        void LoadData( const std::string& filename );
        void Update();
        void Draw() const;

        void SetSubDesc( const std::string& text );
        void StyleAllBtnNormal();
        void StyleBtnNormal( Button& btn );
        void StyleBtnBeChoosen( Button& btn );
        bool IsBtnClick( const SeqID_Btn& idBtn ) const;
        void ResetSub();
        TYPE_CHOOSE_BTN type_choose_btn = TYPE_CHOOSE_BTN::Radio;
        struct sSubNotify
        {
            int time = 5;
            std::string text;
        } sub_desc;
    private:
        FileIO data;
        RectF dest_origin_title{};
        RectF dest_origin_sub_desc{};
        RectF dest_origin_btns{};
        std::string title;

        //Button variables
        std::vector<GUI::Button> buttons;
        int btn_amount = 0;
        std::vector<std::string> btn_titles;
        Vec2 btn_dimen;
        float btn_between_distance = 0.0f;

        bool isExistBtnBeChoosen = false;
    };
////////////////////////////////////////////////////////////////
public:
    enum class Layer
    {
        ChoseMode,
        ChoseSize,
        GameMode
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

    //
    bool IsBtnClick( const Btn_SeqID& idBtn ) const;
    void Btn_UpdateReadyState( const bool isReady );

    std::vector<Button> buttons;
    std::vector<Input> inputs;
    RPSGame rpsGame;
private:
    void OnEnableInput( const GUI::Btn_SeqID& owner, GUI::Input& input, const float lenght = 100.0f );
    void OnDisableInput( const GUI::Btn_SeqID& owner, GUI::Input& input );
private:
    FileIO data;
    GameMod gameMod;

    //Button variables
    int btn_amount = 0;
    std::vector<std::string> btn_titles;
    std::vector<RectF> btn_dests;

    //Input variables
    int input_amount = 0;
    std::vector<RectF> input_dests;
};