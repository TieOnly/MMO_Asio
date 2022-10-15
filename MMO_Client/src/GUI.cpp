#include "../../MMO_Src/pch/raylibCpp.h"
#include "GUI.h"

void GUI::LoadAsset()
{
    data.NewBuff();
    if( data.LoadFromFile( "Config/config.txt" ) )
    {
        std::stringstream buff( data.GetBuff() );
        std::string token;
        while ( std::getline( buff, token ) && token != "//" )
        {
            if( token == "[Button_Amount]" )
            {
                std::getline( buff, token );
                btn_amount = std::stoi(token);
            }
            else if( token == "[Button_Titles]" )
            {
                while ( std::getline( buff, token ) && token != "#" )
                {
                    btn_titles.push_back( token );
                }
            }
            else if( token == "[Button_Dests]" )
            {
                while ( std::getline( buff, token ) && token != "#" )
                {
                    std::stringstream buff_c(token);
                    std::string token_c;
                    std::vector<float> angle_rect;
                    while ( std::getline( buff_c, token_c, ' ' ) && (token_c != "") )
                    {
                        angle_rect.push_back(std::stof(token_c));
                    }
                    btn_dests.push_back( RectF{ angle_rect[0], angle_rect[1], angle_rect[2], angle_rect[3] } );
                }
            }
            else if( token == "[Input_Amount]" )
            {
                std::getline( buff, token );
                input_amount = std::stoi(token);
            }
            else if( token == "[Input_Dests]" )
            {
                while ( std::getline( buff, token ) && token != "#" )
                {
                    std::stringstream buff_c(token);
                    std::string token_c;
                    std::vector<float> angle_rect;
                    while ( std::getline( buff_c, token_c, ' ' ) && (token_c != "") )
                    {
                        angle_rect.push_back(std::stof(token_c));
                    }
                    input_dests.push_back( RectF{ angle_rect[0], angle_rect[1], angle_rect[2], angle_rect[3] } );
                }
            }
        }
        std::cout << "[LoadData]: Success!" << std::endl; 
    }
    else
    {
        std::cout << "[LoadData]: Faile!" << std::endl; 
    }

    //===============Init
    //Layer
    CallLayer() = Layer::ChoseMode;
    //Buttons
    for( int i = 0; i < btn_amount; i++ )
    {
        buttons.emplace_back( Button{ btn_dests[i], btn_titles[i] } );
    }
    for( int i = 0; i < input_amount; i++ )
    {
        inputs.emplace_back( Input{ input_dests[i] } );
        inputs.back().SetState(Input::StateEvent::Disable);
    }
    rpsGame.LoadData("Config/rpsgame_config.txt");
}
void GUI::Update()
{
    if( GetLayer() == Layer::ChoseMode )
    {
        buttons[(int)Btn_SeqID::One_Player].Update();
        buttons[(int)Btn_SeqID::Two_Player].Update();

        if( buttons[(int)Btn_SeqID::One_Player].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            SetLayer( Layer::ChoseSize );
            gameMod.amount_player = GUI::Btn_SeqID::One_Player;
        }
        else if( buttons[(int)Btn_SeqID::Two_Player].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            gameMod.amount_player = GUI::Btn_SeqID::Two_Player;
        }
    }
    else if( GetLayer() == Layer::ChoseSize )
    {
        buttons[(int)Btn_SeqID::Small].Update();
        buttons[(int)Btn_SeqID::Medium].Update();
        buttons[(int)Btn_SeqID::Large].Update();
        buttons[(int)Btn_SeqID::Back].Update();

        if( buttons[(int)Btn_SeqID::Small].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            gameMod.game_size = GUI::Btn_SeqID::Small;
        }
        else if( buttons[(int)Btn_SeqID::Medium].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            gameMod.game_size = GUI::Btn_SeqID::Medium;
        }
        else if( buttons[(int)Btn_SeqID::Large].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            gameMod.game_size = GUI::Btn_SeqID::Large;
        }
        
        if( buttons[(int)Btn_SeqID::Back].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            ResetGameMode();
            SetLayer( Layer::ChoseMode );
        }
    }
    else if( GetLayer() == Layer::GameMode )
    {   
        if( gameMod.amount_player == GUI::Btn_SeqID::One_Player )
        {

        }
        else if( gameMod.amount_player == GUI::Btn_SeqID::Two_Player )
        {
            buttons[(int)Btn_SeqID::Change_Name].Update();
            buttons[(int)Btn_SeqID::Chat].Update();
            buttons[(int)Btn_SeqID::Back].Update();
            buttons[(int)Btn_SeqID::ReadyState].Update();
            inputs[(int)Input_SeqID::Line_1].Update();
            rpsGame.Update();

            //Button Funtion
            if( buttons[(int)Btn_SeqID::Change_Name].GetStateEvent() == Entity::StateEvent::Clicked )
            {
                Input& i = inputs[(int)Input_SeqID::Line_1];
                if( i.GetStateEvent() == Input::StateEvent::Disable )
                {
                    OnEnableInput( Btn_SeqID::Change_Name, i, 40.0f );
                }
                else OnDisableInput( Btn_SeqID::Change_Name, i );
            }
            else if( buttons[(int)Btn_SeqID::Chat].GetStateEvent() == Entity::StateEvent::Clicked )
            {
                Input& i = inputs[(int)Input_SeqID::Line_1];
                if( i.GetStateEvent() == Input::StateEvent::Disable )
                {
                    OnEnableInput( Btn_SeqID::Chat, i, 150.0f );
                }
                else OnDisableInput( Btn_SeqID::Chat, i );
            }

            //Input
            if( inputs[(int)Input_SeqID::Line_1].GetStateEvent() != Entity::StateEvent::Disable )
            {
                if( IsKeyPressed( KEY_ENTER ) || IsKeyPressed( KEY_KP_ENTER ) )
                {
                    gameMod.input_value = inputs[(int)Input_SeqID::Line_1].GetValue();
                    inputs[(int)Input_SeqID::Line_1].SetState(Entity::StateEvent::Disable);
                    inputs[(int)Input_SeqID::Line_1].ClearValue();
                    gameMod.isTypeChating = false;
                }
                else if( gameMod.input_owner == Btn_SeqID::Chat ) gameMod.isTypeChating = true;
                else gameMod.isTypeChating = false;
            }
            else gameMod.isTypeChating = false;

            //Button Back
            if( buttons[(int)Btn_SeqID::Back].GetStateEvent() == Entity::StateEvent::Clicked )
            {
                inputs[(int)Input_SeqID::Line_1].SetState(Input::StateEvent::Disable);
                gameMod.isReset = true;
            }
        }
    }
}
void GUI::ResetGameMode()
{
    gameMod.amount_player = GUI::Btn_SeqID::Non;
    gameMod.game_size = GUI::Btn_SeqID::Non;
    gameMod.input_owner = Btn_SeqID::Non;
    gameMod.input_value.clear();
    gameMod.isReset = false;
}
void GUI::ClearInputValue()
{
    gameMod.input_value.clear();
}
void GUI::OnEnableInput( const GUI::Btn_SeqID& owner, GUI::Input& i, const float lenght )
{
    gameMod.input_owner = owner;
    const RectF& pDest = buttons[(int)owner].GetDest();
    i.SetDest( 
        pDest
        + RectF{ pDest.width + 20.0f, pDest.width + 20.0f + lenght, 0.0f, 0.0f } 
    );
    i.SetState(Input::StateEvent::Normal);
    i.isTyping = true;
}
void GUI::OnDisableInput( const GUI::Btn_SeqID& owner, GUI::Input& input )
{
    input.OnDisable( gameMod );
}
bool GUI::IsBtnClick( const Btn_SeqID& idBtn ) const
{
    return buttons[(int)idBtn].GetStateEvent() == Button::StateEvent::Clicked;
}
void GUI::Btn_UpdateReadyState( const bool isReady )
{
    if( isReady )
        buttons[(int)Btn_SeqID::ReadyState].SetTitle( "Not Ready" );
    else
        buttons[(int)Btn_SeqID::ReadyState].SetTitle( "Ready" );
}

