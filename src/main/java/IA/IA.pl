 :-use_module(library(plunit)).
 :-use_module(library(clpfd)).
 :-use_module(library(random)).


testCase(L,X) :-
    nth0(X,L,I),
    I<0.

testLigne(L,Li,T) :-
    E is Li*4,
    nth0(E,L,I),
    I=\=T,
    F is Li*4+1,
    nth0(F,L,I1),
    I1=\=T,
    G is Li*4+2,
    nth0(G,L,I2),
    I2=\=T,
    H is Li*4+3,
    nth0(H,L,I3),
    I3=\=T.

testColonne(L,Co,T) :-
    nth0(Co,L,I),
    I=\=T,
    E is Co+4,
    nth0(E,L,I1),
    I1=\=T,
    F is Co+8,
    nth0(F,L,I2),
    I2=\=T,
    G is Co+12,
    nth0(G,L,I3),
    I3=\=T.


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



fonction(L,L2,Case,Gagnant,Ligne,Colonne,Piece):-
    (caseGagnante(L,Case,Li,Co,T) ->
    Ligne = Li,
    Colonne = Co,
    Piece = T,
    Gagnant = 1
    );
    select(X,Case,CaseX),
    (testCase(L,X) ->
    Li is X//4,
    Co is X mod 4,
    select(T,L2,L2P),
    (testLigne(L,Li,T) ->
    (testColonne(L,Co,T)->
    (testCarre(L,Li,Co,T)->
    Ligne = Li,
    Colonne = Co,
    Piece = T,
    Gagnant = 0
    ;
    fonction(L,L2,CaseX,Gagnant,Ligne,Colonne,Piece),!)
    ;
    fonction(L,L2,CaseX,Gagnant,Ligne,Colonne,Piece),!)
    ;
    fonction(L,L2,CaseX,Gagnant,Ligne,Colonne,Piece),!)
    ;
    fonction(L,L2,CaseX,Gagnant,Ligne,Colonne,Piece),!).

testLigneGagnant(L,Li,Co, Ps,PsI):-
    E is Li * 4 + Co,
    nth0(E,L,I),
    I =\= -1,
    select(I,Ps,PsI).

testLignesGagnant(L,Li,Co,T) :-
    Ps = [0,1,2,3],
    E is Co + 1 mod 4,
    testLigneGagnant(L,Li,E,Ps,PsI),
    F is Co + 2 mod 4,
    testLigneGagnant(L,Li,F,PsI,PsI1),
    G is Co + 3 mod 4,
    testLigneGagnant(L,Li,G,PsI1,PsI2),
    nth0(0,PsI2,T).


testColonnesGagnant(L,Li,Co,T):-
    Ps = [0,1,2,3],
    E is Co + 4 mod 16,
    testColonneGagnant(L,Li,E,Ps,PsI),
    F is Co + 8 mod 16,
    testColonneGagnant(L,Li,F,PsI,PsI1),
    G is Co + 12 mod 16,
    testColonneGagnant(L,Li,G,PsI1,PsI2),
    nth0(0,PsI2,T).

testColonneGagnant(L,Li,Co,Ps,PsI):-
    E is Li * 4 + Co,
    nth0(E, L, I),
    I =\= -1,
    select(I,Ps,PsI).

testCarreGagnant(L,Li,Co,T):-
    Ps = [0,1,2,3],
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 0,
    G is (1+Li)*4+Co+1,
    nth0(G,L,I),
    I =\= -1,
    select(I,Ps,PsI),
    G1 is Li * 4 + Co + 1,
    nth0(G1,L,I1),
    I1 =\= -1,
    select(I1,PsI,PsI1),
    G2 is (1 + Li) * 4 + Co,
    nth0(G2,L,I2),
    I2 =\= -1,
    select(I2,PsI1,PsI2),
    nth0(0,PsI2,T),!.

testCarreGagnant(L,Li,Co,T):-
    Ps = [0,1,2,3],
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 1,
    G is (1+Li)*4+Co-1,
    nth0(G,L,I),
    I =\= -1,
    select(I,Ps,PsI),
    G1 is Li * 4 + Co - 1,
    nth0(G1,L,I1),
    I1 =\= -1,
    select(I1,PsI,PsI1),
    G2 is (1 + Li) * 4 + Co,
    nth0(G2,L,I2),
    I2 =\= -1,
    select(I2,PsI1,PsI2),
    nth0(0,PsI2,T),!.

testCarreGagnant(L,Li,Co,T):-
    Ps = [0,1,2,3],
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 0,
    G is (Li-1)*4+Co+1,
    nth0(G,L,I),
    I =\= -1,
    select(I,Ps,PsI),
    G1 is Li * 4 + Co + 1,
    nth0(G1,L,I1),
    I1 =\= -1,
    select(I1,PsI,PsI1),
    G2 is (Li -1) * 4 + Co,
    nth0(G2,L,I2),
    I2 =\= -1,
    select(I2,PsI1,PsI2),
    nth0(0,PsI2,T),!.


testCarreGagnant(L,Li,Co,T):-
    Ps = [0,1,2,3],
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 1,
    G is (Li-1)*4+Co-1,
    nth0(G,L,I),
    I =\= -1,
    select(I,Ps,PsI),
    G1 is Li * 4 + Co - 1,
    nth0(G1,L,I1),
    I1 =\= -1,
    select(I1,PsI,PsI1),
    G2 is (Li-1) * 4 + Co,
    nth0(G2,L,I2),
    I2 =\= -1,
    select(I2,PsI1,PsI2),
    nth0(0,PsI2,T),!.

testGagnant(L,Li,Co,T):-
    testCarreGagnant(L,Li,Co,T),!.
testGagnant(L,Li,Co,T):-
    testLignesGagnant(L,Li,Co,T),!.
testGagnant(L,Li,Co,T):-
    testColonnesGagnant(L,Li,Co,T),!.


caseGagnante(L,Case,Li,Co,T):-
    select(C,Case,CaseC),
    nth0(C,L,P),
    Li is C // 4,
    Co is C mod 4,
    testGagnant(L,Li,Co,T),
    testLigne(L,Li,T),
    testColonne(L,Co,T),
    testCarre(L,Li,Co,T).