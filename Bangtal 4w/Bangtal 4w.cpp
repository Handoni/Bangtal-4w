#include <time.h>
#include <stdlib.h>
#include <bangtal.h>
#include <string.h>
#include <stdio.h>

// 찬스 기능을 추가하였습니다.
// 게임 중 chance 버튼을 누르면 횟수에 한해 두 조각을 바꿀 수 있습니다.

SceneID scene;
ObjectID picture[16],start_button, end_button, chance_button;
TimerID timer1;

int pictureSeq[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }, blank = 15, shuffle_num = 100, shf_i = 0, chance_num = 3, chance_pos_1=-1, chance_pos_2=-1;
unsigned int start_time;
bool game_started = false, using_chance = false;

ObjectID createObject(const char* image, SceneID scene, int x, int y, bool shown)
{
    ObjectID object = createObject(image);
    locateObject(object, scene, x, y);

    if (shown)
    {
        showObject(object);
    }
    return object;
}

const char* picture_name(int num)
{
    static char buf[20] = "images/";
    const char* png = ".png";
    sprintf_s(buf + 7, sizeof(buf), "%d", num);
    strcat_s(buf, png);
    return buf;
}

void img_set()
{
    int i, garo, sero;
    for (i = 0; i < 16; i++)
    {
        sero = i / 4;
        garo = i % 4;
        locateObject(picture[pictureSeq[i]], scene, garo * 150 + 300, 470 - sero * 150);
    }
}

void call_picture()
{
    int i, sero, garo;
    for (i = 0; i < 16; i++)
    {
        sero = i / 4;
        garo = i % 4;
        picture[i] = createObject(picture_name(i), scene, garo * 150 + 300, 470 - sero * 150, true);
    }
}

void change_pic_place(int init, int fin)
{
    int temp;
    temp = pictureSeq[init];
    pictureSeq[init] = pictureSeq[fin];
    pictureSeq[fin] = temp;

    img_set();
}

void clear_judge()
{
    char end_message[50];
    int i;

    for (i = 0; i < 15; i++)
    {
        if (pictureSeq[i] != i)
        {
            break;
        }
    }
    if (i == 15)
    {
        game_started = false;
        setObjectImage(start_button, "images/restart.png");
        showObject(start_button);
        showObject(end_button);
        showObject(picture[15]);
        hideObject(chance_button);
        sprintf_s(end_message, "%d초 걸렸습니다!", (int)time(NULL) - start_time);
        showMessage(end_message);
    }
}

void mouseCallback(ObjectID object, int x, int y, MouseAction action)
{
    int i,j;
    bool change = false;
    char end_message[50] = {}, chance_message[100] = {};

    if (object == start_button)
    {
        shf_i = 0;
        chance_num = 3;

        hideObject(picture[15]);
        hideObject(start_button);
        hideObject(end_button);

        showObject(chance_button);
        startTimer(timer1);
        start_time = time(NULL);
    }
    if (object == end_button)
    {
        endGame();
    }
    if (object == chance_button)
    {
        if (chance_num > 0)
        {
            sprintf_s(chance_message, "바꿀 두 조각을 선택해주세요. 찬스 %d개 남았습니다.", chance_num);
            showMessage(chance_message);
            chance_num--;
            chance_pos_1 = -1;
            chance_pos_2 = -1;
            using_chance = true;
        }
        else
        {
            showMessage("찬스를 다 사용했습니다.");
        }
    }
    if (game_started && !using_chance)
    {
        for (i = 0; i < 16; i++)
        {
            
            if (object == picture[i])
            {
                for (j = 0; j < 16; j++)
                {
                    if (pictureSeq[j] == i)
                        break;
                }
                switch (j)
                {
                case 0:
                    if (j + 1 == blank || j + 4 == blank)
                        change = true;
                    break;
                case 1: case 2:
                    if (j - 1 == blank || j + 1 == blank || j + 4 == blank)
                        change = true;
                    break;
                case 3:
                    if (j - 1 == blank || j + 4 == blank)
                        change = true;
                    break;
                case 4: case 8:
                    if (j + 1 == blank || j - 4 == blank || j + 4 == blank)
                        change = true;
                    break;
                case 5: case 6: case 9: case 10:
                    if (j + 1 == blank || j - 1 == blank || j - 4 == blank || j + 4 == blank)
                        change = true;
                    break;
                case 7: case 11:
                    if (j - 1 == blank || j - 4 == blank || j + 4 == blank)
                        change = true;
                    break;
                case 12:
                    if (j + 1 == blank || j - 4 == blank)
                        change = true;
                    break;
                case 13: case 14:
                    if (j + 1 == blank || j - 1 == blank || j - 4 == blank)
                        change = true;
                    break;
                case 15:
                    if (j - 1 == blank || j - 4 == blank)
                        change = true;
                    break;
                default :
                    change = false;
                }

                if (change)
                {
                    change_pic_place(j, blank);
                    blank = j;

                    clear_judge();
                }
                break;
            }
        }
        
    }
    else if (using_chance)
    {
        if (chance_pos_1 == -1)
        {
            for (i = 0; i < 16; i++)
            {
                if (object == picture[i])
                {
                    for (j = 0; j < 16; j++)
                    {
                        if (pictureSeq[j] == i)
                            break;
                    }
                    chance_pos_1 = j;
                    break;
                }
            }
        }
        else if (chance_pos_1 != -1 && chance_pos_2 == -1)
        {
            for (i = 0; i < 16; i++)
            {
                if (object == picture[i])
                {
                    for (j = 0; j < 16; j++)
                    {
                        if (pictureSeq[j] == i)
                            break;
                    }
                    chance_pos_2 = j;
                    change_pic_place(chance_pos_1, chance_pos_2);
                    using_chance = false;

                    clear_judge();
                    break;
                }
            }
        }
    }
}

void timerCallback(TimerID timer)
{
    int next;
    if (timer == timer1)
    {
        if (shf_i < shuffle_num)
        {
            next = rand() % 16;

            if (next == blank)
            {
                shf_i--;
            }

            change_pic_place(next, blank);

            blank = next;

            shf_i++;

            startTimer(timer1);
        }
        else
        {
            game_started = true;
        }
    }
}




int main()
{
    srand((unsigned int)time(NULL));
    setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

    setMouseCallback(mouseCallback);
    setTimerCallback(timerCallback);

    scene = createScene("배경", "images/background.png");
    timer1 = createTimer(0.01f);

    call_picture();
    start_button=createObject("images/start.png", scene, 590, 70, true);
    end_button = createObject("images/end.png", scene, 590, 30, true);
    chance_button = createObject("images/chance.png", scene, 1000, 680, false);

    startGame(scene);

}
