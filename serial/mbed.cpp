
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#include <termios.h>
#include <sstream>

#include <errno.h>

#include <mariadb/mysql.h>

#include <ctime>

#include <unistd.h>

#include <time.h>


using namespace std;

void gsub( std::string& data, const std::string& match, const std::string& replace) {
   // Get the first occurrence
   size_t pos = data.find(match);
 
   // Repeat till end is reached
   while( pos != std::string::npos)
    {
        data.replace(pos, match.size(), replace);
     
       // Get the next occurrence from the current position
        pos = data.find(match, pos+replace.size());
    }
}

void explode( string input, string match , char sep , string &val ) { 

    string temp = input;

    bool show = true;
    
    for( int i = 0 ; i < match.length() ; i++ ){
        if ( temp[i] != match[i] ){
            show = false;
        }
    }
    
    if ( show == true ){
    	gsub( temp , "Press[1]: " , "");
    	gsub( temp , "hPa" , "");
    	//findAndReplaceAll( temp , "hPa" , "");
    	gsub( temp , "Temp[0]: +" , "");
    	gsub( temp , " degC" , "");

    	gsub( temp , "Hum[0]:" , "");
    	gsub( temp , "%" , "");
    	gsub( temp , " " , "");
    	gsub( temp , "\n" , "");

    	//findAndReplaceAll( temp , match , "");
    	
    	float aNumero = strtof( temp.c_str() , 0);
        //cout << "Temp" << aNumero << "\n";
        val = temp;
        return;
    }
    
    return;
}

int explode50( string input, char sep , int counter ){ 

    istringstream buffer(input);
    
    string temp;

        
    bool show = true;
    
    for( int i = 0 ; i < temp.length() ; i++ ){
        if ( temp[i] == '\n' ){
            counter++;
        }
    }
    
    if ( counter < 50 ){
        cout << counter << " " << temp << "\n";
    }
    
    return counter;
}

struct Login  
 {
 	string username;
 	string password;
 	string host;
 	string database;
 }; 


 string Jsonread( string data ){

    

    return "";

 }


struct Login Config( string file ){

	int sfd = open( file.c_str() , O_RDONLY | O_NOCTTY );
	char buf[1024];
	int res = read( sfd , buf , 1024); // lire le buffer
    buf[res] = 0; // met le caractere fin de chaine a la fin du buffer

    //cout << buf << '\n';

    istringstream iss( (string)buf );

    struct Login conf;

    do
    {
        string subs;
        string key = "";
        string value = "";

        iss >> subs;
        //cout << "Substring: " << subs << endl;

        int isvalue = false;

        for( int i = 0 ; i < subs.length() ; i++ ){

        	//cout << subs[i];

        	if ( subs[i] ==  ';' ){
        		//cout << "ok";
        		cout << key << " = " <<  value << " owo\n";

        		if( key == "host"){ 
        			conf.host = value;
        		}else if( key == "username"){ 
        			conf.username = value;
        		}else if( key == "password"){ 
        			conf.password = value;
        		}else if( key == "database"){ 
        			conf.database = value;
        		}

        		//pas genial conf[key] serait parfait mais ..

        		key = "";
        		value = "";
        		isvalue = false;

        	}
        	if ( subs[i] == '=' ){
        		isvalue = true;
        	}else if (isvalue == false){
        		key = key + subs[i];
        	}else{
        		value = value + subs[i];
        	}


    	}

    	//cout << subs;

    } while (iss);

	//https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string

	return conf;

}



int main(){

	MYSQL *conn;

	Login config = Config( "./config" );

	if ((conn = mysql_init(NULL)) == NULL){
		fprintf(stderr, "Could not init DB\n");
		return EXIT_FAILURE;
	}
	if (mysql_real_connect(conn, config.host.c_str(), config.username.c_str() , config.password.c_str() , config.database.c_str() , 0, NULL, 0) == NULL){
		fprintf(stderr, "DB Connection Error\n");
		return EXIT_FAILURE;
	}
	
	if (mysql_query(conn, "CREATE TABLE IF NOT EXISTS `Mesures` (id INT PRIMARY KEY NOT NULL auto_increment  , DATE time NOT NULL DEFAULT CURRENT_TIMESTAMP,  degres FLOAT , humidie FLOAT , pression FLOAT );") != 0){
		fprintf(stderr, "Query Failure\n");
		return EXIT_FAILURE;
	}

	const int BAUDRATE = 115200;
	
	int sfd = open("/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_0671FF323338424E43012758-if02" , O_RDONLY | O_NOCTTY );// ouvrir le fichier 
	
	if( sfd == -1 ){ // verifier si il y a une erreur
		printf( "ERROR %d\n" , errno);
		printf( "%s\n" , strerror(errno) );
		return (-1);
    }else{
        struct termios configTerm;
        
        bzero( &configTerm , sizeof(configTerm) );
        
        configTerm.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
        
        configTerm.c_iflag = IGNPAR | ICRNL;
        
        configTerm.c_lflag =  ICANON;
        
        tcflush( sfd , TCIFLUSH );
        
        tcsetattr( sfd , TCSANOW , &configTerm );
        
        char buf[255];
        char oldbuf[255];
        int res;
        bool newline = false;
        int counter = 0;

        cout << "Port série ouvert en lecture seule" << "\n"; // port ouvert avec succes 
        
       	std::time_t timestamp = std::time(0);

       	std::time_t lasttimestamp = std::time(0);

        cout << timestamp << "\n";

        string temperature = "NULL";

        string pression = "NULL";

        string humidie = "NULL";

        int lignes = 0;

        while( true ){


        	timestamp = std::time(0);

            res = read( sfd , buf , 255); // lire le buffer
            buf[res] = 0; // met le caractere fin de chaine a la fin du buffer

            explode( (string)buf , "Temp[0]" , '\n' , temperature );
            explode( (string)buf , "Hum[0]" , '\n' , humidie );
            explode( (string)buf , "Press[1]" , '\n' , pression );

            //if( timestamp > lasttimestamp + 3 ){ // req sql toute les 3 sec

           	if( lignes > 50 ){
            	lasttimestamp = std::time(0);

            	string str = string ("INSERT INTO mesures ( degres , humidie , pression ) VALUES ( ") + temperature + string(" , ") + humidie + string(" , ") + pression + string(" )");
           		
           		cout << str << "\n";
				
           		if (mysql_query(conn, str.c_str() ) != 0){
					fprintf(stderr, "Query Failure\n");
					
					fprintf(stderr, "\n");
				}

				return 0; // fin du prog
            }
        }
    }
    
    
    
    return 0;
    
    
}


