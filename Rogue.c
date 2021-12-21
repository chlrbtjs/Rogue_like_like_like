#include<stdio.h>
#include<windows.h>
#include<time.h>
#include<windef.h>

#define PLAYER -3
#define WALL -4

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

//monsters
#define slime 10
#define bat 11
#define wolf 12
#define goblin 13
#define skeleton 14

//potions
#define health_potion 20
#define strength_potion 21
#define defense_potion 22

//items
#define ammor 30
#define sword 31
#define heart 32
//staff
#define magic_missile_staff 33
#define genocide_staff 34

///////////////////////////////
#define monsters_num 5
#define potions_num 3
#define items_num 5

int map[22][72], sub_map[22][72];
int size_x = 72, size_y = 22;
int dif;
int player_locate_y, player_locate_x;
int maked_monster, maked_potion, maked_items;
int player_hp = 30, player_demage = 5, player_reach = 1, player_gold, player_fullhp = 30;
int player_ammor = 0, player_weapon=0, player_ring;
int potion_inventory[potions_num];

int wall_num;

const char *monster_name[monsters_num] = {"slime", "bat", "wolf", "goblin", "skeleton"};
const char *potion_name[potions_num] = {"health", "strength", "defense"};
const char *item_name[items_num] = {"ammor", "sword", "heart", "magic_missile_staff", "genocide_staff"};

struct monster
{
    int y, x, hp, demage, reach, type;
    int gold;
};

struct monster m[100];

struct potion
{
    int y, x, type;
};

struct potion p[100];

struct item
{
    int y, x, type, add_demage, add_reach, add_ammor, add_hp, add_fullhp;
};

struct item i[100];

int BFS(int chance);
void print_rule();
void Cellular_Automata();
void printmap(int y1, int x1, int y2, int x2);
int mkrand(int randmin, int randmax);
void mkobject(int y, int x);
void mkmap();
void player_useitem();
void player_taken_damege(int demage);
void player_interaction(int y, int x);
void player_attack(int y, int x);
void monster_attack(struct monster sub);
void player_move();
void player_action();
void monster_action();
void monster_move(int sub);
void monster_move_2(int a, int sub);

int BFS(int chance){
    int check[22][72] = {0, };
    int queue[2][1584];
    int head=1, tail=0;
    int si, sx;
    int chance1 = chance;
    while (1)
    {
        tail = 0;
        head = 1;
        for(int i=1;i<size_y-1;i++){
            for(int j=1;j<size_x-1;j++){
                if(map[i][j] != WALL && !check[i][j]){
                    si = i;
                    sx = j;
                    goto BFSBREAK;
                }
            }
        }
        BFSBREAK:;
        queue[0][0] = si;
        queue[1][0] = sx;
        
        while (tail < head)
        {
            int i = queue[0][tail], j = queue[1][tail];

            if(i==0 || i+1 == size_y || j==0 || j+1==size_x) return 1;

            if(map[i-1][j]==0 && !check[i-1][j]){
                queue[0][head] = i-1;
                queue[1][head] = j;
                check[i-1][j] = 1;
                head++;
            }
            if(map[i+1][j]==0 && !check[i+1][j]){
                queue[0][head] = i+1;
                queue[1][head] = j;
                check[i+1][j] = 1;
                head++;
            }
            if(map[i][j-1]==0 && !check[i][j-1]){
                queue[0][head] = i;
                queue[1][head] = j-1;
                check[i][j-1] = 1;
                head++;
            }
            if(map[i][j+1]==0 && !check[i][j+1]){
                queue[0][head] = i;
                queue[1][head] = j+1;
                check[i][j+1] = 1;
                head++;
            }

            tail++;
        }
        if(tail < 20){
            printmap(0, 0, size_y, size_x);
            for(int i=0;i<tail;i++){
                map[queue[0][i]][queue[1][i]] = WALL;
                chance1--;
            }
            continue;
        }
        else if(tail-1 == chance1){
            return 0;
        }
        else return 1;
    }
}

void print_rule(){
    int n;
    printf("This game was modeled after Rogue and Nethack.\n\n");
    printf("betatester : char[10] = yung min\n\n");
    printf("Enter two integers at the start. The size of the map is determined by this.\n\n");
    printf("Move : Press 2 and move to drop one of w, a, s, and d. If there is an object different from the target position, do not move.\n\n");
    printf("Attack and interaction: Press the corresponding number to show a map within the player's range.\n");
    printf("At this time, enter the desired location using the values i and j.\n\n");
    printf("Use Potion and Staff: Press 3 and follow the instructions.\n\n");
    printf("Press any number to start. > ");
    scanf("%d", &n);
    while (getchar() != '\n');
}

