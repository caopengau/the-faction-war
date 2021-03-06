/*
MultiFactions
New Features:
1. income and recruit system

By Cao Peng, Oct 2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>	// for srand() a seed to rand()
#include <unistd.h>	// for sleep()

/* customized setting */
#define MAPSIZE 20	// multiples of 4
#define PIXEL_SIZE 4
#define MAPFOGMATERIAL '#'
#define WARFOGENABLED 1
#define ENEMYSYMBOL '<'
#define TERRAIN_VARIETY 3	// maximum 2 in this game version
#define ARMYSIZE 9

/* program dependency */
#include "mystructs.h"		// struct and typedef
Expansion ** EMAP;			// Expansions for additional income
#include "helpful.h"			// functions to reduce program redundancy */
#include "commandline.h"	// interaction with player
#include "units.h"			// units related function
#include "terrain.h"		// terrain system
#include "player.h"			// player functions

void pre_turn(Player *p1);
int turn(Player * p1, Player * p2);

int main(){	
	init_expansion();
	// /* brief the players */
	printf("\nWelcome to the Faction War 4.0 version, developed by Cao Peng, Oct 2016.\n\n");
	printf("Notable change in this version:\n1. Troops are recuitable so choose wisely\n");
	printf("\n2. Expansions generates incomes, so secure and protect those hex to gain supply advantage\n\n");

	enter_to_continue();
	printf("Do you want to start a 1v1 game?\n\n");
	enter_to_continue();
	printf("Are you sure? you need exactly 2 players to play the game! unless you can play it left brain vs right brain\n\n");
	enter_to_continue();
	printf("Okay, Let's go!\n\n");
	printf("Generating game...\n\n");
	
	Player p1 = createplayer(1, '@', 2, 2);
	// Unit *archer1 = Archer(0, 0, &p1);	/* testing units */
	// player_recruit(&p1, archer1);
	// Unit *swordman1 = Swordman(0,0, &p1);
	// player_recruit(&p1, swordman1);
	// Unit *horseman1 = Horseman(0,0, &p1);
	// player_recruit(&p1, horseman1);
	
	Player p2 = createplayer(2, '&', MAPSIZE-1, MAPSIZE-1);
	// Unit *archer2 = Archer(0, 0, &p2);	/* testing units */
	// player_recruit(&p2, archer2);
	// Unit *swordman2 = Swordman(0,0, &p2);
	// player_recruit(&p2, swordman2);
	// Unit *horseman2 = Horseman(0,0, &p2);
	// player_recruit(&p2, horseman2);
	
	printf("Mission: find and destroy all enemy units\n\n");
	enter_to_continue();
	while(1){
		if(turn(&p1, &p2)==10){
			printf("\n/****************/\nVictor is player %d!\n/****************/\n", p1.player_id);
			break;
		}
		if(turn(&p2, &p1)==10){
			printf("\n/****************/\nVictor is player %d!\n/****************/\n", p2.player_id);
			break;
		}
		
	}
	return 0;
}

/****************************************************************************/
/* a turn for one player */
int turn(Player * p1, Player * p2){
	int size = 10, i=0; Unit * unit;
	char * action, * raw_user_input, ** commands;
	getmap(p1, p2); pre_turn(p1); report(p1);
	
	printf("\nPlayer %d, take your turn\tgold: %d\n", p1->player_id, p1->resource);
	printf("key in \"help\" to see a list of useful command\n");
	while(1){
		getcommand(&raw_user_input, 3*size);
		commands = parsecommand(raw_user_input, 3);
		raw_user_input = NULL;
		if((i=checkcommand(commands, p1, p2))==1) break;
		else if(i == 3 || i == 2) continue;
		
		/* select a valid unit */
		i = atoi(commands[0])==p1->player_id && isdigit(commands[0][strlen(commands[0])-1]);
		if(i && atoi(commands[0]+strlen(commands[0])-1) <= p1->no_of_units){
			unit = p1->unit_list[atoi(commands[0]+strlen(commands[0])-1)-1];
		}else{
			printf("Invalid troop selected!\n"); continue;
		}
		
		/* perform a valid action */
		while(available_action(unit, p1, commands+1)){
			if(!commands[1]){ // towards a valid target
				getcommand(&action, 2*size);
				commands = parsecommand(action, 2);
				commands[2] = commands[1];
				commands[1] = commands[0];
			}
			if(strcmp(commands[1], "move")==0){
				while((i = move_unit(unit, commands[2], p1, p2))==2){
					commands[2] = 0;
				}
				if(i == 1){	// new vision from movement
					printf("New sight gained!\n");
					enter_to_continue(); getmap(p1, p2);
				}
				commands[1] = 0;			// action complete
			}else if(strcmp(commands[1], "attack")==0){
				if(attack_unit(unit, p1, p2, commands[2])==10) return 10;
				commands[1] = 0; commands[2] = 0;		// action complete
			}else if(strcmp(commands[1], "rest")==0){
				rest_unit(unit); commands[1] = 0;	// action complete
			}else{
				commands[1] = 0;	// invalid action ignored
				if((i = checkcommand(commands, p1, p2))==1) break;
				else if(i == 3) continue;	// report command
				else if(i == 2) break; printf("invalid action!\n"); 
			}
		}
	}
	fillwith(MAPSIZE*PIXEL_SIZE, '='); printf("\nTurn over!\n"); fillwith(MAPSIZE*PIXEL_SIZE, '=');
	printf("\n"); enter_to_continue();
	return 0;
}
