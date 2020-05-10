 :-use_module(library(plunit)).
 :-use_module(library(clpfd)).
 :-use_module(library(random)).


testCase(L,X) :-
    nth0(X,L,I),
    I<0.

testColor(C,C2,T,T2):-
    C\=C2,
    T\=T2.
testColor(C,C,_,_).

testLigne(L,Li,T,C) :-
    E is Li*4,
    nth0(E,L,[Type,Coul]),
    testColor(C,Coul,Type,T),
    F is Li*4+1,
    nth0(F,L,[Type1,Coul1]),
    testColor(C,Coul1,Type1,T),
    G is Li*4+2,
    nth0(G,L,[Type2,Coul2]),
    testColor(C,Coul2,Type2,T),
    H is Li*4+3,
    nth0(H,L,[Type3,Coul3]),
    testColor(C,Coul3,Type3,T).

testColonne(L,Co,T,C) :-
    nth0(Co,L,[Type,Coul]),
    testColor(C,Coul,Type,T),
    E is Co+4,
    nth0(E,L,[Type1,Coul1]),
    testColor(C,Coul1,Type1,T),
    F is Co+8,
    nth0(F,L,[Type2,Coul2]),
    testColor(C,Coul2,Type2,T),
    G is Co+12,
    nth0(G,L,[Type3,Coul3]),
    testColor(C,Coul3,Type3,T).


testCarre(L,Li,Co,T,C):-
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 0,
    G is (1+Li)*4+Co+1,
    nth0(G,L,[Type,Coul]),
    testColor(C,Coul,Type,T),!.
testCarre(L,Li,Co,T,C):-
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 1,
    G is (1+Li)*4+Co-1,
    nth0(G,L,[Type,Coul]),
    testColor(C,Coul,Type,T),!.
testCarre(L,Li,Co,T,C):-
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 0,
    G is (Li-1)*4+Co+1,
    nth0(G,L,[Type,Coul]),
    testColor(C,Coul,Type,T),!.
testCarre(L,Li,Co,T,C):-
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 1,
    G is (Li-1)*4+Co-1,
    nth0(G,L,[Type,Coul]),
    testColor(C,Coul,Type,T),!.

fonction(L,L2,Case,Gagnant,Ligne,Colonne,Piece):-
    (caseGagnante(L,Case,Li,Co,T) ->
    Ligne = Li,
    Colonne = Co,
    Piece = T,
    Gagnant = 1
    );
    random_member(X,Case),
    delete(Case,X,CaseX),
    (testCase(L,X) ->
    Li is X//4,
    Co is X mod 4,
    random_member(T,L2),
    delete(L2,T,L2P),
    (testLigne(L,Li,T) ->
    (testColonne(L,Co,T)->
    (testCarre(L,Li,Co,T)->
    Ligne = Li,
    Colonne = Co,
    Piece = T,
    Gagnant = 0
    ;
    fonction(L,L2P,CaseX,Gagnant,Ligne,Colonne,Piece),!)
    ;
    fonction(L,L2P,CaseX,Gagnant,Ligne,Colonne,Piece),!)
    ;
    fonction(L,L2P,CaseX,Gagnant,Ligne,Colonne,Piece),!)
    ;
    fonction(L,L2,CaseX,Gagnant,Ligne,Colonne,Piece),!).

testLigneGagnant(L,Li,Co,[],Acc1):-
    E is Li * 4 + Co,
    nth0(E,L,[Type,_]),
    Type =\= -1,
    Acc1 = [Type],!.

testLigneGagnant(L,Li,Co,Acc,Acc1):-
    E is Li * 4 + Co,
    nth0(E,L,[Type,_]),
    Type =\= -1,
    \+member(Type,Acc),
    append([Type],Acc,Acc1).
    

testLignesGagnant(L,Li,Co,T) :-
    E is Co + 1, 
    E1 is E mod 4,
    testLigneGagnant(L,Li,E1,[],Acc1),
    F is (Co + 2),
    F1 is F mod 4,
    testLigneGagnant(L,Li,F1,Acc1,Acc2),
    G is Co + 3,
    G1 is G mod 4,
    testLigneGagnant(L,Li,G1,Acc2,Acc3),
    msort(Acc3,Sort),
    select(T,[0,1,2,3],Sort),!.