int mkrand(int randmin, int randmax){
    if(randmin == randmax){
        //printf("div by zero");
        return randmin;
    }
    return (rand()%(randmax - randmin + 1)) + randmin;
}//min ~ max중 한개를 무작위로 반환

void Cellular_Automata(){
    for(int i=0;i<size_y;i++){
        for(int j=0;j<size_x;j++){
            if(mkrand(1, 100) < 31 || i==0 || i==size_y-1 || j==0 || j==size_x-1){
                map[i][j] = WALL;
            }
            else map[i][j] = 0;

            sub_map[i][j] = 0;
        }
    }
    int T = 5;
    while (T--)
    {
        for(int i=1;i<size_y-1;i++){
            for(int j=1;j<size_x-1;j++){
                if((map[i-1][j-1] == -4) + (map[i-1][j] == -4) + (map[i-1][j+1] == -4) +
                (map[i][j-1] == -4) + (map[i][j] == -4) + (map[i][j+1] == -4) +
                (map[i+1][j-1] == -4) + (map[i+1][j] == -4) + (map[i+1][j+1] == -4) > 3){
                    if(map[i][j] != WALL) sub_map[i][j] = WALL;
                }
            }
        }
        for(int i=1;i<size_y-1;i++){
            for(int j=1;j<size_x-1;j++){
                map[i][j] = sub_map[i][j];
            }
        }
    }
}

void printmap(int y1, int x1, int y2, int x2){
    printf("\n\n");
    for(int i=max(y1, 0); i<min(y2, size_y); i++){
        for(int j=max(x1, 0); j<min(x2, size_x); j++){
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
            int cell = map[i][j];
            if(!cell) printf(".");
            else if(cell>0){
                switch ((cell/10)%10)
                {
                case 1:
                    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4);

                    switch (cell%10)
                    {
                    case 0: printf("s"); break;
                    case 1: printf("b"); break;
                    case 2: printf("w"); break;
                    case 3: printf("g"); break;
                    case 4: printf("S"); break;
                    default:
                        system("cls");
                        printf("Rine 49 error, printmap, monsters");
                        break;
                    }

                    break;

                case 2:
                    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 1);

                    printf("p");

                    break;

                case 3:
                    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 2);

                    switch (cell%10)
                    {
                    case 0: printf("a"); break;
                    case 1: printf("w"); break;
                    case 2: printf("h"); break;
                    case 3: printf("m"); break;
                    case 4: printf("j"); break;

                    default:
                        break;
                        system("cls");
                        printf("Rine 85 error, printmap, items");
                    }

                    break;

                default:
                    system("cls");
                    printf("Rine 92 error, printmap");
                    break;
                }

            }
            else{
                if(cell == -1) printf("-");
                else if(cell == -2) printf("|");
                else if(cell == PLAYER) {
                    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 6);
                    printf("@");
                    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
                }
                else if(cell == WALL) printf("#");
                else{
                    system("cls");
                    printf("rine 104 error");
                }
            }

            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");

    printf("\n");
    for(int i=0;i<size_x;i++) printf("__");
    printf("\n");
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4);
    printf("hp");
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    printf(" : %d<%d>  ", player_hp, player_fullhp);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 5);
    printf("demage");
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    printf(" : %d  ", player_demage);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 2);
    printf("reach");
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    printf(" : %d  ", player_reach);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 6);
    printf("gold");
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    printf(" : %d  ", player_gold);
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 8);
    printf("ammor");
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    printf(" : %d\n", player_ammor);
    for(int i=0;i<size_x;i++) printf("__");
    printf("\n");
}