void GUI::Draw() const
{
    if( GetLayer() == Layer::ChoseMode )
    {
        buttons[(int)Btn_SeqID::One_Player].Draw();
        buttons[(int)Btn_SeqID::Two_Player].Draw();
    }
    else if( GetLayer() == Layer::ChoseSize )
    {
        buttons[(int)Btn_SeqID::Small].Draw();
        buttons[(int)Btn_SeqID::Medium].Draw();
        buttons[(int)Btn_SeqID::Large].Draw();
        buttons[(int)Btn_SeqID::Back].Draw();
    }
    else if( GetLayer() == Layer::GameMode )
    {
        if( gameMod.amount_player == GUI::Btn_SeqID::One_Player )
        {

        }
        else if( gameMod.amount_player == GUI::Btn_SeqID::Two_Player )
        {
            buttons[(int)Btn_SeqID::Change_Name].Draw();
            buttons[(int)Btn_SeqID::Chat].Draw();
            buttons[(int)Btn_SeqID::ReadyState].Draw();
            inputs[(int)Input_SeqID::Line_1].Draw();
            rpsGame.Draw();
        }
        
        buttons[(int)Btn_SeqID::Back].Draw();
    }
}
//==================BUTTONS==================//
GUI::Button::Button( const RectF& _dest, const std::string& _title )
    :
    Entity( _dest ),
    title( _title )
{
    colorLayout = WHITE_TRANS;
    colorLine = RED;
    colorFill = PINK;
    colorFont = BLACK;
    colorFontNoActive = BLACK_TRANS;
}
void GUI::Button::Update()
{
    if( isAbleActive )
    {
        ProcessInput();
        lineThick = 1.0f;
        if( GetStateEvent() == Entity::StateEvent::Hover || isHighLight ) lineThick = 3.0f;
    }
}
void GUI::Button::Draw() const
{
    if( isAbleActive )
    {
        DrawFill( colorFill );
        if( AbleDrawOutline ) DrawOutline( colorLine );
        rayCpp::DrawStrCenter( title, dest, fontSize, colorFont );
    }
    else
    {
        DrawFill( Color{ 255, 109, 194, 200 } );
        if( AbleDrawOutline ) DrawOutline( Color{ 230, 41, 55, 200 } );
        rayCpp::DrawStrCenter( title, dest, fontSize, colorFontNoActive );
    }
}
void GUI::Button::SetTitle( const std::string& _title ) { title = _title; }
//==================INPUT==================//
GUI::Input::Input( const RectF& dest )
    :
    Entity( dest )
{
    measure = int( (dest.width - 2*padding) / (fontSize - 8.0f) );
}
void GUI::Input::PutData()
{
    int c = GetCharPressed();
    if( c != 0 )
    {
        value.push_back( c );
    }
    
    static float counterErase = 0.0f;
    static bool isErasing = false;
    if( value.size() > 0 )
    {
        if( IsKeyPressed( KEY_BACKSPACE ) ) value.pop_back();
        //Erasing
        if( IsKeyDown( KEY_BACKSPACE ) )
        {
            counterErase += 0.016f;
            if( counterErase >  0.8f) isErasing = true;
        }
        else if( IsKeyUp( KEY_BACKSPACE ) ) 
        {
            isErasing = false;
            counterErase = 0.0f;
        }
        if( isErasing) value.pop_back();
    }

    UpdateValueAndCursor();
}
void GUI::Input::UpdateValueAndCursor()
{   
    int pos_c = value.size() - measure;
    if( pos_c < 0 )
    {
        draw_value = value;
    }
    else
    {
        draw_value.clear();
        for( size_t i = pos_c; i < value.size(); i++ )
        {
            draw_value += value[i];
        }
    }
    //Cursor pos and blink
    pos_cursor.x = dest.left + padding + (float)MeasureText(draw_value.c_str(), fontSize);
    pos_cursor.y = dest.top + padding;

    counterBlink += 0.016f;
    if( counterBlink > duraBlink )
    {
        counterBlink = 0;
        blink = !blink;
    }
}
void GUI::Input::Update()
{
    if( curState != StateEvent::Disable )
    {
        Entity::ProcessInput();
        
        if( curState == StateEvent::Clicked ) isTyping = true;
        else if( curState == StateEvent::Normal && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
        {
            OnDisable();
        }
        
        if( isTyping ) PutData();
    }
}
const std::string& GUI::Input::GetValue() const { return value; }

void GUI::Input::ClearValue() { value.clear(); draw_value.clear(); }
void GUI::Input::OnDisable( GUI::GameMod& gameMod )
{
    isTyping = false;
    ClearValue();
    gameMod.input_owner = Btn_SeqID::Non;
    curState = StateEvent::Disable;
}
void GUI::Input::OnDisable()
{
    isTyping = false;
    ClearValue();
    SetState( Input::StateEvent::Disable );
}
void GUI::Input::SetDest( const RectF& dest )
{
    Entity::SetDest( dest );
    measure = int( (dest.width - 2*padding) / (fontSize - 8.0f) );
}
void GUI::Input::Draw() const
{
    if( curState != StateEvent::Disable )
    {
        if( isTyping )
        {
            DrawFill( WHITE );
            if( blink ) 
                rayCpp::DrawRectFill( RectF{ pos_cursor, 2.0f, dest.height - 2*padding }, GRAY );
            DrawOutline( SKYBLUE );
            rayCpp::DrawString( draw_value, dest.topleft + padding, fontSize, GRAY );
        }
        else
        {
            DrawFill( RAYWHITE );
            DrawOutline( GRAY );
        }
    }
}
//==================RPSGame==================//
void GUI::RPSGame::LoadData( const std::string& filename )
{
    data.NewBuff();
    if( data.LoadFromFile( filename ) )
    {
        std::stringstream buff( data.GetBuff() );
        std::string token;
        while ( std::getline( buff, token ) && token != "//" )
        {
            if( token == "[Dest_Origin_Title]" )
            {
                std::getline( buff, token );
                dest_origin_title = rayCpp::CreateRectFByData(token, ' ');
            }
            else if( token == "[Dest_Origin_Btns]" )
            {
                std::getline( buff, token );
                dest_origin_btns = rayCpp::CreateRectFByData(token, ' ');
            }
            else if( token == "[Dest_Origin_Sub_Desc]" )
            {
                std::getline( buff, token );
                dest_origin_sub_desc = rayCpp::CreateRectFByData(token);
            }
            else if( token == "[Title]" )
            {
                std::getline( buff, token );
                title = std::move( token );
            }
            else if( token == "[Button_Amount]" )
            {
                std::getline( buff, token );
                btn_amount = std::stoi(token);
            }
            else if( token == "[Button_Titles]" )
            {
                while ( std::getline( buff, token ) && token != "#" )
                {
                    btn_titles.push_back( token );
                }
            }
            else if( token == "[Button_Dimens]" )
            {
                std::getline( buff, token );
                btn_dimen = rayCpp::CreateVecByData(token, ' ');
            }
            else if( token == "[Button_Between_Distance]" )
            {
                std::getline( buff, token );
                btn_between_distance = std::stof(token);
            }
        }
        std::cout << "[LoadData](RPSGame): Success!" << std::endl; 
    }
    else
    {
        std::cout << "[LoadData](RPSGame): Faile!" << std::endl; 
    }
    //Init
    for( int i = 0; i < btn_amount; i++ )
    {
        buttons.emplace_back( Button{ 
            RectF{ 
                dest_origin_btns.GetCenterPoint() 
                + Vec2{-(btn_dimen.x / 2.0f), i*(btn_dimen.y + btn_between_distance)}, 
                btn_dimen.x, btn_dimen.y 
            }, 
            btn_titles[i] 
        } );
        GUI::Button& btn = buttons.back();
        btn.colorLine = Color{247, 73, 42, 255};
        btn.colorFill = Color{245, 114, 66, 255};
        btn.colorFont = WHITE;
        btn.colorFontNoActive = WHITE_TRANS;
    }
    ResetSub();
}
void GUI::RPSGame::SetSubDesc( const std::string& text ) { sub_desc.text = std::move(text); }
bool GUI::RPSGame::IsBtnClick( const SeqID_Btn& idBtn ) const
{
    return buttons[(int)idBtn].GetStateEvent() == GUI::Button::StateEvent::Clicked;
}
void GUI::RPSGame::StyleAllBtnNormal()
{
    for( auto& b : buttons ) StyleBtnNormal(b); 
}
void GUI::RPSGame::StyleBtnNormal( GUI::Button& btn )
{
    btn.isChoosen = false;
    btn.colorLine = Color{247, 73, 42, 255};
    btn.colorFill = Color{245, 114, 66, 255};
    btn.colorFont = WHITE;
    btn.colorFontNoActive = WHITE_TRANS;
    btn.AbleDrawOutline = true;
}
void GUI::RPSGame::StyleBtnBeChoosen( Button& btn )
{
    btn.isChoosen = true;
    btn.colorFill = Color{245, 62, 29, 255};
    btn.colorLine = Color{245, 35, 12, 255};
    btn.AbleDrawOutline = false;
}
void GUI::RPSGame::ResetSub()
{
    sub_desc.text = "Choose your choice...";
    sub_desc.time = 5;
}
void GUI::RPSGame::Update()
{
    isExistBtnBeChoosen = false;
    for( auto& b : buttons ) 
    {
        b.Update();
        if( b.GetStateEvent() == GUI::Button::StateEvent::Clicked )
        {
            isExistBtnBeChoosen = true;
            b.isChoosen = true;
            StyleBtnBeChoosen( b );
        }
    }
    if( type_choose_btn == TYPE_CHOOSE_BTN::Radio && isExistBtnBeChoosen )
    {
        for( GUI::Button& b : buttons )
        {
            if( b.GetStateEvent() != GUI::Button::StateEvent::Clicked )
            {
                StyleBtnNormal( b );
            }
        }
    }
}
void GUI::RPSGame::Draw() const
{
    rayCpp::DrawStrCenter( RPSGame::title, dest_origin_title, 40, SKYBLUE );
    rayCpp::DrawStrCenter( 
        RPSGame::sub_desc.text + std::to_string(RPSGame::sub_desc.time) + "s", 
        dest_origin_sub_desc, 20, WHITE_TRANS 
    );
    for( auto& b : buttons ) b.Draw();
}
