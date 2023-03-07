#include <iostream>
#include <gmp.h>

#include "airthmetic.h"

#define BASE 62

using namespace That;

void Arit::GetNumber(std::string s, char **out, int *nd){
    // Convertim s un string a un nombre de GMP i el guardem a out!
    mpz_t num;
    mpz_init(num);

    // Ho posem a string
    mpz_set_str(num, s.c_str(), 10);
    
    std::string res;
    *out = mpz_get_str(NULL, BASE, num);
    *nd = mpz_sizeinbase(num, BASE);
}