void mkobject(int y, int x){
    int type;
    type = mkrand(1, 10+dif);
    if(type <= 4){     //monsters
        maked_monster+=1;
        m[maked_monster].type = mkrand(0, monsters_num-1);
        m[maked_monster].demage = mkrand(3, 5);
        m[maked_monster].y = y;
        m[maked_monster].x = x;
        m[maked_monster].gold = mkrand(1, 40);
        if(m[maked_monster].type == 4) {
            m[maked_monster].reach = 4;
            m[maked_monster].hp = mkrand(4, 6);
        }
        else {
            m[maked_monster].reach = 1;
            m[maked_monster].hp = mkrand(7, 10);
        }

        map[y][x] = m[maked_monster].type + 10 + (maked_monster * 100);
    }
    else if(type <=7){     //items
        maked_items+=1;
        MKITEM:;
        i[maked_items].type = mkrand(0, items_num-1);
        if(i[maked_items].type == 0){
            i[maked_items].add_ammor = 3;
            i[maked_items].add_demage = 0;
            i[maked_items].add_reach = 0;
            i[maked_items].add_fullhp = 0;
            i[maked_items].add_hp = 0;
        }
        else if(i[maked_items].type == 1){
            i[maked_items].add_ammor = 0;
            i[maked_items].add_demage = 5;
            i[maked_items].add_reach = 0;
            i[maked_items].add_fullhp = 0;
            i[maked_items].add_hp = 0;
        }
        else if(i[maked_items].type == 2){
            if(mkrand(1, 2) != 1) goto MKITEM;
            i[maked_items].add_ammor = 0;
            i[maked_items].add_demage = 0;
            i[maked_items].add_reach = 0;
            i[maked_items].add_fullhp = 10;
            i[maked_items].add_hp = 5;
        }
        else if(i[maked_items].type == 3){
            if(mkrand(1, 5) != 1) goto MKITEM;
            i[maked_items].add_ammor = 0;
            i[maked_items].add_demage = 0;
            i[maked_items].add_reach = 0;
            i[maked_items].add_fullhp = 0;
            i[maked_items].add_hp = 0;
        }
        else if(i[maked_items].type == 4){
            if(mkrand(1, 20) != 1) goto MKITEM;
            i[maked_items].add_ammor = 0;
            i[maked_items].add_demage = 0;
            i[maked_items].add_reach = 0;
            i[maked_items].add_fullhp = 0;
            i[maked_items].add_hp = 0;
        }
        i[maked_items].y = y;
        i[maked_items].x = x;
        map[y][x] = i[maked_items].type + 30 + (maked_items * 100);
    }
    else{   //potions
        maked_potion+=1;
        p[maked_potion].type = mkrand(0, potions_num-1);
        p[maked_potion].y = y;
        p[maked_potion].x = x;
        map[y][x] = p[maked_potion].type + 20 + (maked_potion * 100);
    }
}

void mkmap(){
    int chance = 0;
    int isnotplayerexist = 1;

    BFS:;

    Cellular_Automata();

    chance = 0;
    for(int i=0;i<size_y;i++){
        for(int j=0;j<size_x;j++){
            if(map[i][j] != WALL) chance++;
        }
    }

    if(chance < 100 || BFS(chance)) goto BFS;

    chance = 0;
    for(int i=0;i<size_y;i++){
        for(int j=0;j<size_x;j++){
            if(map[i][j] != WALL) chance++;
        }
    }

    for(int i=0;i<size_y;i++){
        for(int j=0;j<size_x;j++){
            if(map[i][j] != WALL){
                if(isnotplayerexist && mkrand(1, chance)==1){
                    isnotplayerexist = 0;
                    player_locate_y = i;
                    player_locate_x = j;
                    map[i][j] = PLAYER;
                }
                else{
                    if(mkrand(1, 12) == 1){
                        mkobject(i, j);
                    }
                    chance--;
                }
            }
            sub_map[i][j] = map[i][j];
        }
    }
}

