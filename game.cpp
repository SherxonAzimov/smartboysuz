#include <vcl.h>
#pragma hdrstop
#include "Unit1.h"
#include <mmsystem.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
Graphics::TBitmap*ShipPic;
Graphics::TBitmap*InvadersPic[2][3];
Graphics::TBitmap*BackScreen;
TList*Invaders;
int InvadersDirect;
bool LEFT,RIGHT;
float InvadersSpeed;
int Lifes;
int Scores;
//---------------------------------------------------------------------------
class TInvaders
{
public:
float x;
int y;
int Color,State;
int Maxtime;
int time;
TInvaders(int tx,int ty)
        {

        x=tx;y=ty;
        State=random(2);
        Color=random(3);
        time=0;
        Maxtime=random(20)+20;
        }
void Draw()
        {
        BackScreen->Canvas->Draw(x,y,InvadersPic[State][Color]);
        }
};

class TShip
{
public:
int x;int y;
int TimeToNextBullet;
int Time;
bool Fired;
TPoint BulletPos;
TShip()
        {
        Fired=false;
        TimeToNextBullet=30;
        Time=50;
        x=185;
        y=365;
        BulletPos=Point(199,359);
        }
void Draw()
        {
        BackScreen->Canvas->Pen->Color=clRed;
        BackScreen->Canvas->Pen->Width=3;
        BackScreen->Canvas->Draw(x,y,ShipPic);
        BackScreen->Canvas->MoveTo(BulletPos.x,BulletPos.y);
        BackScreen->Canvas->LineTo(BulletPos.x,BulletPos.y+10);
        }
void Fire()
        {
        if(!Fired)
        {
        if(Time>TimeToNextBullet)
                {
                Fired=true;
                Time=0;
                }
        }
        }
};
TShip Ship;
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
Panel1->Hide();
Memo1->Hide();
Button1->Hide();
randomize();
BackScreen=new Graphics::TBitmap();
BackScreen->Width =400;
BackScreen->Height=400;
BackScreen->Canvas->Font->Color=clBlue;
BackScreen->Canvas->Font->Size=10;
Invaders=new TList();
ShipPic=new Graphics::TBitmap();
ShipPic->LoadFromFile("Ship1.bmp");   //qurol
Graphics::TBitmap*Temp=new Graphics::TBitmap();
Temp->LoadFromFile("Invaders2.bmp");   //mishen
for(int j=0;j<3;j++)
        for(int i=0;i<2;i++)
        {
        InvadersPic[i][j]=new Graphics::TBitmap();
        InvadersPic[i][j]->Width =30;
        InvadersPic[i][j]->Height=30;
        InvadersPic[i][j]->Canvas->CopyRect(Rect(0,0,30,30),Temp->Canvas,Rect(i*30,j*30,i*30+30,j*30+30));
        InvadersPic[i][j]->Transparent=true;
        }
InvadersSpeed=0;
Lifes=3;
Scores=0;
NewGame();
InvadersDirect=1;
LEFT=false;RIGHT=false;
}
//---------------------------------------------------------------------------
void TForm1::NewGame()
{
Invaders->Clear();
int x=5,y=5;
for(int j=0;j<4;j++)
        {
        for(int i=0;i<8;i++,x+=40)
        {
        TInvaders*temp=new TInvaders(x,y);
        Invaders->Add((void*)temp);
        }
        x=5;
        y+=30;
        }
InvadersSpeed+=0.5;
}
//-----------------------------------------------------------------------------
TForm1::DrawAllInvaders()
{
       for(int i=0;i<Invaders->Count;i++)
       {
       TInvaders*temp=(TInvaders*)Invaders->Items[i];
       temp->Draw();
       }
return 0;
}
void __fastcall TForm1::FormPaint(TObject *Sender)
{
ClearBack();
DrawAllInvaders();
Ship.Draw();
Flip();
}
//---------------------------------------------------------------------------
void TForm1::ClearBack()
{
       BackScreen->Canvas->Brush->Color=clBlack;//Orqa fon rangi
       BackScreen->Canvas->FillRect(Rect(0,0,450,450));//orqa fon razmeri
}
void TForm1::Flip()
{
        Canvas->Draw(0,0,BackScreen);
}
bool __fastcall TForm1::MoveInvaders()
{
bool MustMoveDown=false;
        TInvaders*temp;
        for(int i=0;i<Invaders->Count;i++)
        {
        temp=(TInvaders*)Invaders->Items[i];
        if(InvadersDirect==1)
        {
        if((temp->x)>370)
                {
                InvadersDirect=2;
                MustMoveDown=true;
                }
        }
        if(InvadersDirect==2)
        {
        if((temp->x)<0)
                {
                InvadersDirect=1;
                MustMoveDown=true;
                }
}
        }
        for(int i=0;i<Invaders->Count;i++)
        {
        temp=(TInvaders*)Invaders->Items[i];
        temp->time++;
        if(temp->time>temp->Maxtime)
                {
                temp->time=0;
                if(temp->State==0)temp->State=1;else temp->State=0;
                }
        if(InvadersDirect==1)temp->x+=InvadersSpeed;
        if(InvadersDirect==2)temp->x-=InvadersSpeed;
        if(MustMoveDown)temp->y+=20;
        if(temp->y>370)return false;
        }
return true;
}
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
 StaticText1->Caption=Now() .TimeString() ;
