#include "raylibCpp.h"
#include "GUI.h"

void GUI::LoadAsset()
{
    data.NewBuff();
    if( data.LoadFromFile( "../MMO_Client/assets/config.txt" ) )
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
            SetLayer( Layer::ChoseSize );
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
            SetLayer( Layer::ChoseMode );
        }
    }
    else if( GetLayer() == Layer::GameMode )
    {
        buttons[(int)Btn_SeqID::Back].Update();

        if( buttons[(int)Btn_SeqID::Back].GetStateEvent() == Entity::StateEvent::Clicked )
        {
            gameMod.isReset = true;
        }
    }
}
void GUI::ResetGameMode()
{
    gameMod.amount_player = GUI::Btn_SeqID::Non;
    gameMod.game_size = GUI::Btn_SeqID::Non;
    gameMod.isReset = false;
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
        buttons[(int)Btn_SeqID::Back].Draw();
    }
}
///////////////////////////////////////////////////////////////////////////
GUI::Button::Button( const RectF& _dest, const std::string& _title )
    :
    Entity( _dest ),
    title( _title )
{}
void GUI::Button::Update()
{
    ProcessInput();
    if( GetStateEvent() == Entity::StateEvent::Hover ) lineThick = 4.0f;
    else if ( GetStateEvent() == Entity::StateEvent::Normal ) lineThick = 2.0f;
}
void GUI::Button::Draw() const
{
    DrawFill( PINK );
    DrawOutline( RED );
    rayCpp::DrawStrCenter( title, dest, fontSize, BLACK );
}