void player_useitem(){
    char use;
    int potion, y, x;
    char genocide, genocideobjmonster;
    printf("what you want use?   p:potion   s:staff\n");
    USEERROR:;
    scanf("%c", &use);
    while (getchar() != '\n');

    if(use != 'p' && use !='s'){
        printf("input p or s\n");
        goto USEERROR;
    }

    switch (use)
    {
    case 'p':
        printf("you have ");
        for(int i=0;i<potions_num;i++) printf("%d %s potion, ", potion_inventory[i], potion_name[i]);
        printf("\n what you want? 0 or 1 or 2\n");

        POTIONERROR:;

        scanf("%d", &potion);
        while (getchar() !='\n');

        if(potion < 0 || potion > potions_num){
            printf("input 0");
            for(int i=1;i<potions_num;i++) printf(" or %d", i);
            printf("\n");

            goto POTIONERROR;
        }

        if(!potion_inventory[potion]){
            printf("You swallowed your saliva\n");
        }
        else{
            printf("you drink %s potion\n", potion_name[potion]);
            if(potion == 0){
                player_hp = min(player_fullhp, player_hp+10);
                printf("your hp + 10\n");
                potion_inventory[potion]--;
            }
            else if(potion == 1){
                player_demage += 1;
                printf("your strength + 1\n");
                potion_inventory[potion]--;
            }
            else if(potion == 2){
                player_ammor += 2;
                printf("your defense + 2\n");
                potion_inventory[potion]--;
            }
        }

        break;

    case 's':
        if(player_weapon == magic_missile_staff){
            MAGICMISSILE:;

            printmap(player_locate_y-5, player_locate_x-5, player_locate_y+5+1, player_locate_x+5+1);
            printf("\n where is you want fire missile? y, x (started 0)\n");
            scanf("%d %d", &y, &x);
            while (getchar() != '\n');

            if(y < 0 || y > 5*2 + 1 ||x < 0 || x > 5*2 + 1){
                printf("Your reach is not enough\n");
                goto MAGICMISSILE;
            }

            player_attack(player_locate_y-5+y, player_locate_x-5+x);
        }
        else if(player_weapon == genocide_staff){
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 5);
            printf("Genocide!!!\ninput the monster   s or b or w or g or S\n");
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);

            GENOCIDE:;

            scanf("%c", &genocide);
            while(getchar() != '\n');

            if(genocide != 's' && genocide != 'b' && genocide != 'w' && genocide != 'g' && genocide != 'S'){
                printf("input the monster   s or b or w or g or S\n");
                goto GENOCIDE;
            }

            switch (genocide)
            {
            case 's': genocideobjmonster = 0; break;
            case 'b': genocideobjmonster = 1; break;
            case 'w': genocideobjmonster = 2; break;
            case 'g': genocideobjmonster = 3; break;
            case 'S': genocideobjmonster = 4; break;

            default:
                printf("Genocide error\n");
                break;
            }

            for(int i=0;i<size_y;i++){
                for(int j=0;j<size_x;j++){
                    if(map[i][j]%100 == 10+genocideobjmonster) {
                        map[i][j] = 0;
                        maked_monster--;
                    }
                }
            }

            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 5);
            printf("Congratulation, you kill every %s\n", monster_name[genocideobjmonster]);
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
        }

        break;

    default:
        break;
    }

}

void player_taken_damege(int demage){
    player_hp -= max(1, (demage - player_ammor));
    printf("you take demage %d, your hp : %d\n", max(1, demage - player_ammor), player_hp);
}

void player_interaction(int y, int x){
    int obj = map[y][x];

    if((obj%100)/10 == 1){
        printf("you touch the %s ... it seems to be angry\n", monster_name[obj%10]);
        player_taken_damege(m[obj/100].demage);
    }
    else if((obj%100)/10 == 2){
        sub_map[y][x] = 0;
        printf("you pick up %s potion\n", potion_name[obj%10]);
        potion_inventory[obj%10]++;
    }
    else if((obj%100)/10 == 3){
        sub_map[y][x] = 0;
        if(obj%10 < 3){ //ammor, sword, heart등의 스텟 장비
            printf("you pick up %s\n", item_name[obj%10]);
            player_ammor += i[obj/100].add_ammor;
            player_demage += i[obj/100].add_demage;
            player_reach += i[obj/100].add_reach;
            player_hp += i[obj/100].add_hp;
            player_fullhp +=i[obj/100].add_fullhp;
        }
        else if(obj%10 == 3 || obj%10 == 4){ //staff등
            if(player_weapon) printf("you change weapon\n");
            printf("you pick up %s\n", item_name[obj%10]);
            player_weapon = obj%100;
        }
    }
    else if(obj == -3){
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 11);
        printf("You have encouraged yourself. Your confidence is rising.\n");
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    }
}

void monster_action(int y1, int x1, int y2, int x2){
    for(int i=y1;i<y2;i++){
        for(int j=x1;j<x2;j++){
            if((map[i][j]%100)/10 == 1){
                struct monster sub = m[map[i][j]/100];

                if(i - sub.reach <= player_locate_y && i + sub.reach >= player_locate_y &&
                j - sub.reach <= player_locate_x && j + sub.reach >= player_locate_x){
                    monster_attack(sub);
                }
                else monster_move(map[i][j]/100);
            }
        }
    }
}

