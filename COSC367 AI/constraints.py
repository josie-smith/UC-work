# Given a Constriant Satisfaction Problem defined in csp.py, we can generate a
# set of solutions, make the csp arc-consistent,


from csp import *
import itertools, copy

# takes a CSP object and returns an iterable of solutions
def generate_and_test(csp):
    names, domains = zip(*csp.var_domains.items())
    for values in itertools.product(*domains):
        assignment = {x:v for x, v in zip(names, values)}
        if all( satisfies(assignment, constraint) for constraint in csp.constraints ):
            yield assignment


# makes the csp arc-consistent
def arc_consistent(csp):
    csp = copy.deepcopy(csp)
    todo = {(x, c) for c in csp.constraints for x in scope(c)} # c is a constraint, x is in the scope of the constraint
    while todo:
        x, c = todo.pop()
        ys = list(scope(c) - {x})           # ys is a list of other variables in the scope of c
        new_domain = set()
        for xval in csp.var_domains[x]:     # xval is in the domain of x
            assignment = {x: xval}
            for yvals in itertools.product(*[csp.var_domains[y] for y in ys]): # yval is in one of the daomains of the ys
                assignment.update({y: yval for y, yval in zip(ys, yvals)})
                if satisfies(assignment, c):
                    new_domain.add(xval)    # add xval to the new domain if there is an assignment of x that satisfies c
                    break
        if csp.var_domains[x] != new_domain:
            csp.var_domains[x] = new_domain
            for cprime in set(csp.constraints) - {c}: # for all other constraints cprime
                if x in scope(cprime):
                   for z in scope(cprime):
                       if x != z:
                           todo.add((z, cprime))
    return csp


# represent a CSP by a collection of relations
def relations(csp):
    names, domains = zip(*csp.var_domains.items())
    relations = []
    for c in csp.constraints:
        header = sorted(list(scope(c)))
        domains = [csp.var_domains[x] for x in scope(c)]
        tuples = set()
        for values in itertools.product(*domains):
            assignment = {x:v for x, v in zip(sorted(scope(c)), values)}
            if satisfies(assignment, c):
                tuples.add(tuple(assignment[x] for x in sorted(scope(c))))
        relations.append((header, tuples))
    return relations


# represents the cryptic cryptic_puzzle
#   two
# + two
# ------
#  four
cryptic_puzzle = CSP(
   var_domains = {var:{0,1,2,3,4,5,6,7,8,9} for var in 'twofurbc'},
   constraints = {
      lambda t: t != 0,
      lambda f: f != 0,
      lambda b, c: b<2 and c<2,
      lambda t,w,o,f,u,r: len({t,w,o,f,u,r}) == 6,
      lambda o, r, c: o+o == 10*c + r,
      lambda w, u, c, b: w+w+c == 10*b + u,
      lambda t, o, b, f: t+t+b == 10*f + o,
      }
   )

new_csp = arc_consistent(cryptic_puzzle)
solutions = []
for solution in generate_and_test(new_csp):
    solutions.append(sorted((x, v) for x, v in solution.items()
                            if x in "twofur"))
print(len(solutions))
solutions.sort()
print(solutions[0])
print(solutions[5])
