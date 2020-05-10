 :-use_module(library(plunit)).
 :-use_module(library(clpfd)).
 :- use_module(library(random)).


testCase(L,X) :-
    nth0(X,L,I),
    I<0.

testLigne(L,Li,T) :-
    E is Li*4,
    nth0(E,L,I),
    I=\=T,
    F is Li*4+1,
    nth0(F,L,I),
    I=\=T,
    G is Li*4+2,
    nth0(G,L,I),
    I=\=T,
    H is Li*4+3,
    nth0(H,L,I),
    I=\=T.

testColonne(L,Co,T) :-
    nth0(Co,L,I),
    I=\=T,
    E is Co+4,
    nth0(E,L,I),
    I=\=T,
    F is Co+8,
    nth0(F,L,I),
    I=\=T,
    G is Co+12,
    nth0(G,L,I),
    I=\=T.


testCarre(L,Li,Co,T):-
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 0,
    G is (1+Li)*4+Co+1,
    nth0(G,L,I),
    I=\=T,!.
testCarre(L,Li,Co,T):-
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 1,
    G is (1+Li)*4+Co-1,
    nth0(G,L,I),
    I=\=T,!.
testCarre(L,Li,Co,T):-
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 0,
    G is (Li-1)*4+Co+1,
    nth0(G,L,I),
    I=\=T,!.
testCarre(L,Li,Co,T):-
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 1,
    G is (Li-1)*4+Co-1,
    nth0(G,L,I),
    I=\=T,!.

fonction(L,L2,Res):-
    random_between(0,15,X),
    length(L2,TailleL2),
    testCase(L,X),
    Li is X//4,
    Co is X mod 4,
    TL2 is TailleL2 -1,
    random_between(0,TL2,P),
    nth0(P,L2,T),
    testLigne(L,Li,T),
    testColonne(L,Co,T),
    testCarre(L,Li,Co,T),
    Res = [Li,Co,T].

testLigneGagnant(L,Li,Co,T, Ps,PsI):-
    E is Li * 4 + Co,
    nth0(E,L,I),
    I =\= -1,
    select(I,Ps,PsI).

testLignesGagnant(L,Li,Co,T,Res) :-
    Ps = [0,1,2,3],
    E is Co + 1 mod 4,
    testLigneGagnant(L,Li,E,T,Ps,PsI),
    F is Co + 2 mod 4,
    testLigneGagnant(L,Li,F,T,PsI,PsI1),
    G is Co + 3 mod 4,
    testLigneGagnant(L,Li,G,T,PsI1,PsI2),
    nth0(0,PsI2,Res).


testColonnesGagnant(L,Li,Co,T,Res):-
    Ps = [0,1,2,3],
    E is Co + 4 mod 16,
    testColonneGagnant(L,Li,E,T,Ps,PsI),
    F is Co + 8 mod 16,
    testColonneGagnant(L,Li,F,T,PsI,PsI1),
    G is Co + 12 mod 16,
    testColonneGagnant(L,Li,G,T,PsI1,PsI2),
    nth0(0,PsI2,Res).

testColonneGagnant(L,Li,Co,T,Ps,PsI):-
    E is Li * 4 + Co,
    nth0(E, L, I),
    I =\= -1,
    select(I,Ps,PsI).

isGagnant(L, Li, Co, Pion):-
    testLigneGagnant(L,Li,Pion), !.
isGagnant(L, Li, Co, Pion):-
    testColonneGagnant(L,Li,Pion), !.