void player_attack(int y, int x){
    int obj = map[y][x];
    int demage = player_demage + mkrand(-5, 5);
    if((obj%100)/10 == 1){
        m[obj/100].hp -= demage;
        printf("demage : %d  obs_hp : %d\n", demage, m[obj/100].hp);
        if(m[obj/100].hp <=0){
            sub_map[y][x] = 0;
            printf("you kill the enemy\n");
            maked_monster--;
            player_gold += m[obj/100].gold;
        }
    }
    else if((obj%100)/10 == 2){
        sub_map[y][x] = 0;
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 12);
        printf("oops!, the potion is broken...\n");
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    }
    else if((obj%100)/10 == 3){
        if(obj%10 == 0){
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 12);
            printf("ouch! that is ammor!\n");
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
            player_taken_damege(5);
        }
        else if(obj%10 == 1){
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4);
            printf("AAAAAAAAAAAAAA!, the sword is so sharp!\n");
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
            player_taken_damege(20);
        }
        else if(obj%10 == 2){
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4);
            printf("there is ring...\n");//staff 만들기
            SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
        }/*
        else if(obs%10 == 3){
            printf("BOOM! the staff is expoled!");
            for(int i=y-3;i<y+4;i++){
                for(int j=x-3;j<x+4;j++){
                    if((map[i][j]%100)/10 == )
                }
            }
        }*/
    }
    else if(obj == -3){
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4);
        printf("I think it's a wise choice\n");
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
        player_taken_damege(player_hp + player_ammor);
    }
}

void monster_attack(struct monster sub){
    int demage = mkrand(-2, 2) + sub.demage;
    if(sub.type == 1){
        sub.hp += demage;
        printf("bat attack you\n");
    }
    else if(sub.type == 3){
        sub.gold += min(player_gold, 5);
        printf("goblen attck you, it take your gold %d\n", min(player_gold, 5));
        player_gold -= min(player_gold, 5);
    }
    else printf("%s attack you\n", monster_name[sub.type]);

    player_taken_damege(demage);
}

void player_move(){
    MOVEERROR:;
    char move;
    scanf(" %c", &move);
    while (getchar() != '\n');

    switch (move)
    {
    case 'w':
        if(player_locate_y == 0 || map[player_locate_y-1][player_locate_x]){
            printf("you can't go there\n");
            return;
        }

        sub_map[player_locate_y][player_locate_x] = 0;
        sub_map[player_locate_y-1][player_locate_x] = PLAYER;

        player_locate_y -= 1;

        break;

    case 'a':
        if(player_locate_x == 0 || map[player_locate_y][player_locate_x-1]){
            printf("you can't go there\n");
            return;
        }

        sub_map[player_locate_y][player_locate_x] = 0;
        sub_map[player_locate_y][player_locate_x-1] = PLAYER;

        player_locate_x -= 1;

        break;

    case 's':
        if(player_locate_y == size_y-1 || map[player_locate_y+1][player_locate_x]){
            printf("you can't go there\n");
            return;
        }

        sub_map[player_locate_y][player_locate_x] = 0;
        sub_map[player_locate_y+1][player_locate_x] = PLAYER;

        player_locate_y += 1;

        break;

    case 'd':
        if(player_locate_x == size_x-1 || map[player_locate_y][player_locate_x+1]){
            printf("you can't go there\n");
            return;
        }

        sub_map[player_locate_y][player_locate_x] = 0;
        sub_map[player_locate_y][player_locate_x+1] = PLAYER;

        player_locate_x += 1;

        break;

    default:
        printf("Please input w, a, s, d\n");
        goto MOVEERROR;
        break;
    }
}

void player_action(){
    for(int i=0;i<size_x;i++) printf("__");
    printf("\n");

    INPUT_ACTION:;

    printf("input action   1:attack   2:move   3:useitem   4:interaction\n\n");

    int act, y, x;
    scanf("%d", &act);
    while (getchar() != '\n');

    switch (act)
    {
    case 1:
        INPUT_ATTACK:;

        printmap(player_locate_y-player_reach, player_locate_x-player_reach, player_locate_y+player_reach+1, player_locate_x+player_reach+1);
        printf("\n where is you want attack? y, x (started 0)\n");
        scanf("%d %d", &y, &x);
        while (getchar() != '\n');

        if(y < 0 || y > player_reach*2 + 1 ||x < 0 || x > player_reach*2 + 1){
            printf("Your reach is not enough\n");
            goto INPUT_ATTACK;
        }

        player_attack(max(player_locate_y-player_reach, 0)+y, min(player_locate_x-player_reach, size_x-1)+x);
        break;

    case 2:
        player_move();
        break;

    case 3:
        player_useitem();
        break;

    case 4:
        INPUT_INTERACTION:;

        printmap(player_locate_y-player_reach, player_locate_x-player_reach, player_locate_y+player_reach+1, player_locate_x+player_reach+1);
        printf("\n where is you want interaction? y, x (started 0)\n");
        scanf("%d %d", &y, &x);
        while (getchar() != '\n');

        if(y < 0 || y > player_reach*2 + 1 ||x < 0 || x > player_reach*2 + 1){
            printf("no you can't\n");
            goto INPUT_INTERACTION;
        }

        player_interaction(max(player_locate_y-player_reach, 0)+y, min(player_locate_x-player_reach, size_x-1)+x);

        break;

    default:
        printf("input 1 or 2 or 3 or 4\n");
        goto INPUT_ACTION;
        break;
    }
}

