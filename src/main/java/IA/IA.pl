 :-use_module(library(plunit)).
 :-use_module(library(clpfd)).


testCase(L,X) :-
    nth0(X,L,I),
    I>0,fail.

testLigne(L,Li,T) :-
    nth0(Li*4,L,I),
    I\=T,
    nth0(Li*4+1,L,I),
    I\=T,
    nth0(Li*4+2,L,I),
    I\=T,
    nth0(Li*4+3,L,I),
    I\=T.

testColonne(L,Co,T) :-
    nth0(Co,L,I),
    I\=T,
    nth0(Co+4,L,I),
    I\=T,
    nth0(Co+8,L,I),
    I\=T,
    nth0(Co+12,L,I).


testCarre(L,Li,Co,T):-
    Li mod 2 == 0,
    Co mod 2 == 0,
    nth0(Li*5+Co+1,L,I),
    I\=T.
testCarre(L,Li,Co,T):-
    Li mod 2 == 0,
    Co mod 2 == 1,
    nth0(Li*5+Co-1,L,I),
    I\=T.
testCarre(L,Li,Co,T):-
    Li mod 2 == 1,
    Co mod 2 == 0,
    nth0(Li*3+Co+1,L,I),
    I\=T.
testCarre(L,Li,Co,T):-
    Li mod 2 == 1,
    Co mod 2 == 1,
    nth0(Li*3+Co-1,L,I),
    I\=T.

fonction(L,Res):-
    X in 0..15,
    T in 0..3,
    testCase(L,X),
    Li is X//4,
    Co is X mod 4,
    testLigne(L,Li,T),
    testColonne(L,Co,T),
    testCarre(L,Li,Co,T),
    Res = [X,T].