if(!MoveInvaders())
        {
        Timer1->Enabled=false;
        Panel1->Show();
        PlaySound("SOUND/task_complete.wav",0,SND_ASYNC);
        if(Lifes>0)
                {
                Lifes--;
                InvadersSpeed-=0.5;
                NewGame();
                }else
                {
                ShowMessage("SIZDA O'YINNI DAVOM ETTIRISH IMKONI YO'Q\n ");
                InvadersSpeed=0;
                Lifes=3;
                Scores=0;
                NewGame();
                }
        Timer1->Enabled=true;
        }
if(LEFT)Ship.x-=3;
if(RIGHT)Ship.x+=3;
MoveBullet();
if(IsPobeda())
        {
        Timer1->Enabled=false;
        PlaySound("SOUND/task_complete.wav",0,SND_ASYNC);
        ShowMessage("TABRIKLAYMIZ!!!\n SIZ YUTDINGIZ");
        NewGame();
        Timer1->Enabled=true;
        }
ClearBack();
DrawAllInvaders();
Ship.Draw();
BackScreen->Canvas->Brush->Style=bsClear;
BackScreen->Canvas->TextOut(10,386,"O'YIN:"+IntToStr(Lifes));
BackScreen->Canvas->TextOut(320,386,"BALL:"+IntToStr(Scores));
BackScreen->Canvas->Brush->Style=bsSolid;
Flip();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
if(Key==VK_RIGHT)RIGHT=true;
if(Key==VK_LEFT)LEFT=true;
if(Key==VK_SPACE)Ship.Fire();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
if(Key==VK_RIGHT)RIGHT=false;
if(Key==VK_LEFT) LEFT =false;
PlaySound("SOUND/CameraR.wav",0,SND_ASYNC);
}
//---------------------------------------------------------------------------
void TForm1::MoveBullet()
{
 if(!Ship.Fired)
        {
        Ship.BulletPos.x=Ship.x+14;
        Ship.BulletPos.y=Ship.y-6;
        }else
        {
        Ship.BulletPos.y-=10;
        Ship.Time++;
        if(Ship.Time>40)
                {
                Ship.Fired=false;
                }
        TInvaders*temp;
        for(int i=0;i<Invaders->Count;i++)
                {
                temp=(TInvaders*)Invaders->Items[i];
                if((Ship.BulletPos.x>=temp->x)&&(Ship.BulletPos.x<=temp->x+30)&&
                   (Ship.BulletPos.y>=temp->y)&&(Ship.BulletPos.y<=temp->y+30))
                        {
                        Ship.Fired=false;
                        Ship.Time=100;
                        Invaders->Delete(i);
                        Scores+=10;
                        delete temp;
                        break;
                        }
                }
        }
}

bool __fastcall TForm1::IsPobeda()
{
        if(Invaders->Count>0)return false;else return true;
}
void __fastcall TForm1::Oyin21Click(TObject *Sender)
{
Form1->Close();
}
//--------------------------------------------------------------------------
void __fastcall TForm1::StaticText2Click(TObject *Sender)
{
Panel1->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
Memo1->Hide();
Button1->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Abautmy1Click(TObject *Sender)
{
Memo1->Show();
Button1->Show();
}
//---------------------------------------------------------------------------
