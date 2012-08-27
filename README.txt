

THIS IS NOW OBSOLETE. PLEASE LOOK AT MY HASKELL VERSION AT:

https://github.com/foopq/coding-challenge-haskell








Requirements:

    This was built on an up to date Ubuntu system with gcc 4.4.3. 
    The only external requirement is the boost library (version 1.40.0 was used).


Building instructions:

    To build, simply run the ./build.sh script.


Running instructions:

    Run using:

        ./snapsort-challenge <listings.txt> <products.txt> <numThreads>

    The result will be in the file "results.json"


Notes:

    The json parser isn't mine, but from http://jsoncpp.sourceforge.net/

    I'm assuming non-unicode data and used std::strings everywhere in my code.

    My coding style is a bottom up approach, so as far as reading the code goes, start at the bottom :)
