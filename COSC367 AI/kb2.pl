% Playing around in Prolog

second([_|[X|_]],X).
swap12([X|[Y|Tail]], [Y|[X|Tail]]).

tran(eins,one).
tran(zwei,two).
tran(drei,three).
tran(vier,four).
tran(fuenf,five).
tran(sechs,six).
tran(sieben,seven).
tran(acht,eight).
tran(neun,nine).
listtran([],[]).
listtran([Hg|Tg], [He|Te]) :- tran(Hg,He), listtran(Tg,Te).
listtran([He|Te], [Hg|Tg]) :- tran(Hg,He), listtran(Te,Tg).

twice([], []).
twice([Head|Tail], [Head|[Head|X]]) :- twice(Tail, X).

remove(_, [], []).
remove(X, [X|Tail], Y) :- remove(X, Tail, Y).
remove(X, [Head|Tail], [Head|Y]) :- remove(X, Tail, Y), Head \= X.

swap_ends(X, Y) :-  X\==[], Y\==[], append([First|Mid], [Last], X), append([Last|Mid], [First], Y).

split_even_odd([], [], []).
split_even_odd([Even|Tail], X, [Even|Y]) :- split_odd_even(Tail, X, Y).
split_odd_even([], [], []).
split_odd_even([Odd|Tail], [Odd|X], Y) :- split_even_odd(Tail, X, Y).

preorder(leaf(X), [X]).
preorder(tree(Root, L, R), [Root|X]) :- preorder(L, Y), preorder(R, Z), append(Y, Z, X).
