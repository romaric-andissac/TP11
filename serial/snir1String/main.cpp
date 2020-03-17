//
//  main.cpp
//  snir1String
//
//  Created by Cécile Tonnerre on 11/03/2020.
//  Copyright © 2020 Cécile Tonnerre. All rights reserved.
//

#include <iostream>
#include <string>
using namespace std;

int main(int argc, const char * argv[]) {
    // utilisation des chaînes de caractères mode c++ : objet de type string
    
    string maChaine ("Les sanlots longs des violons de l'automne ");
    cout << maChaine;
    
    // affiche la longueur
    cout << " longeur : " << maChaine.length() << endl ;
    
    // cherche la chaîne "violons" dans maChaine
    cout << " est-ce qu'il y a violons dedans ? " << maChaine.find("violons") << endl;
    
    cout << " est-ce qu'il y a guitares dedans ? " << maChaine.find("guitares") << endl;
    if (maChaine.find("guitares") == string::npos )
    {
        cout << "guitares n'apparaît pas dans maChaine " <<endl;
    }
    
    // je concatene avec l'operateur '+'
    maChaine += "\nBlessent mon coeur d'une langueur monotone";
    
    cout << "ma nouvelle chaine : \n "<< maChaine << endl;

    
    return 0;
}
