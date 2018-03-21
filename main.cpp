#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <pqxx/pqxx>
#include <cstdlib>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

string player_attr = "PLAYER_ID,TEAM_ID,UNIFORM_NUM,FIRST_NAME,LAST_NAME,MPG,PPG,RPG,APG,SPG,BPG";
string team_attr = "TEAM_ID,NAME,STATE_ID,COLOR_ID,WINS,LOSSES";
string state_attr = "STATE_ID,NAME";
string color_attr = "COLOR_ID,NAME";

void inidatabase(connection * C){

  string create_state("CREATE TABLE STATE(STATE_ID SERIAL PRIMARY KEY NOT NULL, NAME TEXT UNIQUE NOT NULL);");
  string create_color("CREATE TABLE COLOR(COLOR_ID SERIAL PRIMARY KEY NOT NULL, NAME TEXT UNIQUE NOT NULL);");
  string create_team("CREATE TABLE TEAM(TEAM_ID SERIAL PRIMARY KEY NOT NULL, NAME TEXT UNIQUE NOT NULL, STATE_ID INT NOT NULL CHECK(STATE_ID>0), COLOR_ID INT NOT NULL CHECK(COLOR_ID>=0), WINS INT CHECK(WINS>=0), LOSSES INT CHECK(LOSSES>=0), FOREIGN KEY (STATE_ID) REFERENCES STATE(STATE_ID), FOREIGN KEY (COLOR_ID) REFERENCES COLOR(COLOR_ID));");
  string create_player("CREATE TABLE PLAYER(PLAYER_ID SERIAL PRIMARY KEY NOT NULL, TEAM_ID INT NOT NULL CHECK(TEAM_ID>0), UNIFORM_NUM INT CHECK(UNIFORM_NUM>=0), FIRST_NAME TEXT, LAST_NAME TEXT,MPG INT CHECK(MPG>=0), PPG INT CHECK(PPG>=0), RPG INT CHECK(RPG>=0), APG INT CHECK(APG>=0), SPG DECIMAL CHECK(SPG>=0), BPG DECIMAL CHECK(BPG>=0), CONSTRAINT TEAM_ID_UNIFORM_NUM UNIQUE(TEAM_ID,UNIFORM_NUM), FOREIGN KEY (TEAM_ID) REFERENCES TEAM(TEAM_ID));") ;
  
  string drop_player("DROP TABLE  IF EXISTS PLAYER;");
  string drop_team("DROP TABLE IF EXISTS TEAM;");
  string drop_state("DROP TABLE IF EXISTS STATE;");
  string drop_color("DROP TABLE IF EXISTS COLOR;");

  work W(*C);
  W.exec(drop_player);
  W.exec(drop_team);
  W.exec(drop_state);
  W.exec(drop_color);
  W.exec(create_state);
  W.exec(create_color);
  W.exec(create_team);
  W.exec(create_player);
  W.commit();
}

string insert(string t_name, string attr,string value){
  string res("INSERT INTO ");
  res+=t_name;
  res+=" (";
  res+=attr;
  res+=") VALUES (";
  res+=value;
  res+=");";
  return res;
}

void readfile(connection *C){
  string temp;
  ifstream state_fin("state.txt");
  while(getline(state_fin,temp)){
    size_t i=temp.find_first_of(" ");
    string name = temp.substr(i+1);
    add_state(C,name);
  } 

  ifstream color_fin("color.txt");
  while(getline(color_fin,temp)){
    size_t i=temp.find_first_of(" ");
    string name = temp.substr(i+1);
    add_color(C,name);
  }

  ifstream team_fin("team.txt");
  while(getline(team_fin,temp)){
    size_t i=temp.find_first_of(" ");
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    string name = temp.substr(0,i);
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int state_id = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int color_id = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int wins = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    //i=temp.find_first_of(" ");
    int losses = atoi(temp.c_str());
    //temp = temp.substr(i+1);
    add_team(C,name,state_id,color_id,wins,losses);
  }

  ifstream player_fin("player.txt");
  while(getline(player_fin,temp)){
    size_t i=temp.find_first_of(" ");
    temp = temp.substr(i+1);    

    i=temp.find_first_of(" ");
    int team_id = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int uniform_num = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    string fname = temp.substr(0,i);
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    string lname = temp.substr(0,i);
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int mpg = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int ppg = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);    

    i=temp.find_first_of(" ");
    int rpg = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);

    i=temp.find_first_of(" ");
    int apg = atoi(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);          

    i=temp.find_first_of(" ");
    double spg = atof(temp.substr(0,i).c_str());
    temp = temp.substr(i+1);    

    double bpg = atof(temp.c_str());   

    add_player(C,team_id,uniform_num,fname,lname,mpg,ppg,rpg,apg,spg,bpg);  
  }
}
int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=acc_bball user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files
  inidatabase(C);
  readfile(C);

  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