testColonnesGagnant(L,Li,Co,T):-
    E is Co + 4,
    testColonneGagnant(L,Li,E,[],Acc1),
    F is Co + 8,
    testColonneGagnant(L,Li,F,Acc1,Acc2),
    G is Co + 12,
    testColonneGagnant(L,Li,G,Acc2,Acc3),
    msort(Acc3,Sort),
    select(T,[0,1,2,3],Sort).

testColonneGagnant(L,Li,Co,[],Acc1):-
    E is Li * 4 + Co,
    E1 is E mod 16,
    nth0(E1, L, [Type,_]),
    Type =\= -1,
    Acc1 = [Type],!.

testColonneGagnant(L,Li,Co,Acc,Acc1):-
    E is Li * 4 + Co,
    E1 is E mod 16,
    nth0(E1, L, [Type,_]),
    Type =\= -1,
    \+member(Type,Acc),
    append([Type],Acc,Acc1).

testCarreGagnant(L,Li,Co,T):-
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 0,
    G is (1+Li)*4+Co+1,
    nth0(G,L,[Type,_]),
    Type =\= -1,
    Acc = [Type],
    G1 is Li * 4 + Co + 1,
    nth0(G1,L,[Type1,_]),
    Type1 =\= -1,
    \+member(Type1,Acc),
    append([Type1],Acc,Acc1),
    G2 is (1 + Li) * 4 + Co,
    nth0(G2,L,[Type2,_]),
    Type2 =\= -1,
    \+member(Type2,Acc1),
    append([Type2],Acc1,Acc2),
    msort(Acc2,Sort),
    select(T,[0,1,2,3],Sort),!.

testCarreGagnant(L,Li,Co,T):-
    E is Li mod 2,
    E == 0,
    F is Co mod 2,
    F == 1,
    G is (1+Li)*4+Co-1,
    nth0(G,L,[Type,_]),
    Type =\= -1,
    Acc = [Type],
    G1 is Li * 4 + Co - 1,
    nth0(G1,L,[Type1,_]),
    Type1 =\= -1,
    \+member(Type1,Acc),
    append([Type1],Acc,Acc1),
    G2 is (1 + Li) * 4 + Co,
    nth0(G2,L,[Type2,_]),
    Type2 =\= -1,
    \+member(Type2,Acc1),
    append([Type2],Acc1,Acc2),
    msort(Acc2,Sort),
    select(T,[0,1,2,3],Sort),!.

testCarreGagnant(L,Li,Co,T):-
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 0,
    G is (Li-1)*4+Co+1,
    nth0(G,L,[Type,_]),
    Type =\= -1,
    Acc = [Type],
    G1 is Li * 4 + Co + 1,
    nth0(G1,L,[Type1,_]),
    Type1 =\= -1,
    \+member(Type1,Acc),
    append([Type1],Acc,Acc1),
    G2 is (Li -1) * 4 + Co,
    nth0(G2,L,[Type2,_]),
    Type2 =\= -1,
    \+member(Type2,Acc1),
    append([Type2],Acc1,Acc2),
    msort(Acc2,Sort),
    select(T,[0,1,2,3],Sort),!.


testCarreGagnant(L,Li,Co,T):-
    E is Li mod 2,
    E == 1,
    F is Co mod 2,
    F == 1,
    G is (Li-1)*4+Co-1,
    nth0(G,L,[Type,_]),
    Type =\= -1,
    Acc = [Type],
    G1 is Li * 4 + Co - 1,
    nth0(G1,L,[Type1,_]),
    Type1 =\= -1,
    \+member(Type1,Acc),
    append([Type1],Acc,Acc1),
    G2 is (Li-1) * 4 + Co,
    nth0(G2,L,[Type2,_]),
    Type2 =\= -1,
    \+member(Type2,Acc1),
    append([Type2],Acc1,Acc2),
    msort(Acc2,Sort),
    select(T,[0,1,2,3],Sort).

testGagnant(L,Li,Co,T):-
    testCarreGagnant(L,Li,Co,T),!.
testGagnant(L,Li,Co,T):-
    testLignesGagnant(L,Li,Co,T),!.
testGagnant(L,Li,Co,T):-
    testColonnesGagnant(L,Li,Co,T),!.


caseGagnante(L,Case,Li,Co,T):-
    select(C,Case,_),
    Li is C // 4,
    Co is C mod 4,
    testGagnant(L,Li,Co,T),
    testLigne(L,Li,T,0),
    testColonne(L,Co,T,0),
    testCarre(L,Li,Co,T,0).