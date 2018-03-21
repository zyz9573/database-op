#include "query_funcs.h"
#include <ctype.h>
#include <sstream>
string dtos(double d){
	stringstream ss;
	ss<<d;
	return ss.str();
}
string deal_single_quote(string ttemp){
  string res;
  size_t i = 0;
  while(i!=string::npos){
    i=ttemp.find_first_of("'");
    res+=ttemp.substr(0,i);
    if(i==string::npos){break;}
    res+="''";
    ttemp = ttemp.substr(i+1);
  }
  return res;
}

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
	string res("INSERT INTO PLAYER (TEAM_ID,UNIFORM_NUM,FIRST_NAME,LAST_NAME,MPG,PPG,RPG,APG,SPG,BPG) VALUES(");
	try{
	work W(*C);

	res+=to_string(team_id);res+=",";
	res+=to_string(jersey_num);res+=",";
	res+=W.quote(first_name);res+=",";
	res+=W.quote(last_name);res+=",";
	res+=to_string(mpg);res+=",";
	res+=to_string(ppg);res+=",";
	res+=to_string(rpg);res+=",";
	res+=to_string(apg);res+=",";
	res+=dtos(spg);res+=",";
	res+=dtos(bpg);
	res+=");";
	
	
	W.exec(res); 
	W.commit();		
	}
	catch(...){
		cout<<res<<endl;
		cout<<"add player failed\r\n";
	}

}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
	string res("INSERT INTO TEAM (NAME,STATE_ID,COLOR_ID,WINS,LOSSES) VALUES(");
	try{
	work W(*C);
	
	res+=W.quote(name);res+=",";
	res+=to_string(state_id);res+=",";
	res+=to_string(color_id);res+=",";
	res+=to_string(wins);res+=",";
	res+=to_string(losses);
	res+=");";
	
		
	W.exec(res); 
	W.commit();		
	}
	catch(...){
		cout<<res<<endl;
		cout<<"add team failed\r\n";
	}
}


void add_state(connection *C, string name)
{
	string res("INSERT INTO STATE (NAME) VALUES(");
	work W(*C);
	res+=W.quote(name);
	res+=");";
	try{
		
		W.exec(res); 
		W.commit();		
	}
	catch(...){
		cout<<res<<endl;
		cout<<"add state failed\r\n";
	}
}


void add_color(connection *C, string name)
{
	string res("INSERT INTO COLOR (NAME) VALUES(");
	work W(*C);
	res+=W.quote(name);
	res+=");";
	try{
		
		W.exec(res); 
		W.commit();		
	}
	catch(...){
		cout<<res<<endl;
		cout<<"add color failed\r\n";
	}
}


void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
	string res("SELECT * FROM PLAYER WHERE");
	if(use_mpg!=0){
		res = res + " mpg<=" + to_string(max_mpg) + " and "+"mpg>="+to_string(min_mpg)+" and ";
	}
	if(use_ppg!=0){
		res = res + " ppg<=" + to_string(max_ppg) + " and "+"ppg>="+to_string(min_ppg)+" and ";
	}
	if(use_rpg!=0){
		res = res + " rpg<=" + to_string(max_rpg) + " and "+"rpg>="+to_string(min_rpg)+" and ";
	}
	if(use_apg!=0){
		res = res + " apg<=" + to_string(max_apg) + " and "+"apg>="+to_string(min_apg)+" and ";
	}
	if(use_spg!=0){
		res = res + " spg<=" + dtos(max_spg) + " and "+"spg>="+dtos(min_spg)+" and ";
	}
	if(use_bpg!=0){
		res = res + " bpg<=" + dtos(max_bpg) + " and "+"bpg>="+dtos(min_bpg)+" and ";
	}
	res+="TRUE ;";
   	nontransaction N(*C);
    result R( N.exec( res ));

    cout<<"PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"<<"\r\n";
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
      cout <<c[0].as<int>()<<" "<<c[1].as<int>()<<" "<<c[2].as<int>()<<" "<<c[3].as<string>()<<" "<<c[4].as<string>()<<" "<<c[5].as<int>()<<" "<<c[6].as<int>()<<" "<<c[7].as<int>()<<" "<<c[8].as<int>()<<" "<<c[9].as<double>()<<" "<<c[10].as<double>()<<" "<<"\r\n";
    }	
}


void query2(connection *C, string team_color)
{
	string res("select team.name,color.name from team,color where team.color_id = color.color_id");
   	res=res+" and color.name = '"+team_color+"';";
   	nontransaction N(*C);
    result R( N.exec( res ));
    cout<<"TEAM_NAME"<<"\r\n";
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    	cout<<c[0].as<string>()<<"\r\n";
    }
}


void query3(connection *C, string team_name)
{
	string res("select first_name,last_name from player,team where team.team_id = player.team_id and team.name = '");
	res=res+team_name+"' order by ppg desc;";
   	nontransaction N(*C);
    result R( N.exec( res ));	
	cout<<"FIRST_NAME LAST_NAME"<<"\r\n";
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    	cout<<c[0].as<string>()<<" "<<c[1].as<string>()<<"\r\n";
    }
}


void query4(connection *C, string team_state, string team_color)
{
	string res("select first_name,last_name,uniform_num from player,team,color,state where player.team_id = team.team_id and team.color_id = color.color_id and team.state_id = state.state_id and state.name = '");
	res= res+team_state+"' and color.name = '"+team_color+"' ;";
    nontransaction N(*C);
    result R( N.exec( res ));	
	cout<<"FIRST_NAME LAST_NAME UNIFORM_NUM"<<"\r\n";
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    	cout<<c[0].as<string>()<<" "<<c[1].as<string>()<<" "<<c[2].as<int>()<<"\r\n";
    }
}


void query5(connection *C, int num_wins)
{
	string res("select first_name,last_name,team.name,team.wins from player,team where player.team_id = team.team_id and team.wins>=");
	res=res+to_string(num_wins)+";";
    nontransaction N(*C);
    result R( N.exec( res ));	
	cout<<"FIRST_NAME LAST_NAME TEAM_NAME WINS"<<"\r\n";
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    	cout<<c[0].as<string>()<<" "<<c[1].as<string>()<<" "<<c[2].as<string>()<<" "<<c[3].as<int>()<<"\r\n";
    }  	
}
