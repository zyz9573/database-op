#include "exerciser.h"

void exercise(connection *C)
{
	add_state(C,"NC");
	add_player(C,1,1, "Jerome", "Robinson", 34 ,19, 4 ,3 ,1.7, 0.4);
	cout<<"-----query1-----"<<"\r\n";
	query1(C,
		1,35,37,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0);
	cout<<"-----query2-----"<<"\r\n";
	query2(C,"DarkBlue");
	cout<<"-----query3-----"<<"\r\n";
	query3(C,"Duke");
	cout<<"-----query4-----"<<"\r\n";
	query4(C,"NC","DarkBlue");
	cout<<"-----query5-----"<<"\r\n";
	query5(C,7);
}