void monster_move_2(int a, int sub){
    switch (a)
    {
    case 0:
        if(sub_map[m[sub].y+1][m[sub].x]){
            monster_move_2(mkrand(0, 3), sub);
            return;
        }
        sub_map[m[sub].y+1][m[sub].x] = map[m[sub].y][m[sub].x];
        sub_map[m[sub].y][m[sub].x] = 0;
        m[sub].y++;
        break;
        
    case 1:
        if(sub_map[m[sub].y-1][m[sub].x]){
            monster_move_2(mkrand(0, 3), sub);
            return;
        }
        sub_map[m[sub].y-1][m[sub].x] = map[m[sub].y][m[sub].x];
        sub_map[m[sub].y][m[sub].x] = 0;
        m[sub].y--;
        break;
        
    case 2:
        if(sub_map[m[sub].y][m[sub].x+1]){
            monster_move_2(mkrand(0, 3), sub);
            return;
        }
        sub_map[m[sub].y][m[sub].x+1] = map[m[sub].y][m[sub].x];
        sub_map[m[sub].y][m[sub].x] = 0;
        m[sub].x++;
        break;
        
    case 3:
        if(sub_map[m[sub].y][m[sub].x-1]){
            monster_move_2(mkrand(0, 3), sub);
            return;
        }
        sub_map[m[sub].y][m[sub].x-1] = map[m[sub].y][m[sub].x];
        sub_map[m[sub].y][m[sub].x] = 0;
        m[sub].x--;
        break;
    
    default:
        break;
    }
}

void monster_move(int sub){
    if(player_locate_y >= m[sub].y){
        if(player_locate_x >= m[sub].x){
            if(map[m[sub].y][m[sub].x+1] == 0){
                monster_move_2(2, sub);
            }
            else if(map[m[sub].y+1][m[sub].x] == 0){
                monster_move_2(0, sub);
            }
            else monster_move_2(mkrand(0, 4), sub);
        }
        else{
            if(map[m[sub].y][m[sub].x-1] == 0){
                monster_move_2(3, sub);
            }
            else if(map[m[sub].y+1][m[sub].x] == 0){
                monster_move_2(0, sub);
            }
            else monster_move_2(mkrand(0, 4), sub);
        }
    }
    else{
        if(player_locate_x >= m[sub].x){
            if(map[m[sub].y][m[sub].x+1] == 0){
                monster_move_2(2, sub);
            }
            else if(map[m[sub].y-1][m[sub].x] == 0){
                monster_move_2(1, sub);
            }
            else monster_move_2(mkrand(0, 4), sub);
        }
        else{
            if(map[m[sub].y][m[sub].x-1] == 0){
                monster_move_2(3, sub);
            }
            else if(map[m[sub].y-1][m[sub].x] == 0){
                monster_move_2(1, sub);
            }
            else monster_move_2(mkrand(0, 4), sub);
        }
    }
}

int main(){
    system("cls");

    srand(time(NULL));
    print_rule();
    MAPSIZE:;
    
    mkmap();

    while(maked_monster && player_hp > 0){
        printmap(0, 0, size_y, size_x);

        player_action();

        for(int i=0;i<size_y;i++){
            for(int j=0;j<size_x;j++){
                map[i][j] = sub_map[i][j];
            }
        }

        printmap(0, 0, size_y, size_x);

        printf("monster action");
        for(int i=0;i<2;i++){
            printf(". ");
            Sleep(1000);
        }
        printf("\n\n");
        
        monster_action(0, 0, size_y, size_x);

        for(int i=0;i<size_y;i++){
            for(int j=0;j<size_x;j++){
                map[i][j] = sub_map[i][j];
            }
        }
    }

    if(player_hp > 0) {
        system("cls");
        printmap(0, 0, size_y, size_x);
        printf("you kill every monster, you win");
    }
    else if(player_hp <= 0){
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 15);
        printf("you die\n");
        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7);
    }
    else printf("error rine END");

    return 0;
}